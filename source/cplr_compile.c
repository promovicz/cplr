
#include "cplr.h"

int cplr_compile(cplr_t *c) {

  if(tcc_compile_string(c->tcc, c->g_codebuf)) {
    return 1;
  }

  return 0;
}
