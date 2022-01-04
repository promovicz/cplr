
#include "cplr.h"

int cplr_run(cplr_t *c) {
  int res, ret = 1;
  cplr_t *n;

  /* prepare compilation */
  if(cplr_prepare(c)) {
    fprintf(stderr, "Error: Prepare failed.\n");
    goto out;
  }

  /* generate code */
  if(cplr_generate(c)) {
    fprintf(stderr, "Error: Code generation failed.\n");
    goto out;
  }

  /* perform compilation */
  if(cplr_compile(c)) {
    fprintf(stderr, "Error: Compilation failed.\n");
    goto out;
  }

  /* expect success */
  ret = 0;

  /* execute code */
  if(!(c->flag & CPLR_FLAG_NORUN)) {
    /* perform execution */
    res = cplr_execute(c);
    if(res) {
        if(c->flag & CPLR_FLAG_INTERACTIVE) {
          fprintf(stderr, "Program returned %d.\n", res);
        }
        ret = res;
    }
  }

  /* return */
 out:
  return ret;
}
