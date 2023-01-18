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

#include <limits.h>
#include <unistd.h>

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

static int cmd_list(cplr_t *c, int argc, char **argv);
static int cmd_syms(cplr_t *c, int argc, char **argv);
static int cmd_dump(cplr_t *c, int argc, char **argv);
static int cmd_opts(cplr_t *c, int argc, char **argv);
static int cmd_stat(cplr_t *c, int argc, char **argv);
static int cmd_help(cplr_t *c, int argc, char **argv);
static int cmd_quit(cplr_t *c, int argc, char **argv);

static command_t commands[] = {
{ "l", "List code",     cmd_list },
{ "y", "List syms",     cmd_syms },
{ "d", "Show dump",     cmd_dump },
{ "o", "Show opts",     cmd_opts },
{ "s", "Show stat",     cmd_stat },
{ "?", "Show help",     cmd_help },
{ "q", "Quit",          cmd_quit },
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

  /* fail less when interactive */
  if(c->flag & CPLR_FLAG_INTERACTIVE) {
    ret = 0;
  }

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

/* Printing */

static void print_help(void) {
  command_t *ci;

  /* title */
  fprintf(stderr, "\n");

  /* line types */
  fprintf(stderr, "Prefixes: \n");
  fprintf(stderr, "  \\C    Piler command\n");
  fprintf(stderr, "  !C    System command\n");
  fprintf(stderr, "  @S    Push declaration\n");
  fprintf(stderr, "  ^S    Push toplevel\n");
  fprintf(stderr, "  <S    Push before\n");
  fprintf(stderr, "  >S    Push after\n");
  fprintf(stderr, "  .S    Push statement\n");
  fprintf(stderr, "  .     Run the pile\n");
  fprintf(stderr, "  S     Push and run\n");
  fprintf(stderr, "  ?     Show help\n");
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

static bool print_pile(cplr_t *c, const char *name, lh_t *list, bool compact, bool reverse) {
  int i;
  ln_t *n;

  if(l_empty(list)) {
    return false;
  }

  if(compact) {
    fprintf(stderr, "c%d-%s:", c->c_index, name);
  }

  if(reverse) {
    i = 0;
    L_BACKWARDS(list, n) {
      if(compact) {
        fprintf(stderr, " %s", value_get_str(&n->v));
      } else {
        fprintf(stderr, "c%d%s%d: %s\n", c->c_index, name, i, value_get_str(&n->v));
      }
      i++;
    }
  } else {
    i = 0;
    L_FORWARD(list, n) {
      if(compact) {
        fprintf(stderr, " %s", value_get_str(&n->v));
      } else {
        fprintf(stderr, "c%d%s%d: %s\n", c->c_index, name, i, value_get_str(&n->v));
      }
      i++;
    }
  }

  if(compact) {
    fprintf(stderr, "\n");
  }

  return true;
}

static int print_piles(cplr_t *c) {
  print_pile(c, "p", &c->pkgs, true, false);
  print_pile(c, "L", &c->libdirs, true, false);
  print_pile(c, "I", &c->incdirs, true, false);
  print_pile(c, "l", &c->libs, true, false);
  print_pile(c, "i", &c->incs, true, false);
  print_pile(c, "s", &c->srcs, true, false);
  print_pile(c, "d", &c->tlds, true, false);
  print_pile(c, "t", &c->tlfs, false, false);
  print_pile(c, "b", &c->befs, false, false);
  print_pile(c, "s", &c->stms, false, false);
  print_pile(c, "a", &c->afts, false, true);

  return 0;
}

/* Command implementations */

static int cmd_dump(cplr_t *c, int argc, char **argv) {
  int dumpsave = c->dump;
  c->dump = 1;
  cplr_generate(c);
  c->dump = dumpsave;
  return 0;
}

static int cmd_list(cplr_t *c, int argc, char **argv) {
  cplr_t *cur;
  for(cur = c->c_first; cur; cur = cur->c_next) {
    if(!cplr_empty(cur)) {
      print_piles(cur);
    }
  }
  return 0;
}

static int cmd_opts(cplr_t *c, int argc, char **argv) {
  c = c->c_first;
  fprintf(stderr, "opts:");
  if(c->flag & CPLR_FLAG_NODEFAULTS) {
    fprintf(stderr, " nodefaults");
  }
  if(c->flag & CPLR_FLAG_NOCOMPILE) {
    fprintf(stderr, " norun");
  }
  if(c->flag & CPLR_FLAG_NOLINK) {
    fprintf(stderr, " nolink");
  }
  if(c->flag & CPLR_FLAG_NORUN) {
    fprintf(stderr, " norun");
  }
  if(c->flag & CPLR_FLAG_FORK) {
    fprintf(stderr, " fork");
  }
  if(c->flag & CPLR_FLAG_INTERACTIVE) {
    fprintf(stderr, " interactive");
  }
  fprintf(stderr, "\n");
  return 0;
}

static int cmd_stat(cplr_t *c, int argc, char **argv) {
  cplr_t *cur;
  for(cur = c->c_first; cur; cur = cur->c_next) {
    fprintf(stderr, "c%d:", cur->c_index);
    if(cur->flag & CPLR_FLAG_GENERATED) {
      fprintf(stderr, " generated");
    }
    if(cur->flag & CPLR_FLAG_PREPARED) {
      fprintf(stderr, " prepared");
    }
    if(cur->flag & CPLR_FLAG_COMPILED) {
      fprintf(stderr, " compiled");
    }
    if(cur->flag & CPLR_FLAG_EXECUTED) {
      fprintf(stderr, " executed");
    }
    if(cur->flag & CPLR_FLAG_FINISHED) {
      fprintf(stderr, " finished");
    }
    fprintf(stderr, "\n");
  }
  return 0;
}

static void cmd_syms_print_cb(void *ctx, const char *name, const void *val) {
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

static int cmd_syms(cplr_t *c, int argc, char **argv) {
  TCCState *tc = cplr_find_syms(c);
  if(!tc) {
    fprintf(stderr, "No syms.\n");
    return 0;
  } else {
    tcc_list_symbols(tc, c, &cmd_syms_print_cb);
    return 0;
  }
}

static int cmd_help(cplr_t *c, int argc, char **argv) {
  print_help();
  return 0;
}

static int cmd_quit(cplr_t *c, int argc, char **argv) {
  return 1;
}

