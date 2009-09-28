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


    bool Decorator::initOldStyleControllers()
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->initOldStyleControllers();
        }
        return true;
    }

    
    int Decorator::calculateMinimumWidth() const
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->calculateMinimumWidth();
        }
        return 0;
    }


    int Decorator::calculateMinimumHeight() const
    {
        if (mDecoratedElement)
        {
            return mDecoratedElement->calculateMinimumHeight();
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


    int BoxLayoutDecorator::calculateMinimumWidth() const
    {
        int result = mDecoratedElement->calculateMinimumWidth();
        if (getOrient() == HORIZONTAL)
        {
            for (size_t idx = 0; idx != mDecoratorChildren.size(); ++idx)
            {
                result += mDecoratorChildren[idx]->impl()->calculateMinimumWidth();
            }
        }
        return result;
    }


    int BoxLayoutDecorator::calculateMinimumHeight() const
    {
        int result = mDecoratedElement->calculateMinimumHeight();
        if (getOrient() == VERTICAL)
        {
            for (size_t idx = 0; idx != mDecoratorChildren.size(); ++idx)
            {
                result += mDecoratorChildren[idx]->impl()->calculateMinimumHeight();
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
            mScrollbarVisible(false)

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
            
            
    int ScrollDecorator::calculateMinimumWidth() const
    {
        if (getOrient() == HORIZONTAL)
        {
            return Super::calculateMinimumWidth();
        }
        else
        {
            return 0;
        }
    }


    int ScrollDecorator::calculateMinimumHeight() const
    {
        if (getOrient() == HORIZONTAL)
        {
            return 0;
        }
        else
        {
            return Super::calculateMinimumHeight();
        }
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
        int minSize = getOrient() == HORIZONTAL ? mDecoratedElement->calculateMinimumHeight()
                                                  : mDecoratedElement->calculateMinimumWidth();

        if (minSize != 0) // guard against division by zero
        {
            int maxpos = Defaults::Attributes::maxpos();
            float ratio = (float)(getOrient() == HORIZONTAL ? h : w)/(float)minSize;
            int pageincrement = (int)(maxpos*ratio + 0.5);
            int curpos = Utils::getScrollPos(scrollbar->handle());
            Utils::setScrollInfo(scrollbar->handle(), maxpos, pageincrement, curpos);
            mScrollbarVisible = pageincrement < maxpos;
            Utils::setWindowVisible(scrollbar->handle(), mScrollbarVisible);
        }
        Super::move(x, y, w, h);
    }


    bool ScrollDecorator::curposChanged(NativeScrollbar * inSender, int inOldPos, int inNewPos)
    {
        if (NativeComponent * nativeBox = mDecoratedElement->downcast<NativeComponent>())
        {
            int maxpos = Defaults::Attributes::maxpos();
            Rect clientRect(mDecoratedElement->clientRect());
            int minSize = getOrient() == HORIZONTAL ? mDecoratedElement->calculateMinimumHeight()
                                                      : mDecoratedElement->calculateMinimumWidth();
            int clientSize = getOrient() == HORIZONTAL ? clientRect.height()
                                                         : clientRect.height();
            int maxScrollPos = minSize - clientSize - 2;
            if (inNewPos > maxScrollPos)
            {
                inNewPos = maxScrollPos;
            }

            // HACK!
            // For some reason Windows sends a scroll down followed by
            // a scroll up event when pressing the arrow-down button 
            // on a scrollbar is already completely scrolled to bottom.
            // This scroll-down event is blocked by my scrollbar handlemessage function.
            // However the scroll-up event is not. So we do that here.
            // TODO: Find a better fix.
            if (inOldPos > maxScrollPos)
            {
                return false;
            }

            double diff = (double)inNewPos - (double)inOldPos;
            double ratio = diff/(double)Defaults::Attributes::maxpos();
            double rounder = 0.5;
            if (diff < 0)
            {
                rounder = -0.5;
            }
            double scrollAmount = ratio * (double)minSize;
            int dx = getOrient() == VERTICAL   ? (int)(scrollAmount + rounder) : 0;
            int dy = getOrient() == HORIZONTAL ? (int)(scrollAmount + rounder) : 0;
            if (inNewPos <= maxScrollPos)
            {
                ::ScrollWindowEx(nativeBox->handle(), -dx, -dy, 0, 0, 0, 0, SW_SCROLLCHILDREN | SW_INVALIDATE);
            }
        }
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
    
    
    int MarginDecorator::calculateMinimumWidth() const
    {
        return marginLeft() + mDecoratedElement->calculateMinimumWidth() + marginRight();
    }

    
    int MarginDecorator::calculateMinimumHeight() const
    {
        return marginTop() + mDecoratedElement->calculateMinimumHeight() + marginBottom();
    }

} // namespace XULWin
