#ifndef LIBL_H
#define LIBL_H

#include "libx.h"

typedef struct lh lh_t;
typedef struct ln ln_t;

struct lh {
  ln_t *f;
  ln_t *l;
};

struct ln {
  lh_t *h;
  ln_t *n;
  union {
    char *s;
  } v;
};

bool l_empty(lh_t *lh) {
  return lh->f == NULL;
}

void l_appends(lh_t *lh, char *s) {
  ln_t *n = xcalloc(sizeof(ln_t), 1);
  n->h = lh;
  n->v.s = s;
  if(lh->l) {
    lh->l->n = n;
  }
  lh->l = n;
  if(!lh->f) {
    lh->f = n;
  }
}

#define L_FOREACH(l, i) \
  for((i) = ((l)->f); (i); (i) = (i)->n)

#endif /* !LIBL_H */
