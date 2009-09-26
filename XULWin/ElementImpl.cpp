#include "ElementImpl.h"
#include "Decorator.h"
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


    std::string Orientation2String(Orientation inOrient)
    {
        if (inOrient == HORIZONTAL)
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
    
    NativeControl::ComponentsById NativeControl::sComponentsById;

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


            static void SetMargin(ElementImpl * inEl, int inMargin)
            {
                if (MarginDecorator * obj = inEl->owningElement()->impl()->downcast<MarginDecorator>())
                {
                    obj->setMargin(inMargin);
                }
                else if (Decorator * dec = inEl->owningElement()->impl()->downcast<Decorator>())
                {
                    ElementImplPtr newDec(new MarginDecorator(dec->decoratedElement()));
                    dec->setDecoratedElement(newDec);
                    if (MarginDecorator * p = newDec->downcast<MarginDecorator>())
                    {
                        p->setMargin(inMargin);
                    }
                }
            }

            static int GetMargin(ElementImpl * inEl)
            {
                return inEl->downcast<MarginDecorator>()->margin();
            }


            static void SetOverflow(ElementImpl * inEl, const std::string & inOverflow, Orientation inOrient)
            {
                // MDC documenation
                //visible: Default value. Content is not clipped, it may be rendered outside the content box.
                //hidden: The content is clipped and no scrollbars are provided.
                //scroll: The content is clipped and desktop browsers use scrollbars, whether or not any content is clipped. This avoids any problem with scrollbars appearing and disappearing in a dynamic environment. Printers may print overflowing content.
                //auto: Depends on the user agent. Desktop browsers like Firefox provide scrollbars if content overflows.
                if (inOverflow == "auto")
                {
                    if (ScrollDecorator * obj = inEl->owningElement()->impl()->downcast<ScrollDecorator>())
                    {
                        ReportError("Overwriting 'overflow' attribute not yet supported.");
                    }
                    else if (Decorator * dec = inEl->owningElement()->impl()->downcast<Decorator>())
                    {
                        ElementImplPtr newDec(new ScrollDecorator(dec->decoratedElement(), inOrient));
                        dec->setDecoratedElement(newDec);
                    }                    
                }
            }
        };
        StyleGetter cssWidthGetter = boost::bind(&Int2String, boost::bind(&Helper::GetWidth, this));
        StyleSetter cssWidthSetter = boost::bind(&Helper::SetWidth, this, boost::bind(&CssString2Size, _1, Defaults::controlWidth()));
        setStyleController("width", StyleController(cssWidthGetter, cssWidthSetter));

        StyleGetter cssHeightGetter = boost::bind(&Int2String, boost::bind(&Helper::GetHeight, this));
        StyleSetter cssHeightSetter = boost::bind(&Helper::SetHeight, this, boost::bind(&CssString2Size, _1, Defaults::controlHeight()));
        setStyleController("height", StyleController(cssHeightGetter, cssHeightSetter));

        StyleGetter cssMarginGetter = boost::bind(&Int2String, boost::bind(&Helper::GetMargin, this));
        StyleSetter cssMarginSetter = boost::bind(&Helper::SetMargin, this, boost::bind(&CssString2Size, _1, 0));
        setStyleController("margin", StyleController(cssMarginGetter, cssMarginSetter));

        StyleGetter overflowGetter; // no getter
        StyleSetter overflowSetterX = boost::bind(&Helper::SetOverflow, this, _1, HORIZONTAL);
        StyleSetter overflowSetterY = boost::bind(&Helper::SetOverflow, this, _1, VERTICAL);
        setStyleController("overflow-x", StyleController(overflowGetter, overflowSetterX));
        setStyleController("overflow-y", StyleController(overflowGetter, overflowSetterY));
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


    LRESULT NativeComponent::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        switch (inMessage)
        {
            case WM_COMMAND:
            {
                ComponentsById::iterator it = sComponentsById.find(LOWORD(wParam));
                if (it != sComponentsById.end())
                {
                    it->second->handleCommand(wParam, lParam);
                }
                break;
            }
            // These messages get forwarded to the child elements that produced them.
            case WM_VSCROLL:
            case WM_HSCROLL:
            {
                HWND handle = (HWND)lParam;
                Components::iterator it = sComponentsByHandle.find(handle);
                if (it != sComponentsByHandle.end())
                {
                    it->second->handleMessage(inMessage, wParam, lParam);
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

    
    LRESULT CALLBACK NativeComponent::MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        Components::iterator it = sComponentsByHandle.find(hWnd);
        if (it != sComponentsByHandle.end())
        {
            return it->second->handleMessage(inMessage, wParam, lParam);
        }
        return ::DefWindowProc(hWnd, inMessage, wParam, lParam);
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
            case WM_COMMAND:
            {
                int id = LOWORD(wParam);
                ComponentsById::iterator it = sComponentsById.find(id);
                if (it != sComponentsById.end())
                {
                    it->second->handleCommand(wParam, lParam);
                }
                break;
            }
            // These messages get forwarded to the child elements that produced them.
            case WM_VSCROLL:
            case WM_HSCROLL:
            {
                HWND handle = (HWND)lParam;
                Components::iterator it = sComponentsByHandle.find(handle);
                if (it != sComponentsByHandle.end())
                {
                    it->second->handleMessage(inMessage, wParam, lParam);
                }
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
        ElementImpl(inParent)
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

     
    bool VirtualControl::initAttributeControllers()
    {
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
        sComponentsById.insert(std::make_pair(mCommandId.intValue(), this));
    }


    NativeControl::~NativeControl()
    {
        if (mOrigProc)
        {
            ::SetWindowLongPtr(mHandle, GWL_WNDPROC, (LONG)(LONG_PTR)mOrigProc);
            mOrigProc = 0;
        }

        ComponentsById::iterator itById = sComponentsById.find(mCommandId.intValue());
        bool foundById = itById != sComponentsById.end();
        assert(foundById);
        if (foundById)
        {
            sComponentsById.erase(itById);
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
                      GetFlags(inAttributesMapping)),
                      mReadonly(IsReadOnly(inAttributesMapping))
    {
        
    }


    DWORD NativeTextBox::GetFlags(const AttributesMapping & inAttributesMapping)
    {
        DWORD flags = 0;
        AttributesMapping::const_iterator it = inAttributesMapping.find("type");
        if (it != inAttributesMapping.end() && it->second == "password")
        {
            flags |= ES_PASSWORD;
        }
        it = inAttributesMapping.find("multiline");
        if (it != inAttributesMapping.end() && it->second == "true")
        {
            flags |= WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN;            
        }
        else
        {
            flags |= ES_AUTOHSCROLL;            
        }
        return flags;
    } 


    bool NativeTextBox::IsReadOnly(const AttributesMapping & inAttributesMapping)
    {
        AttributesMapping::const_iterator it = inAttributesMapping.find("readonly");
        return it != inAttributesMapping.end() && it->second == "true";
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
        return Defaults::textPadding();
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
        
    
    NativeBox::NativeBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orientation inOrient) :
        VirtualControl(inParent, inAttributesMapping),
        BoxLayouter(inOrient, inOrient == HORIZONTAL ? Start : Stretch)
    {
    }
        
    
    void NativeBox::setAttributeController(const std::string & inAttr, const AttributeController & inController)
    {
        Super::setAttributeController(inAttr, inController);
    }


    bool NativeBox::initAttributeControllers()
    {        
        AttributeSetter orientationSetter = boost::bind(&NativeBox::setOrientation, this, boost::bind(&String2Orientation, _1, VERTICAL));
        AttributeGetter orientationGetter = boost::bind(&Orientation2String, boost::bind(&NativeBox::orientation, this));
        setAttributeController("orient", AttributeController(orientationGetter, orientationSetter));
        setAttributeController("orientation", AttributeController(orientationGetter, orientationSetter));

        AttributeSetter alignSetter = boost::bind(&NativeBox::setAlignment, this, boost::bind(&String2Align, _1, Start));
        AttributeGetter alignGetter = boost::bind(&Align2String, boost::bind(&NativeBox::alignment, this));
        setAttributeController("align", AttributeController(alignGetter, alignSetter));
        return Super::initAttributeControllers();
    }


    int BoxLayouter::calculateMinimumWidth() const
    {
        if (orientation() == HORIZONTAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != elementChildren().size(); ++idx)
            {
                result += elementChildren()[idx]->impl()->minimumWidth();
            }
            return result;
        }
        else if (orientation() == VERTICAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != elementChildren().size(); ++idx)
            {
                int width = elementChildren()[idx]->impl()->minimumWidth();
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


    int BoxLayouter::calculateMinimumHeight() const
    {
        if (orientation() == HORIZONTAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != elementChildren().size(); ++idx)
            {
                int height = elementChildren()[idx]->impl()->minimumHeight();
                if (height > result)
                {
                    result = height;
                }
            }
            return result;
        }
        else if (orientation() == VERTICAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != elementChildren().size(); ++idx)
            {
                result += elementChildren()[idx]->impl()->minimumHeight();
            }
            return result;
        }
        else
        {
            ReportError("Invalid orientation in NativeBox");
            return 0;
        }
    }
    
    
    Orientation BoxLayouter::orientation() const
    {
        return mOrient;
    }

     
    Alignment BoxLayouter::alignment() const
    {
        return mAlign;
    }


    void BoxLayouter::setOrientation(Orientation inOrient)
    {
        mOrient = inOrient;
    }
        
        
    void BoxLayouter::setAlignment(Alignment inAlign)
    {
        mAlign = inAlign;
    }


    BoxLayouter::BoxLayouter(Orientation inOrient, Alignment inAlign) :
        mOrient(inOrient),
        mAlign(inAlign)
    {
    }


    bool BoxLayouter::initAttributeControllers()
    {
        ElementImpl::AttributeSetter orientationSetter = boost::bind(&BoxLayouter::setOrientation, this, boost::bind(&String2Orientation, _1, VERTICAL));
        ElementImpl::AttributeGetter orientationGetter = boost::bind(&Orientation2String, boost::bind(&BoxLayouter::orientation, this));
        setAttributeController("orient", ElementImpl::AttributeController(orientationGetter, orientationSetter));
        setAttributeController("orientation", ElementImpl::AttributeController(orientationGetter, orientationSetter));

        ElementImpl::AttributeSetter alignSetter = boost::bind(&BoxLayouter::setAlignment, this, boost::bind(&String2Align, _1, Start));
        ElementImpl::AttributeGetter alignGetter = boost::bind(&Align2String, boost::bind(&BoxLayouter::alignment, this));
        setAttributeController("align", ElementImpl::AttributeController(alignGetter, alignSetter));
        return true;
    }

    
    void BoxLayouter::rebuildLayout()
    {
        LinearLayoutManager layout(orientation());
        bool horizontal = orientation() == HORIZONTAL;
        
        std::vector<ExtendedSizeInfo> sizeInfos;
        for (size_t idx = 0; idx != elementChildren().size(); ++idx)
        {
            ElementPtr child = elementChildren()[idx];
            int flexValue = String2Int(child->getAttribute("flex"));
            int minSize = horizontal ? child->impl()->minimumWidth() : child->impl()->minimumHeight();
            int minSizeOpposite = horizontal ? child->impl()->minimumHeight() : child->impl()->minimumWidth();
            sizeInfos.push_back(ExtendedSizeInfo(flexValue, minSize, minSizeOpposite, child->impl()->expansive()));
        }
        
        std::vector<Rect> childRects;
        layout.getRects(clientRect(), alignment(), sizeInfos, childRects);

        for (size_t idx = 0; idx != elementChildren().size(); ++idx)
        {
            ElementPtr child = elementChildren()[idx];
            const Rect & rect = childRects[idx];
            child->impl()->move(rect.x(), rect.y(), rect.width(), rect.height());
        }

        rebuildChildLayouts();
    }
        

    NativeScrollBox::NativeScrollBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orientation inOrient) :
        NativeControl(inParent, inAttributesMapping, TEXT("STATIC"), 0, 0),
        BoxLayouter(inOrient, inOrient == HORIZONTAL ? Start : Stretch)
    {
    }
        
    
    void NativeScrollBox::setAttributeController(const std::string & inAttr, const AttributeController & inController)
    {
        Super::setAttributeController(inAttr, inController);
    }
    
        
    bool NativeScrollBox::initAttributeControllers()
    {
        BoxLayouter::initAttributeControllers();
        return Super::initAttributeControllers();
    }


    const std::vector<ElementPtr> & NativeScrollBox::elementChildren() const
    {
        return mElement->children();
    }
    
    
    void NativeScrollBox::rebuildLayout()
    {
        BoxLayouter::rebuildLayout();
    }

    
    int NativeScrollBox::calculateMinimumWidth() const
    {
        return BoxLayouter::calculateMinimumWidth();
    }

    
    int NativeScrollBox::calculateMinimumHeight() const
    {
        return BoxLayouter::calculateMinimumHeight();
    }
    
    
    Rect NativeScrollBox::clientRect() const
    {
        return Super::clientRect();
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
        return 1;
    }

    
    int NativeProgressMeter::calculateMinimumHeight() const
    {
        return Defaults::progressMeterHeight();
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
        AttributeSetter selIndexSetter = boost::bind(&NativeDeck::setSelectedIndex, this, boost::bind(&String2Int, _1));
        AttributeGetter selIndexGetter = boost::bind(&Int2String, boost::bind(&NativeDeck::selectedIndex, this));
        setAttributeController("selectedIndex", AttributeController(selIndexGetter, selIndexSetter));
        return Super::initAttributeControllers();
    }


    DWORD NativeScrollbar::GetFlags(const AttributesMapping & inAttributesMapping)
    {
        DWORD flags = 0;
        AttributesMapping::const_iterator it = inAttributesMapping.find("orient");
        if (it != inAttributesMapping.end())
        {
            const std::string & orient = it->second;
            if (orient == "horizontal")
            {
                flags |= SBS_HORZ | SBS_RIGHTALIGN;
            }
            else if (orient == "vertical")
            {
                flags |= SBS_VERT | SBS_BOTTOMALIGN;
            }
            else
            {
                ReportError("Invalid orient found for scrollbar!");
            }
        }
        return flags;
    }
    
    
    NativeScrollbar::NativeScrollbar(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping,
                      TEXT("SCROLLBAR"),
                      0, // exStyle
                      GetFlags(inAttributesMapping)),
        mEventHandler(0),
        mIncrement(0)
    {
        mExpansive = true;
        Utils::setScrollInfo(handle(), 100, 10, 0);
    }


    int NativeScrollbar::calculateMinimumWidth() const
    {
        return Defaults::scrollbarWidth();
    }

    
    int NativeScrollbar::calculateMinimumHeight() const
    {
        return Defaults::scrollbarWidth();
    }


    void NativeScrollbar::setIncrement(int inIncrement)
    {
        mIncrement = inIncrement;
    }


    int NativeScrollbar::increment() const
    {
        return mIncrement;
    }
    
    
    LRESULT NativeScrollbar::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        if (inMessage == WM_VSCROLL || inMessage == WM_HSCROLL)
        {
            int currentScrollPos = Utils::getScrollPos(handle());
            int totalHeight = 0;
            int pageHeight = 0;
            int currentPosition = 0;
            Utils::getScrollInfo(handle(), totalHeight, pageHeight, currentPosition);
			switch (LOWORD(wParam))
			{
				case SB_LINEUP: // user clicked the top arrow
                {
                    currentPosition -= 1;
					break;
                }
				case SB_LINEDOWN: // user clicked the bottom arrow
                {
                    currentPosition += 1;
					break;
                }				
				case SB_PAGEUP: // user clicked the scroll bar shaft above the scroll box
                {
                    currentPosition -= pageHeight;
					break;
                }				
				case SB_PAGEDOWN: // user clicked the scroll bar shaft below the scroll box
                {
                    currentPosition += pageHeight;
					break;
                }				
				case SB_THUMBTRACK: // user dragged the scroll box
                {
                    currentPosition = HIWORD(wParam);
					break;
                }
				default:
                {
					break; 
                }
			}
            if (currentPosition < 0)
            {
                currentPosition = 0;
            }
            if (currentPosition > totalHeight)
            {
                currentPosition = totalHeight;
            }
            setAttribute("curpos", Int2String(currentPosition));
            return 0;
        }
        return NativeControl::handleMessage(inMessage, wParam, lParam);
    }


    bool NativeScrollbar::initAttributeControllers()
    {
        // TODO: implement!
        //curpos="20"
        //maxpos="100"
        //increment="1"
        //pageincrement="10"/>
        struct Helper
        {
            static void setCurPos(NativeScrollbar * el, int inCurPos)
            {
                int totalHeight = 0;
                int pageHeight = 0;
                int oldCurPos = 0;
                Utils::getScrollInfo(el->handle(), totalHeight, pageHeight, oldCurPos);

                // The order in which setCurPos, setMaxPos and setPageIncrement
                // will be set (alphabetically by attribute name) can cause
                // impossible scrollbar states (i.e. currentpos or pageincrement
                // greater than maxpos). And we want to avoid that.
                // Our workaround is to detect such states here, and change invalid
                // values to valid ones.
                if (pageHeight == 0)
                {
                    pageHeight = 1;
                }
                if (totalHeight < pageHeight)
                {
                    totalHeight = pageHeight + 1;
                }
                //if (inCurPos > pageHeight/2)   // }
                //{                              // } => this makes sure that the scroll box
                //    inCurPos -= pageHeight/2;  // }    is centered around currentpos
                //}                              // }
                if (totalHeight < inCurPos)
                {
                    totalHeight = inCurPos + 1;
                }
                Utils::setScrollInfo(el->handle(), totalHeight, pageHeight, inCurPos);
                if (el->eventHandler())
                {
                    el->eventHandler()->curposChanged(el, oldCurPos, inCurPos);
                }
            }

            static int getCurPos(HWND inHandle)
            {
                int totalHeight = 0;
                int pageHeight = 0;
                int curPos = 0;
                Utils::getScrollInfo(inHandle, totalHeight, pageHeight, curPos);
                return curPos;
            }

            static void setMaxPos(HWND inHandle, int inMaxPos)
            {
                int dummy = 0;
                int pageHeight = 0;
                int curPos = 0;
                Utils::getScrollInfo(inHandle, dummy, pageHeight, curPos);

                // The order in which setCurPos, setMaxPos and setPageIncrement
                // will be set (alphabetically by attribute name) can cause
                // impossible scrollbar states (i.e. currentpos or pageincrement
                // greater than maxpos). And we want to avoid that.
                // Our workaround is to detect such states here, and change invalid
                // values to valid ones.
                if (pageHeight == 0)
                {
                    pageHeight = 1;
                }
                if (inMaxPos <= pageHeight)
                {
                    pageHeight = inMaxPos - 1;
                }
                //if (curPos > pageHeight/2)     // }
                //{                              // } => this makes sure that the scroll box
                //    curPos -= pageHeight/2;    // }    is centered around currentpos
                //}                              // }
                Utils::setScrollInfo(inHandle, inMaxPos, pageHeight, curPos);
            }

            static int getMaxPos(HWND inHandle)
            {
                int totalHeight = 0;
                int pageHeight = 0;
                int curPos = 0;
                Utils::getScrollInfo(inHandle, totalHeight, pageHeight, curPos);
                return totalHeight;
            }

            static void setPageIncrement(HWND inHandle, int inPageIncrement)
            {
                int totalHeight = 0;
                int dummy = 0;
                int curPos = 0;
                Utils::getScrollInfo(inHandle, totalHeight, dummy, curPos);

                // The order in which setCurPos, setMaxPos and setPageIncrement
                // will be set (alphabetically by attribute name) can cause
                // impossible scrollbar states (i.e. currentpos or pageincrement
                // greater than maxpos). And we want to avoid that.
                // Our workaround is to detect such states here, and change invalid
                // values to valid ones.
                //if (curPos > inPageIncrement/2)     // }
                //{                                   // } => this makes sure that the scroll box 
                //    curPos -= inPageIncrement/2;    // }    is centered around currentpos
                //}                                   // }
                if (totalHeight == 0)
                {
                    totalHeight = 1;
                }
                if (curPos > totalHeight)
                {
                    totalHeight += 1;
                }
                if (inPageIncrement >= totalHeight)
                {
                    totalHeight = inPageIncrement + 1;
                }
                Utils::setScrollInfo(inHandle, totalHeight, inPageIncrement, curPos);
            }

            static int getPageIncrement(HWND inHandle)
            {
                int totalHeight = 0;
                int pageHeight = 0;
                int curPos = 0;
                Utils::getScrollInfo(inHandle, totalHeight, pageHeight, curPos);
                return pageHeight;
            }
        };
        
        AttributeSetter pageIncrementSetter = boost::bind(&Helper::setPageIncrement, handle(), boost::bind(&String2Int, _1, Defaults::Attributes::pageincrement()));
        AttributeGetter pageIncrementGetter = boost::bind(&Int2String, boost::bind(&Helper::getPageIncrement, handle()));
        setAttributeController("pageincrement", AttributeController(pageIncrementGetter, pageIncrementSetter));        
        
        
        AttributeSetter maxPosSetter = boost::bind(&Helper::setMaxPos, handle(), boost::bind(&String2Int, _1, Defaults::Attributes::maxpos()));
        AttributeGetter maxPosGetter = boost::bind(&Int2String, boost::bind(&Helper::getMaxPos, handle()));
        setAttributeController("maxpos", AttributeController(maxPosGetter, maxPosSetter));        
        
        
        AttributeSetter curPosSetter = boost::bind(&Helper::setCurPos, this, boost::bind(&String2Int, _1, Defaults::Attributes::curpos()));
        AttributeGetter curPosGetter = boost::bind(&Int2String, boost::bind(&Helper::getCurPos, handle()));
        setAttributeController("curpos", AttributeController(curPosGetter, curPosSetter));        
        
        
        AttributeSetter incrementSetter = boost::bind(&NativeScrollbar::setIncrement, this, boost::bind(&String2Int, _1, 1));
        AttributeGetter incrementGetter = boost::bind(&Int2String, boost::bind(&NativeScrollbar::increment, this));
        setAttributeController("increment", AttributeController(incrementGetter, incrementSetter));        
        return Super::initAttributeControllers();
    }


} // namespace XULWin
