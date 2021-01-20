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
   CPLR_FLAG_DUMP = 1,
   CPLR_FLAG_NORUN = 2,
   CPLR_FLAG_VERBOSE = 4,
   CPLR_FLAG_PRISTINE = 8,
} cplr_flag_t;

/* target environment */
typedef enum {
   CPLR_ENV_NONE = 0,
   CPLR_ENV_POSIX = 1,
   CPLR_ENV_LINUX = (2 | CPLR_ENV_POSIX),
   CPLR_ENV_LINUX_KERNEL = 4,
} cplr_env_t;

/* types of main option */
typedef enum {
  CPLR_MAINOPT_STATEMENT = 0,
  CPLR_MAINOPT_BEFORE = 1,
  CPLR_MAINOPT_AFTER = 2,
  CPLR_MAINOPT_TOPLEVEL = 3,
  CPLR_MAINOPT_FILE = 4,
} cplr_mainopt_t;

/* code generation phases */
typedef enum {
   CPLR_GSTATE_INITIAL = 0,
   CPLR_GSTATE_COMMENT = 1,
   CPLR_GSTATE_INTERNAL = 2,
   CPLR_GSTATE_PREPROC = 3,
   CPLR_GSTATE_TOPLEVEL = 4,
   CPLR_GSTATE_STATEMENT = 5,
} cplr_gstate_t;

typedef struct cplr {
  /* all arguments */
  int    argc;
  char **argv;
  /* first program argument */
  int    argp;
  /* current main option type */
  cplr_mainopt_t argt;

    /* flags */
  cplr_flag_t flag;
  /* target environment (posix, linux...) */
  cplr_env_t t_env;

  /* compiler state */
  TCCState *tcc;

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

} cplr_t;

extern cplr_t *cplr_new(void);
extern void cplr_free(cplr_t *c);

extern int cplr_optparse(cplr_t *c, int argc, char **argv);
extern int cplr_defaults(cplr_t *c);
extern int cplr_prepare(cplr_t *c);
extern int cplr_generate(cplr_t *c);
extern int cplr_compile(cplr_t *c);
extern int cplr_execute(cplr_t *c);

extern int cplr_prepare_package(cplr_t *c, const char *name);
extern int cplr_add_package(cplr_t *c, const char *name, const char *args);

#endif /* CPLR_PRIVATE_H */
