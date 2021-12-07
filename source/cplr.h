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

#ifndef CPLR_PRIVATE_H
#define CPLR_PRIVATE_H

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <libtcc.h>

#include <cext/list.h>
#include <cext/string.h>

/* cplr flags */
typedef enum {
   CPLR_FLAG_DUMP = (1 << 1),
   CPLR_FLAG_NORUN = (1 << 2),
   CPLR_FLAG_PRISTINE = (1 << 4),
   CPLR_FLAG_LOOP = (1 << 5),
   CPLR_FLAG_INTERACTIVE = (1 << 6),
   CPLR_FLAG_EVALUATED = (1 << 7),
} cplr_flag_t;

/* target environment */
typedef enum {
   CPLR_TARGET_NONE = 0,
   CPLR_TARGET_POSIX = 1,
   CPLR_TARGET_LINUX = (2 | CPLR_TARGET_POSIX),
   CPLR_TARGET_LINUX_KERNEL = 4,
} cplr_target_t;

/* code generation phases */
typedef enum {
   CPLR_GSTATE_INITIAL = 0,
   CPLR_GSTATE_COMMENT = 1,
   CPLR_GSTATE_INTERNAL = 2,
   CPLR_GSTATE_PREPROC = 3,
   CPLR_GSTATE_TOPLEVEL = 4,
   CPLR_GSTATE_STATEMENT = 5,
} cplr_gstate_t;

/* forward declarations */
struct cplr;
typedef struct cplr cplr_t;

/* main state structure */
struct cplr {
  /* flags */
  cplr_flag_t flag;
  /* verbosity level */
  int verbosity;
  /* target environment (posix, linux...) */
  cplr_target_t target;
  /* output filename (memory if NULL) */
  char *out;

  /* complete argument count */
  int    argc;
  /* complete argument vector */
  char **argv;
  /* index of first program argument */
  int    argp;

  /* index of this state in loop mode */
  int lindex;
  /* previous instance in loop mode */
  cplr_t *lprev;
  /* next instance in loop mode */
  cplr_t *lnext;

  /* compiler state */
  TCCState *tcc;
  /* flag set on first compiler condition */
  bool tccerr;

  /* code generation state */
  cplr_gstate_t g_state;
  /* state for line numbering */
  int   g_prevline;
  char *g_prevfile;
  /* output streams */
  FILE *g_code;
  FILE *g_dump;
  /* stream buffers in case of memory streams */
  char *g_codebuf;
  char *g_dumpbuf;

  /* the piles */
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
  lh_t tlds;
  lh_t tlfs;
  lh_t stms;
  lh_t befs;
  lh_t afts;
};

/* cplr_state.c - piler state management */
extern cplr_t *cplr_new(void);
extern void cplr_free(cplr_t *c);
extern cplr_t *cplr_clone(cplr_t *c);

/* cplr_optparse.c - command-line option parser */
extern int cplr_optparse(cplr_t *c, int argc, char **argv);

extern int cplr_defaults(cplr_t *c);
extern int cplr_prepare(cplr_t *c);
extern int cplr_generate(cplr_t *c);
extern int cplr_compile(cplr_t *c);
extern int cplr_execute(cplr_t *c);

/* cplr_run.c -  prepare, generate, execute */
extern cplr_t *cplr_run(cplr_t *c);

/* cplr_interactive.c - c interactor */
extern cplr_t *cplr_interact(cplr_t *c);

/* cplr_command.c - command handling */
extern cplr_t *cplr_command_batch(cplr_t *c, const char *line);
extern cplr_t *cplr_command_interactive(cplr_t *c, const char *line);

/* cplr_package.c - pkg-config routines */
extern int cplr_pkgconfig_prepare(cplr_t *c, const char *name);
extern int cplr_pkgconfig_add(cplr_t *c, const char *name, const char *args);

/* cplr_symbols.c - symbol management */
extern TCCState *cplr_find_syms(cplr_t *c);

#endif /* CPLR_PRIVATE_H */
