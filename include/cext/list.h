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

#ifndef CPLR_LIST_H
#define CPLR_LIST_H

#include "attr.h"
#include "value.h"

/* list head */
typedef struct lh lh_t;
/* list node */
typedef struct ln ln_t;

/* structure of list head */
struct lh {
  size_t c;
  ln_t *f;
  ln_t *l;
};

/* strcture of list node */
struct ln {
  lh_t *h;
  ln_t *n;
  ln_t *p;
  value_t v;
};

/* initialize list as empty */
ATTR_ARG_NONNULL(1)
static inline void l_init(lh_t *lh) {
  lh->c = 0; lh->f = NULL; lh->l = NULL;
}

/* check if list is empty */
ATTR_FUN_PURE
ATTR_ARG_NONNULL(1)
static inline bool l_empty(lh_t *lh) {
  return lh->c == 0;
}

/* get length of list */
ATTR_FUN_PURE
ATTR_ARG_NONNULL(1)
static inline size_t l_size(lh_t *lh) {
  return lh->c;
}

/* clear the list */
ATTR_ARG_NONNULL(1)
extern void l_clear(lh_t *lh);

/* append a node to a list */
ATTR_ARG_NONNULL(1,2)
extern void l_append(lh_t *lh, ln_t *n);

ATTR_ARG_NONNULL(1,2)
extern void l_append_str(lh_t *lh, char *s);
ATTR_ARG_NONNULL(1,2)
extern void l_append_str_owned(lh_t *lh, char *s);
ATTR_ARG_NONNULL(1,2)
extern void l_append_str_static(lh_t *lh, const char *s);

#define L_FORWARD(_lhp, _i) \
  for((_i) = ((_lhp)->f); (_i); (_i) = (_i)->n)

#define L_BACKWARDS(_lhp, _i) \
  for((_i) = ((_lhp)->l); (_i); (_i) = (_i)->p)

#endif /* !CPLR_LIST_H */
