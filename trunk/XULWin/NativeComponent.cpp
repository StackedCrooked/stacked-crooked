#include "NativeComponent.h"
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
    
    NativeComponent::Components NativeComponent::sComponentsByHandle;
    
    NativeControl::ControlsById NativeControl::sControlsById;

    NativeComponent::NativeComponent(NativeComponent * inParent, CommandId inCommandId) :
        mParent(inParent),
        mHandle(0),
        mModuleHandle(::GetModuleHandle(0)), // TODO: Fix this hacky thingy!
        mCommandId(inCommandId),
        mMinimumWidth(Defaults::componentMinimumWidth()),
        mMinimumHeight(Defaults::componentMinimumHeight()),
        mExpansive(false)
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
    
    
    int NativeComponent::minimumWidth() const
    {
        return mMinimumWidth;
    }

    
    int NativeComponent::minimumHeight() const
    {
        return mMinimumHeight;
    }
    
    
    bool NativeComponent::expansive() const
    {
        return mExpansive;
    }
    
    
    void NativeComponent::move(int x, int y, int w, int h)
    {
        ::MoveWindow(handle(), x, y, w, h, FALSE);
    }
    
    
    bool NativeComponent::getAttribute(const std::string & inName, std::string & outValue)
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
    
    
    bool NativeComponent::setAttribute(const std::string & inName, const std::string & inValue)
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
    
    
    bool NativeComponent::setAttributeControllers()
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
        AttributeGetter disabledGetter = boost::bind(&Helper::Bool2String, boost::bind(&Utils::isWindowDisabled, handle()));
        AttributeSetter disabledSetter = boost::bind(&Utils::disableWindow, handle(), boost::bind(&Helper::String2Bool, _1));
        setAttributeController("disabled", AttributeController(disabledGetter, disabledSetter));
        
        AttributeGetter labelGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter labelSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("label", AttributeController(labelGetter, labelSetter));
        return true;
    }


    void NativeComponent::setAttributeController(const std::string & inAttr, const AttributeController & inController)
    {
        // Attribute controllers are set in the constructor. Order of construction is A then B if class B inherits class A.
        // So constructor of NativeComponent will always occur before the construction of any of its subclasses.
        // However, this poses a problem if the subclass wants to replace an existing AttributeController for a certain attribute value
        // with its own because the stl map::insert method doesn't overwrite previously existing values.
        // So what we need to do then is remove the existing value first, and then insert the new one.
        // This method does just that.
        AttributeControllers::iterator it = mAttributeControllers.find(inAttr);
        if (it != mAttributeControllers.end())
        {
            mAttributeControllers.erase(it);
        }
        mAttributeControllers.insert(std::make_pair(inAttr, inController));
    }

    
    void NativeComponent::setOwningElement(Element * inElement)
    {
        mElement = inElement;
    }

    
    Element * NativeComponent::owningElement() const
    {
        return mElement;
    }


    NativeComponent * NativeComponent::parent() const
    {
        return mParent;
    }


    HWND NativeComponent::handle() const
    {
        return mHandle;
    }
    

    void NativeComponent::rebuildLayout()
    {
        rebuildChildLayouts();
    }

    
    void NativeComponent::rebuildChildLayouts()
    {
        Children::const_iterator it = mElement->children().begin(), end = mElement->children().end();
        for (; it != end; ++it)
        {
            NativeComponent * nativeComp = (*it)->nativeComponent();
            if (nativeComp)
            {
                nativeComp->rebuildLayout();
            }
        }
    }


    LRESULT NativeComponent::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        assert(false); // should not come here
        return FALSE;
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


    NativeWindow::NativeWindow(NativeComponent * inParent) :
        NativeComponent(inParent, CommandId())
    {
        
        mHandle = ::CreateWindowEx
        (
            0, 
            TEXT("XULWin::Window"),
            TEXT(""),
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
            1, 1, 1, 1,
            mParent ? mParent->handle() : 0,
            (HMENU)0,
            mModuleHandle,
            0
        );

        // set default font
        ::SendMessage(mHandle, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        sComponentsByHandle.insert(std::make_pair(mHandle, this));
    }


    bool NativeWindow::setAttributeControllers()
    {        
        {
            AttributeGetter heightGetter = boost::bind(&Int2String, boost::bind(&Utils::getWindowHeight, handle()));
            AttributeSetter heightSetter = boost::bind(&Utils::setWindowHeight, handle(), boost::bind(&String2Int, _1));
            setAttributeController("height", AttributeController(heightGetter, heightSetter));
        }

        {
            AttributeGetter widthGetter = boost::bind(&Int2String, boost::bind(&Utils::getWindowWidth, handle()));
            AttributeSetter widthSetter = boost::bind(&Utils::setWindowWidth, handle(), boost::bind(&String2Int, _1));
            setAttributeController("width", AttributeController(widthGetter, widthSetter));
        }
        return NativeComponent::setAttributeControllers();
    }
    
    
    Rect NativeWindow::clientRect() const
    {
        RECT rc;
        ::GetClientRect(handle(), &rc);
        return Rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    }

    
    void NativeWindow::rebuildLayout()
    {     
        rebuildChildLayouts();
        ::InvalidateRect(handle(), 0, FALSE);
    }


    void NativeWindow::showModal()
    {      
        int w = Utils::getWindowWidth(handle());
        int h = Utils::getWindowHeight(handle());
        int x = (GetSystemMetrics(SM_CXSCREEN) - w)/2;
        int y = (GetSystemMetrics(SM_CYSCREEN) - h)/2;
        move(x, y, w, h);
        ::ShowWindow(handle(), SW_SHOW);
        rebuildLayout();

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
                    (*it)->nativeComponent()->move(clientRect.x(), clientRect.y(), clientRect.width(), clientRect.height());
                    rebuildLayout();
                }
                break;
            }
            case WM_CLOSE:
            {
                PostQuitMessage(0);
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


    VirtualControl::VirtualControl(NativeComponent * inParent) :
        NativeComponent(inParent, CommandId())
    {
        if (!mParent)
        {
            ReportError("NativeControl constructor failed because parent is NULL.");
            return;
        }
    }
        
        
    HWND VirtualControl::handle() const
    {
        if (mParent)
        {
            return mParent->handle();
        }
        else
        {
            return 0;
        }
    }
    
    
    Rect VirtualControl::clientRect() const
    {
        return mRect;
    }


    void VirtualControl::move(int x, int y, int w, int h)
    {
        mRect = Rect(x, y, w, h);
    }


    VirtualProxy::VirtualProxy(NativeComponent * inSubject) :
        VirtualControl(inSubject->parent()),
        mSubject(inSubject)
    {
    }


    VirtualProxy::~VirtualProxy()
    {
    }

    
    HWND VirtualProxy::handle() const
    {
        return mSubject->handle();
    }


    void VirtualProxy::move(int x, int y, int w, int h)
    {
        mRect = Rect(x, y, w, h);
        mSubject->move(mRect.x(), mRect.y(), mRect.width(), mRect.height());
    }


    bool VirtualProxy::setAttributeControllers()
    {
        if (mSubject)
        {
            return mSubject->setAttributeControllers();
        }
        return NativeComponent::setAttributeControllers();
    }
    
    
    bool VirtualProxy::setAttribute(const std::string & inName, const std::string & inValue)
    {
        if (mSubject)
        {
            return mSubject->setAttribute(inName, inValue);
        }
        return false;
    }


    VirtualPadding::VirtualPadding(NativeComponent * inSubject) :
        VirtualProxy(inSubject)
    {
    }


    VirtualPadding::~VirtualPadding()
    {
    }
    
    
    void VirtualPadding::move(int x, int y, int w, int h)
    {
        mRect = Rect(x + paddingLeft(), y + paddingTop(), w - paddingLeft() - paddingRight(), h - paddingTop() - paddingBottom());
        mSubject->move(mRect.x(), mRect.y(), mRect.width(), mRect.height());
    }


    int VirtualPadding::paddingTop() const
    {
        return 4;
    }

    
    int VirtualPadding::paddingLeft() const
    {
        return 4;
    }

    
    int VirtualPadding::paddingRight() const
    {
        return 4;
    }

    
    int VirtualPadding::paddingBottom() const
    {
        return 4;
    }
    
    
    int VirtualPadding::minimumWidth() const
    {
        return paddingLeft() + mSubject->minimumWidth() + paddingRight();
    }

    
    int VirtualPadding::minimumHeight() const
    {
        return paddingTop() + mSubject->minimumHeight() + paddingBottom();
    }


    NativeControl::NativeControl(NativeComponent * inParent, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle) :
        NativeComponent(inParent, CommandId())
    {
        if (!mParent)
        {
            ReportError("NativeControl constructor failed because parent is NULL.");
            return;
        }

        RECT rc;
        ::GetClientRect(inParent->handle(), &rc);
        
        mHandle = ::CreateWindowEx
        (
            inExStyle, 
            inClassName,
            TEXT(""),
			inStyle | WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
            0, 0, mMinimumWidth, mMinimumHeight,
            mParent ? mParent->handle() : 0,
            (HMENU)mCommandId.intValue(),
            mModuleHandle,
            0
        );

        if (!mHandle)
        {
            MessageBoxA(0, Utils::getLastError(::GetLastError()).c_str(), "XULWin Error", MB_OK);
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
    
    
    NativeButton::NativeButton(NativeComponent * inParent) :
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


    NativeTextBox::NativeTextBox(NativeComponent * inParent) :
        NativeControl(inParent,
                      TEXT("EDIT"),
                      WS_EX_CLIENTEDGE, // exStyle
                      ES_AUTOHSCROLL)
    {
    }

    
    bool NativeTextBox::setAttributeControllers()
    {
        AttributeGetter valueGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter valueSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("value", AttributeController(valueGetter, valueSetter));
        return NativeComponent::setAttributeControllers();
    }


    int NativeTextBox::minimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        width += Defaults::textPadding();
        return width;
    }


    void NativeTextBox::handleCommand(WPARAM wParam, LPARAM lParam)
    {
    }


    NativeLabel::NativeLabel(NativeComponent * inParent) :
        NativeControl(inParent,
                      TEXT("STATIC"),
                      0, // exStyle
                      SS_LEFT | SS_CENTERIMAGE)
    {
    }
        
    
    bool NativeLabel::setAttributeControllers()
    {
        AttributeGetter valueGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter valueSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("value", AttributeController(valueGetter, valueSetter));
        return NativeComponent::setAttributeControllers();
    }


    int NativeLabel::minimumWidth() const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        width += Defaults::textPadding();
        return width;
    }

    
    int NativeLabel::minimumHeight() const
    {
        return Defaults::controlHeight();
    }


    NativeDescription::NativeDescription(NativeComponent * inParent) :
        NativeControl(inParent,
                      TEXT("STATIC"),
                      0, // exStyle
                      SS_LEFT)
    {
    }


    bool NativeDescription::setAttributeControllers()
    {
        AttributeGetter valueGetter = boost::bind(&Utils::getWindowText, handle());
        AttributeSetter valueSetter = boost::bind(&Utils::setWindowText, handle(), _1);
        setAttributeController("value", AttributeController(valueGetter, valueSetter));
        return NativeComponent::setAttributeControllers();
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
    
    
    NativeHBox::NativeHBox(NativeComponent * inParent) :
        NativeBox(inParent, HORIZONTAL)
    {   
    }
        
        
    NativeBox::NativeBox(NativeComponent * inParent, Orientation inOrientation) :
        VirtualControl(inParent),
        mOrientation(inOrientation),
        mAlign(Start)
    {
    }


    bool NativeBox::setAttributeControllers()
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
        setAttributeController("orientation", AttributeController(orientationGetter, orientationSetter));

        AttributeSetter alignSetter = boost::bind(&NativeBox::setAlignment, this, boost::bind(&Helper::String2Align, _1));
        AttributeGetter alignGetter = boost::bind(&Helper::Align2String, boost::bind(&NativeBox::getAlignment, this));
        setAttributeController("align", AttributeController(alignGetter, alignSetter));
        return NativeComponent::setAttributeControllers();
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
                                                       mOrientation == HORIZONTAL ? child->nativeComponent()->minimumWidth() :
                                                                                    child->nativeComponent()->minimumHeight()));
            }
            else
            {
                RECT rw;
                ::GetWindowRect(child->nativeComponent()->handle(), &rw);
                if (mOrientation == HORIZONTAL)
                {
                    availableSpace -= child->nativeComponent()->minimumWidth();
                }
                else
                {
                    availableSpace -= child->nativeComponent()->minimumHeight();
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
            HWND childHandle = child->nativeComponent()->handle();
            int minWidth = child->nativeComponent()->minimumWidth();
            int childWidth = minWidth;
            int boxWidth = minWidth;
            int minHeight = child->nativeComponent()->minimumHeight();
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
            
            if (mAlign == Stretch || child->nativeComponent()->expansive())
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
            child->nativeComponent()->move(clientRect.x() + offsetChildX,
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

            
    int NativeHBox::minimumWidth() const
    {
        int result = 0;
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            result += mElement->children()[idx]->nativeComponent()->minimumWidth();
        }
        return result;
    }


    int NativeHBox::minimumHeight() const
    {
        int result = 0;
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            int height = mElement->children()[idx]->nativeComponent()->minimumHeight();
            if (height > result)
            {
                result = height;
            }
        }
        return result;
    }

            
    int NativeVBox::minimumWidth() const
    {
        int result = 0;
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            int width = mElement->children()[idx]->nativeComponent()->minimumWidth();
            if (width > result)
            {
                result = width;
            }
        }
        return result;
    }


    int NativeVBox::minimumHeight() const
    {
        int result = 0;
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            result += mElement->children()[idx]->nativeComponent()->minimumHeight();
        }
        return result;
    }
    
    
    void NativeMenuList::move(int x, int y, int w, int h)
    {
        // The height of a combobox in Win32 is the height of the dropdown menu + the height of the widget itself.
        h = h + Utils::getComboBoxItemCount(handle()) * Defaults::dropDownListItemHeight();
        NativeComponent::move(x, y, w, h);
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


    NativeSeparator::NativeSeparator(NativeComponent * inParent) :
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


    NativeSpacer::NativeSpacer(NativeComponent * inParent) :
        VirtualControl(inParent)
    {
    }


    NativeMenuButton::NativeMenuButton(NativeComponent * inParent) :
        NativeControl(inParent,
                      TOOLBARCLASSNAME,
                      0, // exStyle
                      0)
    {
        ::SendMessage(handle(), TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

        CommandId id;
        // add to toolbar
        std::wstring itemText = L"test";
        TBBUTTON theToolBarButton;
        theToolBarButton.idCommand = id.intValue();
        theToolBarButton.fsState = TBSTATE_ENABLED;
        theToolBarButton.fsStyle = BTNS_BUTTON | BTNS_DROPDOWN | BTNS_SHOWTEXT;
        theToolBarButton.dwData = 0; 
        theToolBarButton.iString = (INT_PTR)itemText.c_str();
        theToolBarButton.iBitmap = I_IMAGENONE;

        SendMessage(handle(), TB_ADDBUTTONS, 1, (LPARAM)(LPTBBUTTON)&theToolBarButton); 
        SendMessage(handle(), TB_AUTOSIZE, 0, 0);
        ShowWindow(handle(), SW_SHOW); 
    }
        
        
    int NativeMenuButton::minimumWidth() const
    {
        return 200;
    }

    
    int NativeMenuButton::minimumHeight() const
    {
        return 100;
    }


    NativeGrid::NativeGrid(NativeComponent * inParent) :
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
                Row * row = static_cast<Row*>(el.get());
                const Children & children = row->children();
                for (size_t childIdx = 0; childIdx != children.size(); ++childIdx)
                {
                    ElementPtr child = children[childIdx];
                    w += child->nativeComponent()->minimumWidth();
                }
                if (w > result)
                {
                    result = w;
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
                Row * row = static_cast<Row*>(el.get());
                const Children & children = row->children();
                for (size_t childIdx = 0; childIdx != children.size(); ++childIdx)
                {
                    ElementPtr child = children[childIdx];
                    int h = child->nativeComponent()->minimumHeight();
                    if (h > maxHeight)
                    {
                        maxHeight = h;
                    }
                }
                result += maxHeight;
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
                NativeRow * row = static_cast<NativeRow*>(rows->children()[rowIdx]->nativeComponent());
                assert(row->owningElement()->type() == Row::Type());

                NativeColumn * column = static_cast<NativeColumn*>(columns->children()[colIdx]->nativeComponent());
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
        GenericGrid<Rect> rects(numRows, numCols);
        GridLayoutManager::GetRects(clientRect(), proportions, rects);

        for (size_t colIdx = 0; colIdx != rects.numColumns(); ++colIdx)
        {
            for (size_t rowIdx = 0; rowIdx != rects.numRows(); ++rowIdx)
            {
                Rect & childRect = rects.get(rowIdx, colIdx);
                ElementPtr child = rows->children()[rowIdx]->children()[colIdx];
                child->nativeComponent()->move(
                    childRect.x(),
                    childRect.y(),
                    childRect.width(),
                    childRect.height()
                );
            }
        }
        rebuildChildLayouts();
    }


    NativeRows::NativeRows(NativeComponent * inParent) :
        VirtualControl(inParent)
    {
    }


    NativeColumns::NativeColumns(NativeComponent * inParent) :
        VirtualControl(inParent)
    {
    }


    NativeRow::NativeRow(NativeComponent * inParent) :
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
            res += child->nativeComponent()->minimumWidth();
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
            int h = child->nativeComponent()->minimumHeight();
            if (h > res)
            {
                res = h;
            }
        }
        return res;
    }


    NativeColumn::NativeColumn(NativeComponent * inParent) :
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
                    if (child->children()[ownI]->nativeComponent()->commandId() == commandId())
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
            int w = row->children()[ownIndex]->nativeComponent()->minimumWidth();
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
            res += child->nativeComponent()->minimumHeight();
        }
        return res;
    }


} // namespace XULWin
