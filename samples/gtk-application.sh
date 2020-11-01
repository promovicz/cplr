#!/bin/sh

bin/cplr -P gtk+-3.0 -i gtk/gtk.h \
-t 'int clicks = 0' \
   'gtk_init(&argc, &argv)' \
   'GtkApplication *a = gtk_application_new("c.sample", G_APPLICATION_NON_UNIQUE)' \
   'if (!g_application_register(G_APPLICATION(a), NULL, NULL)) { g_error("Registration failed"); exit(1); }'  \
   'GtkWindow *w = gtk_application_window_new(a)' \
   'g_signal_connect(G_OBJECT(w), "destroy", G_CALLBACK (gtk_main_quit), NULL)' \
   'gtk_widget_show_all(w)' \
   'gtk_main()' \
   -p "$@"
