## cplr - the C piler

The C piler is utility for executing C code directly from the shell.

Pretty much everything on a POSIX system can be done in C, and quite a few things can't be done without.

It's like awk with C as the language. You can do anything with it.

The name means that it "piles" up strings.

Personally I do:
```
$ alias c=cplr
```

### Features

 * Command-line utility for running C code directly
 * Fast
 * Will include most UNIX headers by default
 * Has error messages appropriate for command-line use
 * Can import any pkg-config package
 * Uses libtcc internally
 * Could support gcc and/or clang - even embedded
 * Usual compiler options: -I \<incdir\>, -L \<libdir\>, -l \<lib\>
 * Extra convenience options: -i \<include\>, -S <sysincdir\>, -s \<sysinc\>
 * Adding code: \<statement\>, -b \<before\>, -a \<after\>, -t \<toplevel\>
 * Flags: -v (verbose compiler), -d (dump code), -p (pristine env), -n (no execute)
 * Environment: CPLR_DUMP_FILTER="cat -n -"

### Enabling colored source dumps

Normally cplr will print code using "cat -n", resulting in a simple numbered dump:

```
$ cplr 'puts("Hello")' -nd
================================================================================
     1	/* defsysinclude */
     2	#include <sys/stat.h>
     3	#include <sys/types.h>
     4	#include <assert.h>
     5	#include <ctype.h>
     6	#include <errno.h>
     7	#include <fcntl.h>
     8	#include <libgen.h>
     9	#include <limits.h>
    10	#include <locale.h>
    11	#include <stdarg.h>
    12	#include <stdio.h>
    13	#include <stdint.h>
    14	#include <stdlib.h>
    15	#include <string.h>
    16	#include <termios.h>
    17	#include <time.h>
    18	#include <unistd.h>
    19	#include <wchar.h>
    20	/* main */
    21	int main(int argc, char **argv) {
    22	    int ret = 0;
    23	/* main */
    24	    puts("Hello");
    25	/* done */
    26	    return ret;
    27	}
================================================================================
Compilation succeeded.
```

You can set CPLR_DUMP_FILTER to change the filter. I recommend t3highlight:

```
$ export CPLR_DUMP_FILTER="t3highlight -l c | cat -n -"
(no demonstration because ANSI color does not paste)
```

### Why cplr exists and how to use it

If you are a UNIX/C programmer like me then you know the feeling of needing to call a C function to try something out.

The next thing you would usually do is open an editor and type in the words tatooed to your eyelids:

```
#include <stdio.h>
int main(int argc, char **argv) {
	return 0;
}
```

After doing that for the millionths time in your life you would get back to actual problems.

But deep down in your heart you always wanted a C compiler in your shell, cause it would be so much easier.

One that integrates with UNIX natively, just as well as sed, awk, cut, tr and grep.

And so one day I wrote a tool to do just that:

```
# program that prints its arguments
# it also returns part of the birth year of UNIX
$ cplr 'int i' \
    'for(i=1;i<argc;i++) { puts(argv[i]); }' \
	'ret = 69' \
  -- a b c
a
b
c
$ echo $?
69
```

And I made it debuggable:

```
# same thing, but with hack-friendly debugging
# by default there are a lot of includes
# you can disable that with -p or --pristine
# in any case you can add includes with -i or -s
# and also use the usual -I and -L known from cc
$ cplr \
    'int i' \
    'for(i=1;i<argc;i++) { puts(argv[i]); }' \
	-d \
  -- a b c
     1	#include <sys/stat.h>
     2	#include <sys/types.h>
     3	#include <ctype.h>
     4	#include <errno.h>
     5	#include <fcntl.h>
     6	#include <stdarg.h>
     7	#include <stdio.h>
     8	#include <stdint.h>
     9	#include <stdlib.h>
    10	#include <string.h>
    11	#include <termios.h>
    12	#include <time.h>
    13	#include <unistd.h>
    14	int main(int argc, char **argv) {
    15	    int ret = 0;
    16	    int i;
    17	    for(i=1;i<argc;i++) { puts(argv[i]); };
    18	    return ret;
    19	}
a
b
c
```

Enabling verbose mode gives more information and a view under the covers. In the future this mode should also log about the compilation process.

```
$ cplr \
    'int i' \
    'for(i=1;i<argc;i++) { puts(argv[i]); }' \
	-d \
  -- a b c
================================================================================
     1	#line 1 "defsysinclude_0"
     2	#include <sys/stat.h>
     3	#line 1 "defsysinclude_1"
     4	#include <sys/types.h>
     5	#line 1 "defsysinclude_2"
     6	#include <ctype.h>
     7	#line 1 "defsysinclude_3"
     8	#include <errno.h>
     9	#line 1 "defsysinclude_4"
    10	#include <fcntl.h>
    11	#line 1 "defsysinclude_5"
    12	#include <stdarg.h>
    13	#line 1 "defsysinclude_6"
    14	#include <stdio.h>
    15	#line 1 "defsysinclude_7"
    16	#include <stdint.h>
    17	#line 1 "defsysinclude_8"
    18	#include <stdlib.h>
    19	#line 1 "defsysinclude_9"
    20	#include <string.h>
    21	#line 1 "defsysinclude_10"
    22	#include <termios.h>
    23	#line 1 "defsysinclude_11"
    24	#include <time.h>
    25	#line 1 "defsysinclude_12"
    26	#include <unistd.h>
    27	#line 275 "internal"
    28	int main(int argc, char **argv) {
    29	    int ret = 0;
    30	#line 1 "statement_0"
    31	    int i;
    32	#line 1 "statement_1"
    33	    for(i=0;i<argc;i++) { puts(argv[i]); };
    34	#line 292 "internal"
    35	    return ret;
    36	}
================================================================================
a
b
c
```

Those who need this tool most urgently should not need further instructions.

This sure needs some documentation and maybe some small additional features.

Happy hacking!

### Simple examples

Imagine you want to try something using readline:

```
$ bin/cplr -lreadline -ireadline/readline.h 'printf("Input: %s\n", readline("& "))'
& hello
Input: hello
```

You can also run simple libraries straight from source:

```
$ bin/cplr -f linenoise.c -i linenoise.h
> puts(linenoise("% "))
% hello
hello
>
```

Another good use is getting low-level information:

```
$ bin/cplr 'printf("sizeof(struct stat) = %u\n", sizeof(struct stat))'
sizeof(struct stat) = 144
```

You can also smash together a quick executable:

```
$ bin/cplr -o realpath \
  'char *res' \
  'if(argc != 2) { printf("Usage: %s <path>\n", argv[0]); return 1; }' \
  'res = realpath(argv[1], NULL)' \
  'if(!res) { fprintf(stderr, "error: %s\n", strerror(errno)); return 1; }' \
  'puts(res)' 'free(res)'
$ ./realpath /bin/ls
/usr/bin/ls
```

### Examples using Python

You can do all sorts of things with cplr:

```
# the tool also supports pkg-config natively
# this allows including most modern libraries
# here is a simple hello using python
$ bin/cplr -P python3 -i Python.h \
  'Py_Initialize()' \
  'PyRun_SimpleString("print(\"hello\")")' \
  'Py_Finalize()'
hello
```

Even weird ones:

```
# if you like stacking things then check this out
# this is c running python, in turn running a shell
$ bin/cplr \
    -P python3 \
    -i Python.h \
    -U _XOPEN_SOURCE \
    'Py_Initialize()' \
    'for(int i = 1; i < argc; i++) PyRun_SimpleString(argv[i])' \
    'Py_Finalize()' -- \
	'import os' \
	'os.system("echo this actually works")' \
	'os.system("echo oh yes absolutely it does")'
this actually works
oh yes absolutely it does
```

### Examples using GTK

It's quite possible to build simple GUI programs using cplr:

```
bin/cplr -P gtk+-3.0 -i gtk/gtk.h \
-t 'int clicks = 0' \
-t 'void clicked(GtkWidget *w, char *s) { g_message("Click %d on widget %s.", ++clicks, s); }' \
-t 'void closed(GtkWidget *w, char *s) { g_message("Closed by widget %s.", s); gtk_main_quit(); }' \
-b 'gtk_init(&argc, &argv)' \
   'GtkWindow *w = gtk_window_new(GTK_WINDOW_TOPLEVEL)' \
   'GtkButton *b = gtk_button_new()' \
   'gtk_button_set_label(b, "Click me!")' \
   'gtk_container_add (GTK_CONTAINER (w), b)' \
   'g_signal_connect (G_OBJECT(b), "clicked", G_CALLBACK (clicked), "button")' \
   'g_signal_connect (G_OBJECT(w), "destroy", G_CALLBACK (closed), "window")' \
   'gtk_widget_show_all(w)' \
   'gtk_main()' \
   -p "$@"
\<window pops up\>
** Message: 06:40:30.615: Click 1 on widget button.
** Message: 06:40:31.002: Click 2 on widget button.
** Message: 06:40:31.396: Click 3 on widget button.
** Message: 06:40:31.839: Closed by widget window.
\<window closed by user\>
```

Doing this with modern GTK actually provides decent error messages:

```
# note that GTK has some crazy useful error messages
# this sample is something that happened to me
$ bin/cplr -P gtk+-3.0 -i gtk/gtk.h \
  'gtk_init(&argc, &argv)' \
  'GtkApplication *a = gtk_application_new("c.sample", G_APPLICATION_NON_UNIQUE)' \
  'GtkApplicationWindow *w = gtk_application_window_new(a)' \
  'g_signal_connect (G_OBJECT(w), "destroy", G_CALLBACK (gtk_main_quit), NULL)' \
  'gtk_main()' -p

(c:2354): Gtk-CRITICAL **: 12:52:47.374: New application windows must be added after the GApplication::startup signal has been emitted.
^C
```

And this version works fine:

```
$ bin/cplr -P gtk+-3.0 -i gtk/gtk.h \
   'gtk_init(&argc, &argv)' \
   'GtkApplication *a = gtk_application_new("c.sample", G_APPLICATION_NON_UNIQUE)' \
   'if (!g_application_register(G_APPLICATION(a), NULL, NULL)) { g_error("Registration failed"); exit(1); }'  \
   'GtkWindow *w = gtk_application_window_new(a)' \
   'g_signal_connect (G_OBJECT(w), "destroy", G_CALLBACK (gtk_main_quit), NULL)' \
   'gtk_widget_show_all(w)' \
   'gtk_main()' \
   -p "$@"
\<app exits when window is closed\>
```

### Examples using Tcl

```
$ c -Ptcl -itcl/tcl.h \
   'int i, res' \
   'Tcl_Interp *interp' \
   'Tcl_FindExecutable(argv[0])' \
   'if(!(interp = Tcl_CreateInterp())) abort()' \
   'if(Tcl_Init(interp) != TCL_OK) abort()' \
   'for(i = 1; i < argc; i++) { printf("> %s\n", argv[i]); if(Tcl_Eval(interp, argv[i]) != TCL_OK) abort(); }' \
   'Tcl_DeleteInterp(interp)' \
   'Tcl_Finalize()' \
   -- 'set a 123' 'puts $a'
> set a 123
> puts $a
123
$
```
