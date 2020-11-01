#!/bin/sh

c -P gtk+-3.0 -i gtk/gtk.h \
  'gtk_init(&argc, &argv)' \
  'GtkApplication *a = gtk_application_new("c.sample", G_APPLICATION_NON_UNIQUE)' \
  'GtkWindow *w = gtk_application_window_new(a)' \
  'gtk_main()' -p
