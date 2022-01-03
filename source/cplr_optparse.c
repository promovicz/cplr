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

#include <getopt.h>

#include "cplr.h"

#define USE_GETOPT_LONG

/* short options */
const char *shortopts = "-:hHVvdnpyD:U:I:i:S:s:L:l:P:b:a:t:o:f:-";

/* long is optional */
#ifdef USE_GETOPT_LONG
/* long options */
const struct option longopts[] = {
  {"help",     0, NULL, 'h'},
  {"herald",   0, NULL, 'H'},
  {"version",  0, NULL, 'V'},

  {"verbose",     0, NULL, 'v'},
  {"dump",        0, NULL, 'd'},
  {"noexec",      0, NULL, 'n'},
  {"pristine",    0, NULL, 'p'},
  {"interactive", 0, NULL, 'y'},

  {NULL,    1, NULL, 'D'},
  {NULL,    1, NULL, 'U'},
  {NULL,    1, NULL, 'I'},
  {NULL,    1, NULL, 'i'},
  {NULL,    1, NULL, 'S'},
  {NULL,    1, NULL, 's'},

  {NULL,    1, NULL, 'L'},
  {NULL,    1, NULL, 'l'},

  {NULL,    1, NULL, 'P'},

  {NULL,    1, NULL, 'b'},
  {NULL,    1, NULL, 'a'},
  {NULL,    1, NULL, 't'},

  {NULL,    1, NULL, 'o'},
  {NULL,    1, NULL, 'f'},

  {NULL,    0, NULL, '-'},

  {NULL,    0, NULL, 0 },
};
/* help strings for long options */
const char *longhelp[] = {
  "show help message",
  "show herald message",
  "show version string",

  "verbose cplr output",
  "dump generated code",
  "do not run, just compile",
  "use pristine environment",
  "run compiler shell",

  "define cpp symbol",
  "undefine cpp symbol",
  "add include directory",
  "add include",
  "add sysinclude directory",
  "add sysinclude",

  "add library directory",
  "add library",

  "add pkg-config library",

  "add before statement",
  "add after statement",
  "add toplevel statement",

  "output executable",
  "add file (source, object, archive)",

  "begin program arguments",
  NULL,
};
#endif /* USE_GETOPT_LONG */


static void cplr_show_summary(cplr_t *c, FILE*out) {
  fprintf(out, "Usage: %s [-vdnphHV] <statement>...\n", c->argv[0]);
  fprintf(out, "The C piler: a tool for executing C code\n\n");
}

/* help function */
static void cplr_show_help(cplr_t *c, FILE *out) {
  /* short summary */
  cplr_show_summary(c, out);
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
}

/* herald function */
static void cplr_show_herald(cplr_t *c, FILE *out) {
  /* program basename for mail address */
  const char *bn = basename(c->argv[0]);

  /* short summary */
  cplr_show_summary(c, out);

  /* history */
  fprintf(out, "Invented around the ides of October anno MMXX.\n\n");

  /* blessing */
  fprintf(out, "May this be as useful for you as it is for me.\n\n");

  /* copyright information */
  fprintf(out, "Copyright (C) 2020-2021 Ingo Albrecht <cplr@promovicz.org>.\n");
  fprintf(out, "Licensed under the GNU General Public License version 3 or later.\n");
  fprintf(out, "See package file COPYING or https://www.gnu.org/licenses/.\n\n");
}

/* version function */
static void cplr_show_version(cplr_t *c, FILE *out) {
}

/* option parser */
int cplr_optparse(cplr_t *c, int argc, char **argv) {
  int opt;

  /* remember argc/argv */
  c->argc = argc;
  c->argv = argv;

  /* parse options */
  optind = 1;
  opterr = 0;
  while(1) {
    /* get next option */
#ifdef USE_GETOPT_LONG
    opt = getopt_long(argc, argv, shortopts, longopts, NULL);
#else
    opt = getopt(argc, argv, shortopts);
#endif

    /* check if finished */
    if (opt == -1)
      goto done;

    /* dispatch the option */
    switch(opt) {
      /* internal */
    case 0: /* handled by getopt */
      break;
    case 1: /* non-option arguments are statements */
      l_append_str_static(&c->stms, optarg);
      break;

      /* information */
    case 'h': /* help requested */
      goto help;
    case 'H': /* herald requested */
      goto herald;
    case 'V': /* version requested */
      goto version;

      /* flags */
    case 'v': /* enable verbose */
      c->verbosity++;
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
    case 'y': /* enable interactive */
      c->flag |= CPLR_FLAG_INTERACTIVE;
      break;

      /* preprocessor */
    case 'D':
      l_append_str_owned(&c->defs, msnprintf(1024, "-D%s", optarg));
      break;
    case 'U':
      l_append_str_owned(&c->defs, msnprintf(1024, "-U%s", optarg));
      break;
    case 'I':
      l_append_str_static(&c->incdirs, optarg);
      break;
    case 'i':
      l_append_str_static(&c->incs, optarg);
      break;
    case 'S':
      l_append_str_static(&c->sysdirs, optarg);
      break;
    case 's':
      l_append_str_static(&c->syss, optarg);
      break;

      /* libraries */
    case 'L':
      l_append_str_static(&c->libdirs, optarg);
      break;
    case 'l':
      l_append_str_static(&c->libs, optarg);
      break;

      /* pkg-config */
    case 'P':
      l_append_str_static(&c->pkgs, optarg);
      break;

      /* statements */
    case 'e':
      l_append_str_static(&c->stms, optarg);
      break;
    case 'b':
      l_append_str_static(&c->befs, optarg);
      break;
    case 'a':
      l_append_str_static(&c->afts, optarg);
      break;
    case 't':
      l_append_str_static(&c->tlfs, optarg);
      break;

      /* output file */
    case 'o':
      c->flag |= CPLR_FLAG_NORUN;
      c->out = strdup(optarg);
      break;

      /* input files */
    case 'f':
      l_append_str_static(&c->srcs, optarg);
      break;

      /* start of program arguments */
    case '-':
      goto done;

      /* errors */
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
  /* show help */
  cplr_show_help(c, stdout);
  return 2;
 herald:
  /* show herald */
  cplr_show_herald(c, stdout);
  return 2;
 version:
  /* show version */
  cplr_show_version(c, stdout);
  return 2;
}
