#include "Gst/BusListener.h"
#include "Gst/MainLoop.h"
#include "Gst/Support.h"
#include <boost/bind.hpp>
#include <iostream>
#include <stdexcept>


using namespace Gst;


static gboolean on_bus_message(GstBus * bus, GstMessage * msg, MainLoop * loop)
{
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
        {
            std::cout << "End of stream" << std::endl;
            loop->quit();
            break;
        }
        case GST_MESSAGE_ERROR:
        {
            std::cout << Logging::GetLogMessage(msg) << std::endl;
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
    ScopedObject<GstPad> sinkpad(Element::GetStaticPad(decoder, "sink"));       

    // We can now link this pad with the vorbis-decoder sink pad.
    std::cout << "Dynamic pad created, linking demuxer/decoder" << std::endl;    

    Pad::Link(pad, sinkpad);    
}


void run(const std::string & inFileName)
{
    // Create a main loop object.
    MainLoop loop;
    
    // Create gstreamer elements
    ScopedObject<GstElement> pipeline(Pipeline::Create("audio-player"));
    ScopedObject<GstElement> source(Element::Create(pipeline, "filesrc", "file-source"));
    ScopedObject<GstElement> demuxer(Element::Create(pipeline, "oggdemux",      "ogg-demuxer"));
    ScopedObject<GstElement> decoder(Element::Create(pipeline, "vorbisdec",     "vorbis-decoder"));
    ScopedObject<GstElement> conv(Element::Create(pipeline, "audioconvert",  "converter"));
    ScopedObject<GstElement> sink(Element::Create(pipeline, "autoaudiosink", "audio-output"));
    
    // We set the input filename to the source element
    g_object_set(G_OBJECT(source.get()), "location", inFileName.c_str(), NULL);
    
    // Link the first half
    Element::Link(source, demuxer);

    // Link the second half
    Element::Link(decoder, conv, sink);
    
    // Note that the demuxer will be linked to the decoder dynamically.
    // The reason is that Ogg may contain various streams (for example
    // audio and video). The source pad(s) will be created at run time,
    // by the demuxer when it detects the amount and nature of streams.
    // Therefore we connect a callback function which will be executed
    // when the "pad-added" is emitted.    
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), decoder);
    
    // We add a message handler. Also pass the loop as an extra argument.
    ScopedBusListener busListener(pipeline.get(), boost::bind(&on_bus_message, _1, _2, &loop));

    // Set the pipeline to "playing" state
    std::cout << "Now playing: " << inFileName << std::endl;    
    Pipeline::SetState(pipeline, GST_STATE_PLAYING);
    
    // Start the event loop
    std::cout << "Running..." << std::endl;
    loop.run();
    
    // Out of the main loop, clean up nicely
    std::cout << "Returned, stopping playback" << std::endl;
    
    // Stop the pipeline
    Element::SetState(pipeline, GST_STATE_NULL);
    
    std::cout << "Deleting pipeline" << std::endl;
    // ScopedObject performs cleanup in its destructor.
}


int main(int argc, char * argv[])
{
    // Check input arguments
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <Ogg/Vorbis filename>" << std::endl;
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

