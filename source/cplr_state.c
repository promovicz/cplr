
#include "cext/memory.h"

#include "cplr.h"

cplr_t *cplr_new(void) {
  cplr_t *res = xcalloc(sizeof(cplr_t), 1);
  return res;
}

void cplr_free(cplr_t *c) {
}
