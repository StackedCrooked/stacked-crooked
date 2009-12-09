#include <gst/gst.h>
#include <gtk/gtk.h>
#include <gst/interfaces/xoverlay.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

static gboolean expose_cb(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(data), GDK_WINDOW_XWINDOW(widget->window));
    return TRUE;
}


static void makeWindowBlack(GtkWidget * window)
{
    GdkColor color;
    gdk_color_parse ("black", &color);
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);    // needed to ensure black background
}


static gboolean
key_press_event_cb(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    if (event->keyval != 'f')
        return TRUE;
    else 
        g_print("you hit f\n");

    gboolean isFullscreen = (gdk_window_get_state(GDK_WINDOW(widget->window)) == GDK_WINDOW_STATE_FULLSCREEN);

    if (isFullscreen) 
        gtk_window_unfullscreen(GTK_WINDOW(widget));
    else
        gtk_window_fullscreen(GTK_WINDOW(widget));

    return TRUE;
}


gint main (gint argc, gchar *argv[])
{
    GstStateChangeReturn ret;
    GstElement *pipeline, *src, *sink;
    GMainLoop *loop;
    GtkWidget *window;

    /* initialization */
    gst_init (&argc, &argv);
    gtk_init (&argc, &argv);

    loop = g_main_loop_new (NULL, FALSE);

    /* create elements */
    pipeline = gst_pipeline_new ("my_pipeline");

    src = gst_element_factory_make ("videotestsrc", NULL);

    sink = gst_element_factory_make("xvimagesink", "videosink");
    g_object_set(G_OBJECT(sink), "sync", FALSE, NULL);

    if (!sink)
        g_print ("output could not be found - check your install\n");

    gst_bin_add_many (GST_BIN (pipeline), src, sink, NULL);
    g_object_set(G_OBJECT(sink), "force-aspect-ratio", TRUE, NULL);

    /* link everything together */
    if (!gst_element_link(src, sink)) {
        g_print ("Failed to link one or more elements!\n");
        return -1;
    }

    // build window and attach expose event to expose callback
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "expose-event", G_CALLBACK(expose_cb), sink);

    // attach key press signal to key press callback
    gtk_widget_set_events(window, GDK_KEY_PRESS_MASK);
    g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(key_press_event_cb), sink);

    /* run */
    makeWindowBlack(window);
    gtk_widget_show_all(window);

    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_print ("Failed to start up pipeline!\n");
    }

    g_main_loop_run (loop);

    /* clean up */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

    return 0;
}

