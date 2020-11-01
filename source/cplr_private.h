#ifndef CPLR_PRIVATE_H
#define CPLR_PRIVATE_H

#include <stdio.h>

#include <libtcc.h>

#include <cext/list.h>

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
   CPLR_GSTATE_PREPROC = 3,
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
  lh_t mlbdirs;
  lh_t libdirs;
  lh_t defs;
  lh_t syss;
  lh_t incs;
  lh_t mlbs;
  lh_t libs;
  lh_t pkgs;
  lh_t srcs;
  lh_t tlfs;
  lh_t stms;
  lh_t befs;
  lh_t afts;

  cplr_gstate_t g_state;
  int   g_prevline;
  char *g_prevfile;
  char *g_codebuf;
  FILE *g_code;
  FILE *g_dump;

  TCCState *tcc;
} cplr_t;

#endif /* CPLR_PRIVATE_H */
