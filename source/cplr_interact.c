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

#include <readline/readline.h>
#include <readline/history.h>

int cplr_interact(cplr_t *b) {
  int ret = 0;
  cplr_t *c = b;
  char *line;
  char *prompt = "> ";
  bool backslash;

  /* command loop */
  while((line = readline(prompt))) {
    /* create a new state when needed */
    if(c->flag & CPLR_FLAG_LOADED) {
      c = cplr_chain(c);
    }

    /* ignore empty lines */
    if(!strlen(line)) {
      continue;
    }

    /* add to readline history */
    add_history(line);

    /* run the command */
    ret = cplr_command(c, line);
    if(ret != 0) {
      /* fix return code */
      if(ret >= 1) {
        ret -= 1;
      }
      goto out;
    }
  }

 out:

  /* free the chain */
  while(c != b) {
    cplr_t *p = c->c_prev;
    cplr_free(c);
    c = p;
  }

  return ret;
}
