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

#include "cext/memory.h"

#include "cplr.h"

cplr_t *cplr_new(void) {
  cplr_t *res = xcalloc(sizeof(cplr_t), 1);

  res->t_env = CPLR_ENV_POSIX;

  return res;
}

void cplr_free(cplr_t *c) {
  if(c->tcc) {
    tcc_delete(c->tcc);
    c->tcc = NULL;
  }
  lptrfree((void*)&c->g_codebuf);
  lptrfree((void*)&c->g_dumpbuf);
  l_clear(&c->defdef);
  l_clear(&c->defsys);
  l_clear(&c->sysdirs);
  l_clear(&c->incdirs);
  l_clear(&c->mlbdirs);
  l_clear(&c->libdirs);
  l_clear(&c->defs);
  l_clear(&c->syss);
  l_clear(&c->incs);
  l_clear(&c->mlbs);
  l_clear(&c->libs);
  l_clear(&c->pkgs);
  l_clear(&c->srcs);
  l_clear(&c->tlfs);
  l_clear(&c->stms);
  l_clear(&c->befs);
  l_clear(&c->afts);
  xfree(c);
}
