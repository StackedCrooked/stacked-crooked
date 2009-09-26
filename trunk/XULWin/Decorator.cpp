#include "Decorator.h"
#include "ElementImpl.h"
#include "ChromeURL.h"
#include "Defaults.h"
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


    ScrollDecorator::ScrollDecorator(ElementImpl * inDecoratedElement, Orientation inOrient) :
        Decorator(inDecoratedElement),
        mOrient(inOrient)
    {
        if (!mScrollbar)
        {
            AttributesMapping attr;
            attr["orient"] = Orientation2String(mOrient);
            mScrollbar = Element::Create<Scrollbar>(inDecoratedElement->owningElement(), attr);
            mScrollbar->impl()->downcast<NativeScrollbar>()->setEventHandler(this);
        }
    }


    ScrollDecorator::ScrollDecorator(ElementImplPtr inDecoratedElement, Orientation inOrient) :
        Decorator(inDecoratedElement),
        mOrient(inOrient)
    {
        if (!mScrollbar)
        {
            AttributesMapping attr;
            attr["orient"] = Orientation2String(mOrient);
            mScrollbar = Element::Create<Scrollbar>(inDecoratedElement->parent()->owningElement(), attr);
            mScrollbar->impl()->downcast<NativeScrollbar>()->setEventHandler(this);
        }
    }


    ScrollDecorator::~ScrollDecorator()
    {
    }


    bool ScrollDecorator::curposChanged(NativeScrollbar * inSender, int inOldPos, int inNewPos)
    {
        if (NativeComponent * comp = mDecoratedElement->downcast<NativeComponent>())
        {
            int maxpos = String2Int(mScrollbar->getAttribute("maxpos"), 0);
            int dx = mOrient == HORIZONTAL ? (inNewPos - inOldPos) : 0;
            int dy = mOrient == VERTICAL   ? (inNewPos - inOldPos) : 0;
            ::ScrollWindowEx(comp->handle(), -dx, -dy, 0, 0, 0, 0, SW_SCROLLCHILDREN | SW_INVALIDATE);
            
        }
        return true;
    }
    
    
    void ScrollDecorator::rebuildLayout()
    {
        if (mDecoratedElement)
        {
            mDecoratedElement->rebuildLayout();
            if (mOrient == VERTICAL)
            {
                mScrollbar->impl()->move(mRect.x() + mRect.width() - Defaults::scrollbarWidth(),
                                         mRect.y(),
                                         Defaults::scrollbarWidth(),
                                         mRect.height());
            }
            else
            {
                mScrollbar->impl()->move(mRect.x(),
                                         mRect.y() + mRect.height() - Defaults::scrollbarWidth(),
                                         mRect.width(),
                                         Defaults::scrollbarWidth());
            }
        }
    }
    
    
    void ScrollDecorator::move(int x, int y, int w, int h)
    {
        if (mDecoratedElement)
        {
            mRect = Rect(x, y, w, h);

            int minSize = mOrient == VERTICAL ? mDecoratedElement->calculateMinimumHeight()
                                              : mDecoratedElement->calculateMinimumWidth();

            if (minSize != 0) // guard against division by zero
            {
                int maxpos = 100;
                float ratio = (float)(mOrient == VERTICAL ? h : w)/(float)minSize;
                int pageincrement = (int)(100.0*ratio + 0.5);
                if (maxpos > 0)
                {
                    mScrollbar->setAttribute("maxpos", Int2String(maxpos));
                    mScrollbar->setAttribute("pageincrement", Int2String(pageincrement));
                }
            }
            if (mOrient == VERTICAL)
            {
                w -= Defaults::scrollbarWidth();
                if (w < 0)
                {
                    w = 0;
                }
            }
            else
            {
                h -= Defaults::scrollbarWidth();
                if (h < 0)
                {
                    h = 0;
                }
            }
            mDecoratedElement->move(x, y, w, h);
        }
    }
    
    
    int ScrollDecorator::calculateMinimumWidth() const
    {
        if (mOrient == VERTICAL)
        {
            return mDecoratedElement->calculateMinimumWidth() + Defaults::scrollbarWidth();
        }
        else
        {
            return 0;
        }
    }

    
    int ScrollDecorator::calculateMinimumHeight() const
    {
        if (mOrient == HORIZONTAL)
        {
            return mDecoratedElement->calculateMinimumHeight() + Defaults::scrollbarWidth();
        }
        else
        {
            return 0;
        }
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


    void MarginDecorator::setMargin(int inMargin)
    {
        mTop = inMargin;
        mLeft = inMargin;
        mRight = inMargin;
        mBottom = inMargin;
    }


    int MarginDecorator::margin() const
    {
        return mLeft;
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