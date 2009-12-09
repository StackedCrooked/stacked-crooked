#include <gst/gst.h>
#include <gtk/gtk.h>
#include <gst/interfaces/xoverlay.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

gint main(gint argc, gchar *argv[])
{
    gst_init(&argc, &argv);
    gtk_init(&argc, &argv);
    return 0;
}