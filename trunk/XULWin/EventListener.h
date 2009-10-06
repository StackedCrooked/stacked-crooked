#ifndef EVENTLISTENER_H_INCLUDED
#define EVENTLISTENER_H_INCLUDED


#include "Element.h"
#include <windows.h>


namespace XULWin
{


    class EventListener
    {
    public:
        virtual void handleCommand(Element * inSender, WORD inNotificationCode) = 0;        
        
        virtual void handleDialogCommand(Element * inSender, WORD inNotificationCode, WPARAM wParam, LPARAM lParam) = 0;

        virtual void handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;
    };


    class ScopedEventListener : public EventListener,
                                boost::noncopyable
    {
    public:
        ScopedEventListener();

        virtual ~ScopedEventListener();

        typedef boost::function<void(WPARAM, LPARAM)> Action;

        void connect(Element * inEl, const Action & inAction);

        void disconnect(Element * inEl);

        void connect(Element * inEl, UINT inMessage, const Action & inAction);

        void disconnect(Element * inEl, UINT inMessage);

    protected:
        virtual void handleCommand(Element * inSender, WORD inNotificationCode);
        virtual void handleDialogCommand(Element * inSender, WORD inNotificationCode, WPARAM wParam, LPARAM lParam){}
        virtual void handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam);

        void processMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam);

        typedef std::pair<Element*, UINT> MsgId;
        typedef std::map<MsgId, std::vector<Action> > MessageCallbacks;
        MessageCallbacks mMessageCallbacks;
    };


} // namespace XULWin

#endif // EVENTLISTENER_H_INCLUDED
