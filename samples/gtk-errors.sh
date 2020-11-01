#!/bin/sh

bin/cplr -P gtk+-3.0 -i gtk/gtk.h \
  'gtk_init(&argc, &argv)' \
  'GtkApplication *a = gtk_application_new("c.sample", G_APPLICATION_NON_UNIQUE)' \
  'GtkApplicationWindow *w = gtk_application_window_new(a)' \
  'g_signal_connect (G_OBJECT(w), "destroy", G_CALLBACK (gtk_main_quit), NULL)' \
  'gtk_main()' -p
