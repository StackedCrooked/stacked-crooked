#ifndef NATIVECOMPONENT_H_INCLUDED
#define NATIVECOMPONENT_H_INCLUDED


#include "Layout.h"
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
        NativeComponent(NativeComponentPtr inParent, CommandID inCommandID, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle);

        virtual ~NativeComponent();

        int minimumWidth() const;

        int minimumHeight() const;

        void setOwningElement(Element * inElement);

        Element * owningElement() const;

        NativeComponentPtr parent() const;

        HWND handle() const;

        virtual void rebuildLayout();

        virtual void rebuildChildLayouts();

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        virtual void applyAttribute(const std::string & inName, const std::string & inValue);

    protected:

        NativeComponentWPtr mParent;
        Element * mElement;
        int mMinimumWidth;
        int mMinimumHeight;

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

        void showModal();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        void applyAttribute(const std::string & inName, const std::string & inValue);
    };


    class NativeControl : public NativeComponent
    {
    public:
        NativeControl(NativeComponentPtr inParent, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle) :
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


    class NativeLabel: public NativeControl
    {
    public:
        NativeLabel(NativeComponentPtr inParent);

        virtual void applyAttribute(const std::string & inName, const std::string & inValue);
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


    class NativeBox : public NativeControl
    {
    public:
        NativeBox(NativeComponentPtr inParent, Orientation inOrientation = HORIZONTAL) :
            NativeControl(inParent,
                          TEXT("STATIC"),
                          0, // exStyle
                          0),
            mOrientation(inOrientation)
        {
        }

        virtual void rebuildLayout();

        virtual void applyAttribute(const std::string & inName, const std::string & inValue);

        Orientation mOrientation;
    };


    class NativeHBox : public NativeBox
    {
    public:
        NativeHBox(NativeComponentPtr inParent) :
            NativeBox(inParent, HORIZONTAL)
        {
        }
    };


    class NativeVBox : public NativeBox
    {
    public:
        NativeVBox(NativeComponentPtr inParent) :
            NativeBox(inParent, VERTICAL)
        {
        }
    };


    class NativeMenuList : public NativeControl
    {
    public:
        NativeMenuList(NativeComponentPtr inParent) :
            NativeControl(inParent,
                          TEXT("COMBOBOX"),
                          0, // exStyle
                          CBS_DROPDOWNLIST)
        {
        }

        void addMenuItem(const std::string & inText);

        void removeMenuItem(const std::string & inText);
    };

} // namespace XULWin


#endif // NATIVECOMPONENT_H_INCLUDED
