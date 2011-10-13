#include "Gst/Support.h"
#include "Gst/BusListener.h"
#include "Gst/MainLoop.h"
#include <boost/bind.hpp>
#include <iostream>
#include <stdexcept>


static gboolean OnBusMessage(GstBus * bus, GstMessage * msg, Gst::MainLoop * loop)
{
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
        {
            g_print("End of stream\n");
            loop->quit();
            break;
        }
        case GST_MESSAGE_ERROR:
        {
            std::cout << Gst::Logging::GetLogMessage(msg) << std::endl;
            loop->quit();
            break;
        }
        default:
        {
            break;
        }
    }
    return TRUE;
}


static void on_pad_added(GstElement * element, GstPad * pad, gpointer data)
{
    GstElement * decoder = (GstElement *) data;    
    Gst::ScopedObject<GstPad> sinkpad(Gst::Element::GetStaticPad(decoder, "sink"));       

    // We can now link this pad with the vorbis-decoder sink pad.
    std::cout << "Dynamic pad created, linking demuxer/decoder" << std::endl;    

    Gst::Pad::Link(pad, sinkpad);    
}


void run(const std::string & inFileName)
{
    Gst::MainLoop loop;

    // Create gstreamer elements
    Gst::ScopedObject<GstElement> pipeline(Gst::Pipeline::Create("audio-player"));
    Gst::ScopedObject<GstElement> source(Gst::Element::Create(pipeline, "filesrc", "file-source"));
    Gst::ScopedObject<GstElement> demuxer(Gst::Element::Create(pipeline, "oggdemux",      "ogg-demuxer"));
    Gst::ScopedObject<GstElement> decoder(Gst::Element::Create(pipeline, "vorbisdec",     "vorbis-decoder"));
    Gst::ScopedObject<GstElement> conv(Gst::Element::Create(pipeline, "audioconvert",  "converter"));
    Gst::ScopedObject<GstElement> sink(Gst::Element::Create(pipeline, "autoaudiosink", "audio-output"));

    // Set up the pipeline    

    // We set the input filename to the source element
    g_object_set(G_OBJECT(source.get()), "location", inFileName.c_str(), NULL);

    // we link the elements together
    // file-source -> ogg-demuxer ~> vorbis-decoder -> converter -> alsa-output
    Gst::Element::Link(source, demuxer);
    Gst::Element::Link(decoder, conv, sink);
    
    
    // we add a message handler    
    Gst::ScopedBusListener busListener(pipeline.get(), boost::bind(&OnBusMessage, _1, _2, &loop));


    // Note that the demuxer will be linked to the decoder dynamically.
    // The reason is that Ogg may contain various streams (for example
    // audio and video). The source pad(s) will be created at run time,
    // by the demuxer when it detects the amount and nature of streams.
    // Therefore we connect a callback function which will be executed
    // when the "pad-added" is emitted.
    
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), decoder);

    // Set the pipeline to "playing" state
    std::cout << "Now playing: " << inFileName << std::endl;    
    Gst::Pipeline::SetState(pipeline, GST_STATE_PLAYING);
    
    // Start the event loop
    std::cout << "Running..." << std::endl;
    loop.run();

    // Out of the main loop, clean up nicely
    std::cout << "Returned, stopping playback" << std::endl;

    Gst::Element::SetState(pipeline, GST_STATE_NULL);

    std::cout << "Deleting pipeline" << std::endl;
    // Auto cleanup
}



int main(int argc, char * argv[])
{
    // Check input arguments
    if (argc != 2)
    {
        g_printerr("Usage: %s <Ogg/Vorbis filename>\n", argv[0]);
        return -1;
    }
    
    // Initialisation
    gst_init(&argc, &argv);    
    
    try
    {
        run(argv[1]);
    }
    catch (const std::exception & exc)
    {
        std::cerr << exc.what() << std::endl;
    }
    return 0;
}
