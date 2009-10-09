#include "EventListener.h"
#include "Decorator.h"
#include "ElementImpl.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <windows.h>


namespace XULWin
{


    ScopedEventListener::ScopedEventListener()
    {
    }


    ScopedEventListener::~ScopedEventListener()
    {
        while (!mMessageCallbacks.empty())
        {
            const MsgId & id = mMessageCallbacks.begin()->first;
            id.first->removeEventListener(this);
            mMessageCallbacks.erase(mMessageCallbacks.begin());
        }
    }


    void ScopedEventListener::connect(Element * inEl, const Action & inAction)
    {
        connect(inEl, WM_COMMAND, inAction);
    }


    void ScopedEventListener::disconnect(Element * inEl)
    {
        disconnect(inEl, WM_COMMAND);
    }


    void ScopedEventListener::connect(Element * inEl, UINT inMessage, const Action & inAction)
    {
        inEl->addEventListener(this);
        mMessageCallbacks[std::make_pair(inEl, inMessage)].push_back(inAction);
    }

    
    void ScopedEventListener::disconnect(Element * inEl, UINT inMessage)
    {
        MessageCallbacks::iterator it = mMessageCallbacks.find(std::make_pair(inEl, inMessage));
        if (it != mMessageCallbacks.end())
        {
            it->first.first->removeEventListener(this);
            mMessageCallbacks.erase(it);
        }
    }


    LRESULT ScopedEventListener::processMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        MessageCallbacks::iterator it = mMessageCallbacks.find(std::make_pair(inSender, inMessage));
        if (it != mMessageCallbacks.end())
        {
            std::vector<Action> callbacks = it->second;
            for (size_t idx = 0; idx != callbacks.size(); ++idx)
            {
                if (callbacks[idx])
                {
                    return callbacks[idx](wParam, lParam);
                }
            }            
        }
        return 1;
    }

        
    LRESULT ScopedEventListener::handleCommand(Element * inSender, WORD inNotificationCode)
    {
        return processMessage(inSender, WM_COMMAND, 0, 0);
    }

    
    LRESULT ScopedEventListener::handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        return processMessage(inSender, inMessage, wParam, lParam);
    }


} // namespace XULWin
