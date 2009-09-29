#include "Decorator.h"
#include "ElementImpl.h"
#include "ChromeURL.h"
#include "Defaults.h"
#include "Utils/WinUtils.h"
#include "Poco/Path.h"
#include "Poco/UnicodeConverter.h"
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <gdiplus.h>


namespace XULWin
{

    Decorator::Decorator(ElementImpl * inDecoratedElement) :
        ElementImpl(inDecoratedElement->parent()),
        mDecoratedElement(inDecoratedElement)
    {
    }


    Decorator::Decorator(ElementImplPtr inDecoratedElement) :
        ElementImpl(inDecoratedElement->parent()),
        mDecoratedElement(inDecoratedElement)
    {
    }


    Decorator::~Decorator()
    {
    }

    
    void Decorator::setOwningElement(Element * inElement)
    {
        mElement = inElement;
        if (mDecoratedElement)
        {
            mDecoratedElement->setOwningElement(inElement);
        }
    }
    
    
    Element * Decorator::owningElement() const
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->owningElement();
        }
        return mElement;
    }
    
    
    ElementImplPtr Decorator::decoratedElement() const
    {
        return mDecoratedElement;
    }
    
    
    void Decorator::setDecoratedElement(ElementImplPtr inElement)
    {
        mDecoratedElement = inElement;
    }


    void Decorator::move(int x, int y, int w, int h)
    {
        mRect = Rect(x, y, w, h);
        mDecoratedElement->move(mRect.x(), mRect.y(), mRect.width(), mRect.height());
    }


    Rect Decorator::clientRect() const
    {
        return mRect;
    }


    bool Decorator::initAttributeControllers()
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->initAttributeControllers();
        }
        return true;
    }


    bool Decorator::initStyleControllers()
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->initStyleControllers();
        }
        return true;
    }

    
    int Decorator::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->getWidth(inSizeConstraint);
        }
        return 0;
    }


    int Decorator::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->getHeight(inSizeConstraint);
        }
        return 0;
    }


    void Decorator::rebuildLayout()
    {
        if (mDecoratedElement)
        {
            mDecoratedElement->rebuildLayout();
        }
    }
    
    
    bool Decorator::getAttribute(const std::string & inName, std::string & outValue)
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->getAttribute(inName, outValue);
        }
        return false;
    }


    bool Decorator::getStyle(const std::string & inName, std::string & outValue)
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->getStyle(inName, outValue);
        }
        return false;
    }
    
    
    bool Decorator::setAttribute(const std::string & inName, const std::string & inValue)
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->setAttribute(inName, inValue);
        }
        return false;
    }
    
    
    bool Decorator::setStyle(const std::string & inName, const std::string & inValue)
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->setStyle(inName, inValue);
        }
        return false;
    }


    LRESULT Decorator::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->handleMessage(inMessage, wParam, lParam);
        }
        return FALSE;
    }


    WrapDecorator::WrapDecorator(ElementImpl * inParent, ElementImpl * inDecoratedElement) :
        Decorator(inDecoratedElement),
        mParent(inParent)
    {
    }


    void WrapDecorator::addChild(ElementPtr inChild)
    {
        mDecoratorChildren.push_back(inChild);
    }
    
    
    BoxLayoutDecorator::BoxLayoutDecorator(ElementImpl * inParent,
                                           ElementImpl * inDecoratedElement,
                                           Orient inOrient,
                                           Align inAlign) :
        WrapDecorator(inParent, inDecoratedElement),
        BoxLayouter(inOrient, inAlign)
    {
        
    }
    
    
    Rect BoxLayoutDecorator::clientRect() const
    {
        return mRect;
    }

    
    void BoxLayoutDecorator::rebuildLayout()
    {
        return BoxLayouter::rebuildLayout();
    }
    
    
    void BoxLayoutDecorator::rebuildChildLayouts()
    {
        mDecoratedElement->rebuildLayout();
        for (size_t idx = 0; idx != mDecoratorChildren.size(); ++idx)
        {
            mDecoratorChildren[idx]->impl()->rebuildLayout();
        }
    }


    size_t BoxLayoutDecorator::numChildren() const
    {
        // mDecoratedElement + mDecoratorChildren.size()
        return 1 + mDecoratorChildren.size();
    }

    
    const ElementImpl * BoxLayoutDecorator::getChild(size_t idx) const
    {
        if (idx == 0)
        {
            return mDecoratedElement.get();
        }
        else
        {
            return mDecoratorChildren[idx-1]->impl();
        }
    }


    ElementImpl * BoxLayoutDecorator::getChild(size_t idx)
    {
        if (idx == 0)
        {
            return mDecoratedElement.get();
        }
        else
        {
            return mDecoratorChildren[idx-1]->impl();
        }
    }


    int BoxLayoutDecorator::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        int result = mDecoratedElement->getWidth(inSizeConstraint);
        if (getOrient() == HORIZONTAL)
        {
            for (size_t idx = 0; idx != mDecoratorChildren.size(); ++idx)
            {
                result += mDecoratorChildren[idx]->impl()->getWidth(inSizeConstraint);
            }
        }
        return result;
    }


    int BoxLayoutDecorator::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        int result = mDecoratedElement->calculateHeight(inSizeConstraint);
        if (getOrient() == VERTICAL)
        {
            for (size_t idx = 0; idx != mDecoratorChildren.size(); ++idx)
            {
                result += mDecoratorChildren[idx]->impl()->calculateHeight(inSizeConstraint);
            }
        }
        return result;
    }

    
    void BoxLayoutDecorator::setAttributeController(const std::string & inAttr, AttributeController * inController)
    {
        Super::setAttributeController(inAttr, inController);
    }

    
    ScrollDecorator::ScrollDecorator(ElementImpl * inParent,
                                     ElementImpl * inDecoratedElement,
                                     Orient inScrollbarOrient) :
            BoxLayoutDecorator(inParent,
                               inDecoratedElement,
                               inScrollbarOrient == HORIZONTAL ? VERTICAL : HORIZONTAL,
                               Stretch),
            mOldScrollPos(0)

    {        
        AttributesMapping attr;
        attr["orient"] = Orient2String(inScrollbarOrient);
        ElementPtr scrollbar = Scrollbar::Create(inParent->owningElement(), attr);

        // Remove it from the parent so that it is untouched by its layout manager
        inParent->owningElement()->removeChild(scrollbar.get());

        scrollbar->impl()->downcast<NativeScrollbar>()->setEventHandler(this);

        // Add it to our own list of children
        addChild(scrollbar);
    }
            
            
    int ScrollDecorator::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        if (inSizeConstraint == Minimum && getOrient() == VERTICAL)
        {
            return 0;
        }
        return Super::calculateWidth(inSizeConstraint);
    }


    int ScrollDecorator::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        if (inSizeConstraint == Minimum && getOrient() == HORIZONTAL)
        {
            return 0;
        }
        return Super::calculateHeight(inSizeConstraint);
    }
    
    
    void ScrollDecorator::rebuildLayout()
    {
        Super::rebuildLayout();

        // The layout manager will have moved the window to origin again.
        // So set the scrolled state back.
        // TODO: find a better fix
        mOldScrollPos = 0;
        updateWindowScroll();
    }
    
    
    void ScrollDecorator::move(int x, int y, int w, int h)
    {
        if (mDecoratorChildren.empty())
        {
            return;
        }

        // 
        // Update page height of scroll box
        //
        NativeScrollbar * scrollbar = mDecoratorChildren[0]->impl()->downcast<NativeScrollbar>();
        int optSize = getOrient() == HORIZONTAL ? mDecoratedElement->calculateHeight(Optimal)
                                                : mDecoratedElement->getWidth(Optimal);

        if (optSize != 0) // guard against division by zero
        {
            int maxpos = Defaults::Attributes::maxpos();
            float ratio = (float)(getOrient() == HORIZONTAL ? h : w)/(float)optSize;
            int pageincrement = (int)(maxpos*ratio + 0.5);
            int curpos = Utils::getScrollPos(scrollbar->handle());
            Utils::setScrollInfo(scrollbar->handle(), maxpos, pageincrement, curpos);
            scrollbar->setHidden(maxpos <= pageincrement);
        }

        //
        // Move the window
        //
        Super::move(x, y, w, h);
    }


    void ScrollDecorator::updateWindowScroll()
    {
        NativeScrollbar * scrollbar = mDecoratorChildren[0]->impl()->downcast<NativeScrollbar>();
        if (!scrollbar)
        {
            return;
        }

        if (NativeComponent * nativeBox = mDecoratedElement->downcast<NativeComponent>())
        {
            int maxpos = Defaults::Attributes::maxpos();
            Rect clientRect(mDecoratedElement->clientRect());
            int optSize = getOrient() == HORIZONTAL ? mDecoratedElement->calculateHeight(Optimal)
                                                    : mDecoratedElement->getWidth(Optimal);
            int clientSize = getOrient() == HORIZONTAL ? clientRect.height()
                                                       : clientRect.height();
            int scrollPos = Utils::getScrollPos(scrollbar->handle());
            if (scrollPos > maxpos) // just to be sure
            {
                scrollPos = maxpos;
            }

            double ratio = (double)scrollPos/(double)Defaults::Attributes::maxpos();
            int oldScrollPos = mOldScrollPos;
            int newScrollPos = (int)((ratio * (double)optSize) + 0.5);
            int dx = getOrient() == VERTICAL   ? (newScrollPos - mOldScrollPos) : 0;
            int dy = getOrient() == HORIZONTAL ? (newScrollPos - mOldScrollPos) : 0;
            ::ScrollWindowEx(nativeBox->handle(), -dx, -dy, 0, 0, 0, 0, SW_SCROLLCHILDREN | SW_INVALIDATE);
            mOldScrollPos = newScrollPos;
        }
    }


    bool ScrollDecorator::curposChanged(NativeScrollbar * inSender, int inOldPos, int inNewPos)
    {
        updateWindowScroll();
        return true;
    }


    MarginDecorator::MarginDecorator(ElementImpl * inDecoratedElement) :
        Decorator(inDecoratedElement),
        mTop(2),
        mLeft(4),
        mRight(4),
        mBottom(2)
    {
    }


    MarginDecorator::MarginDecorator(ElementImplPtr inDecoratedElement) :
        Decorator(inDecoratedElement),
        mTop(2),
        mLeft(4),
        mRight(4),
        mBottom(2)
    {
    }


    MarginDecorator::~MarginDecorator()
    {
    }
    
    
    void MarginDecorator::move(int x, int y, int w, int h)
    {
        mRect = Rect(x + marginLeft(), y + marginTop(), w - marginLeft() - marginRight(), h - marginTop() - marginBottom());
        mDecoratedElement->move(mRect.x(), mRect.y(), mRect.width(), mRect.height());
    }


    void MarginDecorator::setMargin(int top, int left, int right, int bottom)
    {
        mTop = top;
        mLeft = left;
        mRight = right;
        mBottom = bottom;
    }


    void MarginDecorator::getMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const
    {
        outTop = mTop;
        outLeft = mLeft;
        outRight = mRight;
        outBottom = mBottom;
    }


    int MarginDecorator::marginTop() const
    {
        return mTop;
    }

    
    int MarginDecorator::marginLeft() const
    {
        return mLeft;
    }

    
    int MarginDecorator::marginRight() const
    {
        return mRight;
    }

    
    int MarginDecorator::marginBottom() const
    {
        return mBottom;
    }
    
    
    int MarginDecorator::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return marginLeft() + mDecoratedElement->getWidth(inSizeConstraint) + marginRight();
    }

    
    int MarginDecorator::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return marginTop() + mDecoratedElement->calculateHeight(inSizeConstraint) + marginBottom();
    }

} // namespace XULWin
