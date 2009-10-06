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

        typedef boost::function<void()> CommandCallback;

        void addCommand(Element * inEl, const CommandCallback & inAction);

        void removeCommand(Element * inEl);

        enum NotificationCode
        {
            Ok = IDOK,
            Cancel = IDCANCEL,
            Abort = IDABORT,
            Retry = IDRETRY,
            Ignore = IDIGNORE,
            Yes = IDYES,
            No = IDNO,
            Help = IDHELP,
            TryAgain = IDTRYAGAIN,
            Continue = IDCONTINUE
        };

        typedef boost::function<void(NotificationCode)> DialogCommandCallback;

        void addDialogCommand(Element * inEl, const DialogCommandCallback & inAction);

        void removeDialogCommand(Element * inEl);

    private:
        virtual void handleCommand(Element * inSender, WORD inNotificationCode);
        virtual void handleDialogCommand(Element * inSender, WORD inNotificationCode, WPARAM wParam, LPARAM lParam);

        virtual void handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam);

    protected:
        typedef std::map<Element*, std::vector<CommandCallback> > Callbacks;
        Callbacks mCallbacks;

        typedef std::map<Element*, std::vector<DialogCommandCallback> > DialogCallbacks;
        DialogCallbacks mDialogCallbacks;
    };


} // namespace XULWin

#endif // EVENTLISTENER_H_INCLUDED
