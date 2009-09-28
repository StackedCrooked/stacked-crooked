#ifndef NATIVECOMPONENT_H_INCLUDED
#define NATIVECOMPONENT_H_INCLUDED


#include "Element.h"
#include "Conversions.h"
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
    class BoxLayouter;
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


        template<class ConstType>
        const ConstType * constDowncast() const
        {
            if (const ConstType * obj = dynamic_cast<const ConstType*>(this))
            {
                return obj;
            }
            else if (const Decorator * obj = dynamic_cast<const Decorator*>(this))
            {
                return obj->decoratedElement()->constDowncast<ConstType>();
            }
            return 0;
        }

        int commandId() const { return mCommandId.intValue(); }

        int minimumWidth() const;

        int minimumHeight() const;

        virtual int calculateMinimumWidth() const = 0;

        virtual int calculateMinimumHeight() const = 0;

        // Tendency to expand, used for separators, scrollbars, etc..
        bool expansive() const;

        virtual void move(int x, int y, int w, int h) = 0;

        virtual Rect clientRect() const = 0;

        virtual void setOwningElement(Element * inElement);

        virtual Element * owningElement() const;

        ElementImpl * parent() const;

        virtual void rebuildLayout() = 0;

        void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

        virtual bool getAttribute(const std::string & inName, std::string & outValue);

        virtual bool getStyle(const std::string & inName, std::string & outValue);

        virtual bool setStyle(const std::string & inName, const std::string & inValue);

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool initOldAttributeControllers();

        virtual bool initOldStyleControllers();

    protected:
        ElementImpl * mParent;
        Element * mElement;
        CommandId mCommandId;
        bool mExpansive;
        friend class BoxLayouter;
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
        typedef Controller OldAttributeController;
        void setOldAttributeController(const std::string & inAttr, const OldAttributeController & inController);
        typedef std::map<std::string, OldAttributeController> OldAttributeControllers;
        OldAttributeControllers mOldAttributeControllers;
        
        typedef Getter StyleGetter;
        typedef Setter StyleSetter;
        typedef Controller OldStyleController;
        void setOldStyleController(const std::string & inAttr, const OldStyleController & inController);
        typedef std::map<std::string, OldStyleController> OldStyleControllers;
        OldStyleControllers mOldStyleControllers;

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

        virtual bool initOldAttributeControllers();

        virtual bool initOldStyleControllers();

        /**
         * Override this method if you want your control to handle its own command events.
         * (Normally the parent control handles them through the WM_COMMAND message.)
         */
        virtual void handleCommand(WPARAM wParam, LPARAM lParam) {}

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

    protected:
        HWND mHandle;
        HMODULE mModuleHandle;

        typedef std::map<int, NativeComponent*> ComponentsById;
        static ComponentsById sComponentsById;

        WNDPROC mOrigProc;

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

        virtual bool initOldAttributeControllers();

        virtual bool initOldStyleControllers();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
    };


    class NativeControl : public NativeComponent
    {
    public:
        typedef NativeComponent Super;

        NativeControl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle);

        virtual ~NativeControl();
        
        bool initOldStyleControllers();

        virtual void rebuildLayout();

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

        // Gets a NativeComponent object from this object. This
        // is only needed in constructors of NativeComponents, because
        // they need to know which is their native parent handle object.
        // If this is a NativeComponent, return this.
        // If this is a VirtualControl, return first parent that is a NativeComponent.
        // If this is a Decorator, resolve until a NativeComponent is found.
        static NativeComponent * GetNativeParent(ElementImpl * inElementImpl);
    };


    class VirtualControl : public ElementImpl
    {
    public:
        typedef ElementImpl Super;

        VirtualControl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~VirtualControl();

        virtual bool initOldAttributeControllers();

        virtual bool initOldStyleControllers();

        virtual int calculateMinimumWidth() const { return 0; }

        virtual int calculateMinimumHeight() const { return 0; }

        virtual void rebuildLayout();

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    protected:
        Rect mRect;
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

        virtual bool initOldAttributeControllers();

        virtual bool initOldStyleControllers();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeDescription : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeDescription(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initOldAttributeControllers();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class NativeTextBox : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeTextBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initOldAttributeControllers();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);

    private:
        bool mReadonly;
        static DWORD GetFlags(const AttributesMapping & inAttributesMapping);
        static bool IsReadOnly(const AttributesMapping & inAttributesMapping);
    };


    class NativeCheckBox : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeCheckBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initOldAttributeControllers();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;
    };


    class BoxLayouter
    {
    public:
        BoxLayouter(Orientation inOrient, Alignment inAlign);

        virtual bool initOldAttributeControllers();

        virtual void setOldAttributeController(const std::string & inAttr, const ElementImpl::OldAttributeController & inController) = 0;

        virtual void rebuildLayout();

        Orientation orientation() const;

        void setOrientation(Orientation inOrient);

        Alignment alignment() const;

        void setAlignment(Alignment inAlign);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual size_t numChildren() const = 0;

        virtual const ElementImpl * getChild(size_t idx) const = 0;

        virtual ElementImpl * getChild(size_t idx) = 0;

        virtual Rect clientRect() const = 0;

        virtual void rebuildChildLayouts() = 0;

    private:
        Orientation mOrient;
        Alignment mAlign;
    };


    class VirtualBox : public VirtualControl,
                       public BoxLayouter
    {
    public:
        typedef VirtualControl Super;

        VirtualBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orientation inOrient = HORIZONTAL);

        virtual bool initOldAttributeControllers();

        virtual void rebuildLayout()
        {
            BoxLayouter::rebuildLayout();
        }

        virtual int calculateMinimumWidth() const
        {
            return BoxLayouter::calculateMinimumWidth();
        }

        virtual int calculateMinimumHeight() const
        {
            return BoxLayouter::calculateMinimumHeight();
        }
        
        virtual size_t numChildren() const
        { return mElement->children().size(); }

        virtual const ElementImpl * getChild(size_t idx) const
        { return mElement->children()[idx]->impl(); }

        virtual ElementImpl * getChild(size_t idx)
        { return mElement->children()[idx]->impl(); }

        virtual Rect clientRect() const
        { return Super::clientRect(); }

        virtual void rebuildChildLayouts()
        { return Super::rebuildChildLayouts(); }

        virtual void setOldAttributeController(const std::string & inAttr, const OldAttributeController & inController);
    };


    class NativeBox : public NativeControl,
                      public BoxLayouter
    {
    public:
        typedef NativeControl Super;

        NativeBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orientation inOrient);

        virtual bool initOldAttributeControllers();

        virtual void rebuildLayout();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual Rect clientRect() const;

        virtual size_t numChildren() const
        { return mElement->children().size(); }

        virtual const ElementImpl * getChild(size_t idx) const
        { return mElement->children()[idx]->impl(); }

        virtual ElementImpl * getChild(size_t idx)
        { return mElement->children()[idx]->impl(); }

        virtual void rebuildChildLayouts()
        { return Super::rebuildChildLayouts(); }

        virtual void setOldAttributeController(const std::string & inAttr, const OldAttributeController & inController);

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


    class NativeRadioGroup : public VirtualBox
    {
    public:
        typedef VirtualBox Super;

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

        bool initOldAttributeControllers();
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

        bool initOldAttributeControllers();

    private:
        int mSelectedIndex;
    };


    class NativeScrollbar : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeScrollbar(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        class EventHandler
        {
        public:
            virtual bool curposChanged(NativeScrollbar * inSender, int inOldPos, int inNewPos) = 0;
        };

        EventHandler * eventHandler() { return mEventHandler; }

        void setEventHandler(EventHandler * inEventHandler)
        { mEventHandler = inEventHandler; }

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        bool initOldAttributeControllers();

        void setIncrement(int inIncrement);

        int increment() const;

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        static DWORD GetFlags(const AttributesMapping & inAttributesMapping);

        EventHandler * mEventHandler;
        int mIncrement;
    };


} // namespace XULWin


#endif // NATIVECOMPONENT_H_INCLUDED
