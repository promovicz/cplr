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

cplr_t *cplr_interact(cplr_t *c) {
  cplr_t *new;
  char *line;
  char *prompt = "> ";

  l_clear(&c->srcs);
  l_clear(&c->tlfs);
  l_clear(&c->stms);
  l_clear(&c->befs);
  l_clear(&c->afts);

  new = cplr_clone(c);

  /* chain the states */
  c->lnext = new;
  new->lindex = c->lindex + 1;
  new->lprev = c;

  while((line = readline(prompt))) {
    if(!strlen(line)) {
      continue;
    }

    add_history(line);

    if(line[0] == '^') {
      l_append_str_owned(&new->tlfs, strdup(line+1));
      continue;
    }
    if(line[0] == '<') {
      l_append_str_owned(&new->befs, strdup(line+1));
      continue;
    }
    if(line[0] == '>') {
      l_append_str_owned(&new->afts, strdup(line+1));
      continue;
    }
    if(line[0] == '.') {
      l_append_str_owned(&new->stms, strdup(line+1));
      continue;
    }

    l_append_str_owned(&new->stms, strdup(line));

    break;
  }

  if(!line) {
    cplr_free(new);
    return NULL;
  }

  return new;
}
