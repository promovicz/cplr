
#include "cext/list.h"

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
void l_appends(lh_t *lh, char *s) {
  ln_t *n = xcalloc(sizeof(ln_t), 1);
  /* initialize */
  n->v.s = s;
  /* append */
  l_append(lh, n);
}
