#ifndef CPLR_LIST_H
#define CPLR_LIST_H

#include "attr.h"
#include "value.h"

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
  value_t v;
};

ATTR_ARG_NONNULL(1)
static inline void l_init(lh_t *lh) {
  lh->c = 0; lh->f = NULL; lh->l = NULL;
}

ATTR_FUN_PURE
ATTR_ARG_NONNULL(1)
static inline bool l_empty(lh_t *lh) {
  return lh->c == 0;
}

ATTR_FUN_PURE
ATTR_ARG_NONNULL(1)
static inline size_t l_size(lh_t *lh) {
  return lh->c;
}

ATTR_ARG_NONNULL(1)
extern void l_clear(lh_t *lh);

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
