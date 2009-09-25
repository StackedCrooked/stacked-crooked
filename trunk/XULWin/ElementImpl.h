#ifndef NATIVECOMPONENT_H_INCLUDED
#define NATIVECOMPONENT_H_INCLUDED


#include "Element.h"
#include "Layout.h"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
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
    class ElementImpl;
    class Decorator;
    class NativeComponent;
    typedef boost::shared_ptr<ElementImpl> ElementImplPtr;

    /**
     * ElementImpl is base class for all native UI elements.
     */
    class ElementImpl : boost::noncopyable
    {
    public:
        ElementImpl(ElementImpl * inParent);

        virtual ~ElementImpl() = 0;

        // Downcast that also resolves decorators.
        // Use this instead of manual cast, because
        // you may get a decorator instead of the 
        // actual element.
        template<class Type>
        Type * downcast()
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

        int commandId() const { return mCommandId.intValue(); }

        int minimumWidth() const;

        int minimumHeight() const;

        virtual int calculateMinimumWidth() const = 0;

        virtual int calculateMinimumHeight() const = 0;

        // Tendency to expand, used for separators.
        bool expansive() const;

        virtual void move(int x, int y, int w, int h) = 0;

        virtual Rect clientRect() const = 0;

        virtual void setOwningElement(Element * inElement);

        Element * owningElement() const;

        ElementImpl * parent() const;

        virtual void rebuildLayout() = 0;

        void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

        bool getAttribute(const std::string & inName, std::string & outValue);

        bool getStyle(const std::string & inName, std::string & outValue);

        virtual bool setStyle(const std::string & inName, const std::string & inValue);

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

    protected:
        ElementImpl * mParent;
        Element * mElement;
        CommandId mCommandId;
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

        typedef std::map<HWND, ElementImpl*> Components;
        static Components sComponentsByHandle;
    };


    class NativeComponent : public ElementImpl
    {
    public:
        typedef ElementImpl Super;

        NativeComponent(ElementImpl * inParent, const AttributesMapping & inAttributes);

        virtual ~NativeComponent();

        static void SetModuleHandle(HMODULE inModule);

        virtual HWND handle() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

    protected:
        HWND mHandle;
        HMODULE mModuleHandle;

    private:
        static HMODULE sModuleHandle;
    };


    class NativeWindow : public NativeComponent
    {
    public:
        typedef NativeComponent Super;

        static void Register(HMODULE inModuleHandle);

        NativeWindow(const AttributesMapping & inAttributesMapping);

        void showModal();

        virtual void move(int x, int y, int w, int h);

        virtual void rebuildLayout();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual Rect clientRect() const;

        virtual Rect windowRect() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
    };


    class NativeControl : public NativeComponent
    {
    public:
        typedef NativeComponent Super;

        NativeControl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle);

        virtual ~NativeControl();
        
        bool initStyleControllers();

        virtual void rebuildLayout();

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        /**
         * Override this method if you want your control to handle its own command events.
         * (Normally the parent control handles them through the WM_COMMAND message.)
         */
        virtual void handleCommand(WPARAM wParam, LPARAM lParam) {}

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

    protected:
        typedef std::map<int, NativeControl*> ControlsById;
        static ControlsById sControlsById;

    private:

        // Gets a NativeComponent object from this object. This
        // is only needed in constructors of NativeComponents, because
        // they need to know which is their native parent handle object.
        // If this is a NativeComponent, return this.
        // If this is a VirtualControl, return first parent that is a NativeComponent.
        // If this is a Decorator, resolve until a NativeComponent is found.
        NativeComponent * GetNativeParent(ElementImpl * inElementImpl);

        WNDPROC mOrigProc;
    };


    class VirtualControl : public ElementImpl
    {
    public:
        typedef ElementImpl Super;

        VirtualControl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~VirtualControl();

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        int width() const;

        void setWidth(int inWidth);

        int height() const;

        void setHeight(int inHeight);

        virtual int calculateMinimumWidth() const { return 0; }

        virtual int calculateMinimumHeight() const { return 0; }

        virtual void rebuildLayout();

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    protected:
        Rect mRect;
        int mWidth, mHeight;
    };


    class Decorator : public ElementImpl
    {
    public:
        typedef ElementImpl Super;

        // Takes ownership.
        Decorator(ElementImpl * inDecoratedElement);

        // This constructor is needed when inserting a new object in the Decorator chain.
        Decorator(ElementImplPtr inDecoratedElement);

        virtual ~Decorator();

        ElementImplPtr decoratedElement() const;

        void setDecoratedElement(ElementImplPtr inElement);

        virtual void setOwningElement(Element * inElement);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual void rebuildLayout();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual void move(int x, int y, int w, int h);

        virtual Rect clientRect() const;

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool setStyle(const std::string & inName, const std::string & inValue);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    protected:
        ElementImplPtr mDecoratedElement;
        Rect mRect;
    };


    class PaddingDecorator : public Decorator
    {
    public:
        typedef Decorator Super;

        // Takes ownership.
        PaddingDecorator(ElementImpl * inDecoratedElement);

        // This constructor is needed for insertion of new objects in the Decorator chain.
        PaddingDecorator(ElementImplPtr inDecoratedElement);

        virtual ~PaddingDecorator();

        virtual void move(int x, int y, int w, int h);

        void setPadding(int top, int left, int right, int bottom);

        void setPadding(int inPadding);

        // returns left padding
        int padding() const;

        int paddingTop() const;

        int paddingLeft() const;

        int paddingRight() const;

        int paddingBottom() const;

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

    private:
        int mTop, mLeft, mRight, mBottom;
    };


    class NativeButton : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeButton(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeLabel : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeLabel(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeDescription : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeDescription(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeTextBox : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeTextBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);

    private:
        bool mReadonly;
        static DWORD GetPasswordFlag(const AttributesMapping & inAttributesMapping);
        static bool IsReadOnly(const AttributesMapping & inAttributesMapping);
    };


    class NativeCheckBox : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeCheckBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeBox : public VirtualControl
    {
    public:
        typedef VirtualControl Super;

        NativeBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orientation inOrientation = HORIZONTAL);

        virtual bool initAttributeControllers();

        virtual void rebuildLayout();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        void setOrientation(Orientation inOrientation);

        Orientation getOrientation();

        void setAlignment(Alignment inAlign);

        Alignment getAlignment() const;
    private:
        Orientation mOrientation;
        Alignment mAlign;
    };


    class NativeHBox : public NativeBox
    {
    public:
        typedef NativeBox Super;

        NativeHBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
    };


    class NativeVBox : public NativeBox
    {
    public:
        typedef NativeBox Super;

        NativeVBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
    };


    class NativeMenuList : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeMenuList(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
            
        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual void move(int x, int y, int w, int h);

        void addMenuItem(const std::string & inText);

        void removeMenuItem(const std::string & inText);
    };


    class NativeSeparator : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeSeparator(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeSpacer : public VirtualControl
    {
    public:
        typedef VirtualControl Super;

        NativeSpacer(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeMenuButton : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeMenuButton(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeGrid : public VirtualControl
    {
    public:
        typedef VirtualControl Super;

        NativeGrid(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual void rebuildLayout();
    };


    class NativeRows : public VirtualControl
    {
    public:
        typedef VirtualControl Super;

        NativeRows(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
    };


    class NativeRow : public VirtualControl
    {
    public:
        typedef VirtualControl Super;

        NativeRow(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeColumns : public VirtualControl
    {
    public:
        typedef VirtualControl Super;

        NativeColumns(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
    };


    class NativeColumn : public VirtualControl
    {
    public:
        typedef VirtualControl Super;

        NativeColumn(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeRadioGroup : public NativeBox
    {
    public:
        typedef NativeBox Super;

        NativeRadioGroup(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
    };


    class NativeRadio : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeRadio(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeProgressMeter : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeProgressMeter(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        bool initAttributeControllers();
    };


    class NativeDeck : public VirtualControl
    {
    public:
        typedef VirtualControl Super;

        NativeDeck(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual void rebuildLayout();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        void setSelectedIndex(int inSelectedIndex);

        int selectedIndex() const;

        bool initAttributeControllers();

    private:
        int mSelectedIndex;
    };


    class NativeScrollbar : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeScrollbar(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        bool initAttributeControllers();

    private:
        static DWORD GetFlags(const AttributesMapping & inAttributesMapping);
    };


} // namespace XULWin


#endif // NATIVECOMPONENT_H_INCLUDED
