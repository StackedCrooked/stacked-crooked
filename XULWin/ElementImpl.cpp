#include "ElementImpl.h"
#include "Defaults.h"
#include "Layout.h"
#include "Utils/ErrorReporter.h"
#include "Utils/WinUtils.h"
#include "Poco/String.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <CommCtrl.h>


using namespace Utils;


namespace XULWin
{
    template<class T>
    std::string ToString(const T & inValue)
    {
        try
        {
            return boost::lexical_cast<std::string>(inValue);
        }
        catch (boost::bad_lexical_cast &)
        {
            ReportError("String2Int: lexical cast failed");
            return "";
        }
    }


    std::string Int2String(int inValue)
    {
        return ToString(inValue);
    }


    int String2Int(const std::string & inValue, int inDefault)
    {
        int result = inDefault;
        try
        {
            if (!inValue.empty())
            {
                result = boost::lexical_cast<int>(inValue);
            }
        }
        catch (std::exception &)
        {
            // ok, too bad
        }
        return result;
    }


    int String2Int(const std::string & inValue)
    {
        int result = 0;
        try
        {
            if (!inValue.empty())
            {
                result = boost::lexical_cast<int>(inValue);
            }
        }
        catch (std::exception &)
        {
            ReportError("Int2String: lexical cast failed");
        }
        return result;
    }
    
    
    bool String2Bool(const std::string & inString, bool inDefault)
    {
        if (inString == "true")
        {
            return true;
        }
        else if (inString == "false")
        {
            return false;
        }
        else
        {
            return inDefault;
        }
    }
    
    
    std::string Bool2String(bool inBool)
    {
        return inBool ? "true" : "false";
    }


    Orientation String2Orientation(const std::string & inValue, Orientation inDefault)
    {
        Orientation result = inDefault;
        if (inValue == "horizontal")
        {
            result = HORIZONTAL;
        }
        else if (inValue == "vertical")
        {
            result = VERTICAL;
        }
        return result;
    }


    std::string Orientation2String(Orientation inOrientation)
    {
        if (inOrientation == HORIZONTAL)
        {
            return "horizontal";
        }
        else
        {
            return "vertical";
        }
    }

    
    Alignment String2Align(const std::string & inValue, Alignment inDefault)
    {
        Alignment result = inDefault;
        if (inValue == "start")
        {
            result = Start;
        }
        else if (inValue == "center")
        {
            result = Center;
        }
        else if (inValue == "end")
        {
            result = End;
        }
        else if (inValue == "stretch")
        {
            result = Stretch;
        }
        return result;
    }


    std::string Align2String(Alignment inAlign)
    {
        if (inAlign == Start)
        {
            return "start";
        }
        else if (inAlign == Center)
        {
            return "center";
        }
        else if (inAlign == End)
        {
            return "end";
        }
        else if (inAlign == Stretch)
        {
            return "stretch";
        }
        else
        {
            ReportError("Invalid alignment");
            return "";
        }
    }
    
    
    int CssString2Size(const std::string & inString, int inDefault)
    {
        if (inString.rfind("px") != std::string::npos)
        {
            return String2Int(Poco::replace(inString, "px", ""), inDefault);
        }
        return String2Int(inString, inDefault);
    }


    int CommandId::sId = 101; // start command Ids at 101 to avoid conflicts with Windows predefined values
    
    ElementImpl::Components ElementImpl::sComponentsByHandle;
    
    NativeControl::ControlsById NativeControl::sControlsById;

    ElementImpl::ElementImpl(ElementImpl * inParent) :
        mParent(inParent),
        mCommandId(),
        mExpansive(false)
    {
    }


    ElementImpl::~ElementImpl()
    {
    }
    
    
    int ElementImpl::minimumWidth() const
    {
        int minWidth = calculateMinimumWidth();
        int elementWidthFromDocument = String2Int(owningElement()->getDocumentAttribute("width"), 0);
        return std::max<int>(minWidth, elementWidthFromDocument);
    }

    
    int ElementImpl::minimumHeight() const
    {
        int minHeight = calculateMinimumHeight();
        int elementHeightFromDocument = String2Int(owningElement()->getDocumentAttribute("height"), 0);
        return std::max<int>(minHeight, elementHeightFromDocument);
    }
    
    
    bool ElementImpl::expansive() const
    {
        return mExpansive;
    }
    
    
    bool ElementImpl::getStyle(const std::string & inName, std::string & outValue)
    {
        StyleControllers::iterator it = mStyleControllers.find(inName);
        if (it != mStyleControllers.end())
        {
            const StyleController & controller = it->second;
            const StyleGetter & getter = controller.getter;
            if (getter)
            {
                outValue = getter();
                return true;
            }
        }
        return false;
    }
    
    
    bool ElementImpl::getAttribute(const std::string & inName, std::string & outValue)
    {
        AttributeControllers::iterator it = mAttributeControllers.find(inName);
        if (it != mAttributeControllers.end())
        {
            const AttributeController & controller = it->second;
            const AttributeGetter & getter = controller.getter;
            if (getter)
            {
                outValue = getter();
                return true;
            }
        }
        return false;
    }
    
    
    bool ElementImpl::setStyle(const std::string & inName, const std::string & inValue)
    {
        StyleControllers::iterator it = mStyleControllers.find(inName);
        if (it != mStyleControllers.end())
        {
            const StyleController & controller = it->second;
            const StyleSetter & setter = controller.setter;
            if (setter)
            {
                setter(inValue);
                return true;
            }
        }
        return false;
    }
    
    
    bool ElementImpl::setAttribute(const std::string & inName, const std::string & inValue)
    {
        AttributeControllers::iterator it = mAttributeControllers.find(inName);
        if (it != mAttributeControllers.end())
        {
            const AttributeController & controller = it->second;
            const AttributeSetter & setter = controller.setter;
            if (setter)
            {
                setter(inValue);
                return true;
            }
        }
        return false;
    }


    bool ElementImpl::initAttributeControllers()
    {
        return true;
    }


    bool ElementImpl::initStyleControllers()
    {
        struct Helper
        {
            static void SetWidth(ElementImpl * inEl, int inWidth)
            {
                Rect r = inEl->clientRect();
                inEl->move(r.x(), r.y(), inWidth, r.height());
            }

            static int GetWidth(ElementImpl * inEl)
            {
                return inEl->clientRect().width();
            }
            static void SetHeight(ElementImpl * inEl, int inHeight)
            {
                Rect r = inEl->clientRect();
                inEl->move(r.x(), r.y(), r.width(), inHeight);
            }

            static int GetHeight(ElementImpl * inEl)
            {
                return inEl->clientRect().height();
            }


            static void SetPadding(ElementImpl * inEl, int inPadding)
            {
                if (PaddingDecorator * obj = inEl->owningElement()->impl()->downcast<PaddingDecorator>())
                {
                    obj->setPadding(inPadding);
                }
                else if (Decorator * dec = inEl->owningElement()->impl()->downcast<Decorator>())
                {
                    ElementImplPtr newDec(new PaddingDecorator(dec->decoratedElement()));
                    dec->setDecoratedElement(newDec);
                    if (PaddingDecorator * p = newDec->downcast<PaddingDecorator>())
                    {
                        p->setPadding(inPadding);
                    }
                }
            }

            static int GetPadding(ElementImpl * inEl)
            {
                return inEl->downcast<PaddingDecorator>()->padding();
            }


        };
        {StyleGetter cssWidthGetter = boost::bind(&Int2String, boost::bind(&Helper::GetWidth, this));
        StyleSetter cssWidthSetter = boost::bind(&Helper::SetWidth, this, boost::bind(&CssString2Size, _1, Defaults::controlWidth()));
        setStyleController("width", StyleController(cssWidthGetter, cssWidthSetter));

        StyleGetter cssHeightGetter = boost::bind(&Int2String, boost::bind(&Helper::GetHeight, this));
        StyleSetter cssHeightSetter = boost::bind(&Helper::SetHeight, this, boost::bind(&CssString2Size, _1, Defaults::controlHeight()));
        setStyleController("height", StyleController(cssHeightGetter, cssHeightSetter));}

        StyleGetter cssPaddingGetter = boost::bind(&Int2String, boost::bind(&Helper::GetPadding, this));
        StyleSetter cssPaddingSetter = boost::bind(&Helper::SetPadding, this, boost::bind(&CssString2Size, _1, 0));
        setStyleController("padding", StyleController(cssPaddingGetter, cssPaddingSetter));
        return true;
    }


    void ElementImpl::setStyleController(const std::string & inAttr, const StyleController & inController)
    {
        StyleControllers::iterator it = mStyleControllers.find(inAttr);
        if (it == mStyleControllers.end())
        {
            mStyleControllers.insert(std::make_pair(inAttr, inController));
        }
    }


    void ElementImpl::setAttributeController(const std::string & inAttr, const AttributeController & inController)
    {
        AttributeControllers::iterator it = mAttributeControllers.find(inAttr);
        if (it == mAttributeControllers.end())
        {
            mAttributeControllers.insert(std::make_pair(inAttr, inController));
        }
    }

    
    void ElementImpl::setOwningElement(Element * inElement)
    {
        mElement = inElement;
    }

    
    Element * ElementImpl::owningElement() const
    {
        return mElement;
    }


    ElementImpl * ElementImpl::parent() const
    {
        return mParent;
    }

    
    void ElementImpl::rebuildChildLayouts()
    {
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            ElementImpl * nativeComp = mElement->children()[idx]->impl();
            if (nativeComp)
            {
                nativeComp->rebuildLayout();
            }
        }
    }

    
    HMODULE NativeComponent::sModuleHandle(0);


    NativeComponent::NativeComponent(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        ElementImpl(inParent),
        mHandle(0),
        mModuleHandle(sModuleHandle ? sModuleHandle : ::GetModuleHandle(0))
    {
    }


    NativeComponent::~NativeComponent()
    {            
        if (mHandle)
        {
            Components::iterator it = sComponentsByHandle.find(mHandle);
            bool found = it != sComponentsByHandle.end();
            assert(found);
            if (found)
            {
                sComponentsByHandle.erase(it);
            }

            ::DestroyWindow(mHandle);
        }
    }
        
    
    void NativeComponent::SetModuleHandle(HMODULE inModule)
    {
        sModuleHandle = inModule;
    }


    HWND NativeComponent::handle() const
    {
        return mHandle;
    }


    bool NativeComponent::initStyleControllers()
    {
        return Super::initStyleControllers();
    }
    
    
    bool NativeComponent::initAttributeControllers()
    {    
        struct Helper
        {
            static std::string Bool2String(bool inValue)
            {
                return inValue ? "true" : "false";
            }
            static bool String2Bool(const std::string & inString)
            {
                return inString == "true" ? true : false;
            }
        };

        AttributeGetter heightGetter = boost::bind(&Int2String, boost::bind(&Utils::getWindowHeight, handle()));
        AttributeSetter heightSetter = boost::bind(&Utils::setWindowHeight, handle(), boost::bind(&String2Int, _1));
        setAttributeController("height", AttributeController(heightGetter, heightSetter));

        AttributeGetter widthGetter = boost::bind(&Int2String, boost::bind(&Utils::getWindowWidth, handle()));
        AttributeSetter widthSetter = boost::bind(&Utils::setWindowWidth, handle(), boost::bind(&String2Int, _1));
        setAttributeController("width", AttributeController(widthGetter, widthSetter));

        AttributeGetter disabledGetter = boost::bind(&Helper::Bool2String, boost::bind(&Utils::isWindowDisabled, handle()));
        AttributeSetter disabledSetter = boost::bind(&Utils::disableWindow, handle(), boost::bind(&Helper::String2Bool, _1));
        setAttributeController("disabled", AttributeController(disabledGetter, disabledSetter));
        
        AttributeGetter labelGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter labelSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("label", AttributeController(labelGetter, labelSetter));
        return Super::initAttributeControllers();
    }


    void NativeWindow::Register(HMODULE inModuleHandle)
    {
        WNDCLASSEX wndClass;
        wndClass.cbSize = sizeof(wndClass);
        wndClass.style = 0;
        wndClass.lpfnWndProc = &NativeWindow::MessageHandler;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = inModuleHandle;
        wndClass.hIcon = 0;
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = TEXT("XULWin::Window");
        wndClass.hIconSm = 0;
        if (! RegisterClassEx(&wndClass))
        {
            ReportError("Could not register XUL::Window class.");
        }
    }


    NativeWindow::NativeWindow(const AttributesMapping & inAttributesMapping) :
        NativeComponent(0, inAttributesMapping)
    {
        mHandle = ::CreateWindowEx
        (
            0, 
            TEXT("XULWin::Window"),
            TEXT(""),
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, Defaults::windowWidth(), Defaults::windowHeight(),
            0,
            (HMENU)0,
            mModuleHandle,
            0
        );

        // set default font
        ::SendMessage(mHandle, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        sComponentsByHandle.insert(std::make_pair(mHandle, this));
    }


    bool NativeWindow::initStyleControllers()
    {  
        return Super::initStyleControllers();
    }


    bool NativeWindow::initAttributeControllers()
    {
        AttributeGetter titleGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter titleSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("title", AttributeController(titleGetter, titleSetter));
        return Super::initAttributeControllers();
    }
    
    
    Rect NativeWindow::clientRect() const
    {
        RECT rc;
        ::GetClientRect(handle(), &rc);
        return Rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    }
    
    
    Rect NativeWindow::windowRect() const
    {
        RECT rw;
        ::GetWindowRect(handle(), &rw);
        return Rect(rw.left, rw.top, rw.right - rw.left, rw.bottom - rw.top);
    }
    
    
    int NativeWindow::calculateMinimumWidth() const
    {
        int result = 0;
        Orientation orient = String2Orientation(owningElement()->getAttribute("orient"), VERTICAL);
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child(owningElement()->children()[idx]);
            int minWidth = child->impl()->minimumWidth();
            if (orient == HORIZONTAL)
            {
                result += minWidth;
            }
            else if (minWidth > result)
            {
                result = minWidth;
            }
        }
        return result;
    }
    
    
    int NativeWindow::calculateMinimumHeight() const
    {
        int result = 0;
        Orientation orient = String2Orientation(owningElement()->getAttribute("orient"), VERTICAL);
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child(owningElement()->children()[idx]);
            int minHeight = child->impl()->minimumHeight();
            if (orient == VERTICAL)
            {
                result += minHeight;
            }
            else if (minHeight > result)
            {
                result = minHeight;
            }
        }
        return result;
    }
    
    
    void NativeWindow::move(int x, int y, int w, int h)
    {
        ::MoveWindow(handle(), x, y, w, h, FALSE);
    }

    
    void NativeWindow::rebuildLayout()
    {
        LinearLayoutManager layout(String2Orientation(owningElement()->getAttribute("orient"), VERTICAL));
        bool horizontal = layout.orientation() == HORIZONTAL;
        
        std::vector<ExtendedSizeInfo> sizeInfos;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            int flexValue = String2Int(child->getAttribute("flex"));
            int minSize = horizontal ? child->impl()->minimumWidth() : child->impl()->minimumHeight();
            int minSizeOpposite = horizontal ? child->impl()->minimumHeight() : child->impl()->minimumWidth();
            sizeInfos.push_back(ExtendedSizeInfo(flexValue, minSize, minSizeOpposite, child->impl()->expansive()));
        }
        
        std::vector<Rect> childRects;
        layout.getRects(clientRect(), String2Align(owningElement()->getAttribute("align"), Stretch), sizeInfos, childRects);

        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            const Rect & rect = childRects[idx];
            child->impl()->move(rect.x(), rect.y(), rect.width(), rect.height());
        }

        rebuildChildLayouts();
        ::InvalidateRect(handle(), 0, FALSE);
    }


    void NativeWindow::showModal()
    {  
        int w = calculateMinimumWidth();
        int h = calculateMinimumHeight();
        int x = (GetSystemMetrics(SM_CXSCREEN) - w)/2;
        int y = (GetSystemMetrics(SM_CYSCREEN) - h)/2;
        move(x, y, w, h);
        move(x, y, w, h);
        rebuildLayout();
        ::ShowWindow(handle(), SW_SHOW);
        ::UpdateWindow(handle());

        MSG message;
        while (GetMessage(&message, NULL, 0, 0))
        {
            HWND hActive = GetActiveWindow();
            if (! IsDialogMessage(hActive, &message))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
    }


    LRESULT NativeWindow::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        switch(inMessage)
        {
            case WM_SIZE:
            {
                Children::const_iterator it = owningElement()->children().begin();
                if (it != owningElement()->children().end())
                {
                    Rect clientRect(clientRect());
                    (*it)->impl()->move(clientRect.x(), clientRect.y(), clientRect.width(), clientRect.height());
                    rebuildLayout();
                }
                break;
            }
            case WM_CLOSE:
            {
                PostQuitMessage(0);
                break;
            }
			case WM_GETMINMAXINFO:
			{
                SIZE sizeDiff = GetSizeDifference_WindowRect_ClientRect(handle());
				MINMAXINFO * minMaxInfo = (MINMAXINFO*)lParam;
                minMaxInfo->ptMinTrackSize.x = calculateMinimumWidth() + sizeDiff.cx;
                minMaxInfo->ptMinTrackSize.y = calculateMinimumHeight() + sizeDiff.cy;
                break;
			}
        }
        return ::DefWindowProc(handle(), inMessage, wParam, lParam);
    }

    
    LRESULT CALLBACK NativeWindow::MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        Components::iterator it = sComponentsByHandle.find(hWnd);
        if (it != sComponentsByHandle.end())
        {
            return it->second->handleMessage(inMessage, wParam, lParam);
        }
        return ::DefWindowProc(hWnd, inMessage, wParam, lParam);
    }


    VirtualControl::VirtualControl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        ElementImpl(inParent),
        mWidth(0),
        mHeight(0)
    {
        if (!mParent)
        {
            ReportError("NativeControl constructor failed because parent is NULL.");
            return;
        }
    }
        
        
    VirtualControl::~VirtualControl()
    {

    }

        
    int VirtualControl::width() const
    {
        return mWidth;
    }


    void VirtualControl::setWidth(int inWidth)
    {
        mWidth = inWidth;
    }


    int VirtualControl::height() const
    {
        return mHeight;
    }


    void VirtualControl::setHeight(int inHeight)
    {
        mHeight = inHeight;
    }

     
    bool VirtualControl::initAttributeControllers()
    {
        AttributeGetter heightGetter = boost::bind(&Int2String, boost::bind(&VirtualControl::height, this));
        AttributeSetter heightSetter = boost::bind(&VirtualControl::setHeight, this, boost::bind(&String2Int, _1));
        setAttributeController("height", AttributeController(heightGetter, heightSetter));

        AttributeGetter widthGetter = boost::bind(&Int2String, boost::bind(&VirtualControl::width, this));
        AttributeSetter widthSetter = boost::bind(&VirtualControl::setWidth, this, boost::bind(&String2Int, _1));
        setAttributeController("width", AttributeController(widthGetter, widthSetter));
        return Super::initAttributeControllers();
    }


    bool VirtualControl::initStyleControllers()
    {
        return Super::initStyleControllers();
    }


    void VirtualControl::move(int x, int y, int w, int h)
    {
        mRect = Rect(x, y, w, h);
    }


    void VirtualControl::rebuildLayout()
    {
        rebuildChildLayouts();
    }
    
    
    Rect VirtualControl::clientRect() const
    {
        return mRect;
    }

    
    LRESULT VirtualControl::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        return FALSE;
    }


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


    PaddingDecorator::PaddingDecorator(ElementImpl * inDecoratedElement) :
        Decorator(inDecoratedElement),
        mTop(2),
        mLeft(4),
        mRight(4),
        mBottom(2)
    {
    }


    PaddingDecorator::PaddingDecorator(ElementImplPtr inDecoratedElement) :
        Decorator(inDecoratedElement),
        mTop(2),
        mLeft(4),
        mRight(4),
        mBottom(2)
    {
    }


    PaddingDecorator::~PaddingDecorator()
    {
    }
    
    
    void PaddingDecorator::move(int x, int y, int w, int h)
    {
        mRect = Rect(x + paddingLeft(), y + paddingTop(), w - paddingLeft() - paddingRight(), h - paddingTop() - paddingBottom());
        mDecoratedElement->move(mRect.x(), mRect.y(), mRect.width(), mRect.height());
    }


    void PaddingDecorator::setPadding(int top, int left, int right, int bottom)
    {
        mTop = top;
        mLeft = left;
        mRight = right;
        mBottom = bottom;
    }


    void PaddingDecorator::setPadding(int inPadding)
    {
        mTop = inPadding;
        mLeft = inPadding;
        mRight = inPadding;
        mBottom = inPadding;
    }


    int PaddingDecorator::padding() const
    {
        return mLeft;
    }


    int PaddingDecorator::paddingTop() const
    {
        return mTop;
    }

    
    int PaddingDecorator::paddingLeft() const
    {
        return mLeft;
    }

    
    int PaddingDecorator::paddingRight() const
    {
        return mRight;
    }

    
    int PaddingDecorator::paddingBottom() const
    {
        return mBottom;
    }
    
    
    int PaddingDecorator::calculateMinimumWidth() const
    {
        return paddingLeft() + mDecoratedElement->calculateMinimumWidth() + paddingRight();
    }

    
    int PaddingDecorator::calculateMinimumHeight() const
    {
        return paddingTop() + mDecoratedElement->calculateMinimumHeight() + paddingBottom();
    }


    NativeControl::NativeControl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle) :
        NativeComponent(inParent, inAttributesMapping)
    {
        if (!mParent)
        {
            ReportError("NativeControl constructor failed because parent is NULL.");
            return;
        }

        Rect clientRect = inParent->clientRect();

        NativeComponent * nativeParent = GetNativeParent(inParent);
        if (!nativeParent)
        {
            ReportError("NativeControl constructor failed because no native parent was found.");
            return;
        }
        
        mHandle = ::CreateWindowEx
        (
            inExStyle, 
            inClassName,
            TEXT(""),
			inStyle | WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
            0, 0, 0, 0,
            nativeParent->handle(),
            (HMENU)mCommandId.intValue(),
            mModuleHandle,
            0
        );

        if (!mHandle)
        {
            ReportError(Utils::getLastError(::GetLastError()));
            return;
        }

        // set default font
        ::SendMessage(mHandle, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));

        // subclass
        mOrigProc = (WNDPROC)(LONG_PTR)::SetWindowLongPtr(mHandle, GWL_WNDPROC, (LONG)(LONG_PTR)&NativeControl::MessageHandler);

        sComponentsByHandle.insert(std::make_pair(mHandle, this));
        sControlsById.insert(std::make_pair(mCommandId.intValue(), this));
    }


    NativeControl::~NativeControl()
    {
        if (mOrigProc)
        {
            ::SetWindowLongPtr(mHandle, GWL_WNDPROC, (LONG)(LONG_PTR)mOrigProc);
            mOrigProc = 0;
        }

        ControlsById::iterator itById = sControlsById.find(mCommandId.intValue());
        bool foundById = itById != sControlsById.end();
        assert(foundById);
        if (foundById)
        {
            sControlsById.erase(itById);
        }
    }


    bool NativeControl::initStyleControllers()
    {
        return Super::initStyleControllers();
    }


    void NativeControl::rebuildLayout()
    {
        rebuildChildLayouts();
    }
    
    
    void NativeControl::move(int x, int y, int w, int h)
    {
        ::MoveWindow(handle(), x, y, w, h, FALSE);
    }
    
    
    Rect NativeControl::clientRect() const
    {
        RECT rc;
        ::GetClientRect(handle(), &rc);
        return Rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    }


    LRESULT NativeControl::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        switch (inMessage)
        {
            case WM_COMMAND:
            {
                ControlsById::iterator it = sControlsById.find(LOWORD(wParam));
                if (it != sControlsById.end())
                {
                    it->second->handleCommand(wParam, lParam);
                    //it->second->owningElement()->OnCommand(0);
                }
                break;
            }
        }

        if (mOrigProc)
		{
			return ::CallWindowProc(mOrigProc, mHandle, inMessage, wParam, lParam);
		}
		else
		{
			return ::DefWindowProc(mHandle, inMessage, wParam, lParam);
		}
    }

    
    LRESULT CALLBACK NativeControl::MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        Components::iterator it = sComponentsByHandle.find(hWnd);
        if (it != sComponentsByHandle.end())
        {
            return it->second->handleMessage(inMessage, wParam, lParam);
        }
        return ::DefWindowProc(hWnd, inMessage, wParam, lParam);
    } 
    
    
    NativeComponent * NativeControl::GetNativeParent(ElementImpl * inElementImpl)
    {
        if (NativeComponent * obj = dynamic_cast<NativeComponent*>(inElementImpl))
        {
            return obj;
        }
        else if (Decorator * obj = dynamic_cast<Decorator*>(inElementImpl))
        {
            return GetNativeParent(obj->decoratedElement().get());
        }
        else if (VirtualControl * obj = dynamic_cast<VirtualControl*>(inElementImpl))
        {
            return GetNativeParent(obj->parent());
        }
        return 0;
    }
    
    
    NativeButton::NativeButton(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("BUTTON"),
                      0, // exStyle
                      BS_PUSHBUTTON)
    {   
    }
    
    
    int NativeButton::calculateMinimumHeight() const
    {
        return Defaults::buttonHeight();
    }
    
    
    int NativeButton::calculateMinimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        width += Defaults::textPadding();
        return width;
    }
    
    
    NativeCheckBox::NativeCheckBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping, TEXT("BUTTON"), 0, BS_AUTOCHECKBOX)
    {
    }
    
    
    int NativeCheckBox::calculateMinimumHeight() const
    {
        return Defaults::controlHeight();
    }
    
    
    int NativeCheckBox::calculateMinimumWidth() const
    {
        return Defaults::checkBoxMinimumWidth() + Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx;
    }

    
    bool NativeCheckBox::initAttributeControllers()
    {
        struct Helper
        {
            static std::string Bool2String(bool inValue)
            {
                return inValue ? "true" : "false";
            }
            static bool String2Bool(const std::string & inString)
            {
                return inString == "false" ? false : true;
            }
        };
        AttributeGetter checkedGetter = boost::bind(&Helper::Bool2String, boost::bind(&Utils::isCheckBoxChecked, handle()));
        AttributeSetter checkedSetter = boost::bind(&Utils::setCheckBoxChecked, handle(), boost::bind(&Helper::String2Bool, _1));
        setAttributeController("checked", AttributeController(checkedGetter, checkedSetter));
        return Super::initAttributeControllers();
    }


    NativeTextBox::NativeTextBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("EDIT"),
                      WS_EX_CLIENTEDGE, // exStyle
                      ES_AUTOHSCROLL | GetPasswordFlag(inAttributesMapping)),
                      mReadonly(IsReadOnly(inAttributesMapping))
    {
    }


    DWORD NativeTextBox::GetPasswordFlag(const AttributesMapping & inAttributesMapping)
    {
        AttributesMapping::const_iterator it = inAttributesMapping.find("type");
        return (it != inAttributesMapping.end()) ? ES_PASSWORD : 0;
    } 


    bool NativeTextBox::IsReadOnly(const AttributesMapping & inAttributesMapping)
    {
        AttributesMapping::const_iterator it = inAttributesMapping.find("readonly");
        return it != inAttributesMapping.end();
    } 

    
    bool NativeTextBox::initAttributeControllers()
    {
        AttributeGetter valueGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter valueSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("value", AttributeController(valueGetter, valueSetter));

        {AttributeGetter readOnlyGetter = boost::bind(&Bool2String, boost::bind(&Utils::isTextBoxReadOnly, handle()));
        AttributeSetter readOnlySetter = boost::bind(&Utils::setTextBoxReadOnly, handle(), boost::bind(&String2Bool, _1, false));
        setAttributeController("readonly", AttributeController(readOnlyGetter, readOnlySetter));}
        return Super::initAttributeControllers();
    }


    int NativeTextBox::calculateMinimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        width += Defaults::textPadding();
        return width;
    }


    int NativeTextBox::calculateMinimumHeight() const
    {
        return Defaults::controlHeight() * String2Int(owningElement()->getAttribute("rows"), 1);
    }


    void NativeTextBox::handleCommand(WPARAM wParam, LPARAM lParam)
    {
    }


    NativeLabel::NativeLabel(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("STATIC"),
                      0, // exStyle
                      SS_LEFT)
    {
    }
        
    
    bool NativeLabel::initAttributeControllers()
    {
        AttributeGetter valueGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter valueSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("value", AttributeController(valueGetter, valueSetter));
        return Super::initAttributeControllers();
    }
    
    
    bool NativeLabel::initStyleControllers()
    {
        struct Helper
        {
            static LONG String2TextAlign(const std::string & inTextAlign)
            {
                if (inTextAlign == "left")
                {
                    return SS_LEFT;
                }
                else if (inTextAlign == "center")
                {
                    return SS_CENTER;
                }
                else if (inTextAlign == "right")
                {
                    return SS_RIGHT;
                }
                else
                {
                    ReportError("Unrecognized value for text-align style property: '" + inTextAlign + "'");
                }
                return 0;
            }
        };
        StyleGetter textAlignGetter; // no getter
        StyleSetter textAlignSetter = boost::bind(&Utils::addWindowStyle, handle(), boost::bind(&Helper::String2TextAlign, _1));
        setStyleController("text-align", StyleController(textAlignGetter, textAlignSetter));
        return Super::initStyleControllers();
    }


    int NativeLabel::calculateMinimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        return width;
    }

    
    int NativeLabel::calculateMinimumHeight() const
    {
        return Utils::getTextSize(handle(), Utils::getWindowText(handle())).cy;
    }


    NativeDescription::NativeDescription(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("STATIC"),
                      0, // exStyle
                      SS_LEFT)
    {
    }


    bool NativeDescription::initAttributeControllers()
    {
        AttributeGetter valueGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter valueSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("value", AttributeController(valueGetter, valueSetter));
        return Super::initAttributeControllers();
    }


    int NativeDescription::calculateMinimumWidth() const
    {
        //std::string text = Utils::getWindowText(handle());
        //int width = Utils::getTextSize(handle(), text).cx;
        //width += Defaults::textPadding();
        //return width;
        return 0;
    }

    
    int NativeDescription::calculateMinimumHeight() const
    {
        return Utils::getMultilineTextHeight(handle());
    }
    
    
    NativeHBox::NativeHBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeBox(inParent, inAttributesMapping, HORIZONTAL)
    {   
    }
    
    
    NativeVBox::NativeVBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeBox(inParent, inAttributesMapping, VERTICAL)
    {   
    }
        
        
    NativeBox::NativeBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orientation inOrientation) :
        VirtualControl(inParent, inAttributesMapping),
        mOrientation(inOrientation),
        mAlign(inOrientation == HORIZONTAL ? Start : Stretch)
    {
    }


    bool NativeBox::initAttributeControllers()
    {        
        AttributeSetter orientationSetter = boost::bind(&NativeBox::setOrientation, this, boost::bind(&String2Orientation, _1, VERTICAL));
        AttributeGetter orientationGetter = boost::bind(&Orientation2String, boost::bind(&NativeBox::getOrientation, this));
        setAttributeController("orient", AttributeController(orientationGetter, orientationSetter));
        setAttributeController("orientation", AttributeController(orientationGetter, orientationSetter));

        AttributeSetter alignSetter = boost::bind(&NativeBox::setAlignment, this, boost::bind(&String2Align, _1, Start));
        AttributeGetter alignGetter = boost::bind(&Align2String, boost::bind(&NativeBox::getAlignment, this));
        setAttributeController("align", AttributeController(alignGetter, alignSetter));
        return Super::initAttributeControllers();
    }


    int NativeBox::calculateMinimumWidth() const
    {
        if (mOrientation == HORIZONTAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != mElement->children().size(); ++idx)
            {
                result += mElement->children()[idx]->impl()->minimumWidth();
            }
            return result;
        }
        else if (mOrientation == VERTICAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != mElement->children().size(); ++idx)
            {
                int width = mElement->children()[idx]->impl()->minimumWidth();
                if (width > result)
                {
                    result = width;
                }
            }
            return result;
        }
        else
        {
            ReportError("Invalid orientation in NativeBox"); 
            return 0;
        }
    }


    int NativeBox::calculateMinimumHeight() const
    {
        if (mOrientation == HORIZONTAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != mElement->children().size(); ++idx)
            {
                int height = mElement->children()[idx]->impl()->minimumHeight();
                if (height > result)
                {
                    result = height;
                }
            }
            return result;
        }
        else if (mOrientation == VERTICAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != mElement->children().size(); ++idx)
            {
                result += mElement->children()[idx]->impl()->minimumHeight();
            }
            return result;
        }
        else
        {
            ReportError("Invalid orientation in NativeBox");
            return 0;
        }
    }


    void NativeBox::setOrientation(Orientation inOrientation)
    {
        mOrientation = inOrientation;
    }

    
    Orientation NativeBox::getOrientation()
    {
        return mOrientation;
    }
        
        
    void NativeBox::setAlignment(Alignment inAlign)
    {
        mAlign = inAlign;
    }

    
    Alignment NativeBox::getAlignment() const
    {
        return mAlign;
    }

    
    void NativeBox::rebuildLayout()
    {
        LinearLayoutManager layout(mOrientation);
        bool horizontal = mOrientation == HORIZONTAL;
        
        std::vector<ExtendedSizeInfo> sizeInfos;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            int flexValue = String2Int(child->getAttribute("flex"));
            int minSize = horizontal ? child->impl()->minimumWidth() : child->impl()->minimumHeight();
            int minSizeOpposite = horizontal ? child->impl()->minimumHeight() : child->impl()->minimumWidth();
            sizeInfos.push_back(ExtendedSizeInfo(flexValue, minSize, minSizeOpposite, child->impl()->expansive()));
        }
        
        std::vector<Rect> childRects;
        layout.getRects(clientRect(), getAlignment(), sizeInfos, childRects);

        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            const Rect & rect = childRects[idx];
            child->impl()->move(rect.x(), rect.y(), rect.width(), rect.height());
        }

        rebuildChildLayouts();
    }
    
    
    NativeMenuList::NativeMenuList(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("COMBOBOX"),
                      0, // exStyle
                      CBS_DROPDOWNLIST)
    {
    }
    
    
    int NativeMenuList::calculateMinimumWidth() const
    {
        return Defaults::dropDownListMinimumWidth() + Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx;
    }


    int NativeMenuList::calculateMinimumHeight() const
    {
        return Defaults::controlHeight();
    }
    
    
    void NativeMenuList::move(int x, int y, int w, int h)
    {
        // The height of a combobox in Win32 is the height of the dropdown menu + the height of the widget itself.
        h = h + Utils::getComboBoxItemCount(handle()) * Defaults::dropDownListItemHeight();
        NativeControl::move(x, y, w, h);
    }


    void NativeMenuList::addMenuItem(const std::string & inText)
    {
        Utils::addStringToComboBox(handle(), inText);
        int count = Utils::getComboBoxItemCount(handle());		
		if (count == 1)
		{
            Utils::selectComboBoxItem(handle(), 0);
		}

        // size needs to be updated
        mParent->rebuildLayout();
    }


    void NativeMenuList::removeMenuItem(const std::string & inText)
    {
        int idx = Utils::findStringInComboBox(handle(), inText);
        if (idx == CB_ERR)
        {
            ReportError("MenuList: remove failed because item not found: '" + inText + "'.");
            return;
        }

        Utils::deleteStringFromComboBox(handle(), idx);

        // size needs to be updated
        mParent->rebuildLayout();
    }


    NativeSeparator::NativeSeparator(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("STATIC"),
                      0, // exStyle
                      SS_GRAYFRAME)
    {
        mExpansive = true;
    }
        
        
    int NativeSeparator::calculateMinimumWidth() const
    {
        return 1;
    }

    
    int NativeSeparator::calculateMinimumHeight() const
    {
        return 1;
    }


    NativeSpacer::NativeSpacer(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualControl(inParent, inAttributesMapping)
    {
    }

        
    int NativeSpacer::calculateMinimumWidth() const
    {
        return 0;
    }

     
    int NativeSpacer::calculateMinimumHeight() const
    {
        return 0;
    }


    NativeMenuButton::NativeMenuButton(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("BUTTON"),
                      0, // exStyle
                      BS_PUSHBUTTON)
    {
        //::SendMessage(handle(), TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

        //CommandId id;
        //// add to toolbar
        //std::wstring itemText = L"test";
        //TBBUTTON theToolBarButton;
        //theToolBarButton.idCommand = id.intValue();
        //theToolBarButton.fsState = TBSTATE_ENABLED;
        //theToolBarButton.fsStyle = BTNS_BUTTON | BTNS_DROPDOWN | BTNS_SHOWTEXT;
        //theToolBarButton.dwData = 0; 
        //theToolBarButton.iString = (INT_PTR)itemText.c_str();
        //theToolBarButton.iBitmap = I_IMAGENONE;

        //SendMessage(handle(), TB_ADDBUTTONS, 1, (LPARAM)(LPTBBUTTON)&theToolBarButton); 
        //SendMessage(handle(), TB_AUTOSIZE, 0, 0);
        //ShowWindow(handle(), SW_SHOW); 
    }
        
        
    int NativeMenuButton::calculateMinimumWidth() const
    {
        return Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx + Defaults::textPadding()*2;
    }

    
    int NativeMenuButton::calculateMinimumHeight() const
    {
        return Defaults::buttonHeight();
    }


    NativeGrid::NativeGrid(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualControl(inParent, inAttributesMapping)
    {
    }
        
        
    int NativeGrid::calculateMinimumWidth() const
    {
        int result = 0;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            if (NativeColumns * cols = child->impl()->downcast<NativeColumns>())
            {
                for (size_t idx = 0; idx != cols->owningElement()->children().size(); ++idx)
                {
                    if (NativeColumn * col = cols->owningElement()->children()[idx]->impl()->downcast<NativeColumn>())
                    {
                        result += col->minimumWidth();
                    }
                }
            }
        }
        return result;
    }

    
    int NativeGrid::calculateMinimumHeight() const
    {
        int result = 0;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            if (NativeRows * rows = child->impl()->downcast<NativeRows>())
            {
                for (size_t idx = 0; idx != rows->owningElement()->children().size(); ++idx)
                {
                    if (NativeRow * row = rows->owningElement()->children()[idx]->impl()->downcast<NativeRow>())
                    {
                        result += row->minimumHeight();
                    }
                }
            }
        }
        return result;
    }


    void NativeGrid::rebuildLayout()
    {
        //
        // Initialize helper variables
        //
        int numCols = 0;
        int numRows = 0;
        ElementPtr columns;
        ElementPtr rows;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            if (child->type() == Rows::Type())
            {
                rows = child;
                numRows = rows->children().size();
            }
            else if (child->type() == Columns::Type())
            {
                columns = child;
                numCols = columns->children().size();
            }
            else
            {
                ReportError("Grid contains incompatible child element: '" + child->type() + "'");
            }
        }
        if (!rows || !columns)
        {
            ReportError("Grid has no rows or no columns!");
            return;
        }


        //
        // Get column size infos (min width and flex)
        //
        std::vector<SizeInfo> colWidths;
        for (size_t colIdx = 0; colIdx != columns->children().size(); ++colIdx)
        {
            if (NativeColumn * col = columns->children()[colIdx]->impl()->downcast<NativeColumn>())
            {
                colWidths.push_back(SizeInfo(FlexWrap(String2Int(col->owningElement()->getAttribute("flex"), 0)), col->minimumWidth()));
            }
        }


        //
        // Get row size infos (min height and flex)
        //
        std::vector<SizeInfo> rowHeights;
        for (size_t rowIdx = 0; rowIdx != rows->children().size(); ++rowIdx)
        {
            if (NativeRow * row = rows->children()[rowIdx]->impl()->downcast<NativeRow>())
            {
                rowHeights.push_back(SizeInfo(FlexWrap(String2Int(row->owningElement()->getAttribute("flex"), 0)), row->minimumHeight()));
            }
        }


        //
        // Get bounding rect for all cells
        //
        GenericGrid<Rect> outerRects(numRows, numCols);
        GridLayoutManager::GetOuterRects(clientRect(), colWidths, rowHeights, outerRects);


        //
        // Get size info for each cell
        //
        GenericGrid<CellInfo> widgetInfos(numRows, numCols, CellInfo(0, 0, Start, Start));
        for (size_t rowIdx = 0; rowIdx != numRows; ++rowIdx)
        {
            if (NativeRow * row = rows->children()[rowIdx]->impl()->downcast<NativeRow>())
            {
                int rowHeight = row->minimumHeight();
                for (size_t colIdx = 0; colIdx != numCols; ++colIdx)
                {
                    if (NativeColumn * column = columns->children()[colIdx]->impl()->downcast<NativeColumn>())
                    {
                        if (colIdx < row->owningElement()->children().size())
                        {                            
                            ElementImpl * child = row->owningElement()->children()[colIdx]->impl();
                            widgetInfos.set(rowIdx, colIdx,
                                            CellInfo(child->minimumWidth(),
                                                     child->minimumHeight(), 
                                                     String2Align(row->owningElement()->getAttribute("align"), Stretch),
                                                     String2Align(column->owningElement()->getAttribute("align"), Stretch)));
                        }
                    }
                }
            }
        }


        //
        // Get inner rect for each cell
        //
        GenericGrid<Rect> innerRects(numRows, numCols);
        GridLayoutManager::GetInnerRects(outerRects, widgetInfos, innerRects);


        //
        // Apply inner rect to each widget inside a cell
        //
        for (size_t rowIdx = 0; rowIdx != numRows; ++rowIdx)
        {
            for (size_t colIdx = 0; colIdx != numCols; ++colIdx)
            {
                if (rowIdx < rows->children().size())
                {
                    ElementPtr rowEl = rows->children()[rowIdx];
                    if (colIdx < rowEl->children().size())
                    {
                        ElementImpl * child = rowEl->children()[colIdx]->impl();
                        const Rect & r = innerRects.get(rowIdx, colIdx);
                        child->move(r.x(), r.y(), r.width(), r.height());
                    }
                }
            }
        }

        //
        // Rebuild child layoutss
        //
        rebuildChildLayouts();
    }


    NativeRows::NativeRows(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualControl(inParent, inAttributesMapping)
    {
    }


    NativeColumns::NativeColumns(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualControl(inParent, inAttributesMapping)
    {
    }


    NativeRow::NativeRow(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualControl(inParent, inAttributesMapping)
    {
    }

    
    int NativeRow::calculateMinimumWidth() const
    {
        int res = 0;
        const Children & children = owningElement()->children();
        for (size_t idx = 0; idx != children.size(); ++idx)
        {
            ElementPtr child = children[idx];
            res += child->impl()->minimumWidth();
        }
        return res;
    }


    int NativeRow::calculateMinimumHeight() const
    {
        int res = 0;
        const Children & children = owningElement()->children();
        for (size_t idx = 0; idx != children.size(); ++idx)
        {
            ElementPtr child = children[idx];
            int h = child->impl()->minimumHeight();
            if (h > res)
            {
                res = h;
            }
        }
        return res;
    }


    NativeColumn::NativeColumn(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualControl(inParent, inAttributesMapping)
    {
    }

    
    int NativeColumn::calculateMinimumWidth() const
    {
        ElementPtr rows;
        int ownIndex = -1;
        Element * grid = owningElement()->parent()->parent();
        for (size_t idx = 0; idx != grid->children().size(); ++idx)
        {
            ElementPtr child = grid->children()[idx];
            if (child->type() == Rows::Type())
            {
                rows = child;
            }
            else if (child->type() == Columns::Type())
            {
                for (size_t ownI = 0; ownI != child->children().size(); ++ownI)
                {
                    if (child->children()[ownI]->impl()->commandId() == commandId())
                    {
                        ownIndex = ownI;
                    }
                }
            }
            if (rows && ownIndex != -1)
            {
                break;
            }
        }
        if (!rows)
        {
            ReportError("Could not find 'rows' element in Grid.");
            return 0;
        }
        if (ownIndex == -1)
        {
            ReportError("Column was unable to find itself in its parent container.");
            return 0;
        }

        int res = 0;
        for (size_t rowIdx = 0; rowIdx != rows->children().size(); ++rowIdx)
        {

            ElementPtr row = rows->children()[rowIdx];
            int w = row->children()[ownIndex]->impl()->minimumWidth();
            if (w > res)
            {
                res = w;
            }
        }
        return res;
    }


    int NativeColumn::calculateMinimumHeight() const
    {
        int res = 0;
        const Children & children = owningElement()->children();
        for (size_t idx = 0; idx != children.size(); ++idx)
        {
            ElementPtr child = children[idx];
            res += child->impl()->minimumHeight();
        }
        return res;
    }

    
    NativeRadioGroup::NativeRadioGroup(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeBox(inParent, inAttributesMapping)
    {
    }

    
    NativeRadio::NativeRadio(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("BUTTON"),
                      0, // exStyle
                      BS_RADIOBUTTON)
    {
    }


    int NativeRadio::calculateMinimumWidth() const
    {
        return Defaults::radioButtonMinimumWidth() + Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx;
    }

    
    int NativeRadio::calculateMinimumHeight() const
    {
        return Defaults::controlHeight();
    }

    
    NativeProgressMeter::NativeProgressMeter(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      PROGRESS_CLASS,
                      0, // exStyle
                      PBS_SMOOTH)
    {
        Utils::initializeProgressMeter(mHandle, 100);
    }


    int NativeProgressMeter::calculateMinimumWidth() const
    {
        return 80;
    }

    
    int NativeProgressMeter::calculateMinimumHeight() const
    {
        return 21;
    }


    bool NativeProgressMeter::initAttributeControllers()
    {
        AttributeSetter orientationSetter = boost::bind(&Utils::setProgressMeterProgress, handle(), boost::bind(&String2Int, _1));
        AttributeGetter orientationGetter = boost::bind(&Int2String, boost::bind(&Utils::getProgressMeterProgress, handle()));
        setAttributeController("value", AttributeController(orientationGetter, orientationSetter));
        return Super::initAttributeControllers();
    }

    
    NativeDeck::NativeDeck(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualControl(inParent, inAttributesMapping),
        mSelectedIndex(0)
    {
    }
        
        
    void NativeDeck::rebuildLayout()
    {
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr element = owningElement()->children()[idx];
            bool visible = idx == mSelectedIndex;
            if (NativeControl * nativeControl = element->impl()->downcast<NativeControl>())
            {
                Utils::setWindowVisible(nativeControl->handle(), visible);
            }
            if (visible)
            {
                Rect rect = clientRect();
                ElementImpl * n = element->impl();
                n->move(rect.x(), rect.y(), rect.width(), rect.height());
            }
        }
        rebuildChildLayouts();
    }


    int NativeDeck::calculateMinimumWidth() const
    {
        int res = 0;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            int w = owningElement()->children()[idx]->impl()->minimumWidth();
            if (w > res)
            {
                res = w;
            }
        }
        return res;
    }

    
    int NativeDeck::calculateMinimumHeight() const
    {
        int res = 0;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            int h = owningElement()->children()[idx]->impl()->minimumHeight();
            if (h > res)
            {
                res = h;
            }
        }
        return res;
    }

    
    void NativeDeck::setSelectedIndex(int inSelectedIndex)
    {
        mSelectedIndex = inSelectedIndex;
        rebuildLayout();
    }

    
    int NativeDeck::selectedIndex() const
    {
        return mSelectedIndex;
    }


    bool NativeDeck::initAttributeControllers()
    {
        AttributeSetter orientationSetter = boost::bind(&NativeDeck::setSelectedIndex, this, boost::bind(&String2Int, _1));
        AttributeGetter orientationGetter = boost::bind(&Int2String, boost::bind(&NativeDeck::selectedIndex, this));
        setAttributeController("selectedIndex", AttributeController(orientationGetter, orientationSetter));
        return Super::initAttributeControllers();
    }


} // namespace XULWin
