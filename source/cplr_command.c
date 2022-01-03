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

/* Internal types */

typedef int (*command_cb_t)(cplr_t *c, int argc, char **argv);

typedef struct {
  char *name;
  char *help;
  command_cb_t handler;
} command_t;

/* Internal functions */

static void print_help(void);

/* Command definitions */

static int cmd_help(cplr_t *c, int argc, char **argv);
static int cmd_history(cplr_t *c, int argc, char **argv);
static int cmd_chain(cplr_t *c, int argc, char **argv);
static int cmd_piles(cplr_t *c, int argc, char **argv);
static int cmd_dump(cplr_t *c, int argc, char **argv);
static int cmd_library(cplr_t *c, int argc, char **argv);
static int cmd_memory(cplr_t *c, int argc, char **argv);
static int cmd_package(cplr_t *c, int argc, char **argv);
static int cmd_state(cplr_t *c, int argc, char **argv);
static int cmd_symbol(cplr_t *c, int argc, char **argv);
static int cmd_quit(cplr_t *c, int argc, char **argv);

static command_t commands[] = {
{ "?", "Show help", cmd_help },
{ "h", "Show history", cmd_history },
{ "c", "Show chain", cmd_chain },
{ "p", "Show piles", cmd_piles },
#if 0
{ "u", "Move up in chain", cmd_up },
{ "d", "Move down in chain", cmd_down },
#endif
{ "D", "Show dump", cmd_dump },
{ "L", "Show libraries", cmd_library },
{ "M", "Show memory", cmd_memory },
{ "P", "Show packages", cmd_package },
{ "S", "Show state", cmd_state },
{ "Y", "Show symbols", cmd_symbol },
{ "q", "Quit", cmd_quit },
{ NULL, NULL, NULL },
};

/* Exported functions */

static int cplr_command_internal(cplr_t *c, const char *line) {
  int ret = 1;
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
    if(c->flag & CPLR_FLAG_INTERACTIVE) {
      print_help();
      ret = 0;
    } else {
      fprintf(stderr, "No command.\n");
    }
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

 out:
  /* clean up */
  if(argv) {
    for(argn = 0; argn < argc; argn++) {
      xfree(argv[argn]);
    }
    free(argv);
  }

  /* return */
  return ret;
}

int cplr_command(cplr_t *c, const char *line) {
  int res, ret = 0;

  /* skip initial whitespace */
  while(*line && (isspace(*line) || iscntrl(*line))) { line++; };

  /* ignore empty lines */
  if(strlen(line) == 0) {
    goto out;
  }

  switch(line[0]) {
    /* show help */
  case '?':
    print_help();
    goto out;

    /* internal command */
  case '\\':
    ret = cplr_command_internal(c, line+1);
    goto out;

    /* system command */
  case '!':
    if(strlen(line+1) == 0) {
      fprintf(stderr, "Must provide command.\n");
      goto out;
    }
    system(line+1);
    goto out;

    /* declaration */
  case '@':
    l_append_str_owned(&c->tlds, strdup(line+1));
    goto out;
    /* toplevel */
  case '^':
    l_append_str_owned(&c->tlfs, strdup(line+1));
    goto out;
    /* before */
  case '<':
    l_append_str_owned(&c->befs, strdup(line+1));
    goto out;
    /* after */
  case '>':
    l_append_str_owned(&c->afts, strdup(line+1));
    goto out;
    /* statement */
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

  /* run the pile */
  res = cplr_run(c);
  if(res) {
    fprintf(stderr, "Run failed\n");
  }

 out:
  /* always succeed */
  return ret;
}

/* Command implementations */

static int cmd_help(cplr_t *c, int argc, char **argv) {
  print_help();
  return 0;
}

static int cmd_history(cplr_t *c, int argc, char **argv) {
  printf("History\n");
  return 0;
}

static int cmd_chain(cplr_t *c, int argc, char **argv) {
  printf("Chain\n");
  return 0;
}

static bool print_pile(cplr_t *c, const char *name, lh_t *list, bool reverse) {
  int i;
  ln_t *n;

  if(l_empty(list)) {
    return false;
  }

  if(reverse) {
    i = 0;
    L_BACKWARDS(list, n) {
      fprintf(stderr, "  %s%d: %s\n", name, i, value_get_str(&n->v));
      i++;
    }
  } else {
    i = 0;
    L_FORWARD(list, n) {
      fprintf(stderr, "  %s%d: %s\n", name, i, value_get_str(&n->v));
      i++;
    }
  }

  return true;
}

static int cmd_piles(cplr_t *c, int argc, char **argv) {
  bool any = false;
  any |= print_pile(c, "d", &c->tlds, false);
  any |= print_pile(c, "t", &c->tlfs, false);
  if(any)
    fprintf(stderr, "\n");
  print_pile(c, "b", &c->befs, false);
  print_pile(c, "s", &c->stms, false);
  print_pile(c, "a", &c->afts, true);
  return 0;
}

static int cmd_dump(cplr_t *c, int argc, char **argv) {
  int dumpsave = c->dump;
  c->dump = 1;
  cplr_generate(c);
  c->dump = dumpsave;
  return 0;
}

static int cmd_library(cplr_t *c, int argc, char **argv) {
  return 0;
}

static int cmd_memory(cplr_t *c, int argc, char **argv) {
  return 0;
}

static int cmd_package(cplr_t *c, int argc, char **argv) {
  return 0;
}

static int cmd_state(cplr_t *c, int argc, char **argv) {
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

static int cmd_symbol(cplr_t *c, int argc, char **argv) {
  if(!c->tcc) {
    fprintf(stderr, "No symbols.\n");
  }
  tcc_list_symbols(cplr_find_syms(c), c, &print_sym_cb);
  return 0;
}

static int cmd_quit(cplr_t *c, int argc, char **argv) {
  return 1;
}

/* Internal functions */

static void print_help(void) {
  command_t *ci;

  /* title */
  fprintf(stderr, "\n");

  /* line types */
  fprintf(stderr, "Prefixes: \n");
  fprintf(stderr, "  \\C   Piler command\n");
  fprintf(stderr, "  !C   System command\n");
  fprintf(stderr, "  @S   Push declaration\n");
  fprintf(stderr, "  ^S   Push toplevel\n");
  fprintf(stderr, "  <S   Push before\n");
  fprintf(stderr, "  >S   Push after\n");
  fprintf(stderr, "  .S   Push statement\n");
  fprintf(stderr, "  .    Run the pile\n");
  fprintf(stderr, "  S    Push and run\n");
  fprintf(stderr, "  ?    Show help\n");
  fprintf(stderr, "\n");

  /* command summary */
  fprintf(stderr, "Commands: \n");
  ci = &commands[0];
  while(ci->name) {
    fprintf(stderr, "  \\%-4s %s\n", ci->name, ci->help);
    ci++;
  }
  fprintf(stderr, "\n");
}
