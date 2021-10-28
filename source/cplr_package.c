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

#include "cpkg.h"

#include "cplr.h"

int cplr_prepare_package(cplr_t *c, const char *name) {
    char *s;
    bool verbose = (c->verbosity >= 2);
    TCCState *t = c->tcc;

    if(!cpkg_exists(name, verbose)) {
      fprintf(stderr, "Error: Could not find package %s\n", name);
      return 1;
    }

    s = cpkg_retrieve(name, "--cflags --libs", verbose);
    if(!s) {
      fprintf(stderr, "Error: Could not get options for package %s\n", name);
      return 1;
    }
    if(verbose)
      fprintf(stderr, "Package definitions for %s: %s\n", name, s);
    tcc_set_options(t, s);
    if(cplr_add_package(c, name, s)) {
      fprintf(stderr, "Error: Failed to process package %s.\n", name);
      return 1;
    }
    free(s);

    if(c->verbosity >= 1) {
      fprintf(stderr, "Package %s found\n", name);
    }

    return 0;
}

int cplr_add_package(cplr_t *c, const char *name, const char *args) {
  bool dash = false;
  char opt = 0;
  char *o = (char*)args, *b, *s;
  do {
    char n = *o;
    if(!n) {
      break;
    }
    if(opt) {
      b = o;
      while(*o && isgraph(*o)) o++;
      s = strndup(b, o - b);
      switch(opt) {
      case 'D':
      case 'U':
	l_append_str_owned(&c->defs, msprintf("-%c%s", opt, s));
	xfree(s);
	if(c->verbosity >= 2)
	  fprintf(stderr, "Package %s define: -%c%s\n", name, opt, s);
	break;
      case 'I':
	l_append_str_owned(&c->incdirs, s);
	if(c->verbosity >= 2)
	  fprintf(stderr, "Package %s include dir: %s\n", name, s);
	break;
      case 'L':
	l_append_str_owned(&c->libdirs, s);
	if(c->verbosity >= 2)
	  fprintf(stderr, "Package %s library dir: %s\n", name, s);
	break;
      case 'l':
	l_append_str_owned(&c->libs, s);
	if(c->verbosity >= 2)
	  fprintf(stderr, "Package %s library: %s\n", name, s);
	break;
      default:
	xfree(s);
	break;
      }
      opt = 0;
    } else if(dash) {
      switch(n) {
      case 'D':
      case 'I':
      case 'L':
      case 'l':
	opt = n;
	while(*o && isspace(*o)) o++;
	break;
      case 'p':
	if(strstr(o, "pthread") == o) {
	  if(c->verbosity >= 2)
	    fprintf(stderr, "Package %s uses pthreads.\n", name);
	  tcc_set_options(c->tcc, "-pthread");
	  o += 6;
	  break;
	}
	/* fall through */
      default:
	fprintf(stderr, "Warning: unhandled option -%c in package %s: %s\n", n, name, o-1);
	while(*o && isgraph(*o)) o++;
	//return 1;
      }
      dash = false;
    } else {
      if(n == '-') {
	dash = true;
      } else if(isspace(n)) {
	continue;
      } else {
	fprintf(stderr, "Invalid string in package %s: %s\n", name, o);
	return 1;
      }
    }
  } while(*o++);
  return 0;
}
