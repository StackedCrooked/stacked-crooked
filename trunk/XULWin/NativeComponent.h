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
    class NativeElement;
    class Decorator;
    class NativeComponent;
    typedef boost::shared_ptr<NativeElement> NativeComponentPtr;

    /**
     * NativeElement is base class for all native UI elements.
     */
    class NativeElement
    {
    public:
        NativeElement(NativeElement * inParent);

        virtual ~NativeElement() = 0;

        // Downcast that also resolves decorators.
        template<class Type> Type * downcast()
        {
            if (Type * obj = dynamic_cast<Type*>(this))
            {
                return obj;
            }
            else if (Decorator * obj = dynamic_cast<Decorator*>(this))
            {
                return obj->decoratedElement()->downcast<Type>();
            }
            return 0;
        }

        // Gets a NativeComponent object from this object. This
        // is only needed in constructors of NativeComponents, because
        // they need to know which is their native parent handle object.
        // If this is a NativeComponent, return this.
        // If this is a VirtualControl, return first parent NativeComponent.
        // If this is a Decorator, resolve until a NativeComponent is found.
        NativeComponent * toNativeParent();

        int commandId() const { return mCommandId.intValue(); }

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        bool expansive() const;

        virtual void move(int x, int y, int w, int h) = 0;

        virtual Rect clientRect() const = 0;

        void setOwningElement(Element * inElement);

        Element * owningElement() const;

        NativeElement * parent() const;

        virtual void rebuildLayout();

        virtual void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        bool getAttribute(const std::string & inName, std::string & outValue);

        virtual bool setStyle(const std::string & inName, const std::string & inValue);

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool initAttributeControllers() = 0;

        virtual bool initStyleControllers(){ return true; }

    protected:
        NativeElement * mParent;
        Element * mElement;
        CommandId mCommandId;
        int mMinimumWidth;
        int mMinimumHeight;
        bool mExpansive;
        
        typedef boost::function<std::string()> Getter;
        typedef boost::function<void(const std::string &)> Setter;
        struct Controller
        {
            Controller(Getter & inGetter, Setter & inSetter) :
                getter(inGetter),
                setter(inSetter)
            {
            }
            Getter getter;
            Setter setter;
        };

        typedef Getter AttributeGetter;
        typedef Setter AttributeSetter;        
        typedef Controller AttributeController;
        void setAttributeController(const std::string & inAttr, const AttributeController & inController);
        typedef std::map<std::string, AttributeController> AttributeControllers;
        AttributeControllers mAttributeControllers;
        
        typedef Getter StyleGetter;
        typedef Setter StyleSetter;
        typedef Controller StyleController;
        void setStyleController(const std::string & inAttr, const StyleController & inController);
        typedef std::map<std::string, StyleController> StyleControllers;
        StyleControllers mStyleControllers;

        typedef std::map<HWND, NativeElement*> Components;
        static Components sComponentsByHandle;
    };


    class NativeComponent : public NativeElement
    {
    public:
        NativeComponent(NativeElement * inParent);

        virtual ~NativeComponent();

        static void SetModuleHandle(HMODULE inModule);

        virtual HWND handle() const;

        virtual bool initAttributeControllers();

    protected:
        HWND mHandle;
        HMODULE mModuleHandle;

    private:
        static HMODULE sModuleHandle;
    };


    class NativeWindow : public NativeComponent
    {
    public:
        static void Register(HMODULE inModuleHandle);

        NativeWindow();

        void showModal();

        virtual void move(int x, int y, int w, int h);

        virtual void rebuildLayout();

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        virtual Rect clientRect() const;

        virtual bool initAttributeControllers();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
    };


    class NativeControl : public NativeComponent
    {
    public:
        NativeControl(NativeElement * inParent, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle);

        virtual ~NativeControl();

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

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


    class VirtualControl : public NativeElement
    {
    public:
        VirtualControl(NativeElement * inParent);

        virtual ~VirtualControl(){}

        virtual bool initAttributeControllers() { return true; }

        virtual int minimumWidth() const { return 0; }

        virtual int minimumHeight() const { return 0; }

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

    protected:
        Rect mRect;
    };


    class Decorator : public NativeElement
    {
    public:
        Decorator(NativeElement * inDecoratedElement);

        virtual ~Decorator();

        NativeElement * decoratedElement();

        const NativeElement * decoratedElement() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual void move(int x, int y, int w, int h);

        virtual Rect clientRect() const;

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

    protected:
        NativeComponentPtr mDecoratedElement;
        Rect mRect;
    };


    class PaddingDecorator : public Decorator
    {
    public:
        PaddingDecorator(NativeElement * inDecoratedElement);

        virtual ~PaddingDecorator();

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
        NativeButton(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeLabel : public NativeControl
    {
    public:
        NativeLabel(NativeElement * inParent);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeDescription : public NativeControl
    {
    public:
        NativeDescription(NativeElement * inParent);

        virtual bool initAttributeControllers();

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeTextBox : public NativeControl
    {
    public:
        NativeTextBox(NativeElement * inParent);

        virtual bool initAttributeControllers();

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);
    };


    class NativeCheckBox : public NativeControl
    {
    public:
        NativeCheckBox(NativeElement * inParent);

        virtual bool initAttributeControllers();

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeBox : public VirtualControl
    {
    public:
        NativeBox(NativeElement * inParent, Orientation inOrientation = HORIZONTAL);

        virtual bool initAttributeControllers();

        virtual void rebuildLayout();

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

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
        NativeHBox(NativeElement * inParent);
    };


    class NativeVBox : public NativeBox
    {
    public:
        NativeVBox(NativeElement * inParent) :
            NativeBox(inParent, VERTICAL)
        {
        }
    };


    class NativeMenuList : public NativeControl
    {
    public:
        NativeMenuList(NativeElement * inParent);
            
        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        virtual void move(int x, int y, int w, int h);

        void addMenuItem(const std::string & inText);

        void removeMenuItem(const std::string & inText);
    };


    class NativeSeparator : public NativeControl
    {
    public:
        NativeSeparator(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeSpacer : public VirtualControl
    {
    public:
        NativeSpacer(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeMenuButton : public NativeControl
    {
    public:
        NativeMenuButton(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeGrid : public VirtualControl
    {
    public:
        NativeGrid(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        virtual void rebuildLayout();
    };


    class NativeRows : public VirtualControl
    {
    public:
        NativeRows(NativeElement * inParent);
    };


    class NativeRow : public VirtualControl
    {
    public:
        NativeRow(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeColumns : public VirtualControl
    {
    public:
        NativeColumns(NativeElement * inParent);
    };


    class NativeColumn : public VirtualControl
    {
    public:
        NativeColumn(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeRadioGroup : public NativeBox
    {
    public:
        NativeRadioGroup(NativeElement * inParent);
    };


    class NativeRadio : public NativeControl
    {
    public:
        NativeRadio(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;
    };


    class NativeProgressMeter : public NativeControl
    {
    public:
        NativeProgressMeter(NativeElement * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        bool initAttributeControllers();
    };


} // namespace XULWin


#endif // NATIVECOMPONENT_H_INCLUDED
