#if defined(minilib_before)

for(int _foreach_arg_i = 1; _foreach_arg_i < argc; _foreach_arg_i++) {
  const int argi = _foreach_arg_i;
  const char *arg = argv[argi];

#ifdef _foreach_string
#define _foreach_arg_string _foreach_string
#endif
#define _foreach_string arg

#elif defined(minilib_after)

#ifdef _foreach_arg_string
#define _foreach_string _foreach_arg_string
#else
#undef _foreach_string
#endif

}

#endif
