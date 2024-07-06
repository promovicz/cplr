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

static void cplr_tcc_error(void *o, const char *msg) {
  cplr_t *c = (cplr_t*)o;
  char *clone = strdup(msg);
  char *cur, *save;
  if(!c->tccerr) {
    c->tccerr = true;
    fprintf(stderr, "Compiler condition:\n");
  }
  cur = strtok_r(clone, "\n", &save);
  do {
    fprintf(stderr, "  %s\n", cur);
  } while((cur = strtok_r(NULL, "\n", &save)));
  cext_free(clone);
}

static int cplr_tcc_prepare_libdir(cplr_t *c) {
  const char *libdir = NULL;
  /* check the environment */
  libdir = getenv("CPLR_TINYCC_LIBDIR");
  /* default to our custom directory */
#ifndef CPLR_TINYCC_EXTERNAL
  if(!libdir) {
    libdir = CPLR_TINYCC_LIBDIR;
  }
#endif
  /* give the path to tcc - owned by it */
  if(libdir) {
    tcc_set_lib_path(c->tcc, libdir);
  }
}

static int cplr_tcc_prepare(cplr_t *c) {
  int ret = 1;
  TCCState *t;
  int otype;
  ln_t *i;

  /* new compiler */
  t = tcc_new();
  if(!t) {
    fprintf(stderr, "Failed to create compiler instance\n");
  }
  c->tcc = t;

  /* set up libdir */
  cplr_tcc_prepare_libdir(c);

  /* error handling */
  tcc_set_error_func(t, c, cplr_tcc_error);

  /* set output type */
  if(c->out == NULL) {
    otype = TCC_OUTPUT_MEMORY;
  } else {
    otype = TCC_OUTPUT_EXE;
  }
  if(tcc_set_output_type(c->tcc, otype)) {
    fprintf(stderr, "Failed to set output type %d\n", otype);
    goto out;
  }

  /* basic options */
  tcc_set_options(t, "-Wall");
  //tcc_set_options(t, "-g");
  //tcc_set_options(t, "-bt");

  /* packages */
  L_FORWARD(&c->pkgs, i) {
    if(cplr_pkgconfig_prepare(c, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to prepare package %s\n", value_get_str(&i->v));
      goto out;
    }
  }

  /* cpp defines */
  L_FORWARD(&c->defdef, i) {
    tcc_set_options(t, value_get_str(&i->v));
  }

  /* cpp undefines */
  L_FORWARD(&c->defs, i) {
    tcc_set_options(t, value_get_str(&i->v));
  }

  /* sysinclude dirs */
  L_FORWARD(&c->sysdirs, i) {
    if(tcc_add_sysinclude_path(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add sysinclude path %s\n", value_get_str(&i->v));
      goto out;
    }
  }

  /* include dirs */
  L_FORWARD(&c->incdirs, i) {
    if(tcc_add_include_path(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add include path %s\n", value_get_str(&i->v));
      goto out;
    }
  }
  if(tcc_add_include_path(t, ".")) {
    fprintf(stderr, "Failed to add include path .\n");
    goto out;
  }

  /* library dirs */
  L_FORWARD(&c->libdirs, i) {
    if(tcc_add_library_path(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add library path %s\n", value_get_str(&i->v));
      goto out;
    }
  }
  if(tcc_add_library_path(t, ".")) {
    fprintf(stderr, "Failed to add library path .\n");
    goto out;
  }

  /* libraries */
  L_FORWARD(&c->libs, i) {
    if(tcc_add_library(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add library %s\n", value_get_str(&i->v));
      goto out;
    }
  }

  /* source files */
  L_FORWARD(&c->srcs, i) {
    if(tcc_add_file(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add file %s\n", value_get_str(&i->v));
      goto out;
    }
  }

  /* done */
  ret = 0;

 out:

  return ret;
}

int cplr_prepare(cplr_t *c) {
  int ret = 1;

  /* report */
  if(c->verbosity >= 1) {
    fprintf(stderr, "Preparation phase\n");
  }

  /* call backend method */
  switch(c->backend) {
  case CPLR_BACKEND_LIBTCC:
    ret = cplr_tcc_prepare(c);
    break;
  }

  /* flag the context */
  c->flag |= CPLR_FLAG_PREPARED;

  /* done */
  return ret;
}
