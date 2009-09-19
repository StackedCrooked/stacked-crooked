#ifndef NATIVECOMPONENT_H_INCLUDED
#define NATIVECOMPONENT_H_INCLUDED


#include "Layout.h"
#include <boost/shared_ptr.hpp>
#include <windows.h>
#include <map>


namespace XULWin
{
    class CommandId
    {
    public:
        CommandId() : mId(sId++) {}

        CommandId(int inId) : mId(inId) {}

        int intValue() const { return mId; }

    private:
        int mId;
        static int sId;
    };


    class Element;
    class NativeComponent;
    typedef boost::shared_ptr<NativeComponent> NativeComponentPtr;

    /**
     * NativeComponent is base class for all native UI elements.
     */
    class NativeComponent
    {
    public:
        NativeComponent(NativeComponentPtr inParent, CommandId inCommandId);

        virtual ~NativeComponent() = 0;

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        void setOwningElement(Element * inElement);

        Element * owningElement() const;

        NativeComponent * parent() const;

        HWND handle() const;

        virtual void rebuildLayout();

        virtual void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

        virtual bool applyAttribute(const std::string & inName, const std::string & inValue);

    protected:

        NativeComponent * mParent;
        Element * mElement;
        HMODULE mModuleHandle;
        HWND mHandle;
        CommandId mCommandId;
        int mMinimumWidth;
        int mMinimumHeight;

        typedef std::map<HWND, NativeComponent*> Components;
        static Components sComponentsByHandle;
    };


    class NativeWindow : public NativeComponent
    {
    public:
        static void Register(HMODULE inModuleHandle);

        NativeWindow(NativeComponentPtr inParent);

        void showModal();

        bool applyAttribute(const std::string & inName, const std::string & inValue);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
    };


    class NativeControl : public NativeComponent
    {
    public:
        NativeControl(NativeComponentPtr inParent, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle);

        virtual ~NativeControl();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        /**
         * Override this method if you want your control to handle its own command events.
         * (Normally the parent control handles them through the WM_COMMAND message.)
         */
        virtual void handleCommand(WPARAM wParam, LPARAM lParam) {}

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        WNDPROC mOrigProc;

    protected:
        typedef std::map<int, NativeControl*> ControlsById;
        static ControlsById sControlsById;
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

        virtual bool applyAttribute(const std::string & inName, const std::string & inValue);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeTextBox : public NativeControl
    {
    public:
        NativeTextBox(NativeComponentPtr inParent);

        virtual bool applyAttribute(const std::string & inName, const std::string & inValue);

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);
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

        virtual bool applyAttribute(const std::string & inName, const std::string & inValue);

        Orientation mOrientation;
    };


    class NativeHBox : public NativeBox
    {
    public:
        NativeHBox(NativeComponentPtr inParent) :
            NativeBox(inParent, HORIZONTAL)
        {
        }
            
        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeVBox : public NativeBox
    {
    public:
        NativeVBox(NativeComponentPtr inParent) :
            NativeBox(inParent, VERTICAL)
        {
        }
            
        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
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
