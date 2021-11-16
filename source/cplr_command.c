/*
 * cplr - Utility for running C code
 *
 * Copyright (C) 2020-2021 Ingo Albrecht <copyright@promovicz.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "cplr.h"

typedef int (*command_cb_t)(cplr_t *c, int argc, char **argv);

struct command {
  char *name;
  char *help;
  command_cb_t handler;
};

typedef struct command command_t;

static void print_help(void);
static int cmd_help(cplr_t *c, int argc, char **argv);
static int cmd_hist(cplr_t *c, int argc, char **argv);
static int cmd_code(cplr_t *c, int argc, char **argv);
static int cmd_dump(cplr_t *c, int argc, char **argv);
static int cmd_stat(cplr_t *c, int argc, char **argv);
static int cmd_libs(cplr_t *c, int argc, char **argv);
static int cmd_syms(cplr_t *c, int argc, char **argv);

static command_t commands[] = {
{ "help", "Show help", cmd_help },
{ "hist", "Show history", cmd_hist },
{ "code", "Show current code", cmd_code },
{ "dump", "Show current output", cmd_dump },
{ "stat", "Show current status", cmd_stat },
{ "libs", "Show library table", cmd_libs },
{ "syms", "Show symbol table", cmd_syms },
#if 0
{ "prev", "Switch to previous piler", cmd_prev },
{ "next", "Switch to next piler", cmd_next },
#endif
{ NULL, NULL, NULL },
};

static void print_help(void) {
  command_t *ci;

  /* title */
  fprintf(stderr, "\n");

  /* line types */
  fprintf(stderr, "Prefixes: \n\n");
  fprintf(stderr, "  :CMD  Run piler command\n");
  fprintf(stderr, "  !CMD  Run system command\n");
  fprintf(stderr, "  #CPP  Push tld for cpp\n");
  fprintf(stderr, "  ^STM  Push tld statement\n");
  fprintf(stderr, "  $STM  Push tlf statement\n");
  fprintf(stderr, "  <STM  Push bef statement\n");
  fprintf(stderr, "  >STM  Push aft statement\n");
  fprintf(stderr, "  .STM  Push statement\n");
  fprintf(stderr, "  .     Run the pile\n");
  fprintf(stderr, "  STM   Push statement and run pile\n");
  fprintf(stderr, "  ?     Show help\n");
  fprintf(stderr, "\n");

  /* command summary */
  fprintf(stderr, "Commands: \n\n");
  ci = &commands[0];
  while(ci->name) {
    fprintf(stderr, "  :%-8s %s\n", ci->name, ci->help);
    ci++;
  }
  fprintf(stderr, "\n");
}

static int cmd_help(cplr_t *c, int argc, char **argv) {
  print_help();
  return 0;
}

static int cmd_hist(cplr_t *c, int argc, char **argv) {
  printf("History\n");
  return 0;
}

static void print_section(cplr_t *c, const char *name, lh_t *list) {
  int i;
  ln_t *n;

  if(l_empty(list)) {
    return;
  }

  i = 0;
  L_FORWARD(list, n) {
    fprintf(stderr, "  %s[%d]: %s\n", name, i, value_get_str(&n->v));
    i++;
  }
}

static int cmd_code(cplr_t *c, int argc, char **argv) {
  print_section(c, "tld", &c->tlds);
  print_section(c, "tlf", &c->tlfs);
  print_section(c, "bef", &c->befs);
  print_section(c, "stm", &c->stms);
  print_section(c, "aft", &c->afts);
  return 0;
}

static int cmd_dump(cplr_t *c, int argc, char **argv) {
  cplr_flag_t flagsave = c->flag;
  c->flag |= CPLR_FLAG_DUMP;
  cplr_generate(c);
  c->flag = flagsave;
  return 0;
}

static int cmd_stat(cplr_t *c, int argc, char **argv) {
  fprintf(stderr, "Piler status:\n\n");
  return 0;
}

static void print_sym_cb(void *ctx, const char *name, const void *val) {
  if(name[0] == '_') {
    return;
  }
  if(strprefix(name, "tcc_")) {
    return;
  }
  if(strsuffix(name, "@plt")) {
    return;
  }
  fprintf(stderr, "  %-12s\t%p\n", name, val);
}

static int cmd_libs(cplr_t *c, int argc, char **argv) {
  return 0;
}

static int cmd_syms(cplr_t *c, int argc, char **argv) {
  tcc_list_symbols(cplr_find_syms(c), c, &print_sym_cb);
  return 0;
}

cplr_t *cplr_command_batch(cplr_t *c, const char *line) {
  int ret = 0;
  int argc, argn;
  char **argv = NULL;
  char *tokline;
  char *token;
  char *toksave;
  char *cmd;
  command_t *ci;

  /* count arguments */
  tokline = strdup(line);
  argn = 0;
  while((token = strtok_r(tokline, " \t\r\n", &toksave))) {
    size_t len;
    tokline = NULL;
    len = strlen(token);
    if(!len) {
      continue;
    }
    argn++;
  }
  argc = argn;
  free(tokline);

  /* split arguments */
  tokline = strdup(line);
  argn = 0;
  argv = xcalloc(argc + 1, sizeof(char*));
  argv[argc] = NULL;
  while((token = strtok_r(tokline, " \t\r\n", &toksave))) {
    tokline = NULL;
    argv[argn] = strdup(token);
    if(c->verbosity >= 3) {
      fprintf(stderr, "cmd[%d] = %s\n", argn, argv[argn]);
    }
    argn++;
  }
  free(tokline);

  /* check for command */
  if(argc == 0) {
    fprintf(stderr, "No command.\n");
    ret = 1;
    goto out;
  }

  /* get command */
  cmd = argv[0];

  /* find and run command */
  ci = &commands[0];
  while(ci->name) {
    if(!strcmp(ci->name, cmd)) {
      ret = ci->handler(c, argc - 1, argv + 1);
      goto out;
    }
    ci++;
  }

  /* unknown command */
  fprintf(stderr, "Unknown command \"%s\".\n", cmd);
  ret = 1;

 out:
  /* clean up */
  if(argv) {
    for(argn = 0; argn < argc; argn++) {
      xfree(argv[argn]);
    }
    free(argv);
  }
  /* return */
  return c;
}

cplr_t *cplr_command_interactive(cplr_t *c, const char *line) {

  while(*line && isspace(*line)) { line++; };

  if(strlen(line) == 0) {
    return c;
  }

  switch(line[0]) {
    /* show help */
  case '?':
    print_help();
    goto out;

    /* piler command */
  case ':':
    cplr_command_batch(c, line+1);
    goto out;

    /* system command */
  case '!':
    system(line+1);
    goto out;

    /* cpp must go to toplevel */
  case '#':
    l_append_str_owned(&c->tlds, strdup(line));
    goto out;
    /* toplevel declarations */
  case '^':
    l_append_str_owned(&c->tlds, strdup(line+1));
    goto out;
    /* toplevel statements (definitions) */
  case '$':
    l_append_str_owned(&c->tlfs, strdup(line+1));
    goto out;
    /* before and after statements */
  case '<':
    l_append_str_owned(&c->befs, strdup(line+1));
    goto out;
  case '>':
    l_append_str_owned(&c->afts, strdup(line+1));
    goto out;

    /* append statements */
  case '.':
    if(strlen(line+1)) {
      l_append_str_owned(&c->stms, strdup(line+1));
      goto out;
    }
    break;

  default:
    l_append_str_owned(&c->stms, strdup(line));
    break;
  }

  return cplr_evaluate(c);

 out:
  return c;
}
