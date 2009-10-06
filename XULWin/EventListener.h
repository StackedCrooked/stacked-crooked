#ifndef EVENTLISTENER_H_INCLUDED
#define EVENTLISTENER_H_INCLUDED


#include "Element.h"
#include <windows.h>


namespace XULWin
{


    class EventListener
    {
    public:
        virtual void handleCommand(Element * inSender, unsigned short inNotificationCode) = 0;
        
        virtual void handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;
    };


    class ScopedEventListener : public EventListener
    {
    public:
        ScopedEventListener();

        virtual ~ScopedEventListener();

        typedef boost::function<void()> Action;

        void setAction(Element * inEl, const Action & inAction);

        void removeAction(Element * inEl);

    private:
        virtual void handleCommand(Element * inSender, unsigned short inNotificationCode) = 0;

        virtual void handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

    protected:
        typedef std::map<Element*, Action> Callbacks;

        // because we have a destructor we need a copy ctor and assignment operator
        // wrapping our only member in a boost::shared_ptr takes care of that
        boost::shared_ptr<Callbacks> mCallbacks;
    };


    class ButtonListener : public ScopedEventListener
    {
    private:
        virtual void handleCommand(Element * inSender, unsigned short inNotificationCode);

        virtual void handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam){}
    };


} // namespace XULWin

#endif // EVENTLISTENER_H_INCLUDED
