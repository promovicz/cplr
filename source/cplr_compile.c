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

#include <cext/string.h>

static int cplr_cc_compile(cplr_t *c) {
  return 0;
}

static void cplr_tcc_redefsym_cb(void *ctx, const char *name, const void *val) {
  cplr_t *c = (cplr_t *)ctx;
  /* ignore various built-ins (FIXME better method?) */
  if(name[0] == '_') {
    return;
  }
  if(strcmp(name, "main") == 0) {
    return;
  }
  if(strcmp(name, "exit") == 0) {
    return;
  }
  if(strcmp(name, "atexit") == 0) {
    return;
  }
  if(strcmp(name, "on_exit") == 0) {
    return;
  }
  if(strprefix(name, "tcc_")) {
    return;
  }
  if(strsuffix(name, "@plt")) {
    return;
  }
  /* log for debugging */
  if(c->verbosity >= 3) {
    fprintf(stderr, "Redefining symbol %s as %p\n", name, val);
  }
  /* define the symbol */
  tcc_add_symbol(c->tcc, name, val);
}

static int cplr_tcc_compile(cplr_t *c) {
  /* compile the code */
  if(tcc_compile_string(c->tcc, c->g_codebuf)) {
    return 1;
  }
  /* compilation was successful */
  c->flag |= CPLR_FLAG_COMPILED;
  /* if chained, redefine symbols from previous context */
  if(c->c_prev && c->c_prev->tcc) {
    if(c->verbosity >= 3) {
      fprintf(stderr, "Redefining symbols\n");
    }
    tcc_list_symbols(c->c_prev->tcc, c, &cplr_tcc_redefsym_cb);
  }
  /* produce compilation output */
  if(c->out == NULL) {
    /* this was a memory compilation, so mark state as loaded */
    c->flag |= CPLR_FLAG_LOADED;
  } else {
    /* produce an object file or executable */
    if(tcc_output_file(c->tcc, c->out)) {
      fprintf(stderr, "Failed to output file %s\n", c->out);
      return 1;
    }
  }
  /* done */
  return 0;
}

int cplr_compile(cplr_t *c) {
  int ret = 1;
  /* report status */
  if(c->verbosity >= 1) {
    fprintf(stderr, "Compilation phase\n");
  }
  /* run backend method */
  switch(c->backend) {
  case CPLR_BACKEND_CC:
    ret = cplr_cc_compile(c);
    break;
  case CPLR_BACKEND_LIBTCC:
    ret = cplr_tcc_compile(c);
    break;
  }
  /* report */
  if(ret != 0) {
    fprintf(stderr, "Compilation failed\n");
  } else {
    if(c->verbosity >= 1) {
      fprintf(stderr, "Compilation succeeded\n");
    }
  }
  /* done */
  return ret;
}
