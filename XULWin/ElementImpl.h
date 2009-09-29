#ifndef NATIVECOMPONENT_H_INCLUDED
#define NATIVECOMPONENT_H_INCLUDED


#include "Element.h"
#include "AttributeController.h"
#include "StyleController.h"
#include "Conversions.h"
#include "Layout.h"
#include "Utils/Fallible.h"
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
    class ElementImpl : public virtual WidthController,
                        public virtual HeightController,
                        public virtual FlexController,
                        public virtual HiddenController,
                        public virtual CSSMarginController,
                        public virtual CSSWidthController,
                        public virtual CSSHeightController,
                        boost::noncopyable
    {
    public:
        ElementImpl(ElementImpl * inParent);

        virtual ~ElementImpl() = 0;

        // WidthController methods
        // Returns value from last call to setWidth. If setWidth has not yet
        // been called, then this method returns the value as defined in XUL
        // document. If the value in the XUL document has not been defined,
        // then the optimal size of the element is returned.
        virtual int getWidth() const;

        virtual void setWidth(int inWidth);

        // HeightController methods
        virtual int getHeight() const;

        virtual void setHeight(int inHeight);

        // FlexController methods
        virtual int getFlex() const;

        virtual void setFlex(int inFlex);        

        // HiddenController methods
        virtual bool isHidden() const;

        virtual void setHidden(bool inHidden);

        // CSSWidthController methods
        virtual int getCSSWidth() const;

        virtual void setCSSWidth(int inWidth);

        // CSSHeightController methods
        virtual int getCSSHeight() const;

        virtual void setCSSHeight(int inHeight);

        // CSSMarginController methods
        virtual void getCSSMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const;

        virtual void setCSSMargin(int inTop, int inLeft, int inRight, int inBottom);

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

        int getWidth(SizeConstraint inSizeConstraint) const;

        int getHeight(SizeConstraint inSizeConstraint) const;

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const = 0;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const = 0;

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

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        void setAttributeController(const std::string & inAttr, AttributeController * inController);

        void setStyleController(const std::string & inAttr, StyleController * inController);

    protected:
        friend class BoxLayouter;
        ElementImpl * mParent;
        Element * mElement;
        CommandId mCommandId;
        bool mExpansive;
        int mFlex;
        Utils::Fallible<int> mWidth;
        Utils::Fallible<int> mHeight;
        Utils::Fallible<int> mCSSWidth;
        Utils::Fallible<int> mCSSHeight;

        // We need to remember the hidden state ourselves
        // because we can't rely on WinAPI IsWindowVisible
        // call, because it will return false for child windows
        // of which the parent is not visible.
        // This is a problem because in NativeWindow::showModal
        // we calculate the minimum height before showing the 
        // window. Calculation of minimum height depends on
        // visibility of child items. Minimum height of hidden
        // items is 0. Now, this means that all child items
        // would return a minimum height of zero, because the
        // Windows API says that they are not visible.
        bool mHidden;
        
        typedef std::map<std::string, AttributeController *> AttributeControllers;
        AttributeControllers mAttributeControllers;

        typedef std::map<std::string, StyleController *> StyleControllers;
        StyleControllers mStyleControllers;

        typedef std::map<HWND, ElementImpl*> Components;
        static Components sComponentsByHandle;
    };


    class NativeComponent : public ElementImpl,
                            public virtual DisabledController,
                            public virtual LabelController
    {
    public:
        typedef ElementImpl Super;

        NativeComponent(ElementImpl * inParent, const AttributesMapping & inAttributes);

        virtual ~NativeComponent();

        // DisabledController methods
        virtual bool isDisabled() const;

        virtual void setDisabled(bool inDisabled);

        // LabelController methods
        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

        virtual void setHidden(bool inHidden);

        static void SetModuleHandle(HMODULE inModule);

        virtual HWND handle() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

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


    class BoxLayouter : public virtual OrientController,
                        public virtual AlignController
    {
    public:
        BoxLayouter(Orient inOrient, Align inAlign);

        // OrientController methods
        virtual Orient getOrient() const;

        virtual void setOrient(Orient inOrient);

        // AlignController methods
        virtual Align getAlign() const;

        virtual void setAlign(Align inAlign);

        virtual bool initAttributeControllers();

        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController) = 0;

        //virtual void setOldAttributeController(const std::string & inAttr, const ElementImpl::OldAttributeController & inController) = 0;

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual size_t numChildren() const = 0;

        virtual const ElementImpl * getChild(size_t idx) const = 0;

        virtual ElementImpl * getChild(size_t idx) = 0;

        virtual Rect clientRect() const = 0;

        virtual void rebuildChildLayouts() = 0;

    private:
        Orient mOrient;
        Align mAlign;
    };


    class NativeWindow : public NativeComponent,
                         public BoxLayouter,
                         public virtual TitleController
    {
    public:
        typedef NativeComponent Super;

        static void Register(HMODULE inModuleHandle);

        NativeWindow(const AttributesMapping & inAttributesMapping);

        // TitleController methods
        virtual std::string getTitle() const;

        virtual void setTitle(const std::string & inTitle);

        // BoxLayouter methods
        virtual size_t numChildren() const;

        virtual const ElementImpl * getChild(size_t idx) const;

        virtual ElementImpl * getChild(size_t idx);

        virtual void rebuildChildLayouts()
        { return Super::rebuildChildLayouts(); }

        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController)
        { return Super::setAttributeController(inAttr, inController); }

        void showModal();

        virtual void move(int x, int y, int w, int h);

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

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

        // Gets a NativeComponent object from this object. This
        // is only needed in constructors of NativeComponents, because
        // they need to know which is their native parent handle object.
        // If this is a NativeComponent, return this.
        // If this is a VirtualComponent, return first parent that is a NativeComponent.
        // If this is a Decorator, resolve until a NativeComponent is found.
        static NativeComponent * GetNativeParent(ElementImpl * inElementImpl);
    };


    class VirtualComponent : public ElementImpl
    {
    public:
        typedef ElementImpl Super;

        VirtualComponent(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~VirtualComponent();

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const { return 0; }

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const { return 0; }

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

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeLabel : public NativeControl,
                        public virtual StringValueController,
                        public virtual CSSTextAlignController
    {
    public:
        typedef NativeControl Super;

        NativeLabel(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        // StringValueController methods
        virtual std::string getValue() const;

        virtual void setValue(const std::string & inStringValue);

        // CSSTextAlignController methods
        virtual CSSTextAlign getCSSTextAlign() const;

        virtual void setCSSTextAlign(CSSTextAlign inValue);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeDescription : public NativeControl,
                              public virtual StringValueController
    {
    public:
        typedef NativeControl Super;

        NativeDescription(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        // StringValueController methods
        virtual std::string getValue() const;

        virtual void setValue(const std::string & inStringValue);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeTextBox : public NativeControl,
                          public virtual StringValueController,
                          public virtual ReadOnlyController,
                          public virtual RowsController
    {
    public:
        typedef NativeControl Super;

        NativeTextBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        // StringValueController methods
        virtual std::string getValue() const;

        virtual void setValue(const std::string & inStringValue);

        // ReadOnlyController methods
        virtual bool isReadOnly() const;

        virtual void setReadOnly(bool inReadOnly);

        // RowsControll methods
        virtual int getRows() const;

        virtual void setRows(int inRows);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);

    private:
        bool mReadonly;
        int mRows;
        static DWORD GetFlags(const AttributesMapping & inAttributesMapping);
        static bool IsReadOnly(const AttributesMapping & inAttributesMapping);
    };


    class NativeCheckBox : public NativeControl,
                           public virtual CheckedController
    {
    public:
        typedef NativeControl Super;

        NativeCheckBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        // CheckedController methods
        virtual bool isChecked() const;

        virtual void setChecked(bool inChecked);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class VirtualBox : public VirtualComponent,
                       public BoxLayouter
    {
    public:
        typedef VirtualComponent Super;

        VirtualBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orient inOrient, Align inAlign);

        virtual bool initAttributeControllers();

        virtual void rebuildLayout()
        {
            BoxLayouter::rebuildLayout();
        }

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const
        {
            return BoxLayouter::calculateWidth(inSizeConstraint);
        }

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const
        {
            return BoxLayouter::calculateHeight(inSizeConstraint);
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

        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController);
    };


    class NativeBox : public NativeControl,
                      public BoxLayouter
    {
    public:
        typedef NativeControl Super;

        NativeBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orient inOrient, Align inAlign);

        virtual bool initAttributeControllers();

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual Rect clientRect() const;

        virtual size_t numChildren() const
        { return mElement->children().size(); }

        virtual const ElementImpl * getChild(size_t idx) const
        { return mElement->children()[idx]->impl(); }

        virtual ElementImpl * getChild(size_t idx)
        { return mElement->children()[idx]->impl(); }

        virtual void rebuildChildLayouts()
        { return Super::rebuildChildLayouts(); }
        
        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController);

        //virtual void setOldAttributeController(const std::string & inAttr, const OldAttributeController & inController);

    };


    class NativeMenuList : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeMenuList(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
            
        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual void move(int x, int y, int w, int h);

        void addMenuItem(const std::string & inText);

        void removeMenuItem(const std::string & inText);
    };


    class NativeSeparator : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeSeparator(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeSpacer : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeSpacer(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeMenuButton : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeMenuButton(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeGrid : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeGrid(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual void rebuildLayout();
    };


    class NativeRows : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeRows(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
    };


    class NativeRow : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeRow(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeColumns : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeColumns(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);
    };


    class NativeColumn : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeColumn(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
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

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeProgressMeter : public NativeControl,
                                public virtual IntValueController
    {
    public:
        typedef NativeControl Super;

        NativeProgressMeter(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        // IntValueController methods
        virtual int getValue() const;

        virtual void setValue(int inValue);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        bool initAttributeControllers();
    };


    class NativeDeck : public VirtualComponent,
                       public virtual SelectedIndexController
    {
    public:
        typedef VirtualComponent Super;

        NativeDeck(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        // SelectedIndexController methods
        virtual int getSelectedIndex() const;

        virtual void setSelectedIndex(int inSelectedIndex);

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        bool initAttributeControllers();

    private:
        int mSelectedIndex;
    };


    class NativeScrollbar : public NativeControl,
                            public virtual ScrollbarCurrentPositionController,
                            public virtual ScrollbarMaxPositionController,
                            public virtual ScrollbarIncrementController,
                            public virtual ScrollbarPageIncrementController
    {
    public:
        typedef NativeControl Super;

        NativeScrollbar(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int getCurrentPosition() const;

        virtual void setCurrentPosition(int inCurrentPosition);

        virtual int getMaxPosition() const;

        virtual void setMaxPosition(int inMaxPosition);

        virtual int getIncrement() const;

        virtual void setIncrement(int inIncrement);

        virtual int getPageIncrement() const;

        virtual void setPageIncrement(int inPageIncrement);

        class EventHandler
        {
        public:
            virtual bool curposChanged(NativeScrollbar * inSender, int inOldPos, int inNewPos) = 0;
        };

        EventHandler * eventHandler() { return mEventHandler; }

        void setEventHandler(EventHandler * inEventHandler)
        { mEventHandler = inEventHandler; }

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        bool initAttributeControllers();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        static DWORD GetFlags(const AttributesMapping & inAttributesMapping);

        EventHandler * mEventHandler;
        int mIncrement;
    };


} // namespace XULWin


#endif // NATIVECOMPONENT_H_INCLUDED
