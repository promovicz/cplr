/*
 * c - utility for running C code
 *
 * Copyright (C) 2020 Ingo Albrecht <copyright@promovicz.org>
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

/*
 * NOTICE
 *
 * This program may embody magical lowlevel programming powers
 * that may be illegal to possess in your domain or directory
 * of residence.
 *
 * My explicit advice is to use it anyway.
 *
 * Use the power of ~/.local/bin and enjoy it's freedoms.
 */

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libtcc.h>

#include "libx.h"
#include "libl.h"
#include "libs.h"

#define USE_GETOPT_LONG

const char *bar =
  "========================================"
  "========================================";

int pkg_exists(const char *name) {
  int res;
  char cbuf[64];
  snprintf(cbuf, sizeof(cbuf), "pkg-config --exists %s", name);
  res = system(cbuf);
  if(res == -1 || res == 127) {
    fprintf(stderr, "Error: Could not execute \"%s\".\n", cbuf);
    return 1;
  } else if(res) {
    fprintf(stderr, "Error: Package %s not present.\n", name);
  }
  return 0;
}

char *pkg_retrieve(const char *name, const char *what) {
  int res;
  FILE *ps;
  char cbuf[64], rbuf[1024];
  snprintf(cbuf, sizeof(cbuf), "pkg-config %s %s", what, name);
  ps = popen(cbuf, "r");
  if(!ps) {
    fprintf(stderr, "Error: Could not popen \"%s\".\n", cbuf);
    return NULL;
  }
  res = fread(rbuf, 1, sizeof(rbuf), ps);
  if(res < 0) {
    fprintf(stderr, "Error: Failed to read from \"%s\".\n", cbuf);
    return NULL;
  }
  if(res == sizeof(rbuf)) {
    fprintf(stderr, "Error: Package options for %s are to long.\n", name);
    return NULL;
  }
  rbuf[res] = 0;
  if(rbuf[res - 1] == '\n')
    rbuf[res - 1] = 0;
  pclose(ps);
  return strdup(rbuf);
}

typedef enum {
   CPLR_FLAG_DUMP = 1,
   CPLR_FLAG_NORUN = 2,
   CPLR_FLAG_VERBOSE = 4,
   CPLR_FLAG_PRISTINE = 8,
} cplr_flag_t;

typedef enum {
   CPLR_GSTATE_INITIAL = 0,
   CPLR_GSTATE_COMMENT = 1,
   CPLR_GSTATE_INTERNAL = 2,
   CPLR_GSTATE_INCLUDE = 3,
   CPLR_GSTATE_TOPLEVEL = 4,
   CPLR_GSTATE_STATEMENT = 5,
} cplr_gstate_t;

typedef struct cplr {
  cplr_flag_t flag;

  int    argc;
  int    argp;
  char **argv;

  lh_t defdef;
  lh_t defsys;

  lh_t sysdirs;
  lh_t incdirs;
  lh_t libdirs;
  lh_t defs;
  lh_t syss;
  lh_t incs;
  lh_t libs;
  lh_t pkgs;
  lh_t srcs;
  lh_t tlfs;
  lh_t stms;
  lh_t befs;
  lh_t afts;

  cplr_gstate_t g_state;
  int   g_prevline;
  char *g_codebuf;
  FILE *g_code;
  FILE *g_dump;

  TCCState *tcc;
} cplr_t;

void cplr_emit(cplr_t *c,
	       cplr_gstate_t nstate,
	       const char * file, int line,
	       const char *fmt, ...) {
  bool needline = false;
  va_list a;
  if(nstate == CPLR_GSTATE_COMMENT) {
    needline = false;
  } else if(c->g_state != nstate) {
    needline = true;
  } else {
    needline = (c->g_prevline && (line != (c->g_prevline+1)));
  }
  va_start(a, fmt);
  if(c->g_code) {
    if(needline)
      fprintf(c->g_code, "#line %d \"%s\"\n", line, file);
    vfprintf(c->g_code, fmt, a);
  }
  va_end(a);
  va_start(a, fmt);
  if(c->g_dump) {
    if(needline && (c->flag & CPLR_FLAG_VERBOSE))
      fprintf(c->g_dump, "#line %d \"%s\"\n", line, file);
    vfprintf(c->g_dump, fmt, a);
  }
  va_end(a);
  if(nstate != CPLR_GSTATE_COMMENT) {
    c->g_state = nstate;
    c->g_prevline = line;
  }
}

#define CPLR_EMIT_COMMENT(c, fmt, ...)		\
  cplr_emit(c, CPLR_GSTATE_COMMENT, NULL,		\
	    1, "/* " fmt " */\n", ##__VA_ARGS__)
#define CPLR_EMIT_INCLUDE(c, fn, fmt, ...)		\
  cplr_emit(c, CPLR_GSTATE_INCLUDE, fn,			\
	    1, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_TOPLEVEL(c, fn, fmt, ...)	\
  cplr_emit(c, CPLR_GSTATE_TOPLEVEL, fn,	\
	    1, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_INTERNAL(c, fmt, ...)			\
  cplr_emit(c, CPLR_GSTATE_INTERNAL, "internal",	\
	    __LINE__, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_STATEMENT(c, fn, fmt, ...)	\
  cplr_emit(c, CPLR_GSTATE_STATEMENT, fn,	\
	    1, fmt, ##__VA_ARGS__)

int cplr_code(cplr_t *c) {
  int i;
  ln_t *n;
  char fn[32];
  if(!l_empty(&c->defsys)) {
    CPLR_EMIT_COMMENT(c, "defsysinclude");
    i = 0;
    L_FOREACH(&c->defsys, n) {
      snprintf(fn, sizeof(fn), "defsysinclude_%d", i++);
      CPLR_EMIT_INCLUDE(c, fn, "#include <%s>\n", n->v.s);
    }
  }
  if(!l_empty(&c->syss)) {
    CPLR_EMIT_COMMENT(c, "sysinclude");
    i = 0;
    L_FOREACH(&c->syss, n) {
      snprintf(fn, sizeof(fn), "sysinclude_%d", i++);
      CPLR_EMIT_INCLUDE(c, fn, "#include <%s>\n", n->v.s);
    }
  }
  if(!l_empty(&c->incs)) {
    CPLR_EMIT_COMMENT(c, "include");
    i = 0;
    L_FOREACH(&c->incs, n) {
      snprintf(fn, sizeof(fn), "include_%d", i++);
      CPLR_EMIT_INCLUDE(c, fn, "#include \"%s\"\n", n->v.s);
    }
  }
  if(!l_empty(&c->tlfs)) {
    CPLR_EMIT_COMMENT(c, "toplevel");
    i = 0;
    L_FOREACH(&c->tlfs, n) {
      snprintf(fn, sizeof(fn), "toplevel_%d", i++);
      CPLR_EMIT_TOPLEVEL(c, fn, "%s;\n", n->v.s);
    }
  }
  CPLR_EMIT_COMMENT(c, "main");
  CPLR_EMIT_INTERNAL(c, "int main(int argc, char **argv) {\n");
  CPLR_EMIT_INTERNAL(c, "    int ret = 0;\n");
  if(!l_empty(&c->befs)) {
    CPLR_EMIT_COMMENT(c, "before");
    i = 0;
    L_FOREACH(&c->befs, n) {
      snprintf(fn, sizeof(fn), "before_%d", i++);
      CPLR_EMIT_STATEMENT(c, fn, "    %s;\n", n->v.s);
    }
  }
  if(!l_empty(&c->stms)) {
    CPLR_EMIT_COMMENT(c, "statement");
    i = 0;
    L_FOREACH(&c->stms, n) {
      snprintf(fn, sizeof(fn), "statement_%d", i++);
      CPLR_EMIT_STATEMENT(c, fn, "    %s;\n", n->v.s);
    }
  }
  if(!l_empty(&c->afts)) {
    CPLR_EMIT_COMMENT(c, "after");
    i = 0;
    L_FOREACH(&c->afts, n) {
      snprintf(fn, sizeof(fn), "after_%d", i++);
      CPLR_EMIT_STATEMENT(c, fn, "    %s;\n", n->v.s);
    }
  }
  CPLR_EMIT_INTERNAL(c, "    return ret;\n");
  CPLR_EMIT_INTERNAL(c, "}\n");
  return 0;
}

int cplr_generate(cplr_t *c) {
  c->g_codebuf = xcalloc(4096, 1);
  c->g_code = fmemopen(c->g_codebuf, 4096, "w");
  if(c->flag & CPLR_FLAG_DUMP) {
    if(c->flag & CPLR_FLAG_VERBOSE)
      fprintf(stderr, "%s\n", bar);
    c->g_dump = popen("cat -n 1>&2", "w");
  }
  cplr_code(c);
  if(c->flag & CPLR_FLAG_DUMP) {
    pclose(c->g_dump);
    if(c->flag & CPLR_FLAG_VERBOSE)
      fprintf(stderr, "%s\n", bar);
  }
  fclose(c->g_code);
  return 0;
}

static void cplr_tcc_error(cplr_t *c, const char *msg) {
  fprintf(stderr, "Compiler condition:\n%s\n", msg);
  /* TODO split and indent */
}

int cplr_add_package(cplr_t *c, const char *name, const char *args) {
  bool dash = false;
  char opt = 0;
  char *o = (char*)args, *b, *s;
  do {
    char n = *o;
    if(!n) {
      break;
    }
    if(opt) {
      b = o;
      while(*o && isgraph(*o)) o++;
      s = strndup(b, o - b);
      switch(opt) {
      case 'D':
	l_appends(&c->defs, msnprintf(1024, "-D%s", s));
	if(c->flag & CPLR_FLAG_VERBOSE)
	  fprintf(stderr, "Package %s define: -D%s\n", name, s);
	free(s);
	break;
      case 'I':
	l_appends(&c->incdirs, s);
	if(c->flag & CPLR_FLAG_VERBOSE)
	  fprintf(stderr, "Package %s include dir: %s\n", name, s);
	break;
      case 'L':
	l_appends(&c->libdirs, s);
	if(c->flag & CPLR_FLAG_VERBOSE)
	  fprintf(stderr, "Package %s library dir: %s\n", name, s);
	break;
      case 'l':
	l_appends(&c->libs, s);
	if(c->flag & CPLR_FLAG_VERBOSE)
	  fprintf(stderr, "Package %s library: %s\n", name, s);
	break;
      }
      opt = 0;
    } else if(dash) {
      switch(n) {
      case 'D':
      case 'I':
      case 'L':
      case 'l':
	opt = n;
	while(*o && isspace(*o)) o++;
	break;
      case 'p':
	if(strstr(o, "pthread") == o) {
	  if(c->flag & CPLR_FLAG_VERBOSE)
	    fprintf(stderr, "Package %s uses pthreads.\n", name);
	  tcc_set_options(c->tcc, "-pthread");
	  l_appends(&c->libs, "pthread");
	  o += 6;
	  break;
	}
	/* fall through */
      default:
	fprintf(stderr, "Warning: unhandled option -%c in package %s: %s\n", n, name, o-1);
	while(*o && isgraph(*o)) o++;
	//return 1;
      }
      dash = false;
    } else {
      if(n == '-') {
	dash = true;
      } else if(isspace(n)) {
	continue;
      } else {
	fprintf(stderr, "Invalid string in package %s: %s\n", name, o);
	return 1;
      }
    }
  } while(*o++);
  return 0;
}

int cplr_prepare_package(cplr_t *c, const char *name) {
    char *s;
    TCCState *t = c->tcc;

    if(pkg_exists(name)) {
      return 1;
    }

    s = pkg_retrieve(name, "--cflags --libs");
    if(!s) {
      return 1;
    }
    if(c->flag & CPLR_FLAG_VERBOSE)
      fprintf(stderr, "Package %s: %s\n", name, s);
    tcc_set_options(t, s);
    if(cplr_add_package(c, name, s)) {
      fprintf(stderr, "Error: Failed to process package %s.\n", name);
      return 1;
    }
    free(s);

    return 0;
}

int cplr_prepare(cplr_t *c) {
  ln_t *i;
  TCCState *t = tcc_new();
  c->tcc = t;
  tcc_set_error_func(t, c, (void (*)(void *, const char *))&cplr_tcc_error);
  if(tcc_set_output_type(t, TCC_OUTPUT_MEMORY)) {
    return 1;
  }
  L_FOREACH(&c->pkgs, i) {
    if(cplr_prepare_package(c, i->v.s)) {
      return 1;
    }
  }
  L_FOREACH(&c->defdef, i) {
    tcc_set_options(t, i->v.s);
  }
  L_FOREACH(&c->defs, i) {
    tcc_set_options(t, i->v.s);
  }
  L_FOREACH(&c->sysdirs, i) {
    if(tcc_add_sysinclude_path(t, i->v.s)) {
      return 1;
    }
  }
  L_FOREACH(&c->incdirs, i) {
    if(tcc_add_include_path(t, i->v.s)) {
      return 1;
    }
  }
  L_FOREACH(&c->libdirs, i) {
    if(tcc_add_library_path(t, i->v.s)) {
      return 1;
    }
  }
  L_FOREACH(&c->libs, i) {
    if(tcc_add_library(t, i->v.s)) {
      return 1;
    }
  }
  L_FOREACH(&c->srcs, i) {
    if(tcc_add_file(t, i->v.s)) {
      return 1;
    }
  }
  return 0;
}

int cplr_compile(cplr_t *c) {

  if(tcc_compile_string(c->tcc, c->g_codebuf)) {
    return 1;
  }

  return 0;
}

int cplr_execute(cplr_t *c) {
  int i;
  int argc = 1 + (c->argc - c->argp);
  char **argv = xcalloc(sizeof(char*), argc+1);
  argv[0] = "c";
  for(i = 1; i < argc; i++) {
    argv[i] = c->argv[c->argp + (i - 1)];
  }
  return tcc_run(c->tcc, argc, argv);
}

int cplr_defaults(cplr_t *c) {
  l_appends(&c->defdef, "-D_DEFAULT_SOURCE");
  l_appends(&c->defdef, "-D_XOPEN_SOURCE=600");
  l_appends(&c->defdef, "-D_POSIX_SOURCE=200809L");
  l_appends(&c->defsys, "sys/stat.h");
  l_appends(&c->defsys, "sys/types.h");
  l_appends(&c->defsys, "assert.h");
  l_appends(&c->defsys, "ctype.h");
  l_appends(&c->defsys, "errno.h");
  l_appends(&c->defsys, "fcntl.h");
  l_appends(&c->defsys, "libgen.h");
  l_appends(&c->defsys, "limits.h");
  l_appends(&c->defsys, "stdarg.h");
  l_appends(&c->defsys, "stdio.h");
  l_appends(&c->defsys, "stdint.h");
  l_appends(&c->defsys, "stdlib.h");
  l_appends(&c->defsys, "string.h");
  l_appends(&c->defsys, "termios.h");
  l_appends(&c->defsys, "time.h");
  l_appends(&c->defsys, "unistd.h");
  return 0;
}

/* short options */
const char *shortopts = "-:hHVvdnpP:D:U:I:L:S:i:l:s:f:t:e:b:a:-";

/* long is optional */
#ifdef USE_GETOPT_LONG
/* long options */
const struct option longopts[] = {
	{"help",     0, NULL, 'h'},
	{"herald",   0, NULL, 'H'},
	{"version",  0, NULL, 'V'},
	{"verbose",  0, NULL, 'v'},
	{"dump",     0, NULL, 'd'},
	{"noexec",   0, NULL, 'n'},
	{"pristine", 0, NULL, 'p'},
	{NULL,    1, NULL, 'P'},
	{NULL,    1, NULL, 'D'},
	{NULL,    1, NULL, 'U'},
	{NULL,    1, NULL, 'I'},
	{NULL,    1, NULL, 'L'},
	{NULL,    1, NULL, 'S'},
	{NULL,    1, NULL, 'i'},
	{NULL,    1, NULL, 'l'},
	{NULL,    1, NULL, 's'},
	{NULL,    1, NULL, 'f'},
	{NULL,    1, NULL, 't'},
	{NULL,    1, NULL, 'e'},
	{NULL,    1, NULL, 'b'},
	{NULL,    1, NULL, 'a'},
	{NULL,    0, NULL, 0 },
};
/* help strings for long options */
const char *longhelp[] = {
	"show help message",
	"show herald message",
	"show version string",

	"print verbose messages",
	"dump generated code",
	"do not run, just compile",
	"pristine environment",
	"add package",
	"define cpp symbol",
	"undefine cpp symbol",
	"add include directory",
	"add library directory",
	"add sysinclude directory",
	"add include",
	"add library",
	"add sysinclude",
	"add source file",
	"add toplevel statement",
	"add main statement",
	"add before-main statement",
	"add after-main statement",
	NULL,
};
#endif /* USE_GETOPT_LONG */

/* help function */
static void cplr_show_help(cplr_t *c, FILE *out) {
  /* program basename for mail address */
  const char *bn = basename(c->argv[0]);
  /* short summary */
  fprintf(out, "Usage: %s [-vdnphHV] <statement>...\n", c->argv[0]);
  fprintf(out, "The C piler: a tool for executing C code\n\n");
  /* with getopt_long we can easily print more information */
#ifdef USE_GETOPT_LONG
  int i;
  for(i = 0; longhelp[i]; i++) {
    if(longopts[i].name) {
      fprintf(out, "  -%c, --%-10s\t%s\n",
	      (char)longopts[i].val,
	      longopts[i].name,
	      longhelp[i]);
	  } else {
      fprintf(out, "  -%c\t\t\t%s\n",
	      (char)longopts[i].val,
	      longhelp[i]);
    }
  }
  fprintf(out, "\n");
#endif
  fprintf(out, "Copyright (C) 2020 Ingo Albrecht <%s@promovicz.org>.\n", bn);
  fprintf(out, "Licensed under the GNU General Public License version 3 or later.\n");
  fprintf(out, "See package file COPYING or https://www.gnu.org/licenses/.\n\n");
}

/* herald function */
static void cplr_show_herald(cplr_t *c, FILE *out) {
}

/* version function */
static void cplr_show_version(cplr_t *c, FILE *out) {
}

/* option parser */
static int cplr_optparse(cplr_t *c, int argc, char **argv) {
  int opt;
  /* remember argc/argv */
  c->argc = argc;
  c->argv = argv;
  /* parse options */
  optind = 1;
  opterr = 0;
  while(1) {
#ifdef USE_GETOPT_LONG
    opt = getopt_long(argc, argv, shortopts, longopts, NULL);
#else
    opt = getopt(argc, argv, shortopts);
#endif
    if (opt == -1)
      goto done;
    switch(opt) {
    case 'h': /* help requested */
      goto help;
    case 'H': /* herald requested */
      goto herald;
    case 'V': /* version requested */
      goto version;
    case 0: /* handled by getopt */
      break;
    case 1: /* non-option argument */
      l_appends(&c->stms, optarg);
      break;
    case '-': /* terminating argument */
      goto done;
    case 'v': /* enable verbose */
      c->flag |= CPLR_FLAG_VERBOSE;
      break;
    case 'd': /* enable dump */
      c->flag |= CPLR_FLAG_DUMP;
      break;
    case 'n': /* enable norun */
      c->flag |= CPLR_FLAG_NORUN;
      break;
    case 'p': /* enable pristine */
      c->flag |= CPLR_FLAG_PRISTINE;
      break;
    case 'P':
      l_appends(&c->pkgs, optarg);
      break;
    case 'D':
      l_appends(&c->defs, msnprintf(1024, "-D%s", optarg));
      break;
    case 'U':
      l_appends(&c->defs, msnprintf(1024, "-U%s", optarg));
      break;
    case 'I':
      l_appends(&c->incdirs, optarg);
      break;
    case 'S':
      l_appends(&c->sysdirs, optarg);
      break;
    case 'L':
      l_appends(&c->libdirs, optarg);
      break;
    case 'i':
      l_appends(&c->incs, optarg);
      break;
    case 's':
      l_appends(&c->syss, optarg);
      break;
    case 'l':
      l_appends(&c->libs, optarg);
      break;
    case 'f':
      l_appends(&c->srcs, optarg);
      break;
    case 't':
      l_appends(&c->tlfs, optarg);
      break;
    case 'e':
      l_appends(&c->stms, optarg);
      break;
    case 'b':
      l_appends(&c->befs, optarg);
      break;
    case 'a':
      l_appends(&c->afts, optarg);
      break;
    case ':': /* missing option argument */
      fprintf(stderr, "Missing argument for option -%c\n", optopt);
      goto err;
    case '?': /* unknown option */
      fprintf(stderr, "Unknown option -%c\n", optopt);
      goto err;
    default:
      fprintf(stderr, "Option processing problem\n");
      goto err;
    }
  }
 done:
  /* set index of program args */
  c->argp = optind;
  return 0;
 err:
  return 1;
 help:
  cplr_show_help(c, stdout);
  return 0;
 herald:
  cplr_show_herald(c, stdout);
  return 0;
 version:
  cplr_show_version(c, stdout);
  return 0;
}

int main(int argc, char **argv) {
  int res;
  cplr_t c;
  memset(&c, 0, sizeof(c));

  if(cplr_optparse(&c, argc, argv)) {
    return 1;
  }

  if(!(c.flag & CPLR_FLAG_PRISTINE)) {
    if(cplr_defaults(&c)) {
      fprintf(stderr, "Error: Default initialization failed.\n");
      return 1;
    }
  }

  if(cplr_prepare(&c)) {
    fprintf(stderr, "Error: Prepare failed.\n");
    return 1;
  }

  if(cplr_generate(&c)) {
    fprintf(stderr, "Error: Code generation failed.\n");
    return 1;
  }

  if(cplr_compile(&c)) {
    fprintf(stderr, "Error: Compilation failed.\n");
    return 1;
  }

  if(c.flag & CPLR_FLAG_NORUN) {
    fprintf(stderr, "Compilation succeeded.\n");
  } else {
    res = cplr_execute(&c);
    if(res) {
      return res;
    }
  }

  return 0;
}
