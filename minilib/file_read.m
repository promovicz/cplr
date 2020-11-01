#if defined(minilib_before)

FILE *file = fopen(_foreach_string, "r");
if(!file) {
  perror("fopen");
  exit(1);
}

#define _foreach_file file

#elif defined(minilib_after)

#undef _foreach_file

fclose(file);

#endif
