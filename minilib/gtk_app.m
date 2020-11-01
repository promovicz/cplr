
#include <gtk/gtk.h>

#if defined(minilib_toplevel)
GtkApplication *app;
#elif defined(minilib_before)
g_message("Initializing gtk_app");
if(!gtk_init_check(&argc, &argv)) {
  g_error("Could not initialize GTK.");
}
app = gtk_application_new("cplr.program", G_APPLICATION_NON_UNIQUE);
if (!g_application_register(G_APPLICATION(app), NULL, NULL)) {
  g_error("Registration failed"); exit(1);
}
#elif defined(minilib_after)
g_message("Entering gtk_app");
gtk_main();
#endif
