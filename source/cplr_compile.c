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

int cplr_compile(cplr_t *c) {
  /* report status */
  if(c->flag & CPLR_FLAG_VERBOSE) {
    fprintf(stderr, "Compilation phase\n");
  }
  /* compile the code */
  if(tcc_compile_string(c->tcc, c->g_codebuf)) {
    return 1;
  }
  /* produce output if requested */
  if(c->out != NULL) {
    if(tcc_output_file(c->tcc, c->out)) {
      fprintf(stderr, "Failed to output file %s\n", c->out);
      return 1;
    }
  }
  /* report success */
  if(c->flag & CPLR_FLAG_VERBOSE) {
    fprintf(stderr, "Compilation succeeded\n");
  }
  /* done */
  return 0;
}
