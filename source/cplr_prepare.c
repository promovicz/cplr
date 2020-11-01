
static void cplr_tcc_error(cplr_t *c, const char *msg) {
  fprintf(stderr, "Compiler condition:\n%s\n", msg);
  /* TODO split and indent */
}

int cplr_prepare(cplr_t *c) {
  ln_t *i;
  TCCState *t = tcc_new();
  c->tcc = t;
  tcc_set_error_func(t, c, (void (*)(void *, const char *))&cplr_tcc_error);
  if(tcc_set_output_type(t, TCC_OUTPUT_MEMORY)) {
    return 1;
  }
  L_FORWARD(&c->pkgs, i) {
    if(cplr_prepare_package(c, i->v.s)) {
      return 1;
    }
  }
  L_FORWARD(&c->defdef, i) {
    tcc_set_options(t, i->v.s);
  }
  L_FORWARD(&c->defs, i) {
    tcc_set_options(t, i->v.s);
  }
  L_FORWARD(&c->sysdirs, i) {
    if(tcc_add_sysinclude_path(t, i->v.s)) {
      return 1;
    }
  }
  L_FORWARD(&c->incdirs, i) {
    if(tcc_add_include_path(t, i->v.s)) {
      return 1;
    }
  }
  L_FORWARD(&c->libdirs, i) {
    if(tcc_add_library_path(t, i->v.s)) {
      return 1;
    }
  }
  L_FORWARD(&c->libs, i) {
    if(tcc_add_library(t, i->v.s)) {
      return 1;
    }
  }
  L_FORWARD(&c->srcs, i) {
    if(tcc_add_file(t, i->v.s)) {
      return 1;
    }
  }
  return 0;
}
