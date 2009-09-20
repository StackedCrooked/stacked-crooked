#ifndef NATIVECOMPONENT_H_INCLUDED
#define NATIVECOMPONENT_H_INCLUDED


#include "Layout.h"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <windows.h>
#include <string>
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

        bool expansive() const;

        virtual void move(int x, int y, int w, int h);

        void setOwningElement(Element * inElement);

        Element * owningElement() const;

        NativeComponent * parent() const;

        HWND handle() const;

        virtual void rebuildLayout();

        virtual void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

        bool getAttribute(const std::string & inName, std::string & outValue);

        bool setAttribute(const std::string & inName, const std::string & inValue);

        //virtual bool applyAttribute(const std::string & inName, const std::string & inValue);

    protected:
        NativeComponent * mParent;
        Element * mElement;
        HMODULE mModuleHandle;
        HWND mHandle;
        CommandId mCommandId;
        int mMinimumWidth;
        int mMinimumHeight;
        bool mExpansive;
        
        typedef boost::function<std::string()> AttributeGetter;
        typedef boost::function<void(const std::string &)> AttributeSetter;
        struct AttributeController
        {
            AttributeController(AttributeGetter & inGetter, AttributeSetter & inSetter) :
                getter(inGetter),
                setter(inSetter)
            {
            }
            AttributeGetter getter;
            AttributeSetter setter;
        };

        void setAttributeController(const std::string & inAttr, const AttributeController & inController);

        typedef std::map<std::string, AttributeController> AttributeControllers;
        AttributeControllers mAttributeControllers;

        typedef std::map<HWND, NativeComponent*> Components;
        static Components sComponentsByHandle;
    };


    class NativeWindow : public NativeComponent
    {
    public:
        static void Register(HMODULE inModuleHandle);

        NativeWindow(NativeComponentPtr inParent);

        void showModal();

        //virtual bool applyAttribute(const std::string & inName, const std::string & inValue);

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
        NativeButton(NativeComponentPtr inParent);

        virtual int minimumWidth() const;
    };


    class NativeLabel: public NativeControl
    {
    public:
        NativeLabel(NativeComponentPtr inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeTextBox : public NativeControl
    {
    public:
        NativeTextBox(NativeComponentPtr inParent);

        virtual int minimumWidth() const;

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
        NativeBox(NativeComponentPtr inParent, Orientation inOrientation = HORIZONTAL);

        virtual void rebuildLayout();

        void setOrientation(Orientation inOrientation);

        Orientation getOrientation();

        enum Align
        {
            Start,
            Center,
            End,
            Stretch
        };

        void setAlignment(Align inAlign);

        Align getAlignment() const;
    private:
        Orientation mOrientation;
        Align mAlign;
    };


    class NativeHBox : public NativeBox
    {
    public:
        NativeHBox(NativeComponentPtr inParent);
            
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

        virtual void move(int x, int y, int w, int h);

        void addMenuItem(const std::string & inText);

        void removeMenuItem(const std::string & inText);
    };


    class NativeSeparator : public NativeControl
    {
    public:
        NativeSeparator(NativeComponentPtr inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeMenuButton : public NativeControl
    {
    public:
        NativeMenuButton(NativeComponentPtr inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };

} // namespace XULWin


#endif // NATIVECOMPONENT_H_INCLUDED
