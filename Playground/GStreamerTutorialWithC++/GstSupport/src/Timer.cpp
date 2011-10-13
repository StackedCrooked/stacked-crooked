#include "Gst/Timer.h"
#include "Gst/Support.h"
#include <assert.h>
extern "C"
{
    #include <gst/interfaces/xoverlay.h>
}


namespace Gst
{
	
	Timer::Instances Timer::sInstances;
    
	Timer::Timer(guint inIntervalMs, const Action & inAction) :
		mAction(inAction)
    {
		g_timeout_add(inIntervalMs, &Timer::OnTimeout, this);
		sInstances.insert(this);
    }
    
    
    Timer::~Timer()
    {
		sInstances.erase(this);
    }


	gboolean Timer::OnTimeout(gpointer inData)
	{
		Instances::iterator it = sInstances.find(static_cast<Timer*>(inData));
		if (it != sInstances.end())
		{
			Timer * pThis = static_cast<Timer*>(inData);
			return pThis->onTimeout();
		}

		// If Timer instance was not found, then stop the timer.
		return FALSE;
	}


	gboolean Timer::onTimeout()
	{
		if (mAction)
		{
			return mAction();
		}
		return FALSE;
	}


} // namespace Gst
