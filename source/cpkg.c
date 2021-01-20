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

#include <stdio.h>
#include <stdlib.h>

#include <cext/memory.h>
#include <cext/string.h>

#include "cpkg.h"

bool cpkg_exists(const char *name, bool verbose) {
  int res;
  bool ret = false;
  char *cmd = msprintf("pkg-config --exists %s", name);
  if(verbose) {
    fprintf(stderr, "Running \"%s\"\n", cmd);
  }
  res = system(cmd);
  if(res == -1 || res == 127) {
    if(verbose) {
      fprintf(stderr, "Error: Could not execute \"%s\"\n", cmd);
    }
  } else if(res) {
    if(verbose) {
      fprintf(stderr, "Error: Package %s not present\n", name);
    }
  } else {
    if(verbose) {
      fprintf(stderr, "Package %s found\n", name);
    }
    ret = true;
  }
  xfree(cmd);
  return ret;
}

char *cpkg_retrieve(const char *name, const char *what, bool verbose) {
  int res;
  FILE *ps;
  char *cmd;
  char rbuf[1024];
  cmd = msprintf("pkg-config %s %s", what, name);
  if(verbose) {
    fprintf(stderr, "Running \"%s\"\n", cmd);
  }
  ps = popen(cmd, "r");
  if(!ps) {
    if(verbose) {
      fprintf(stderr, "Error: Could not popen \"%s\"\n", cmd);
    }
    goto err;
  }
  res = fread(rbuf, 1, sizeof(rbuf), ps);
  if(res < 0) {
    if(verbose) {
      fprintf(stderr, "Error: Failed to read from \"%s\"\n", cmd);
    }
    goto err;
  }
  if(res == sizeof(rbuf)) {
    if(verbose) {
      fprintf(stderr, "Error: Package options for %s are to long.\n", name);
    }
    goto err;
  }
  rbuf[res] = 0;
  if(rbuf[res - 1] == '\n')
    rbuf[res - 1] = 0;
  pclose(ps);
  xfree(cmd);
  return strdup(rbuf);
 err:
  xfree(cmd);
  return NULL;
}
