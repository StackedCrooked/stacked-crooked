#include "ElementImpl.h"
#include "Defaults.h"
#include "Layout.h"
#include "Utils/ErrorReporter.h"
#include "Utils/WinUtils.h"
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


    int String2Int(const std::string & inValue)
    {
        int result = 0;
        try
        {
            result = boost::lexical_cast<int>(inValue);
        }
        catch (std::exception &)
        {
            ReportError("Int2String: lexical cast failed");
        }
        return result;
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
    
    
    bool ElementImpl::expansive() const
    {
        return mExpansive;
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
        return true;
    }


    void ElementImpl::setStyleController(const std::string & inAttr, const StyleController & inController)
    {
        StyleControllers::iterator it = mStyleControllers.find(inAttr);
        if (it != mStyleControllers.end())
        {
            mStyleControllers.erase(it);
        }
        mStyleControllers.insert(std::make_pair(inAttr, inController));
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
        Children::const_iterator it = mElement->children().begin(), end = mElement->children().end();
        for (; it != end; ++it)
        {
            ElementImpl * nativeComp = (*it)->impl();
            if (nativeComp)
            {
                nativeComp->rebuildLayout();
            }
        }
    }

    
    HMODULE NativeComponent::sModuleHandle(0);


    NativeComponent::NativeComponent(ElementImpl * inParent) :
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


    NativeWindow::NativeWindow() :
        NativeComponent(0)
    {
        mHandle = ::CreateWindowEx
        (
            0, 
            TEXT("XULWin::Window"),
            TEXT(""),
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
            1, 1, 1, 1,
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
    
    
    int NativeWindow::minimumWidth() const
    {
        if (!owningElement()->children().empty())
        {
            return owningElement()->children().begin()->get()->impl()->minimumWidth();
        }
        return 0;
    }
    
    
    int NativeWindow::minimumHeight() const
    {
        if (!owningElement()->children().empty())
        {
            return owningElement()->children().begin()->get()->impl()->minimumHeight();
        }
        return 0;
    }
    
    
    void NativeWindow::move(int x, int y, int w, int h)
    {
        ::MoveWindow(handle(), x, y, w, h, FALSE);
    }

    
    void NativeWindow::rebuildLayout()
    {     
        rebuildChildLayouts();
        ::InvalidateRect(handle(), 0, FALSE);
    }


    void NativeWindow::showModal()
    {      
        {
            SIZE sizeDiff = GetSizeDifference_WindowRect_ClientRect(handle());
            int w = minimumWidth() + sizeDiff.cx;
            int h = minimumHeight() + sizeDiff.cy;
            int x = (GetSystemMetrics(SM_CXSCREEN) - w)/2;
            int y = (GetSystemMetrics(SM_CYSCREEN) - h)/2;
            move(x, y, w, h);
            rebuildLayout();
        }   
        {
            SIZE sizeDiff = GetSizeDifference_WindowRect_ClientRect(handle());
            int w = minimumWidth() + sizeDiff.cx;
            int h = minimumHeight() + sizeDiff.cy;
            int x = (GetSystemMetrics(SM_CXSCREEN) - w)/2;
            int y = (GetSystemMetrics(SM_CYSCREEN) - h)/2;
            move(x, y, w, h);
            rebuildLayout();
        }
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
                minMaxInfo->ptMinTrackSize.x = minimumWidth() + sizeDiff.cx;
                minMaxInfo->ptMinTrackSize.y = minimumHeight() + sizeDiff.cy;
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


    VirtualControl::VirtualControl(ElementImpl * inParent) :
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
    
    
    ElementImpl * Decorator::decoratedElement()
    {
        return mDecoratedElement.get();
    }

    
    const ElementImpl * Decorator::decoratedElement() const
    {
        return mDecoratedElement.get();
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
        mTop(4),
        mLeft(4),
        mRight(4),
        mBottom(4)
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
    
    
    int PaddingDecorator::minimumWidth() const
    {
        return paddingLeft() + mDecoratedElement->minimumWidth() + paddingRight();
    }

    
    int PaddingDecorator::minimumHeight() const
    {
        return paddingTop() + mDecoratedElement->minimumHeight() + paddingBottom();
    }


    NativeControl::NativeControl(ElementImpl * inParent, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle) :
        NativeComponent(inParent)
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
            return GetNativeParent(obj->decoratedElement());
        }
        else if (VirtualControl * obj = dynamic_cast<VirtualControl*>(inElementImpl))
        {
            return GetNativeParent(obj->parent());
        }
        return 0;
    }
    
    
    NativeButton::NativeButton(ElementImpl * inParent) :
        NativeControl(inParent,
                      TEXT("BUTTON"),
                      0, // exStyle
                      BS_PUSHBUTTON)
    {   
    }
    
    
    int NativeButton::minimumHeight() const
    {
        return Defaults::buttonHeight();
    }
    
    
    int NativeButton::minimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        width += Defaults::textPadding();
        return width;
    }
    
    
    NativeCheckBox::NativeCheckBox(ElementImpl * inParent) :
        NativeControl(inParent, TEXT("BUTTON"), 0, BS_AUTOCHECKBOX)
    {
    }
    
    
    int NativeCheckBox::minimumHeight() const
    {
        return Defaults::controlHeight();
    }
    
    
    int NativeCheckBox::minimumWidth() const
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


    NativeTextBox::NativeTextBox(ElementImpl * inParent) :
        NativeControl(inParent,
                      TEXT("EDIT"),
                      WS_EX_CLIENTEDGE, // exStyle
                      ES_AUTOHSCROLL)
    {
    }

    
    bool NativeTextBox::initAttributeControllers()
    {
        AttributeGetter valueGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter valueSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("value", AttributeController(valueGetter, valueSetter));
        return Super::initAttributeControllers();
    }


    int NativeTextBox::minimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        width += Defaults::textPadding();
        return width;
    }


    int NativeTextBox::minimumHeight() const
    {
        return Defaults::controlHeight();
    }


    void NativeTextBox::handleCommand(WPARAM wParam, LPARAM lParam)
    {
    }


    NativeLabel::NativeLabel(ElementImpl * inParent) :
        NativeControl(inParent,
                      TEXT("STATIC"),
                      0, // exStyle
                      SS_LEFT | SS_CENTERIMAGE)
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


    int NativeLabel::minimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        return width;
    }

    
    int NativeLabel::minimumHeight() const
    {
        return Defaults::controlHeight();
    }


    NativeDescription::NativeDescription(ElementImpl * inParent) :
        NativeControl(inParent,
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


    int NativeDescription::minimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        width += Defaults::textPadding();
        return width;
    }

    
    int NativeDescription::minimumHeight() const
    {
        return Utils::getMultilineTextHeight(handle());
    }
    
    
    NativeHBox::NativeHBox(ElementImpl * inParent) :
        NativeBox(inParent, HORIZONTAL)
    {   
    }
        
        
    NativeBox::NativeBox(ElementImpl * inParent, Orientation inOrientation) :
        VirtualControl(inParent),
        mOrientation(inOrientation),
        mAlign(inOrientation == HORIZONTAL ? Center : Stretch)
    {
    }


    bool NativeBox::initAttributeControllers()
    {        
        struct Helper
        {
            static Orientation String2Orientation(const std::string & inValue)
            {
                Orientation result = VERTICAL;
                if (inValue == "horizontal")
                {
                    result = HORIZONTAL;
                }
                return result;
            }
            static std::string Orientation2String(Orientation inOrientation)
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
            static Align String2Align(const std::string & inValue)
            {
                Align result = Start;
                if (inValue == "center")
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
            static std::string Align2String(Align inAlign)
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
        };
        AttributeSetter orientationSetter = boost::bind(&NativeBox::setOrientation, this, boost::bind(&Helper::String2Orientation, _1));
        AttributeGetter orientationGetter = boost::bind(&Helper::Orientation2String, boost::bind(&NativeBox::getOrientation, this));
        setAttributeController("orient", AttributeController(orientationGetter, orientationSetter));
        setAttributeController("orientation", AttributeController(orientationGetter, orientationSetter));

        AttributeSetter alignSetter = boost::bind(&NativeBox::setAlignment, this, boost::bind(&Helper::String2Align, _1));
        AttributeGetter alignGetter = boost::bind(&Helper::Align2String, boost::bind(&NativeBox::getAlignment, this));
        setAttributeController("align", AttributeController(alignGetter, alignSetter));
        return Super::initAttributeControllers();
    }


    int NativeBox::minimumWidth() const
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


    int NativeBox::minimumHeight() const
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
        
        
    void NativeBox::setAlignment(Align inAlign)
    {
        mAlign = inAlign;
    }

    
    NativeBox::Align NativeBox::getAlignment() const
    {
        return mAlign;
    }

    
    void NativeBox::rebuildLayout()
    {

        LinearLayoutManager layoutManager(mOrientation);
        
        //
        // Obtain the flex values
        //
        std::vector<int> allFlexValues;
        std::vector<SizeInfo> nonZeroFlexValues;
        Rect clientRect(clientRect());
        int availableSpace = clientRect.width();
        if (mOrientation == VERTICAL)
        {
            availableSpace = clientRect.height();
        }
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            ElementPtr child = mElement->children()[idx];
            std::string flex = child->getAttribute("flex");

            int flexValue = Defaults::Attributes::flex();
            if (!flex.empty())
            {
                try
                {
                    flexValue = boost::lexical_cast<int>(flex);
                }
                catch (const boost::bad_lexical_cast & )
                {
                    // TODO: this should be logged as warning instead of error reporting.
                    ReportError("Lexical cast failed for value: " + flex + ".");
                }
            }
            if (flexValue != 0)
            {
                nonZeroFlexValues.push_back(SizeInfo(flexValue,
                                                       mOrientation == HORIZONTAL ? child->impl()->minimumWidth() :
                                                                                    child->impl()->minimumHeight()));
            }
            else
            {
                if (mOrientation == HORIZONTAL)
                {
                    availableSpace -= child->impl()->minimumWidth();
                }
                else
                {
                    availableSpace -= child->impl()->minimumHeight();
                }
            }
            allFlexValues.push_back(flexValue);
        }
        
        //
        // Use the flex values to obtain the child rectangles
        //
        std::vector<int> portions;
        layoutManager.GetSizes(availableSpace, nonZeroFlexValues, portions);

        //
        // Apply the new child rectangles
        //
        int offsetBoxX = 0;
        int offsetChildX = 0;
        int offsetBoxY = 0;
        int offsetChildY = 0;
        int portionIdx = 0;
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            ElementPtr child = mElement->children()[idx];
            int minWidth = child->impl()->minimumWidth();
            int childWidth = minWidth;
            int boxWidth = minWidth;
            int minHeight = child->impl()->minimumHeight();
            int childHeight = minHeight;
            int boxHeight = minHeight;
            int portion = 0;
            if (allFlexValues[idx] != 0)
            {     
                portion = portions[portionIdx];
                portionIdx++;
            }
            else
            {
                portion = mOrientation == HORIZONTAL ? minWidth : minHeight;
            }
            struct Helper
            {
                static void calculateLengths(NativeBox::Align inAlignment,
                                             int portion,
                                             int minLength,
                                             int & boxLength,
                                             int & childLength, 
                                             int & childOffset, 
                                             int & childOffset2)
                {
                    boxLength = portion;
                    if (boxLength < minLength)
                    {
                        boxLength = minLength;
                    }
                    childLength = boxLength;
                    if (boxLength > minLength)
                    {
                        if (inAlignment == Center)
                        {
                            childOffset2 += (boxLength - childLength)/2;
                        }
                        else if (inAlignment == End)
                        {
                            childOffset += boxLength - childLength;
                        }
                    }
                }
            };
            if (mOrientation == HORIZONTAL)
            {
                Helper::calculateLengths(mAlign,
                                         portion,
                                         minWidth,
                                         boxWidth,
                                         childWidth,
                                         offsetChildX,
                                         offsetChildY);
            }
            else
            {
                Helper::calculateLengths(mAlign,
                                         portion,
                                         minHeight,
                                         boxHeight,
                                         childHeight,
                                         offsetChildY,
                                         offsetChildX);
            }
            
            if (mAlign == Stretch || child->impl()->expansive())
            {
                if (mOrientation == HORIZONTAL)
                {
                    childHeight = clientRect.height();
                }
                else
                {
                    childWidth = clientRect.width();
                }
            }
            child->impl()->move(clientRect.x() + offsetChildX,
                                           clientRect.y() + offsetChildY,
                                           childWidth, childHeight);

            if (mOrientation == HORIZONTAL)
            {
                offsetBoxX += boxWidth;
                offsetChildX = offsetBoxX;
                offsetChildY = 0;
            }
            if (mOrientation == VERTICAL)
            {
                offsetBoxY += boxHeight;
                offsetChildX = 0;
                offsetChildY = offsetBoxY;
            }
        }
        rebuildChildLayouts();
    }
    
    
    NativeMenuList::NativeMenuList(ElementImpl * inParent) :
        NativeControl(inParent,
                      TEXT("COMBOBOX"),
                      0, // exStyle
                      CBS_DROPDOWNLIST)
    {
    }
    
    
    int NativeMenuList::minimumWidth() const
    {
        return Defaults::dropDownListMinimumWidth() + Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx;
    }


    int NativeMenuList::minimumHeight() const
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


    NativeSeparator::NativeSeparator(ElementImpl * inParent) :
        NativeControl(inParent,
                      TEXT("STATIC"),
                      0, // exStyle
                      SS_GRAYFRAME)
    {
        mExpansive = true;
    }
        
        
    int NativeSeparator::minimumWidth() const
    {
        return 1;
    }

    
    int NativeSeparator::minimumHeight() const
    {
        return 1;
    }


    NativeSpacer::NativeSpacer(ElementImpl * inParent) :
        VirtualControl(inParent)
    {
    }

        
    int NativeSpacer::minimumWidth() const
    {
        return 0;
    }

     
    int NativeSpacer::minimumHeight() const
    {
        return 0;
    }


    NativeMenuButton::NativeMenuButton(ElementImpl * inParent) :
        NativeControl(inParent,
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
        
        
    int NativeMenuButton::minimumWidth() const
    {
        return Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx + Defaults::textPadding()*2;
    }

    
    int NativeMenuButton::minimumHeight() const
    {
        return Defaults::buttonHeight();
    }


    NativeGrid::NativeGrid(ElementImpl * inParent) :
        VirtualControl(inParent)
    {
    }
        
        
    int NativeGrid::minimumWidth() const
    {
        int result = 0;
        ElementPtr rows;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            if (child->type() == Rows::Type())
            {
                rows = child;
                break;
            }
        }

        if (!rows)
        {
            ReportError("Rows element not found in grid!");
            return 0;
        }

        for (size_t idx = 0; idx != rows->children().size(); ++idx)
        {
            ElementPtr el = rows->children()[idx];
            if (el->type() == Row::Type())
            {
                int w = 0;
                if (Row * row = el->downcast<Row>())
                {
                    const Children & children = row->children();
                    for (size_t childIdx = 0; childIdx != children.size(); ++childIdx)
                    {
                        ElementPtr child = children[childIdx];
                        w += child->impl()->minimumWidth();
                    }
                    if (w > result)
                    {
                        result = w;
                    }
                }
            }
        }
        
        ElementPtr columns;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            if (child->type() == Columns::Type())
            {
                columns = child;
                break;
            }
        }
        return result;
    }

    
    int NativeGrid::minimumHeight() const
    {
        int result = 0;
        ElementPtr rows;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child = owningElement()->children()[idx];
            if (child->type() == Rows::Type())
            {
                rows = child;
                break;
            }
        }

        if (!rows)
        {
            ReportError("Rows element not found in grid!");
            return 0;
        }

        for (size_t idx = 0; idx != rows->children().size(); ++idx)
        {
            ElementPtr el = rows->children()[idx];
            if (el->type() == Row::Type())
            {
                int maxHeight = 0;
                if (Row * row = el->downcast<Row>())
                {
                    const Children & children = row->children();
                    for (size_t childIdx = 0; childIdx != children.size(); ++childIdx)
                    {
                        ElementPtr child = children[childIdx];
                        int h = child->impl()->minimumHeight();
                        if (h > maxHeight)
                        {
                            maxHeight = h;
                        }
                    }
                    result += maxHeight;
                }
            }
        }
        return result;
    }


    void NativeGrid::rebuildLayout()
    {
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

        GenericGrid<GridProportion> proportions(numRows, numCols, GridProportion(SizeInfo(Defaults::Attributes::flex(), 1), SizeInfo(Defaults::Attributes::flex(), 1)));
        for (size_t colIdx = 0; colIdx != numCols; ++colIdx)
        {
            for (size_t rowIdx = 0; rowIdx != numRows; ++rowIdx)
            {
                if (NativeRow * row = rows->children()[rowIdx]->impl()->downcast<NativeRow>())
                {
                    assert(row->owningElement()->type() == Row::Type());

                    if (NativeColumn * column = columns->children()[colIdx]->impl()->downcast<NativeColumn>())
                    {
                        assert(column->owningElement()->type() == Column::Type());

                        ElementPtr child = rows->children()[rowIdx]->children()[colIdx];
                        if (child)
                        {
                            int hflex = 0;
                            try
                            {
                                std::string flex = column->owningElement()->getAttribute("flex");
                                if (!flex.empty())
                                {
                                    hflex = boost::lexical_cast<int>(flex);
                                }
                            }
                            catch (const boost::bad_lexical_cast & )
                            {
                                ReportError("Bad lexical cast for flex.");
                            }

                            int vflex = 0;
                            try
                            {
                                std::string flex = row->owningElement()->getAttribute("flex");
                                if (!flex.empty())
                                {
                                    vflex = boost::lexical_cast<int>(flex);
                                }
                            }
                            catch (const boost::bad_lexical_cast & )
                            {
                                ReportError("Bad lexical cast for flex.");
                            }

                            proportions.set(
                                rowIdx, colIdx,
                                GridProportion(
                                    SizeInfo(hflex, column->minimumWidth()),
                                    SizeInfo(vflex, row->minimumHeight())
                                )
                            );
                        }
                    }
                }
            }
        }
        GenericGrid<Rect> rects(numRows, numCols);
        GridLayoutManager::GetRects(clientRect(), proportions, rects);

        for (size_t colIdx = 0; colIdx != rects.numColumns(); ++colIdx)
        {
            for (size_t rowIdx = 0; rowIdx != rects.numRows(); ++rowIdx)
            {
                Rect & childRect = rects.get(rowIdx, colIdx);
                ElementPtr child = rows->children()[rowIdx]->children()[colIdx];
                child->impl()->move(
                    childRect.x(),
                    childRect.y(),
                    childRect.width(),
                    childRect.height()
                );
            }
        }
        rebuildChildLayouts();
    }


    NativeRows::NativeRows(ElementImpl * inParent) :
        VirtualControl(inParent)
    {
    }


    NativeColumns::NativeColumns(ElementImpl * inParent) :
        VirtualControl(inParent)
    {
    }


    NativeRow::NativeRow(ElementImpl * inParent) :
        VirtualControl(inParent)
    {
    }

    
    int NativeRow::minimumWidth() const
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


    int NativeRow::minimumHeight() const
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


    NativeColumn::NativeColumn(ElementImpl * inParent) :
        VirtualControl(inParent)
    {
    }

    
    int NativeColumn::minimumWidth() const
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
                        ownIndex = idx;
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


    int NativeColumn::minimumHeight() const
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

    
    NativeRadioGroup::NativeRadioGroup(ElementImpl * inParent) :
        NativeBox(inParent)
    {
    }

    
    NativeRadio::NativeRadio(ElementImpl * inParent) :
        NativeControl(inParent,
                      TEXT("BUTTON"),
                      0, // exStyle
                      BS_RADIOBUTTON)
    {
    }


    int NativeRadio::minimumWidth() const
    {
        return Defaults::radioButtonMinimumWidth() + Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx;
    }

    
    int NativeRadio::minimumHeight() const
    {
        return Defaults::controlHeight();
    }

    
    NativeProgressMeter::NativeProgressMeter(ElementImpl * inParent) :
        NativeControl(inParent,
                      PROGRESS_CLASS,
                      0, // exStyle
                      PBS_SMOOTH)
    {
        Utils::initializeProgressMeter(mHandle, 100);
    }


    int NativeProgressMeter::minimumWidth() const
    {
        return 80;
    }

    
    int NativeProgressMeter::minimumHeight() const
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

    
    NativeDeck::NativeDeck(ElementImpl * inParent) :
        VirtualControl(inParent),
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


    int NativeDeck::minimumWidth() const
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

    
    int NativeDeck::minimumHeight() const
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
