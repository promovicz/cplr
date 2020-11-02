
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
  ln_t *n = xcalloc(sizeof(ln_t), 1);
  value_set_str(&n->v, s);
  l_append(lh, n);
}

ATTR_ARG_NONNULL(1,2)
void l_append_str_owned(lh_t *lh, char *s) {
  ln_t *n = xcalloc(sizeof(ln_t), 1);
  value_set_str_owned(&n->v, s);
  l_append(lh, n);
}

ATTR_ARG_NONNULL(1,2)
void l_append_str_static(lh_t *lh, const char *s) {
  ln_t *n = xcalloc(sizeof(ln_t), 1);
  value_set_str_static(&n->v, s);
  l_append(lh, n);
}
