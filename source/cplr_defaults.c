
#include "cplr.h"

int cplr_defaults(cplr_t *c) {
  if(c->t_env & CPLR_ENV_POSIX) {
    l_append_str_static(&c->defdef, "-D_DEFAULT_SOURCE");
    l_append_str_static(&c->defdef, "-D_XOPEN_SOURCE=600");
    l_append_str_static(&c->defdef, "-D_POSIX_SOURCE=200809L");
    l_append_str_static(&c->defsys, "sys/stat.h");
    l_append_str_static(&c->defsys, "sys/types.h");
    l_append_str_static(&c->defsys, "assert.h");
    l_append_str_static(&c->defsys, "ctype.h");
    l_append_str_static(&c->defsys, "errno.h");
    l_append_str_static(&c->defsys, "fcntl.h");
    l_append_str_static(&c->defsys, "libgen.h");
    l_append_str_static(&c->defsys, "limits.h");
    l_append_str_static(&c->defsys, "stdarg.h");
    l_append_str_static(&c->defsys, "stdio.h");
    l_append_str_static(&c->defsys, "stdint.h");
    l_append_str_static(&c->defsys, "stdlib.h");
    l_append_str_static(&c->defsys, "string.h");
    l_append_str_static(&c->defsys, "termios.h");
    l_append_str_static(&c->defsys, "time.h");
    l_append_str_static(&c->defsys, "unistd.h");
  }
  if(c->t_env & CPLR_ENV_LINUX) {
  }
  return 0;
}
