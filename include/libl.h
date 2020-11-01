#ifndef LIBL_H
#define LIBL_H

#include "libx.h"

typedef struct lh lh_t;
typedef struct ln ln_t;

struct lh {
  size_t c;
  ln_t *f;
  ln_t *l;
};

struct ln {
  lh_t *h;
  ln_t *n;
  ln_t *p;
  union {
    char *s;
  } v;
};

bool l_empty(lh_t *lh) {
  return lh->c == 0;
}

size_t l_size(lh_t *lh) {
  return lh->c;
}

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

void l_appends(lh_t *lh, char *s) {
  ln_t *n = xcalloc(sizeof(ln_t), 1);
  /* initialize */
  n->v.s = s;
  /* append */
  l_append(lh, n);
}

#define L_FORWARD(_lhp, _i) \
  for((_i) = ((_lhp)->f); (_i); (_i) = (_i)->n)

#define L_BACKWARDS(_lhp, _i) \
  for((_i) = ((_lhp)->l); (_i); (_i) = (_i)->p)

#endif /* !LIBL_H */
