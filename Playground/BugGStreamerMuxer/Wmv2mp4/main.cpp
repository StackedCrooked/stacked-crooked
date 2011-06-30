extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}
#include <assert.h>


GstElement* gDecodebin = 0;
GstElement* gPipeline = 0;
GstElement* gMP4Mux = 0;
GMainLoop * gMainLoop = 0;


void linkAudioPad(GstPad* decodedPad)
{
    GstElement* faac = gst_element_factory_make("faac", NULL);
    assert(faac);

    gst_bin_add(GST_BIN(gPipeline), faac);

    GstPad* faac_sinkpad = gst_element_get_static_pad(faac, "sink");
    assert(faac_sinkpad);
    gst_pad_link(decodedPad, faac_sinkpad);

    GstPad* faac_srcpad = gst_element_get_static_pad(faac, "src");
    assert(faac_srcpad);

    GstPad* mp4mux_sinkpad = gst_element_get_request_pad(gMP4Mux, "audio_%d");
    assert(mp4mux_sinkpad); //!! FAILS HERE !!
    gst_pad_link(faac_srcpad, mp4mux_sinkpad);
}


void linkVideoPad(GstPad* decodedPad)
{
    GstElement* x264enc = gst_element_factory_make("x264enc", NULL);
    assert(x264enc);

    gst_bin_add(GST_BIN(gPipeline), x264enc);

    GstPad* x264_sinkpad = gst_element_get_static_pad(x264enc, "sink");
    assert(x264_sinkpad);

    gst_pad_link(decodedPad, x264_sinkpad);

    GstPad* x264_srcpad = gst_element_get_static_pad(x264enc, "src");
    assert(x264_srcpad);

    GstPad* mp4mux_sinkpad = gst_element_get_request_pad(gMP4Mux, "video_%d");
    assert(mp4mux_sinkpad); // !! FAILS HERE !!

    gst_pad_link(x264_srcpad, mp4mux_sinkpad);
}


bool isAudioPad(GstPad * pad)
{
    GstCaps * caps = gst_pad_get_caps(pad);
    const gchar * capsName = gst_structure_get_name(gst_caps_get_structure(caps, 0));
    return NULL != g_strrstr(capsName, "audio");
}


bool isVideoPad(GstPad * pad)
{
    GstCaps * caps(gst_pad_get_caps(pad));
    const gchar * capsName = gst_structure_get_name(gst_caps_get_structure(caps, 0));
    return NULL != g_strrstr(capsName, "video");
}


static void cbNewPad(GstElement* decodebin,
                     GstPad* decodedPad,
                     gboolean last,
                     gpointer data)
{
    if (isAudioPad(decodedPad))
    {
        linkAudioPad(decodedPad);
    }
    else if (isVideoPad(decodedPad))
    {
        linkVideoPad(decodedPad);
    }
    else
    {
        g_printerr("New decoded pad is neither of type audio or video.");
    }
}


static gboolean onBusMessage(GstBus* bus, GstMessage* msg, gpointer data)
{
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
        {
            g_print("End of stream\n");
            g_main_loop_quit(gMainLoop);
            break;
        }
        case GST_MESSAGE_INFO:
        {
            gchar* debug;
            GError* error;
            gst_message_parse_info(msg, &error, &debug);
            g_free(debug);
            g_printerr("Info: %s\n", error->message);
            g_error_free(error);
            break;
        }
        case GST_MESSAGE_WARNING:
        {
            gchar* debug;
            GError* error;
            gst_message_parse_warning(msg, &error, &debug);
            g_free(debug);
            g_printerr("Warning: %s\n", error->message);
            g_error_free(error);
            break;
        }
        case GST_MESSAGE_ERROR:
        {
            gchar* debug;
            GError* error;
            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);
            g_printerr("Error: %s\n", error->message);
            g_error_free(error);
            g_main_loop_quit(gMainLoop);
            break;
        }
        default:
        {
            break;
        }
    }
    return TRUE;
}


int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);
    if (argc != 2)
    {
        g_printerr("Usage: %s <input filename>\n", argv[0]);
        return -1;
    }

    GstElement* file = gst_element_factory_make("filesrc", 0);
    assert(file);

    g_object_set(G_OBJECT(file), "location", argv[1], NULL);
    
    gDecodebin = gst_element_factory_make("decodebin2", 0);
    assert(gDecodebin);

    gMP4Mux = gst_element_factory_make("mp4mux", 0);
    assert(gMP4Mux);

    GstElement* filesink = gst_element_factory_make("filesink", 0);
    assert(filesink);

    g_object_set(G_OBJECT(filesink), "location", "test.mp4", NULL);

    gPipeline = gst_pipeline_new(0);
    assert(gPipeline);

    gst_bin_add_many(GST_BIN(gPipeline), file, gDecodebin,  gMP4Mux, filesink, 0);
  
    if (!gst_element_link(file, gDecodebin))
    {
        assert(false);
    }

    if (!gst_element_link(gMP4Mux, filesink))
    {
        assert(false);
    }

    GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(gPipeline));
    gst_bus_add_watch(bus, onBusMessage, NULL);

    g_signal_connect(gDecodebin, "new-decoded-pad", G_CALLBACK(cbNewPad), NULL);
          
    gMainLoop = g_main_loop_new(NULL, FALSE);
    assert(gMainLoop);

    GstStateChangeReturn ret = gst_element_set_state(gPipeline, GST_STATE_PLAYING);
    assert(ret != GST_STATE_CHANGE_FAILURE);

    g_main_loop_run(gMainLoop);

    ret = gst_element_set_state(gPipeline, GST_STATE_NULL);
    assert(ret != GST_STATE_CHANGE_FAILURE);

    gst_object_unref(GST_OBJECT(gPipeline));
    return 0;
}
