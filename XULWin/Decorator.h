#ifndef DECORATOR_H_INCLUDED
#define DECORATOR_H_INCLUDED


#include "Element.h"
#include "ElementImpl.h"


namespace XULWin
{

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

        virtual Element * owningElement() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual void rebuildLayout();

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual void move(int x, int y, int w, int h);

        virtual Rect clientRect() const;

        virtual bool getAttribute(const std::string & inName, std::string & outValue);

        virtual bool getStyle(const std::string & inName, std::string & outValue);

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool setStyle(const std::string & inName, const std::string & inValue);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    protected:
        ElementImplPtr mDecoratedElement;
        Rect mRect;
    };


    class WrapDecorator : public Decorator
    {
    public:
        typedef Decorator Super;

        WrapDecorator(ElementImpl * inParent, ElementImpl * inDecoratedElement);

        // takes ownership
        void addChild(ElementPtr inChild);

    protected:
        ElementImpl * mParent;
        std::vector<ElementPtr> mDecoratorChildren;
    };


    class BoxLayoutDecorator : public WrapDecorator,
                               public BoxLayouter
    {
    public:
        typedef WrapDecorator Super;

        BoxLayoutDecorator(ElementImpl * inParent,
                           ElementImpl * inDecoratedElement,
                           Orientation inOrient,
                           Alignment inAlign);
        
        virtual void setAttributeController(const std::string & inAttr,
                                            const ElementImpl::AttributeController & inController);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual Rect clientRect() const;

        virtual void rebuildLayout();

        virtual void rebuildChildLayouts();

        virtual size_t numChildren() const;

        virtual const ElementImpl * getChild(size_t idx) const;

        virtual ElementImpl * getChild(size_t idx);
    };


    class ScrollDecorator : public BoxLayoutDecorator,
                            public NativeScrollbar::EventHandler
    {
    public:
        typedef BoxLayoutDecorator Super;

        // Takes ownership.
        ScrollDecorator(ElementImpl * inParent,
                        ElementImpl * inDecoratedElement,
                        Orientation inScrollbarOrient);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        virtual void move(int x, int y, int w, int h);

        virtual bool curposChanged(NativeScrollbar * inSender, int inOldPos, int inNewPos);

    private:
        bool mScrollbarVisible;
    };


    class MarginDecorator : public Decorator
    {
    public:
        typedef Decorator Super;

        // Takes ownership.
        MarginDecorator(ElementImpl * inDecoratedElement);

        // This constructor is needed for insertion of new objects in the Decorator chain.
        MarginDecorator(ElementImplPtr inDecoratedElement);

        virtual ~MarginDecorator();

        virtual void move(int x, int y, int w, int h);

        void setMargin(int top, int left, int right, int bottom);

        void setMargin(int inPadding);

        // returns left margin
        int margin() const;

        int marginTop() const;

        int marginLeft() const;

        int marginRight() const;

        int marginBottom() const;

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

    private:
        int mTop, mLeft, mRight, mBottom;
    };

} // namespace XULWin

#endif // DECORATOR_H_INCLUDED
