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

static void cplr_defaults_posix(cplr_t *c);
static void cplr_defaults_linux(cplr_t *c);

int cplr_defaults(cplr_t *c) {
  if(c->target & CPLR_TARGET_POSIX) {
    cplr_defaults_posix(c);
  }
  if(c->target & CPLR_TARGET_LINUX) {
    cplr_defaults_linux(c);
  }
  return 0;
}

static void cplr_defaults_posix(cplr_t *c) {
  l_append_str_static(&c->defdef, "-D_DEFAULT_SOURCE");
  l_append_str_static(&c->defdef, "-D_XOPEN_SOURCE=600");
  l_append_str_static(&c->defdef, "-D_POSIX_SOURCE=200809L");
  l_append_str_static(&c->defsys, "sys/stat.h");
  l_append_str_static(&c->defsys, "sys/types.h");
  l_append_str_static(&c->defsys, "assert.h");
  l_append_str_static(&c->defsys, "ctype.h");
  l_append_str_static(&c->defsys, "errno.h");
  l_append_str_static(&c->defsys, "fcntl.h");
  l_append_str_static(&c->defsys, "libgen.h");
  l_append_str_static(&c->defsys, "limits.h");
  l_append_str_static(&c->defsys, "locale.h");
  l_append_str_static(&c->defsys, "stdarg.h");
  l_append_str_static(&c->defsys, "stdio.h");
  l_append_str_static(&c->defsys, "stdint.h");
  l_append_str_static(&c->defsys, "stdlib.h");
  l_append_str_static(&c->defsys, "string.h");
  l_append_str_static(&c->defsys, "termios.h");
  l_append_str_static(&c->defsys, "time.h");
  l_append_str_static(&c->defsys, "unistd.h");
  l_append_str_static(&c->defsys, "wchar.h");
}

static void cplr_defaults_linux(cplr_t *c) {
}
