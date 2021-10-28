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

int cplr_execute(cplr_t *c) {
  int i, ret;
  /* report status */
  if(c->verbosity >= 1) {
    fprintf(stderr, "Execution phase\n");
  }
  /* copy arguments */
  int argc = 1 + (c->argc - c->argp);
  char **argv = xcalloc(sizeof(char*), argc+1);
  argv[0] = "c";
  for(i = 1; i < argc; i++) {
    argv[i] = c->argv[c->argp + (i - 1)];
  }
  /* run the program */
  ret = tcc_run(c->tcc, argc, argv);
  /* free arguments */
  xfree(argv);
  /* report progress */
  if(c->verbosity >= 1) {
    fprintf(stderr, "Execution finished (ret=%d).\n", ret);
  }
  return ret;
}
