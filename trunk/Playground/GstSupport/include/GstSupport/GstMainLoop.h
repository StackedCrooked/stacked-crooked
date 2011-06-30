#ifndef GSTMAINLOOP_H_INCLUDED
#define GSTMAINLOOP_H_INCLUDED


extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}
#include <boost/noncopyable.hpp>


namespace Gst
{

    /**
     * MainLoop
     *
     * Helper class for setting up and starting a GMainLoop instance.
     *
     * NOTE:
     * I have noticed crashes when starting a GMainLoop instance in a separate thread.
     * The GMainLoop is probably meant to be used in the main application thread.
     * So if your application already has a main loop (for example in GUI application)
     * then you should not use this class.
     *
     * A workaround is to start a (non-threaded) timer and check for
     * bus messages using these calls:
     * - gst_bus_have_pending
     * - gst_bus_peek
     * - gst_bus_pop
     *
     */
    class MainLoop : boost::noncopyable
    {
    public:
        MainLoop();

        virtual ~MainLoop();

        /**
         * run
         *
         * Starts the main message loop.
         * This call will block program flow until a quit message has been received.
         */
        virtual void run();

        virtual void quit();

    private:
        GMainLoop * mMainLoop;
    };

} // namespace Gst


#endif // GSTMAINLOOP_H_INCLUDED
