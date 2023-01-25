## cplr examples

### Using libc

A small implementation of the *realpath* utitlity:
```
$ cplr -o realpath \
   'char *res' \
   'if(argc != 2) { printf("Usage: %s <path>\n", argv[0]); return 1; }' \
   'res = realpath(argv[1], NULL)' \
   'if(!res) { perror("realpath"); return 1; }' \
   'puts(res)' \
   'free(res)'
$ ./realpath /bin/ls
/usr/bin/ls
```

### Using Python

An example of embedding Python 3:
```
$ cplr -P python3-embed -i Python.h \
   'Py_Initialize()' \
   'for (int i = 1; i < argc; i++) PyRun_SimpleString(argv[i])' \
   'Py_Finalize()' \
      -- \
      'import os' \
      'print(os.getuid())'
1000
```

### Using Tcl

An example of embedding Tcl:
```
$ cplr -P tcl -i tcl/tcl.h \
   'int i, res' \
   'Tcl_Interp *interp' \
   'Tcl_FindExecutable(argv[0])' \
   'if(!(interp = Tcl_CreateInterp())) abort()' \
   'if(Tcl_Init(interp) != TCL_OK) abort()' \
   'for(i = 1; i < argc; i++) { printf("> %s\n", argv[i]); if(Tcl_Eval(interp, argv[i]) != TCL_OK) abort(); }' \
   'Tcl_DeleteInterp(interp)' \
   'Tcl_Finalize()' \
      -- \
      'set a 123' \
      'puts $a'
> set a 123
> puts $a
123
```

### Using Gtk

It's quite possible to build simple GUI programs using *cplr*:
```
$ cplr -P gtk+-3.0 -i gtk/gtk.h \
-t 'int clicks = 0' \
-t 'void clicked(GtkWidget *w, char *s) { g_message("Click %d on widget %s.", ++clicks, s); }' \
-t 'void closed(GtkWidget *w, char *s) { g_message("Closed by widget %s.", s); gtk_main_quit(); }' \
   'gtk_init(&argc, &argv)' \
   'GtkWindow *w = gtk_window_new(GTK_WINDOW_TOPLEVEL)' \
   'GtkButton *b = gtk_button_new()' \
   'gtk_button_set_label(b, "Click me!")' \
   'gtk_container_add (GTK_CONTAINER (w), b)' \
   'g_signal_connect (G_OBJECT(b), "clicked", G_CALLBACK (clicked), "button")' \
   'g_signal_connect (G_OBJECT(w), "destroy", G_CALLBACK (closed), "window")' \
   'gtk_widget_show_all(w)' \
   'gtk_main()'
\<window pops up\>
** Message: 06:40:30.615: Click 1 on widget button.
** Message: 06:40:31.002: Click 2 on widget button.
** Message: 06:40:31.396: Click 3 on widget button.
** Message: 06:40:31.839: Closed by widget window.
\<window closed by user\>
```

Doing this with modern GTK provides decent error messages:
```
$ cplr -P gtk+-3.0 -i gtk/gtk.h \
   'gtk_init(&argc, &argv)' \
   'GtkApplication *a = gtk_application_new("c.sample", G_APPLICATION_NON_UNIQUE)' \
   'GtkApplicationWindow *w = gtk_application_window_new(a)' \
   'g_signal_connect (G_OBJECT(w), "destroy", G_CALLBACK (gtk_main_quit), NULL)' \
   'gtk_main()'

(c:2354): Gtk-CRITICAL **: 12:52:47.374: New application windows must be added after the GApplication::startup signal has been emitted.
^C
```

And this version works fine:
```
$ cplr -P gtk+-3.0 -i gtk/gtk.h \
   'gtk_init(&argc, &argv)' \
   'GtkApplication *a = gtk_application_new("c.sample", G_APPLICATION_NON_UNIQUE)' \
   'if (!g_application_register(G_APPLICATION(a), NULL, NULL)) { g_error("Registration failed"); exit(1); }'  \
   'GtkWindow *w = gtk_application_window_new(a)' \
   'g_signal_connect (G_OBJECT(w), "destroy", G_CALLBACK (gtk_main_quit), NULL)' \
   'gtk_widget_show_all(w)' \
   'gtk_main()'
\<app exits when window is closed\>
```
