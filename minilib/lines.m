#if defined(minilib_before)

#ifndef _foreach_file
#define _foreach_file stdin
#endif

char _foreach_line_buf[1024];
while(fgets(_foreach_line_buf, sizeof(_foreach_line_buf), _foreach_file)) {
  const char *line = _foreach_line_buf;
  const size_t linelen = strlen(line);

#ifdef _foreach_string
#define _foreach_line_string _foreach_string
#endif
#define _foreach_string line

#elif defined(minilib_after)

#ifdef _foreach_line_string
#define _foreach_string _foreach_line_string
#else
#undef _foreach_string
#endif

}

#endif
