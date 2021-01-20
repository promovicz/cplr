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

#include <stdio.h>

#include "cplr.h"

int main(int argc, char **argv) {
  int res, ret = 1;
  cplr_t *c = cplr_new();

  /* parse options */
  res = cplr_optparse(c, argc, argv);
  switch(res) {
  case 0:
    break;
  case 2:
    ret = 0;
    goto done;
  default:
    goto done;
  }

  /* apply defaults */
  if(!(c->flag & CPLR_FLAG_PRISTINE)) {
    if(cplr_defaults(c)) {
      fprintf(stderr, "Error: Default initialization failed.\n");
      goto done;
    }
  }

  /* prepare compilation */
  if(cplr_prepare(c)) {
    fprintf(stderr, "Error: Prepare failed.\n");
    goto done;
  }

  /* generate code */
  if(cplr_generate(c)) {
    fprintf(stderr, "Error: Code generation failed.\n");
    goto done;
  }

  /* perform compilation */
  if(cplr_compile(c)) {
    fprintf(stderr, "Error: Compilation failed.\n");
    goto done;
  }

  /* report success */
  if(c->flag & (CPLR_FLAG_DUMP | CPLR_FLAG_VERBOSE)) {
    fprintf(stderr, "Compilation succeeded.\n");
  }

  /* execute code */
  if(!(c->flag & CPLR_FLAG_NORUN)) {
    ret = cplr_execute(c);
    if(ret) {
      ret = 1;
      goto done;
    }
  }

  /* report success */
  if(c->flag & (CPLR_FLAG_VERBOSE)) {
    fprintf(stderr, "Execution succeeded.\n");
  }

  /* overall success */
  ret = 0;

 done:

  /* clean up */
  cplr_free(c);

  /* return result */
  return ret;
}
