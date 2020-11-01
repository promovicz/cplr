
/* short options */
const char *shortopts = "-:hHVvdnpP:D:U:I:M:L:S:i:m:l:s:f:t:e:b:a:-";

/* long is optional */
#ifdef USE_GETOPT_LONG
/* long options */
const struct option longopts[] = {
  {"help",     0, NULL, 'h'},
  {"herald",   0, NULL, 'H'},
  {"version",  0, NULL, 'V'},
  {"verbose",  0, NULL, 'v'},
  {"dump",     0, NULL, 'd'},
  {"noexec",   0, NULL, 'n'},
  {"pristine", 0, NULL, 'p'},
  {NULL,    1, NULL, 'P'},
  {NULL,    1, NULL, 'D'},
  {NULL,    1, NULL, 'U'},
  {NULL,    1, NULL, 'I'},
  {NULL,    1, NULL, 'M'},
  {NULL,    1, NULL, 'L'},
  {NULL,    1, NULL, 'S'},
  {NULL,    1, NULL, 'i'},
  {NULL,    1, NULL, 'm'},
  {NULL,    1, NULL, 'l'},
  {NULL,    1, NULL, 's'},
  {NULL,    1, NULL, 'f'},
  {NULL,    1, NULL, 't'},
  {NULL,    1, NULL, 'e'},
  {NULL,    1, NULL, 'b'},
  {NULL,    1, NULL, 'a'},
  {NULL,    0, NULL, 0 },
};
/* help strings for long options */
const char *longhelp[] = {
  "show help message",
  "show herald message",
  "show version string",

  "print verbose messages",
  "dump generated code",
  "do not run, just compile",
  "pristine environment",

  "add package",

  "define cpp symbol",
  "undefine cpp symbol",

  "add include directory",
  "add minilib directory",
  "add library directory",
  "add sysinclude directory",

  "add include",
  "add minilib",
  "add library",
  "add sysinclude",

  "add source file",

  "add toplevel statement",
  "add main statement",
  "add before-main statement",
  "add after-main statement",
  NULL,
};
#endif /* USE_GETOPT_LONG */


static void cplr_show_summary(cplr_t *c, FILE*out) {
  fprintf(out, "Usage: %s [-vdnphHV] <statement>...\n", c->argv[0]);
  fprintf(out, "The C piler: a tool for executing C code\n\n");
}

/* help function */
static void cplr_show_help(cplr_t *c, FILE *out) {
  /* short summary */
  cplr_show_summary(c, out);
  /* with getopt_long we can easily print more information */
#ifdef USE_GETOPT_LONG
  int i;
  for(i = 0; longhelp[i]; i++) {
    if(longopts[i].name) {
      fprintf(out, "  -%c, --%-10s\t%s\n",
              (char)longopts[i].val,
              longopts[i].name,
              longhelp[i]);
          } else {
      fprintf(out, "  -%c\t\t\t%s\n",
              (char)longopts[i].val,
              longhelp[i]);
    }
  }
  fprintf(out, "\n");
#endif
}

/* herald function */
static void cplr_show_herald(cplr_t *c, FILE *out) {
  /* program basename for mail address */
  const char *bn = basename(c->argv[0]);

  /* short summary */
  cplr_show_summary(c, out);

  /* history */
  fprintf(out, "Invented around the ides of October anno MMXX.\n\n");

  /* blessing */
  fprintf(out, "May this be as useful for you as it is for me.\n\n");

  /* copyright information */
  fprintf(out, "Copyright (C) 2020 Ingo Albrecht <%s@promovicz.org>.\n", bn);
  fprintf(out, "Licensed under the GNU General Public License version 3 or later.\n");
  fprintf(out, "See package file COPYING or https://www.gnu.org/licenses/.\n\n");
}

/* version function */
static void cplr_show_version(cplr_t *c, FILE *out) {
}

/* option parser */
static int cplr_optparse(cplr_t *c, int argc, char **argv) {
  int opt;
  /* remember argc/argv */
  c->argc = argc;
  c->argv = argv;
  /* parse options */
  optind = 1;
  opterr = 0;
  while(1) {
#ifdef USE_GETOPT_LONG
    opt = getopt_long(argc, argv, shortopts, longopts, NULL);
#else
    opt = getopt(argc, argv, shortopts);
#endif
    if (opt == -1)
      goto done;
    switch(opt) {
    case 'h': /* help requested */
      goto help;
    case 'H': /* herald requested */
      goto herald;
    case 'V': /* version requested */
      goto version;
    case 0: /* handled by getopt */
      break;
    case 1: /* non-option argument */
      l_appends(&c->stms, optarg);
      break;
    case '-': /* terminating argument */
      goto done;
    case 'v': /* enable verbose */
      c->flag |= CPLR_FLAG_VERBOSE;
      break;
    case 'd': /* enable dump */
      c->flag |= CPLR_FLAG_DUMP;
      break;
    case 'n': /* enable norun */
      c->flag |= CPLR_FLAG_NORUN;
      break;
    case 'p': /* enable pristine */
      c->flag |= CPLR_FLAG_PRISTINE;
      break;
    case 'P':
      l_appends(&c->pkgs, optarg);
      break;
    case 'D':
      l_appends(&c->defs, msnprintf(1024, "-D%s", optarg));
      break;
    case 'U':
      l_appends(&c->defs, msnprintf(1024, "-U%s", optarg));
      break;
    case 'I':
      l_appends(&c->incdirs, optarg);
      break;
    case 'M':
      l_appends(&c->mlbdirs, optarg);
      break;
    case 'S':
      l_appends(&c->sysdirs, optarg);
      break;
    case 'L':
      l_appends(&c->libdirs, optarg);
      break;
    case 'i':
      l_appends(&c->incs, optarg);
      break;
    case 'm':
      l_appends(&c->mlbs, optarg);
      break;
    case 's':
      l_appends(&c->syss, optarg);
      break;
    case 'l':
      l_appends(&c->libs, optarg);
      break;
    case 'f':
      l_appends(&c->srcs, optarg);
      break;
    case 't':
      l_appends(&c->tlfs, optarg);
      break;
    case 'e':
      l_appends(&c->stms, optarg);
      break;
    case 'b':
      l_appends(&c->befs, optarg);
      break;
    case 'a':
      l_appends(&c->afts, optarg);
      break;
    case ':': /* missing option argument */
      fprintf(stderr, "Missing argument for option -%c\n", optopt);
      goto err;
    case '?': /* unknown option */
      fprintf(stderr, "Unknown option -%c\n", optopt);
      goto err;
    default:
      fprintf(stderr, "Option processing problem\n");
      goto err;
    }
  }
 done:
  /* set index of program args */
  c->argp = optind;
  return 0;
 err:
  return 1;
 help:
  cplr_show_help(c, stdout);
  return 2;
 herald:
  cplr_show_herald(c, stdout);
  return 2;
 version:
  cplr_show_version(c, stdout);
  return 2;
}
