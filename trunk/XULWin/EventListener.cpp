#include "EventListener.h"
#include <windows.h>


namespace XULWin
{


    ScopedEventListener::ScopedEventListener()
    {
    }


    void ScopedEventListener::removeCommand(Element * inEl)
    {
        Callbacks::iterator it = mCallbacks.find(inEl);
        if (it != mCallbacks.end())
        {
            it->first->removeEventListener(this);
            mCallbacks.erase(it);
        }
    }


    ScopedEventListener::~ScopedEventListener()
    {
        while (!mCallbacks.empty())
        {
            removeCommand(mCallbacks.begin()->first);
        }
        while (!mDialogCallbacks.empty())
        {
            removeDialogCommand(mDialogCallbacks.begin()->first);
        }
    }


    void ScopedEventListener::addCommand(Element * inEl, const CommandCallback & inAction)
    {
        inEl->addEventListener(this);
        mCallbacks[inEl].push_back(inAction);
    }

    
    void ScopedEventListener::addDialogCommand(Element * inEl, const DialogCommandCallback & inAction)
    {
        inEl->addEventListener(this);
        mDialogCallbacks[inEl].push_back(inAction);
    }

    
    void ScopedEventListener::removeDialogCommand(Element * inEl)
    {
        DialogCallbacks::iterator it = mDialogCallbacks.find(inEl);
        if (it != mDialogCallbacks.end())
        {
            it->first->removeEventListener(this);
            mDialogCallbacks.erase(it);
        }
    }


    void ScopedEventListener::handleCommand(Element * inSender, unsigned short inNotificationCode)
    {
        Callbacks::iterator it = mCallbacks.find(inSender);
        if (it != mCallbacks.end())
        {
            std::vector<CommandCallback> callbacks = it->second;
            for (size_t idx = 0; idx != callbacks.size(); ++idx)
            {
                if (callbacks[idx])
                {
                    callbacks[idx]();
                }
            }
        }
    }
    
    
    void ScopedEventListener::handleDialogCommand(Element * inSender, WORD inNotificationCode, WPARAM wParam, LPARAM lParam)
    {
        DialogCallbacks::iterator it = mDialogCallbacks.find(inSender);
        if (it != mDialogCallbacks.end())
        {
            std::vector<DialogCommandCallback> callbacks = it->second;
            for (size_t idx = 0; idx != callbacks.size(); ++idx)
            {
                if (callbacks[idx])
                {
                    callbacks[idx](static_cast<NotificationCode>(inNotificationCode));
                }
            }
        }      
    }

    
    void ScopedEventListener::handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
    }

} // namespace XULWin
