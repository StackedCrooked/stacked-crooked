#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED


#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <windows.h>
#include <map>


namespace XULWin
{
    class CommandID
    {
    public:
        CommandID() : mID(sID++) {}

        CommandID(int inID) : mID(inID) {}

        int intValue() const { return mID; }

    private:
        int mID;
        static int sID;
    };


    class Element;
    class NativeComponent;
    typedef boost::shared_ptr<NativeComponent> NativeComponentPtr;
    typedef boost::weak_ptr<NativeComponent> NativeComponentWPtr;

    /**
     * NativeComponent is base class for all native UI elements.
     */
    class NativeComponent
    {
    public:
        NativeComponent(NativeComponentPtr inParent, CommandID inCommandID, LPCWSTR inClassName, DWORD inExStyle, DWORD inStyle);

        virtual ~NativeComponent();

        void setOwningElement(Element * inElement);

        Element * owningElement() const;

        NativeComponentPtr parent() const;

        HWND handle() const;

        virtual void rebuildLayout();

        virtual void rebuildChildLayouts();

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

        virtual LRESULT handleMessage(UINT inMessaage, WPARAM wParam, LPARAM lParam);

    protected:
        NativeComponentWPtr mParent;
        Element * mElement;

    private:
        typedef std::map<HWND, NativeComponent*> Components;
        static Components sComponents;
        CommandID mCommandID;
        HMODULE mModuleHandle;
        HWND mHandle;
    };


    class NativeWindow : public NativeComponent
    {
    public:
        static void Register(HMODULE inModuleHandle);

        NativeWindow() :
            NativeComponent(
                NativeComponentPtr(),
                CommandID(0), // command id is hmenu id for Window
                TEXT("XULWin::Window"),
                0, // exStyle
                WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
            )
        {
        }

        virtual LRESULT handleMessage(UINT inMessaage, WPARAM wParam, LPARAM lParam);
    };


    class NativeControl : public NativeComponent
    {
    public:
        NativeControl(NativeComponentPtr inParent, LPCWSTR inClassName, DWORD inExStyle, DWORD inStyle) :
            NativeComponent(inParent,
                            CommandID(),
                            inClassName,
                            inExStyle,
                            inStyle | WS_TABSTOP
                                    | WS_CHILD
                                    | WS_CLIPSIBLINGS
                                    | WS_CLIPCHILDREN
                                    | WS_VISIBLE)
        {
        }
    };


    class NativeButton : public NativeControl
    {
    public:
        NativeButton(NativeComponentPtr inParent) :
            NativeControl(inParent,
                          TEXT("BUTTON"),
                          0, // exStyle
                          BS_PUSHBUTTON)
        {
        }
    };


    class NativeCheckBox : public NativeControl
    {
    public:
        NativeCheckBox(NativeComponentPtr inParent) :
            NativeControl(inParent,
                          TEXT("BUTTON"),
                          0, // exStyle
                          BS_AUTOCHECKBOX)
        {
        }
    };


    class NativeHBox : public NativeControl
    {
    public:
        NativeHBox(NativeComponentPtr inParent) :
            NativeControl(inParent,
                          TEXT("STATIC"),
                          0, // exStyle
                          0)
        {
        }

        virtual void rebuildLayout();

        virtual LRESULT handleMessage(UINT inMessaage, WPARAM wParam, LPARAM lParam);
    };

} // namespace XULWin


#endif // WINDOW_H_INCLUDED
