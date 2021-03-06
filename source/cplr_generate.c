/*
 * cplr - Utility for running C code
 *
 * Copyright (C) 2020-2021 Ingo Albrecht <copyright@promovicz.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <limits.h>
#include <stdlib.h>

#include "cplr.h"

/* bar for separating the dump - 80 chars */
static const char *bar =
  "========================================"
  "========================================";

static void cplr_emit(cplr_t *c,
		      cplr_gstate_t nstate,
		      const char * file, int line,
		      const char *fmt, ...) {
  bool needline = false;
  char *sline, *sfmt;
  va_list a;
  if(nstate == CPLR_GSTATE_COMMENT) {
    needline = false;
  } else if(c->g_state != nstate) {
    needline = true;
  } else if(c->g_prevfile == NULL || strcmp(c->g_prevfile, file) != 0) {
    needline = true;
  } else if(nstate == CPLR_GSTATE_PREPROC) {
    needline = false;
  } else {
    needline = (c->g_prevline && (line != (c->g_prevline+1)));
  }

  if(needline)
    sline = msprintf("#line %d \"%s\"\n", line, file);
  va_start(a, fmt);
  sfmt = vmsprintf(fmt, a);
  va_end(a);

  if(c->g_code) {
    if(needline)
      fwrite(sline, 1, strlen(sline), c->g_code);
    fwrite(sfmt, 1, strlen(sfmt), c->g_code);
  }

  if(c->g_dump) {
    if(needline && (c->flag & CPLR_FLAG_VERBOSE))
      fwrite(sline, 1, strlen(sline), c->g_dump);
    fwrite(sfmt, 1, strlen(sfmt), c->g_dump);
  }

  if(needline)
    xfree(sline);
  xfree(sfmt);

  if(nstate != CPLR_GSTATE_COMMENT) {
    c->g_state = nstate;
    c->g_prevline = line;
    if(c->g_prevfile) {
      xptrfree((void**)&c->g_prevfile);
    }
    c->g_prevfile = strdup(file);
  }
}

#define CPLR_EMIT_COMMENT(c, fmt, ...)			\
  cplr_emit(c, CPLR_GSTATE_COMMENT, NULL,		\
	    1, "/* " fmt " */\n", ##__VA_ARGS__)
#define CPLR_EMIT_PREPROC(c, fn, fmt, ...)		\
  cplr_emit(c, CPLR_GSTATE_PREPROC, fn,			\
	    1, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_TOPLEVEL(c, fn, fmt, ...)     \
  cplr_emit(c, CPLR_GSTATE_TOPLEVEL, fn,	\
	    1, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_INTERNAL(c, fmt, ...)			\
  cplr_emit(c, CPLR_GSTATE_INTERNAL, "internal",	\
	    __LINE__, fmt, ##__VA_ARGS__)
#define CPLR_EMIT_STATEMENT(c, fn, fmt, ...)    \
  cplr_emit(c, CPLR_GSTATE_STATEMENT, fn,       \
	    1, fmt, ##__VA_ARGS__)

static void cplr_emit_minilib(cplr_t *c, const char *phase, const char *mlb, int count) {
  if(c->flag & CPLR_FLAG_VERBOSE) {
    fprintf(stderr, "Emitting minilib '%s' phase '%s'\n", mlb, phase);
  }
  char *fn = msprintf("%s_mlib_%d", phase, count);
  CPLR_EMIT_PREPROC(c, fn, "#define minilib_%s\n", phase);
  CPLR_EMIT_PREPROC(c, fn, "#include \"%s.m\"\n", mlb);
  CPLR_EMIT_PREPROC(c, fn, "#undef minilib_%s\n", phase);
  ptrfree((void**)&fn);
}

static void cplr_emit_minilibs(cplr_t *c, const char *phase, bool reverse) {
  int i;
  ln_t *n;
  if(reverse) {
    i = l_size(&c->mlbs);
    L_BACKWARDS(&c->mlbs, n) {
      cplr_emit_minilib(c, phase, value_get_str(&n->v), i--);
    }
  } else {
    i = 0;
    L_FORWARD(&c->mlbs, n) {
      cplr_emit_minilib(c, phase, value_get_str(&n->v), i++);
    }
  }
}

static void cplr_generate_section(cplr_t *c,
				  const char *name,
				  lh_t *list,
				  bool reverse, bool minilibs,
				  const char *fmt) {
  int i;
  ln_t *n;
  char fn[64];
  if(c->flag & CPLR_FLAG_VERBOSE) {
    fprintf(stderr, "Generating section %s\n", name);
  }
  CPLR_EMIT_COMMENT(c, "%s", name);
  if(reverse) {
    i = l_size(list);
    L_BACKWARDS(list, n) {
      snprintf(fn, sizeof(fn), "%s_%d", name, i--);
      CPLR_EMIT_STATEMENT(c, fn, fmt, value_get_str(&n->v));
    }
    if(minilibs) {
      cplr_emit_minilibs(c, name, true);
    }
  } else {
    if(minilibs) {
      cplr_emit_minilibs(c, name, false);
    }
    i = 0;
    L_FORWARD(list, n) {
      snprintf(fn, sizeof(fn), "%s_%d", name, i++);
      CPLR_EMIT_PREPROC(c, fn, fmt, value_get_str(&n->v));
    }
  }
}

static int cplr_generate_code(cplr_t *c) {
  if(c->flag & CPLR_FLAG_VERBOSE) {
    fprintf(stderr, "Generating code\n");
  }
  bool minilibs = !l_empty(&c->mlbs);
  bool havemain = !(l_empty(&c->stms)
		    && l_empty(&c->befs)
		    && l_empty(&c->afts));
  bool havecode = (minilibs || havemain
		   || !l_empty(&c->tlfs));
  if(havecode) {
    if(minilibs || !l_empty(&c->defsys)) {
      cplr_generate_section(c, "defsysinclude", &c->defsys,
			    false, false, "#include <%s>\n");
    }
    if(minilibs || !l_empty(&c->syss)) {
      cplr_generate_section(c, "sysinclude", &c->syss,
			    false, minilibs, "#include <%s>\n");
    }
    if(minilibs || !l_empty(&c->incs)) {
      cplr_generate_section(c, "include", &c->incs,
			    false, minilibs, "#include <%s>\n");
    }
    if(minilibs || !l_empty(&c->tlfs)) {
      cplr_generate_section(c, "toplevel", &c->tlfs,
			    false, minilibs, "%s;\n");
    }
  }
  if(havemain) {
    CPLR_EMIT_COMMENT(c, "main");
    CPLR_EMIT_INTERNAL(c, "int main(int argc, char **argv) {\n");
    CPLR_EMIT_INTERNAL(c, "    int ret = 0;\n");
    if(minilibs || !l_empty(&c->befs)) {
      cplr_generate_section(c, "before", &c->befs,
			    false, minilibs, "    %s;\n");
    }
    if(minilibs || !l_empty(&c->stms)) {
      cplr_generate_section(c, "main", &c->stms,
			    false, minilibs, "    %s;\n");
    }
    if(minilibs || !l_empty(&c->afts)) {
      cplr_generate_section(c, "after", &c->afts,
			    true, minilibs, "    %s;\n");
    }
    CPLR_EMIT_COMMENT(c, "done");
    CPLR_EMIT_INTERNAL(c, "    return ret;\n");
    CPLR_EMIT_INTERNAL(c, "}\n");
  }
  return 0;
}

#ifdef _GNU_SOURCE
static ssize_t code_stream_write(void *cp, const char *buf, size_t size) {
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
static cookie_io_functions_t code_stream_functions = {
  NULL, &code_stream_write, NULL, NULL
};
static ssize_t dump_stream_write(void *cp, const char *buf, size_t size) {
  cplr_t *c = (cplr_t*)cp;
  assert(c->g_dumpbuf);
  assert(size < SSIZE_MAX);
  size_t ol = strlen(c->g_dumpbuf);
  size_t nl = ol + size + 1;
  char *n = xrealloc(c->g_dumpbuf, nl);
  c->g_dumpbuf = n;
  strncpy(n + ol, buf, size + 1);
  return size;
}
static cookie_io_functions_t dump_stream_functions = {
  NULL, &dump_stream_write, NULL, NULL
};
#endif

static void cplr_generate_open(cplr_t *c) {
#ifdef _GNU_SOURCE
  c->g_codebuf = strdup("");
  c->g_code = fopencookie(c, "w", code_stream_functions);
#else
  c->g_codebuf = xcalloc(2^16, 1);
  c->g_code = fmemopen(c->g_codebuf, 2^16, "w");
#endif
  if(c->flag & CPLR_FLAG_DUMP) {
#ifdef _GNU_SOURCE
    c->g_dumpbuf = strdup("");
    c->g_dump = fopencookie(c, "w", dump_stream_functions);
#else
    c->g_dumpbuf = xcalloc(2^16, 1);
    c->g_dump = fmemopen(c->g_dumpbuf, 2^16, "w");
#endif
  }
}

static void cplr_generate_close(cplr_t *c) {
  fflush(c->g_code);
  fclose(c->g_code);
  c->g_code = NULL;
  if(c->g_dump) {
    fflush(c->g_dump);
    fclose(c->g_dump);
    c->g_dump = NULL;
  }
}

static void cplr_generate_dump(cplr_t *c) {
  const char *filter = getenv("CPLR_DUMP_FILTER");
  if(!filter) {
    filter = "cat -n -";
  }
  if(c->flag & CPLR_FLAG_DUMP) {
    fprintf(stderr, "%s\n", bar);
    fflush(stderr);
    size_t total = strlen(c->g_dumpbuf);
    char *buf = c->g_dumpbuf;
    FILE *dumpout = popen(filter, "w");
    size_t done = 0;
    while(done < total) {
      size_t step = total - done;
      if(step > sizeof(buf)) {
	step = sizeof(buf);
      }
      size_t n = fwrite(buf + done, 1, step, dumpout);
      if(n < step) {
	perror("fwrite");
	exit(1);
      }
      done += n;
    }
    pclose(dumpout);
    fprintf(stderr, "%s\n", bar);
    fflush(stderr);
  }
}

static void cplr_generate_report(cplr_t *c) {
 if(c->flag & CPLR_FLAG_VERBOSE) {
    size_t cl = 0, dl = 0;
    if(c->g_codebuf)
      cl = strlen(c->g_codebuf);
    if(c->g_dumpbuf)
      dl = strlen(c->g_dumpbuf);
    fprintf(stderr, "Generated bytes: %zu code, %zu dump\n", cl, dl);
  }
}

static void cplr_generate_finish(cplr_t *c) {
  if(c->g_prevfile) {
    xptrfree((void**)&c->g_prevfile);
  }
}

int cplr_generate(cplr_t *c) {
  /* say hello */
  if(c->flag & CPLR_FLAG_VERBOSE) {
    fprintf(stderr, "Generation phase\n");
  }
  /* alloc buffers and open streams */
  cplr_generate_open(c);
  /* perform code generation */
  cplr_generate_code(c);
  /* flush and close */
  cplr_generate_close(c);
  /* emit the dump buffer */
  cplr_generate_dump(c);
  /* report stats */
  cplr_generate_report(c);
  /* clean up */
  cplr_generate_finish(c);
  /* done */
  return 0;
}
