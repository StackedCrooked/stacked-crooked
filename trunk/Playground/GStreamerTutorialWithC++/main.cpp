#include <gst/gst.h>
#include <glib.h>
#include "Gst/Support.h"


static gboolean
bus_call(GstBus * bus, GstMessage * msg, gpointer data)
{
    GMainLoop * loop = (GMainLoop *) data;

    switch (GST_MESSAGE_TYPE(msg))
    {

        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            g_main_loop_quit(loop);
            break;

        case GST_MESSAGE_ERROR:
        {
            gchar * debug;
            GError * error;

            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);

            g_printerr("Error: %s\n", error->message);
            g_error_free(error);

            g_main_loop_quit(loop);
            break;
        }
        default:
            break;
    }
    return TRUE;
}


static void
on_pad_added(GstElement * element,
             GstPad   *  pad,
             gpointer    data)
{
    GstPad * sinkpad;
    GstElement * decoder = (GstElement *) data;

    /* We can now link this pad with the vorbis-decoder sink pad */
    g_print("Dynamic pad created, linking demuxer/decoder\n");

    sinkpad = gst_element_get_static_pad(decoder, "sink");

    gst_pad_link(pad, sinkpad);

    gst_object_unref(sinkpad);
}



int main(int argc, char * argv[])
{


    /* Check input arguments */
    if (argc != 2)
    {
        g_printerr("Usage: %s <Ogg/Vorbis filename>\n", argv[0]);
        return -1;
    }
    
    // Initialisation
    gst_init(&argc, &argv);
       
    
    GMainLoop * loop;
    loop = g_main_loop_new(NULL, FALSE);

    // Create gstreamer elements
    Gst::ScopedObject<GstElement> pipeline(Gst::Pipeline::Create("audio-player"));
    Gst::ScopedObject<GstElement> source(Gst::Element::Create(pipeline, "filesrc", "file-source"));
    Gst::ScopedObject<GstElement> demuxer(Gst::Element::Create(pipeline, "oggdemux",      "ogg-demuxer"));
    Gst::ScopedObject<GstElement> decoder(Gst::Element::Create(pipeline, "vorbisdec",     "vorbis-decoder"));
    Gst::ScopedObject<GstElement> conv(Gst::Element::Create(pipeline, "audioconvert",  "converter"));
    Gst::ScopedObject<GstElement> sink(Gst::Element::Create(pipeline, "autoaudiosink", "audio-output"));
    
    // Set up the pipeline

    // we set the input filename to the source element    
    g_object_set(G_OBJECT(source.get()), "location", argv[1], NULL);

    // we add a message handler
    GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline.get()));
    gst_bus_add_watch(bus, bus_call, loop);
    gst_object_unref(bus);


    // we link the elements together
    // file-source -> ogg-demuxer ~> vorbis-decoder -> converter -> alsa-output */
    Gst::Element::Link(source, demuxer);
    Gst::Element::Link(decoder, conv, sink);

    g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), decoder);

    /* note that the demuxer will be linked to the decoder dynamically.
       The reason is that Ogg may contain various streams (for example
       audio and video). The source pad(s) will be created at run time,
       by the demuxer when it detects the amount and nature of streams.
       Therefore we connect a callback function which will be executed
       when the "pad-added" is emitted.*/


    /* Set the pipeline to "playing" state*/
    g_print("Now playing: %s\n", argv[1]);
    
    Gst::Pipeline::SetState(pipeline, GST_STATE_PLAYING);


    /* Iterate */
    g_print("Running...\n");
    
    
    g_main_loop_run(loop);


    /* Out of the main loop, clean up nicely */
    g_print("Returned, stopping playback\n");
    gst_element_set_state(pipeline, GST_STATE_NULL);

    g_print("Deleting pipeline\n");

    return 0;
}
