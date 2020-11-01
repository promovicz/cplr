
void cplr_emit(cplr_t *c,
               cplr_gstate_t nstate,
               const char * file, int line,
               const char *fmt, ...) {
  bool needline = false;
  va_list a;
  if(nstate == CPLR_GSTATE_COMMENT) {
    needline = false;
  } else if(c->g_state != nstate) {
    needline = true;
  } else {
    needline = (c->g_prevline && (line != (c->g_prevline+1)));
  }
  va_start(a, fmt);
  if(c->g_code) {
    if(needline)
      fprintf(c->g_code, "#line %d \"%s\"\n", line, file);
    vfprintf(c->g_code, fmt, a);
  }
  va_end(a);
  va_start(a, fmt);
  if(c->g_dump) {
    if(needline && (c->flag & CPLR_FLAG_VERBOSE))
      fprintf(c->g_dump, "#line %d \"%s\"\n", line, file);
    vfprintf(c->g_dump, fmt, a);
  }
  va_end(a);
  if(nstate != CPLR_GSTATE_COMMENT) {
    c->g_state = nstate;
    c->g_prevline = line;
  }
}

#define CPLR_EMIT_COMMENT(c, fmt, ...)          \
  cplr_emit(c, CPLR_GSTATE_COMMENT, NULL,               \
            1, "/* " fmt " */\n", ##__VA_ARGS__)
#define CPLR_EMIT_INCLUDE(c, fn, fmt, ...)              \
  cplr_emit(c, CPLR_GSTATE_INCLUDE, fn,                 \
            1, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_TOPLEVEL(c, fn, fmt, ...)     \
  cplr_emit(c, CPLR_GSTATE_TOPLEVEL, fn,        \
            1, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_INTERNAL(c, fmt, ...)                 \
  cplr_emit(c, CPLR_GSTATE_INTERNAL, "internal",        \
            __LINE__, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_STATEMENT(c, fn, fmt, ...)    \
  cplr_emit(c, CPLR_GSTATE_STATEMENT, fn,       \
            1, fmt, ##__VA_ARGS__)

int cplr_code(cplr_t *c) {
  int i;
  ln_t *n;
  char fn[32];
  if(!l_empty(&c->defsys)) {
    CPLR_EMIT_COMMENT(c, "defsysinclude");
    i = 0;
    L_FORWARD(&c->defsys, n) {
      snprintf(fn, sizeof(fn), "defsysinclude_%d", i++);
      CPLR_EMIT_INCLUDE(c, fn, "#include <%s>\n", n->v.s);
    }
  }
  if(!l_empty(&c->syss)) {
    CPLR_EMIT_COMMENT(c, "sysinclude");
    i = 0;
    L_FORWARD(&c->syss, n) {
      snprintf(fn, sizeof(fn), "sysinclude_%d", i++);
      CPLR_EMIT_INCLUDE(c, fn, "#include <%s>\n", n->v.s);
    }
  }
  if(!l_empty(&c->incs)) {
    CPLR_EMIT_COMMENT(c, "include");
    i = 0;
    L_FORWARD(&c->incs, n) {
      snprintf(fn, sizeof(fn), "include_%d", i++);
      CPLR_EMIT_INCLUDE(c, fn, "#include \"%s\"\n", n->v.s);
    }
  }
  if(!l_empty(&c->tlfs)) {
    CPLR_EMIT_COMMENT(c, "toplevel");
    i = 0;
    L_FORWARD(&c->tlfs, n) {
      snprintf(fn, sizeof(fn), "toplevel_%d", i++);
      CPLR_EMIT_TOPLEVEL(c, fn, "%s;\n", n->v.s);
    }
  }
  CPLR_EMIT_COMMENT(c, "main");
  CPLR_EMIT_INTERNAL(c, "int main(int argc, char **argv) {\n");
  CPLR_EMIT_INTERNAL(c, "    int ret = 0;\n");
  if(!l_empty(&c->befs)) {
    CPLR_EMIT_COMMENT(c, "before");
    i = 0;
    L_FORWARD(&c->befs, n) {
      snprintf(fn, sizeof(fn), "before_%d", i++);
      CPLR_EMIT_STATEMENT(c, fn, "    %s;\n", n->v.s);
    }
  }
  if(!l_empty(&c->stms)) {
    CPLR_EMIT_COMMENT(c, "statement");
    i = 0;
    L_FORWARD(&c->stms, n) {
      snprintf(fn, sizeof(fn), "statement_%d", i++);
      CPLR_EMIT_STATEMENT(c, fn, "    %s;\n", n->v.s);
    }
  }
  if(!l_empty(&c->afts)) {
    CPLR_EMIT_COMMENT(c, "after");
    i = 0;
    L_BACKWARDS(&c->afts, n) {
      snprintf(fn, sizeof(fn), "after_%d", i++);
      CPLR_EMIT_STATEMENT(c, fn, "    %s;\n", n->v.s);
    }
  }
  CPLR_EMIT_COMMENT(c, "done");
  CPLR_EMIT_INTERNAL(c, "    return ret;\n");
  CPLR_EMIT_INTERNAL(c, "}\n");
  return 0;
}

#ifdef _GNU_SOURCE
static ssize_t stream_write(void *cp, const char *buf, size_t size) {
  cplr_t *c = (cplr_t*)cp;
  assert(c->g_codebuf);
  assert(size < SSIZE_MAX);
  size_t ol = strlen(c->g_codebuf);
  size_t nl = ol + size + 1;
  char *n = xrealloc(c->g_codebuf, nl);
  c->g_codebuf = n;
  strncpy(n + ol, buf, size + 1);
  return size;
}
static cookie_io_functions_t stream_functions = {
  NULL, &stream_write, NULL, NULL
};
#endif

int cplr_generate(cplr_t *c) {
#ifdef _GNU_SOURCE
  c->g_codebuf = strdup("");
  c->g_code = fopencookie(c, "w", stream_functions);
#else
  c->g_codebuf = xcalloc(4096, 1);
  c->g_code = fmemopen(c->g_codebuf, 4096, "w");
#endif
  if(c->flag & CPLR_FLAG_DUMP) {
    if(c->flag & CPLR_FLAG_VERBOSE)
      fprintf(stderr, "%s\n", bar);
    c->g_dump = popen("cat -n 1>&2", "w");
  }
  cplr_code(c);
  if(c->flag & CPLR_FLAG_DUMP) {
    pclose(c->g_dump);
    if(c->flag & CPLR_FLAG_VERBOSE)
      fprintf(stderr, "%s\n", bar);
  }
  fclose(c->g_code);
  c->g_code = NULL;

  if(c->flag & CPLR_FLAG_VERBOSE) {
    fprintf(stderr, "Generated %zu bytes of C code\n", strlen(c->g_codebuf));
  }
  return 0;
}
