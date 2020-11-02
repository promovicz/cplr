
#include "cext/memory.h"

#include "cplr.h"

cplr_t *cplr_new(void) {
  cplr_t *res = xcalloc(sizeof(cplr_t), 1);

  res->t_env = CPLR_ENV_POSIX;

  return res;
}

void cplr_free(cplr_t *c) {
  if(c->tcc) {
    tcc_delete(c->tcc);
    c->tcc = NULL;
  }
  lptrfree((void*)&c->g_codebuf);
  lptrfree((void*)&c->g_dumpbuf);
  l_clear(&c->defdef);
  l_clear(&c->defsys);
  l_clear(&c->sysdirs);
  l_clear(&c->incdirs);
  l_clear(&c->mlbdirs);
  l_clear(&c->libdirs);
  l_clear(&c->defs);
  l_clear(&c->syss);
  l_clear(&c->incs);
  l_clear(&c->mlbs);
  l_clear(&c->libs);
  l_clear(&c->pkgs);
  l_clear(&c->srcs);
  l_clear(&c->tlfs);
  l_clear(&c->stms);
  l_clear(&c->befs);
  l_clear(&c->afts);
  xfree(c);
}
