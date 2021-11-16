
#include "cplr.h"

cplr_t *cplr_evaluate(cplr_t *c) {
  int ret;
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

  /* execute code */
  if(!(c->flag & CPLR_FLAG_NORUN)) {
    /* perform execution */
    ret = cplr_execute(c);
    if(ret) {
        if(c->flag & CPLR_FLAG_INTERACTIVE) {
          fprintf(stderr, "Program returned %d.\n", ret);
        }
        /* XXX preserving ret, exiting !? */
    }
  }

  c->flag |= CPLR_FLAG_EVALUATED;

 out:

  /* clone the context */
  n = cplr_clone(c);
  n->lindex = c->lindex + 1;
  c->lnext = n;
  n->lprev = c;
  n->flag &= ~CPLR_FLAG_EVALUATED;

  /* clear statement piles */
  l_clear(&n->srcs);
  l_clear(&n->tlfs);
  l_clear(&n->stms);
  l_clear(&n->befs);
  l_clear(&n->afts);

  /* return the new state */
  return n;
}
