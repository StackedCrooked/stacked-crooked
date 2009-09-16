#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED


#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <windows.h>


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

        NativeComponentPtr parent() const;

        HWND handle() const;        

    private:
        NativeComponentWPtr mParent;
        CommandID mCommandID;
        HMODULE mModuleHandle;
        HWND mHandle;
    };


    class NativeWindow : public NativeComponent
    {
    public:
        static void Register(HMODULE inModuleHandle, WNDPROC inWndProc);

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

} // namespace XULWin


#endif // WINDOW_H_INCLUDED
