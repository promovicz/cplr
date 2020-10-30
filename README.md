## c - Utility for running C code

### Features

 * Command-line utility for running C code directly
 * Fast with a bang, like a bomb
 * Will include most UNIX headers by default
 * Has error messages appropriate for command-line use
 * Can import any pkg-config package
 * Uses tcc/libtcc internally
 * Usual compiler options: -I \<incdir\>, -L \<libdir\>, -l \<lib\>
 * Extra convenience options: -i \<include\>, -S <sysincdir\>, -s \<sysinc\>
 * Two options for adding code: -e \<statement\>, -t \<toplevel\>
 * Flags: -v (verbose compiler), -d (dump code), -p (pristine env), -n (no execute)

### Why this tool exists

If you are a UNIX/C programmer like me then you know the feeling of needing to call a C function to try something out.

The next thing you would usually do is open an editor and type in the words tatooed to your eyelids:

```
#include <stdio.h>
int main(int argc, char **argv) {
	return 0;
}
```

After doing that for the millionths time in your life you could get back to actual problems.

But deep down in your heart you always wanted a C compiler in your shell, cause it would be so much easier.

One that integrates with UNIX natively, just as well as sed, awk, cut, tr and grep.

Well. It exists now:

```
$ c 'int i' \
    'for(i=0;i<argc;i++) { puts(argv[i]); }' \
  -- a b c
a
b
c
$ c -d \
    'int i' \
    'for(i=0;i<argc;i++) { puts(argv[i]); }' \
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
$ c -P python3 -i Python.h \
  'Py_Initialize()' \
  'PyRun_SimpleString("print(\"hello\")")' \
  'Py_Finalize()'
hello
$ c -P gtk+-3.0 -i gtk/gtk.h \
  'gtk_init(&argc, &argv)' \
  'GtkWindow *w = gtk_window_new(GTK_WINDOW_TOPLEVEL)' \
  'gtk_widget_show(w)' \
  'gtk_main()' -p
<window pops up>
$ c -P gtk+-3.0 -i gtk/gtk.h \
  'gtk_init(&argc, &argv)' \
  'GtkWindow *w = gtk_window_new(GTK_WINDOW_TOPLEVEL)' \
  'gtk_widget_show(w)' \
  'gtk_main()' -pd
     1	#include "gtk/gtk.h"
     2	int main(int argc, char **argv) {
     3	    int ret = 0;
     4	    gtk_init(&argc, &argv);
     5	    GtkWindow *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
     6	    gtk_widget_show(w);
     7	    gtk_main();
     8	    return ret;
     9	}
<window pops up>
```

If you need this tool then you shouldn't need instructions, so I'll leave that up to you for now.

And yes, this tool needs some additional features. Small features. Contributions welcome.

Happy hacking!

