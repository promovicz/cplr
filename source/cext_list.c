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

#include "cext/list.h"

#include "cext/memory.h"

ATTR_ARG_NONNULL(1)
void l_clear(lh_t *lh) {
  ln_t *n, *next;
  /* clear and free nodes */
  n = lh->f;
  while(n) {
    next = n->n;
    value_clear(&n->v);
    xfree(n);
    n = next;
  }
  /* clear head */
  lh->c = 0;
  lh->f = NULL;
  lh->l = NULL;
}


ATTR_ARG_NONNULL(1,2)
void l_append(lh_t *lh, ln_t *n) {
  /* chain */
  n->h = lh;
  if(lh->l) {
    n->p = lh->l;
    lh->l->n = n;
  }
  /* header first */
  if(!lh->f) {
    lh->f = n;
  }
  /* header last */
  lh->l = n;
  /* header count */
  lh->c++;
}

ATTR_ARG_NONNULL(1,2)
void l_append_str(lh_t *lh, char *s) {
  ln_t *n = xcalloc(1, sizeof(ln_t));
  value_set_str(&n->v, s);
  l_append(lh, n);
}

ATTR_ARG_NONNULL(1,2)
void l_append_str_owned(lh_t *lh, char *s) {
  ln_t *n = xcalloc(1, sizeof(ln_t));
  value_set_str_owned(&n->v, s);
  l_append(lh, n);
}

ATTR_ARG_NONNULL(1,2)
void l_append_str_static(lh_t *lh, const char *s) {
  ln_t *n = xcalloc(sizeof(ln_t), 1);
  value_set_str_static(&n->v, s);
  l_append(lh, n);
}
