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

static void cplr_tcc_error(cplr_t *c, const char *msg) {
  fprintf(stderr, "Compiler condition:\n%s\n", msg);
  /* TODO split and indent */
}

int cplr_prepare(cplr_t *c) {
  ln_t *i;
  TCCState *t;

  /* new compiler */
  t = tcc_new();
  if(!t) {
    fprintf(stderr, "Failed to create compiler instance\n");
  }

  /* remember the compiler */
  c->tcc = t;

  /* error handling */
  tcc_set_error_func(t, c, (void (*)(void *, const char *))&cplr_tcc_error);

  /* basic options */
  tcc_set_options(t, "-Wall");
  //tcc_set_options(t, "-g");

  /* set output type */
  if(tcc_set_output_type(c->tcc, TCC_OUTPUT_MEMORY)) {
    return 1;
  }

  /* define minilib dirs as include dirs */
  L_FORWARD(&c->mlbdirs, i) {
    l_append_str(&c->incdirs, value_get_str(&i->v));
  }

  /* packages */
  L_FORWARD(&c->pkgs, i) {
    if(cplr_prepare_package(c, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to prepare package %s\n", value_get_str(&i->v));
      return 1;
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
      return 1;
    }
  }

  /* include dirs */
  L_FORWARD(&c->incdirs, i) {
    if(tcc_add_include_path(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add include path %s\n", value_get_str(&i->v));
      return 1;
    }
  }

  /* library dirs */
  L_FORWARD(&c->libdirs, i) {
    if(tcc_add_library_path(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add library path %s\n", value_get_str(&i->v));
      return 1;
    }
  }

  /* libraries */
  L_FORWARD(&c->libs, i) {
    if(tcc_add_library(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add library %s\n", value_get_str(&i->v));
      return 1;
    }
  }

  /* source files */
  L_FORWARD(&c->srcs, i) {
    if(tcc_add_file(t, value_get_str(&i->v))) {
      fprintf(stderr, "Failed to add file %s\n", value_get_str(&i->v));
      return 1;
    }
  }

  /* done */
  return 0;
}
