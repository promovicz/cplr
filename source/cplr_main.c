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
 * Use the power of ~/.local/bin and enjoy it's freedoms.
 */

#include <stdio.h>
#include <unistd.h>

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

  /* switch to interactive when no statement and on a tty */
  if(l_empty(&c->stms) && (isatty(0) == 1) && (isatty(1) == 1)) {
    c->flag |= CPLR_FLAG_INTERACTIVE;
    c->flag |= CPLR_FLAG_LOOP;
  }

  /* main loop */
  do {
    /* assume failure */
    ret = 1;

    /* prepare compilation */
    if(cplr_prepare(c)) {
      fprintf(stderr, "Error: Prepare failed.\n");
      goto stepback;
    }

    /* generate code */
    if(cplr_generate(c)) {
      fprintf(stderr, "Error: Code generation failed.\n");
      goto stepback;
    }

    /* perform compilation */
    if(cplr_compile(c)) {
      fprintf(stderr, "Error: Compilation failed.\n");
      goto stepback;
    }

    /* execute code */
    if(!(c->flag & CPLR_FLAG_NORUN)) {
      /* perform execution */
      ret = cplr_execute(c);
      if(ret) {
        if(c->flag & CPLR_FLAG_INTERACTIVE) {
          fprintf(stderr, "Program returned %d.\n", ret);
        }
        /* preserving ret */
        goto next;
      }
    }

    /* run has succeeded */
    ret = 0;

    goto next;

  stepback:
    /* XXX temporary fix for symbol chaining until we implement symbol search !?°^%& */
    if(c->lprev) {
      c = c->lprev;
    }

  next:

    if(c->flag & CPLR_FLAG_INTERACTIVE) {
      /* start interacting after initial run */
      cplr_t *new = cplr_interact(c);
      if(!new) {
        fprintf(stderr, "\n");
        goto done;
      }
      c = new;
    } else {
      /* in non-interactive mode errors are terminal */
      if(ret) {
        goto done;
      }
    }

    /* fall through to next iteration */
  } while(c->flag & CPLR_FLAG_LOOP);

  /* we get here when execution is finished */
 done:

  /* clean up */
  cplr_free(c);

  /* return result */
  return ret;
}
