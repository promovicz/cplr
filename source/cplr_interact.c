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
  cplr_t *n = c;
  char *line;
  char *prompt = "> ";
  bool backslash;

  /* command loop */
  while((line = readline(prompt))) {
    if(!strlen(line)) {
      continue;
    }

    /* all lines end up in history */
    add_history(line);

    /* run the command */
    n = cplr_command_interactive(c, line);

    /* finish on zero return */
    if(!n) {
      return c;
    }

    c = n;

  }

  /* return the new state */
  return n;
}
