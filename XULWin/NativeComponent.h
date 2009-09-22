#ifndef NATIVECOMPONENT_H_INCLUDED
#define NATIVECOMPONENT_H_INCLUDED


#include "Element.h"
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
        NativeComponent(NativeComponent * inParent, CommandId inCommandId);

        virtual ~NativeComponent() = 0;

        int commandId() const { return mCommandId.intValue(); }

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        bool expansive() const;

        virtual void move(int x, int y, int w, int h);

        virtual Rect clientRect() const = 0;

        void setOwningElement(Element * inElement);

        Element * owningElement() const;

        NativeComponent * parent() const;

        virtual HWND handle() const;

        virtual void rebuildLayout();

        virtual void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        bool getAttribute(const std::string & inName, std::string & outValue);

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool initAttributeControllers();

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

        NativeWindow(NativeComponent * inParent);

        void showModal();

        virtual void rebuildLayout();

        virtual Rect clientRect() const;

        virtual bool initAttributeControllers();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
    };


    class NativeControl : public NativeComponent
    {
    public:
        NativeControl(NativeComponent * inParent, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle);

        virtual ~NativeControl();

        virtual Rect clientRect() const;

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


    class VirtualControl : public NativeComponent
    {
    public:
        VirtualControl(NativeComponent * inParent);

        virtual ~VirtualControl(){}

        virtual Rect clientRect() const;
        
        virtual HWND handle() const;

        virtual void move(int x, int y, int w, int h);

    protected:
        Rect mRect;
    };


    class VirtualProxy : public VirtualControl
    {
    public:
        VirtualProxy(NativeComponent * inSubject);

        virtual ~VirtualProxy();
        
        virtual HWND handle() const;

        virtual void move(int x, int y, int w, int h);

        virtual bool initAttributeControllers();

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

    protected:
        NativeComponentPtr mSubject;
    };


    class PaddingProxy : public VirtualProxy
    {
    public:
        PaddingProxy(NativeComponent * inSubject);

        virtual ~PaddingProxy();

        virtual void move(int x, int y, int w, int h);

        int paddingTop() const;

        int paddingLeft() const;

        int paddingRight() const;

        int paddingBottom() const;

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeButton : public NativeControl
    {
    public:
        NativeButton(NativeComponent * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeLabel : public NativeControl
    {
    public:
        NativeLabel(NativeComponent * inParent);

        virtual bool initAttributeControllers();

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeDescription : public NativeControl
    {
    public:
        NativeDescription(NativeComponent * inParent);

        virtual bool initAttributeControllers();

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeTextBox : public NativeControl
    {
    public:
        NativeTextBox(NativeComponent * inParent);

        virtual bool initAttributeControllers();

        virtual int minimumWidth() const;

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);
    };


    class NativeCheckBox : public NativeControl
    {
    public:
        NativeCheckBox(NativeComponent * inParent) :
            NativeControl(inParent,
                          TEXT("BUTTON"),
                          0, // exStyle
                          BS_AUTOCHECKBOX)
        {
        }
    };


    class NativeBox : public VirtualControl
    {
    public:
        NativeBox(NativeComponent * inParent, Orientation inOrientation = HORIZONTAL);

        virtual bool initAttributeControllers();

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
        NativeHBox(NativeComponent * inParent);
            
        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeVBox : public NativeBox
    {
    public:
        NativeVBox(NativeComponent * inParent) :
            NativeBox(inParent, VERTICAL)
        {
        }
            
        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeMenuList : public NativeControl
    {
    public:
        NativeMenuList(NativeComponent * inParent) :
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
        NativeSeparator(NativeComponent * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeSpacer : public VirtualControl
    {
    public:
        NativeSpacer(NativeComponent * inParent);
    };


    class NativeMenuButton : public NativeControl
    {
    public:
        NativeMenuButton(NativeComponent * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeGrid : public VirtualControl
    {
    public:
        NativeGrid(NativeComponent * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        virtual void rebuildLayout();
    };


    class NativeRows : public VirtualControl
    {
    public:
        NativeRows(NativeComponent * inParent);
    };


    class NativeRow : public VirtualControl
    {
    public:
        NativeRow(NativeComponent * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeColumns : public VirtualControl
    {
    public:
        NativeColumns(NativeComponent * inParent);
    };


    class NativeColumn : public VirtualControl
    {
    public:
        NativeColumn(NativeComponent * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


} // namespace XULWin


#endif // NATIVECOMPONENT_H_INCLUDED
