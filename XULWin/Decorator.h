#ifndef DECORATOR_H_INCLUDED
#define DECORATOR_H_INCLUDED


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

        virtual bool initImpl();
        
        // WidthController methods
        virtual int getWidth() const;

        virtual void setWidth(int inWidth);

        // HeightController methods
        virtual int getHeight() const;

        virtual void setHeight(int inHeight);

        // StrokeController methods
        virtual void setStroke(const RGBColor & inColor);

        virtual const RGBColor & getStroke() const;

        // StrokeWidthController methods
        virtual void setStrokeWidth(int inStrokeWidth);

        virtual int getStrokeWidth() const;

        // FillController methods
        virtual void setFill(const RGBColor & inColor);

        virtual const RGBColor & getFill() const;

        // FlexController methods
        virtual int getFlex() const;

        virtual void setFlex(int inFlex);

        // HiddenController methods
        virtual bool isHidden() const;

        virtual void setHidden(bool inHidden);

        // OrientController methods
        virtual Orient getOrient() const;
        
        virtual void setOrient(Orient inOrient);

        // AlignController methods
        virtual Align getAlign() const;

        virtual void setAlign(Align inAlign);

        // CSSWidthController methods
        virtual int getCSSWidth() const;

        virtual void setCSSWidth(int inWidth);

        // CSSHeightController methods
        virtual int getCSSHeight() const;

        virtual void setCSSHeight(int inHeight);

        // CSSMarginController methods
        virtual void getCSSMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const;

        virtual void setCSSMargin(int inTop, int inLeft, int inRight, int inBottom);

        // CSSFillController methods
        virtual void setCSSFill(const RGBColor & inColor);

        virtual const RGBColor & getCSSFill() const;

        // CSSStrokeController methods
        virtual void setCSSStroke(const RGBColor & inColor);

        virtual const RGBColor & getCSSStroke() const;

        // CSSXController methods
        virtual int getCSSX() const;

        virtual void setCSSX(int inX);

        // CSSYController methods
        virtual int getCSSY() const;

        virtual void setCSSY(int inY);

        virtual int commandId() const;

        virtual int getWidth(SizeConstraint inSizeConstraint) const;

        virtual int getHeight(SizeConstraint inSizeConstraint) const;

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        // Tendency to expand, used for separators, scrollbars, etc..
        virtual bool expansive() const;

        virtual void move(int x, int y, int w, int h);

        virtual Rect clientRect() const;

        virtual void setOwningElement(Element * inElement);

        virtual Element * owningElement() const;

        virtual ElementImpl * parent() const;

        virtual void rebuildLayout();

        virtual void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController);

        virtual void setStyleController(const std::string & inAttr, StyleController * inController);

        ElementImplPtr decoratedElement() const;

        void setDecoratedElement(ElementImplPtr inElement);

        virtual bool getAttribute(const std::string & inName, std::string & outValue);

        virtual bool getStyle(const std::string & inName, std::string & outValue);

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool setStyle(const std::string & inName, const std::string & inValue);

    protected:
        ElementImplPtr mDecoratedElement;
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


    class ScrollDecorator : public Decorator,
                            public NativeScrollbar::EventListener
    {
    public:
        typedef Decorator Super;

        // Takes ownership.
        ScrollDecorator(ElementImpl * inParent,
                        ElementImpl * inDecoratedElement,
                        CSSOverflow inOverflowX,
                        CSSOverflow inOverflowY);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual int getWidth(SizeConstraint inSizeConstraint) const;

        virtual int getHeight(SizeConstraint inSizeConstraint) const;

        virtual void move(int x, int y, int w, int h);

        virtual void rebuildLayout();

        virtual bool curposChanged(NativeScrollbar * inSender, int inOldPos, int inNewPos);

    private:
        void updateWindowScroll();

        CSSOverflow mOverflowX;
        CSSOverflow mOverflowY;
        int mOldHorScrollPos;
        int mOldVerScrollPos;
        ElementPtr mVerticalScrollbar;
        ElementPtr mHorizontalScrollbar;
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

        void setMargin(int top, int left, int right, int bottom);

        void getMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const;

        int marginTop() const;

        int marginLeft() const;

        int marginRight() const;

        int marginBottom() const;
      
        virtual int getWidth() const;

        virtual void setWidth(int inWidth);

        virtual int getHeight() const;

        virtual void setHeight(int inHeight);

        virtual int getWidth(SizeConstraint inSizeConstraint) const;

        virtual int getHeight(SizeConstraint inSizeConstraint) const;
        
        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    private:
        int mTop, mLeft, mRight, mBottom;
        Rect mOuterRect;
        Rect mInnerRect;
    };

} // namespace XULWin


#endif // DECORATOR_H_INCLUDED
