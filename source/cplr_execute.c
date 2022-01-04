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

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

int cplr_tcc_execute(cplr_t *c, int argc, char **argv) {
  int ret;
  /* run the program */
  ret = tcc_run(c->tcc, argc, argv);
  /* free arguments */
  xfree(argv);
  /* return */
  return ret;
}

int cplr_execute(cplr_t *c) {
  int i, rc, ret = 1;
  int argc; char **argv;
  pid_t p = 0, rp;

  /* report status */
  if(c->verbosity >= 1) {
    fprintf(stderr, "Execution phase\n");
  }

  /* copy arguments */
  argc = 1 + (c->argc - c->argp);
  argv = xcalloc(sizeof(char*), argc+1);
  argv[0] = "c";
  for(i = 1; i < argc; i++) {
    argv[i] = c->argv[c->argp + (i - 1)];
  }

  /* set context as executed */
  c->flag |= CPLR_FLAG_EXECUTED;

  /* fork if requested */
  if(c->flag & CPLR_FLAG_FORK) {
    if(c->verbosity >= 1) {
      fprintf(stderr, "Forking for execution\n");
    }
    /* do the fork */
    p = fork();
    if(p < 0) {
      fprintf(stderr, "Failed to fork\n");
      goto out;
    }
    if(p != 0) {
      /* parent waits for child */
      do {
        rp = waitpid(p, &rc, 0);
        if(rp < 0 && errno != EAGAIN) {
          fprintf(stderr, "Failed to wait for child\n");
          goto out;
        }
      } while(rp != p);
      /* use the childs return code */
      ret = rc;
      if(c->verbosity >= 1) {
        fprintf(stderr, "Child has returned\n");
      }
      /* done */
      goto out;
    }
    /* we are the child - continue executing */
  }

  /* call backend method */
  switch(c->backend) {
  case CPLR_BACKEND_LIBTCC:
    ret = cplr_tcc_execute(c, argc, argv);
    break;
  }

  /* exit the fork quickly */
  if(c->flag & CPLR_FLAG_FORK && p == 0) {
    exit(ret);
  }

  /* we are finished */
  c->flag |= CPLR_FLAG_FINISHED;

  /* report */
  if(c->verbosity >= 1) {
    fprintf(stderr, "Execution finished (ret=%d)\n", ret);
  }

 out:

  /* free arguents */
  xfree(argv);

  return ret;
}
