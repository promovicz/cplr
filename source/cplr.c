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

#define _POSIX_C_SOURCE 200809L

#define USE_GETOPT_LONG

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cext/memory.h"
#include "cext/list.h"
#include "cext/string.h"

#include "cpkg.c"

#include "cplr_private.h"

#include "cplr_misc.c"
#include "cplr_optparse.c"
#include "cplr_defaults.c"
#include "cplr_package.c"
#include "cplr_prepare.c"
#include "cplr_generate.c"
#include "cplr_compile.c"
#include "cplr_execute.c"

int main(int argc, char **argv) {
  int res;
  cplr_t c;
  memset(&c, 0, sizeof(c));

  res = cplr_optparse(&c, argc, argv);
  switch(res) {
  case 0:
    break;
  case 2:
    return 0;
  default:
    return 1;
  }

  if(!(c.flag & CPLR_FLAG_PRISTINE)) {
    if(cplr_defaults(&c)) {
      fprintf(stderr, "Error: Default initialization failed.\n");
      return 1;
    }
  }

  if(cplr_prepare(&c)) {
    fprintf(stderr, "Error: Prepare failed.\n");
    return 1;
  }

  if(cplr_generate(&c)) {
    fprintf(stderr, "Error: Code generation failed.\n");
    return 1;
  }

  if(cplr_compile(&c)) {
    fprintf(stderr, "Error: Compilation failed.\n");
    return 1;
  }

  if(c.flag & (CPLR_FLAG_DUMP | CPLR_FLAG_VERBOSE)) {
    fprintf(stderr, "Compilation succeeded.\n");
  }

  if(!(c.flag & CPLR_FLAG_NORUN)) {
    res = cplr_execute(&c);
    if(res) {
      return res;
    }
  }

  return 0;
}
