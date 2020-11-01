
#include <gtk/gtk.h>

#if defined(minilib_toplevel)
GtkApplication *app;
GtkApplicationWindow *win;
#elif defined(minilib_before)
g_message("Initializing gtk_app_win");
if(!gtk_init_check(&argc, &argv)) {
  g_error("Could not initialize GTK.");
}
app = gtk_application_new("cplr.program", G_APPLICATION_NON_UNIQUE);
if (!g_application_register(G_APPLICATION(app), NULL, NULL)) {
  g_error("Registration failed"); exit(1);
}
win = gtk_application_window_new(app);
g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK (gtk_main_quit), "window");
#elif defined(minilib_after)
g_message("Entering gtk_app_win");
gtk_widget_show_all(win);
gtk_main();
#endif
