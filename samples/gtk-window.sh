#!/bin/sh

bin/cplr -P gtk+-3.0 -i gtk/gtk.h \
-b 'gtk_init(&argc, &argv)' \
   'GtkWindow *w = gtk_window_new(GTK_WINDOW_TOPLEVEL)' \
   'g_signal_connect (G_OBJECT(w), "destroy", G_CALLBACK (gtk_main_quit), "window")' \
   'gtk_widget_show_all(w)' \
   'gtk_main()' \
   -p "$@"
