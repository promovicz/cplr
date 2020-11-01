#!/bin/sh

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

