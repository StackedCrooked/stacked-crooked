#ifndef GSTTIMER_H_INCLUDED
#define GSTTIMER_H_INCLUDED


extern "C"
{
    #include <glib.h>
}
#include <set>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>


namespace Gst
{

    /**
     * Timer
     *
     * GLib Timer wrapper.
     * Requires that a main loop is running.
     */
    class Timer : boost::noncopyable
    {
    public:
		typedef boost::function<gboolean()> Action;

        Timer(guint inIntervalMs, const Action & inAction);

        ~Timer();

    private:
        static gboolean OnTimeout(gpointer inData);
        gboolean onTimeout();        
        Action mAction;
		typedef std::set<Timer*> Instances;
		static Instances sInstances;
    };

} // namespace Gst


#endif // GSTTIMER_H_INCLUDED
