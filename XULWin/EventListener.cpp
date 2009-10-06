#include "EventListener.h"
#include <windows.h>


namespace XULWin
{


    ScopedEventListener::ScopedEventListener() :
        mCallbacks(new Callbacks)
    {
    }


    void ScopedEventListener::removeAction(Element * inEl)
    {
        Callbacks::iterator it = mCallbacks->find(inEl);
        if (it != mCallbacks->end())
        {
            it->first->removeEventListener(this);
            mCallbacks->erase(it);
        }
    }


    ScopedEventListener::~ScopedEventListener()
    {
        while (!mCallbacks->empty())
        {
            removeAction(mCallbacks->begin()->first);
        }
    }


    void ScopedEventListener::setAction(Element * inEl, const Action & inAction)
    {
        inEl->addEventListener(this);
        (*mCallbacks)[inEl] = inAction;
    }


    void ButtonListener::handleCommand(Element * inSender, unsigned short inNotificationCode)
    {
        Callbacks::iterator it = mCallbacks->find(inSender);
        if (it != mCallbacks->end() && it->second)
        {
            (*it).second();
        }
    }

} // namespace XULWin
