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

/*
 * NOTICE
 *
 * This program may embody magical lowlevel programming powers
 * that may be illegal to possess in your domain or directory
 * of residence.
 *
 * My explicit advice is to use it anyway.
 *
 * Use the power of ~/.local/bin and enjoy its freedoms.
 */

#include <stdio.h>
#include <unistd.h>
#include <readline/history.h>

#include "cplr.h"
#include "cgc.h"

int main(int argc, char **argv) {
  int res, ret = 1;
  cplr_t *c;

  /* initialize garbage collector */
#ifdef CPLR_USE_GC
  cgc_init();
#endif

  /* create the root piler */
  c = cplr_new();

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
  if(!(c->flag & CPLR_FLAG_NODEFAULTS)) {
    if(cplr_defaults(c)) {
      fprintf(stderr, "Error: Default initialization failed.\n");
      goto done;
    }
  }

  /* switch to interactive when no statement and on a tty */
  if(l_empty(&c->stms) && (isatty(0) == 1) && (isatty(1) == 1)) {
    c->flag |= CPLR_FLAG_INTERACTIVE;
    /* read history */
    if(c->verbosity >= 3) {
      fprintf(stderr, "Reading history\n");
    }
    read_history(realpath("~/.cplr_history", NULL));
  }

  /* run code on the command line */
  ret = cplr_run(c);
  if(ret) {
    goto done;
  }

  /* run the interactor */
  if(c->flag & CPLR_FLAG_INTERACTIVE) {
    ret = cplr_interact(c);
  }

  /* we get here when execution is finished */
 done:

  /* save history */
  if(c->flag & CPLR_FLAG_INTERACTIVE) {
    if(c->verbosity >= 3) {
      fprintf(stderr, "Writing history\n");
    }
    write_history(realpath("~/.cplr_history", NULL));
  }

  /* clean up */
  cplr_free(c);

  /* return result */
  return ret;
}
