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


    int CommandId::sId = 101; // start handleCommand Ids at 101 to avoid conflicts with Windows predefined values
    
    ConcreteElement::Components ConcreteElement::sComponentsByHandle;
    
    NativeControl::ComponentsById NativeControl::sComponentsById;

    ConcreteElement::ConcreteElement(ElementImpl * inParent) :
        mParent(inParent),
        mCommandId(),
        mExpansive(false),
        mElement(0),
        mFlex(0),
        mHidden(false),
        mWidth(0),
        mHeight(0),
        mCSSWidth(0),
        mCSSHeight(0)
    {
        mWidth.setInvalid();
        mHeight.setInvalid();
        mCSSWidth.setInvalid();
        mCSSHeight.setInvalid();
    }


    ConcreteElement::~ConcreteElement()
    {
    }


    int ConcreteElement::getWidth() const
    {
        if (mWidth.isValid())
        {
            return mWidth;
        }

        if (mCSSWidth.isValid())
        {
            return std::max<int>(mCSSWidth.getValue(), getWidth(Minimum));
        }

        return getWidth(Optimal);
    }
    
    
    void ConcreteElement::setWidth(int inWidth)
    {
        mWidth = inWidth;
    }


    int ConcreteElement::getHeight() const
    {
        if (mHeight.isValid())
        {
            return mHeight;
        }

        if (mCSSHeight.isValid())
        {
            return std::max<int>(mCSSHeight.getValue(), getHeight(Minimum));
        }

        return getHeight(Optimal);
    }
    
    
    void ConcreteElement::setHeight(int inHeight)
    {
        mHeight = inHeight;
    }


    int ConcreteElement::getFlex() const
    {
        return mFlex;
    }
    
    
    void ConcreteElement::setFlex(int inFlex)
    {
        mFlex = inFlex;
    }

    
    bool ConcreteElement::isHidden() const
    {
        return mHidden;
    }


    void ConcreteElement::setHidden(bool inHidden)
    {
        mHidden = inHidden;
    }


    int ConcreteElement::getCSSWidth() const
    {
        if (mWidth.isValid())
        {
            return mWidth.getValue();
        }
        return 0;
    }


    void ConcreteElement::setCSSWidth(int inWidth)
    {
        mCSSWidth = inWidth;
    }

    
    int ConcreteElement::getCSSHeight() const
    {
        if (mHeight.isValid())
        {
            return mHeight.getValue();
        }
        return 0;
    }


    void ConcreteElement::setCSSHeight(int inHeight)
    {
        mCSSHeight = inHeight;
    }

    
    void ConcreteElement::getCSSMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const
    {
        int margin = 0;
        if (const MarginDecorator * marginDecorator = constDowncast<MarginDecorator>())
        {
            marginDecorator->getMargin(outTop, outLeft, outRight, outBottom);
        }
    }


    void ConcreteElement::setCSSMargin(int inTop, int inLeft, int inRight, int inBottom)
    {
        if (!owningElement())
        {
            ReportError("ConcreteElement::setCSSMargin failed because no owning element was present.");
            return;
        }

        // Find a margin decorator, and set the margin value.
        if (MarginDecorator * obj = owningElement()->impl()->downcast<MarginDecorator>())
        {
            obj->setMargin(inTop, inLeft, inRight, inBottom);
        }
        // If no margin decorator found, insert one, and set the value.
        else if (Decorator * dec = owningElement()->impl()->downcast<Decorator>())
        {
            ElementImplPtr newDec(new MarginDecorator(dec->decoratedElement()));
            dec->setDecoratedElement(newDec);
            if (MarginDecorator * p = newDec->downcast<MarginDecorator>())
            {
                p->setMargin(inTop, inLeft, inRight, inBottom);
            }
        }
        else
        {
            ReportError("No decorator found!");
        }
    }
    
    
    int ConcreteElement::getWidth(SizeConstraint inSizeConstraint) const
    {
        if (isHidden())
        {
            return 0;
        }

        if (mWidth.isValid())
        {
            return mWidth.getValue();
        }

        return calculateWidth(inSizeConstraint);
    }

    
    int ConcreteElement::getHeight(SizeConstraint inSizeConstraint) const
    {
        if (isHidden())
        {
            return 0;
        }

        if (mHeight.isValid())
        {
            return mHeight.getValue();
        }

        return calculateHeight(inSizeConstraint);
    }
    
    
    bool ConcreteElement::expansive() const
    {
        return mExpansive;
    }
    
    
    bool ConcreteElement::getStyle(const std::string & inName, std::string & outValue)
    {
        StyleControllers::iterator it = mStyleControllers.find(inName);
        if (it != mStyleControllers.end())
        {
            StyleController * controller = it->second;
            controller->get(outValue);
        }
        return false;
    }
    
    
    bool ConcreteElement::getAttribute(const std::string & inName, std::string & outValue)
    {
        AttributeControllers::iterator it = mAttributeControllers.find(inName);
        if (it != mAttributeControllers.end())
        {
            it->second->get(outValue);
            return true;
        }
        return false;
    }
    
    
    bool ConcreteElement::setStyle(const std::string & inName, const std::string & inValue)
    {
        StyleControllers::iterator it = mStyleControllers.find(inName);
        if (it != mStyleControllers.end())
        {
            StyleController * controller = it->second;
            controller->set(inValue);
        }
        return false;
    }
    
    
    bool ConcreteElement::setAttribute(const std::string & inName, const std::string & inValue)
    {
        AttributeControllers::iterator it = mAttributeControllers.find(inName);
        if (it != mAttributeControllers.end())
        {
            it->second->set(inValue);
            return true;
        }
        return false;
    }


    bool ConcreteElement::initAttributeControllers()
    {        
        // STATIC CAST NEEDED HERE OTHERWISE WE GET COMPILER ERROR:
        // error C2594: '=' : ambiguous conversions from 'Element *const ' to 'AttributeController *'
        setAttributeController("width", static_cast<WidthController*>(this));
        setAttributeController("height", static_cast<HeightController*>(this));
        setAttributeController("flex", static_cast<FlexController*>(this));
        setAttributeController("hidden", static_cast<HiddenController*>(this));
        return true;
    }


    bool ConcreteElement::initStyleControllers()
    {
        setStyleController(CSSMarginController::PropertyName(), static_cast<CSSMarginController*>(this));
        setStyleController(CSSWidthController::PropertyName(), static_cast<CSSWidthController*>(this));
        setStyleController(CSSHeightController::PropertyName(), static_cast<CSSHeightController*>(this));
        return true;
    }


    void ConcreteElement::setAttributeController(const std::string & inAttr, AttributeController * inController)
    {
        AttributeControllers::iterator it = mAttributeControllers.find(inAttr);
        if (it == mAttributeControllers.end())
        {
            mAttributeControllers.insert(std::make_pair(inAttr, inController));
        }
    }
    
    
    void ConcreteElement::setStyleController(const std::string & inAttr, StyleController * inController)
    {
        StyleControllers::iterator it = mStyleControllers.find(inAttr);
        if (it == mStyleControllers.end())
        {
            mStyleControllers.insert(std::make_pair(inAttr, inController));
        }
    }

    
    void ConcreteElement::setOwningElement(Element * inElement)
    {
        mElement = inElement;
    }

    
    Element * ConcreteElement::owningElement() const
    {
        return mElement;
    }


    ElementImpl * ConcreteElement::parent() const
    {
        return mParent;
    }

    
    void ConcreteElement::rebuildChildLayouts()
    {
        if (!mElement)
        {
            return;
        }

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
        ConcreteElement(inParent),
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
    
    
    bool NativeComponent::isDisabled() const
    {
        return Utils::isWindowDisabled(handle());
    }


    void NativeComponent::setDisabled(bool inDisabled)
    {
        Utils::disableWindow(handle(), inDisabled);
    }


    std::string NativeComponent::getLabel() const
    {
        return Utils::getWindowText(handle());
    }


    void NativeComponent::setLabel(const std::string & inLabel)
    {
        Utils::setWindowText(handle(), inLabel);
    }
    
    
    void NativeComponent::setHidden(bool inHidden)
    {
        Super::setHidden(inHidden);
        Utils::setWindowVisible(handle(), !inHidden);
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
        setAttributeController("disabled", static_cast<DisabledController*>(this));
        setAttributeController("label", static_cast<LabelController*>(this));
        return Super::initAttributeControllers();
    }


    bool NativeComponent::addEventListener(EventListener * inEventListener)
    {       
        EventListeners::iterator it = mEventListeners.find(inEventListener);
        if (it == mEventListeners.end())
        {
            mEventListeners.insert(inEventListener);
            return true;
        }
        return false;
    }


    bool NativeComponent::removeEventListener(EventListener * inEventListener)
    {       
        EventListeners::iterator it = mEventListeners.find(inEventListener);
        if (it != mEventListeners.end())
        {
            mEventListeners.erase(it);
            return true;
        }
        return false;
    }
    
    
    void NativeComponent::handleCommand(WPARAM wParam, LPARAM lParam)
    {
        unsigned short notificationCode = HIWORD(wParam);
        EventListeners::iterator it = mEventListeners.begin(), end = mEventListeners.end();
        for (; it != end; ++it)
        {
            (*it)->handleCommand(owningElement(), notificationCode);
        }
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

        // Forward to event handlers
        EventListeners::iterator it = mEventListeners.begin(), end = mEventListeners.end();
        for (; it != end; ++it)
        {
            (*it)->handleMessage(owningElement(), inMessage, wParam, lParam);
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
        NativeComponent(0, inAttributesMapping),
        BoxLayouter(VERTICAL, Stretch)
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
        Super::setAttributeController("title", static_cast<TitleController*>(this));
        BoxLayouter::initAttributeControllers();
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
    
    
    int NativeWindow::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        int result = 0;
        
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child(owningElement()->children()[idx]);
            int width = child->impl()->getWidth(inSizeConstraint);
            if (getOrient() == HORIZONTAL)
            {
                result += width;
            }
            else if (width > result)
            {
                result = width;
            }
        }
        return result;
    }
    
    
    int NativeWindow::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        int result = 0;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            ElementPtr child(owningElement()->children()[idx]);
            int height = child->impl()->getHeight(inSizeConstraint);
            if (getOrient() == VERTICAL)
            {
                result += height;
            }
            else if (height > result)
            {
                result = height;
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
        BoxLayouter::rebuildLayout();
        ::InvalidateRect(handle(), 0, FALSE);
    }        
    
    
    std::string NativeWindow::getTitle() const
    {
        return Utils::getWindowText(handle());
    }


    void NativeWindow::setTitle(const std::string & inTitle)
    {
        Utils::setWindowText(handle(), inTitle);
    }

    
    size_t NativeWindow::numChildren() const
    {
        if (owningElement())
        {
            return owningElement()->children().size();
        }
        return 0;
    }


    const ElementImpl * NativeWindow::getChild(size_t idx) const
    {
        if (owningElement())
        {
            return owningElement()->children()[idx]->impl();
        }
        return 0;
    }


    ElementImpl * NativeWindow::getChild(size_t idx)
    {
        if (owningElement())
        {
            return owningElement()->children()[idx]->impl();
        }
        return 0;
    }


    void NativeWindow::showModal()
    {
        rebuildLayout();
        SIZE sz = Utils::GetSizeDifference_WindowRect_ClientRect(handle());
        int w = getWidth() + sz.cx;
        int h = getHeight() + sz.cy;
        int x = (GetSystemMetrics(SM_CXSCREEN) - w)/2;
        int y = (GetSystemMetrics(SM_CYSCREEN) - h)/2;
        move(x, y, w, h);
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
                rebuildLayout();
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
                minMaxInfo->ptMinTrackSize.x = getWidth(Minimum) + sizeDiff.cx;
                minMaxInfo->ptMinTrackSize.y = getHeight(Minimum) + sizeDiff.cy;
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

        // Forward to event handlers
        EventListeners::iterator it = mEventListeners.begin(), end = mEventListeners.end();
        for (; it != end; ++it)
        {
            (*it)->handleMessage(owningElement(), inMessage, wParam, lParam);
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


    VirtualComponent::VirtualComponent(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        ConcreteElement(inParent)
    {
        if (!mParent)
        {
            ReportError("NativeControl constructor failed because parent is NULL.");
            return;
        }
    }
        
        
    VirtualComponent::~VirtualComponent()
    {

    }

     
    bool VirtualComponent::initAttributeControllers()
    {
        return Super::initAttributeControllers();
    }


    bool VirtualComponent::initStyleControllers()
    {
        return Super::initStyleControllers();
    }


    void VirtualComponent::move(int x, int y, int w, int h)
    {
        mRect = Rect(x, y, w, h);
    }


    void VirtualComponent::rebuildLayout()
    {
        rebuildChildLayouts();
    }
    
    
    Rect VirtualComponent::clientRect() const
    {
        return mRect;
    }

    
    LRESULT VirtualComponent::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
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
        else if (VirtualComponent * obj = dynamic_cast<VirtualComponent*>(inElementImpl))
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
    
    
    int NativeButton::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return Defaults::buttonHeight();
    }
    
    
    int NativeButton::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        std::string text = Utils::getWindowText(handle());
        int minWidth = Utils::getTextSize(handle(), text).cx;
        minWidth += Defaults::textPadding();
        return std::max<int>(minWidth, Defaults::buttonWidth());
    }
    
    
    NativeCheckBox::NativeCheckBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping, TEXT("BUTTON"), 0, BS_AUTOCHECKBOX)
    {
    }
    
    
    int NativeCheckBox::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return Defaults::controlHeight();
    }
    
    
    int NativeCheckBox::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return Defaults::checkBoxMinimumWidth() + Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx;
    }

    
    bool NativeCheckBox::isChecked() const
    {
        return Utils::isCheckBoxChecked(handle());
    }


    void NativeCheckBox::setChecked(bool inChecked)
    {
        Utils::setCheckBoxChecked(handle(), inChecked);
    }

    
    bool NativeCheckBox::initAttributeControllers()
    {
        setAttributeController("checked", static_cast<CheckedController *>(this));
        return Super::initAttributeControllers();
    }


    NativeTextBox::NativeTextBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent,
                      inAttributesMapping,
                      TEXT("EDIT"),
                      WS_EX_CLIENTEDGE, // exStyle
                      GetFlags(inAttributesMapping)),
        mReadonly(IsReadOnly(inAttributesMapping)),
        mRows(1)
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


    std::string NativeTextBox::getValue() const
    {
        return Utils::getWindowText(handle());
    }


    void NativeTextBox::setValue(const std::string & inStringValue)
    {
        Utils::setWindowText(handle(), inStringValue);
    }
    
    
    bool NativeTextBox::isReadOnly() const
    {
        return Utils::isTextBoxReadOnly(handle());
    }

    
    void NativeTextBox::setReadOnly(bool inReadOnly)
    {
        Utils::setTextBoxReadOnly(handle(), inReadOnly);
    }


    int NativeTextBox::getRows() const
    {
        return mRows;
    }


    void NativeTextBox::setRows(int inRows)
    {
        mRows = inRows;
    }

    
    bool NativeTextBox::initAttributeControllers()
    {
        setAttributeController("value", static_cast<StringValueController*>(this));
        setAttributeController("readonly", static_cast<ReadOnlyController*>(this));
        setAttributeController("rows", static_cast<RowsController*>(this));
        return Super::initAttributeControllers();
    }


    int NativeTextBox::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        width += Defaults::textPadding();
        return width;
    }


    int NativeTextBox::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return Defaults::controlHeight() * getRows();
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
    
        
    std::string NativeLabel::getValue() const
    {
        return Utils::getWindowText(handle());
    }


    void NativeLabel::setValue(const std::string & inStringValue)
    {
        Utils::setWindowText(handle(), inStringValue);
    }
    
    
    CSSTextAlign NativeLabel::getCSSTextAlign() const
    {
        LONG styles = Utils::getWindowStyles(handle());
        if (styles & SS_LEFT)
        {
            return CSSTextAlign_Left;
        }
        else if (styles & SS_CENTER)
        {
            return CSSTextAlign_Center;
        }
        else if (styles & SS_RIGHT)
        {
            return CSSTextAlign_Right;
        }
        else
        {
            return CSSTextAlign_Left;
        }
    }


    void NativeLabel::setCSSTextAlign(CSSTextAlign inValue)
    {
        LONG styles = Utils::getWindowStyles(handle());
        styles &= ~SS_LEFT;
        styles &= ~SS_CENTER;
        styles &= ~SS_RIGHT;

        switch (inValue)
        {
            case CSSTextAlign_Left:
            {
                styles |= SS_LEFT;
                break;
            }
            case CSSTextAlign_Center:
            {
                styles |= SS_CENTER;
                break;
            }
            case CSSTextAlign_Right:
            {
                styles |= SS_RIGHT;
                break;
            }
            //case CSSTextAlign_Justify:
            //{
            //    styles |= 0;
            //    break;
            //}
        }
        Utils::setWindowStyle(handle(), styles);
    }

    
    bool NativeLabel::initAttributeControllers()
    {
        setAttributeController("value", static_cast<StringValueController*>(this));
        return Super::initAttributeControllers();
    }
    
    
    bool NativeLabel::initStyleControllers()
    {
        setStyleController(CSSTextAlignController::PropertyName(), static_cast<CSSTextAlignController*>(this));
        return Super::initStyleControllers();
    }


    int NativeLabel::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        std::string text = Utils::getWindowText(handle());
        int width = Utils::getTextSize(handle(), text).cx;
        return width;
    }

    
    int NativeLabel::calculateHeight(SizeConstraint inSizeConstraint) const
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
    
        
    std::string NativeDescription::getValue() const
    {
        return Utils::getWindowText(handle());
    }


    void NativeDescription::setValue(const std::string & inStringValue)
    {
        Utils::setWindowText(handle(), inStringValue);
    }


    bool NativeDescription::initAttributeControllers()
    {
        setAttributeController("value", static_cast<StringValueController*>(this));
        return Super::initAttributeControllers();
    }


    int NativeDescription::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return Defaults::textPadding();
    }

    
    int NativeDescription::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return Utils::getMultilineTextHeight(handle());
    }

    
    VirtualBox::VirtualBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orient inOrient, Align inAlign) :
        VirtualComponent(inParent, inAttributesMapping),
        BoxLayouter(inOrient, inAlign)
    {
    }
    
    
    void VirtualBox::setAttributeController(const std::string & inAttr, AttributeController * inController)
    {
        Super::setAttributeController(inAttr, inController);
    }


    bool VirtualBox::initAttributeControllers()
    {
        return Super::initAttributeControllers();
    }


    int BoxLayouter::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        if (getOrient() == HORIZONTAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != numChildren(); ++idx)
            {
                result += getChild(idx)->getWidth(inSizeConstraint);
            }
            return result;
        }
        else if (getOrient() == VERTICAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != numChildren(); ++idx)
            {
                int width = getChild(idx)->getWidth(inSizeConstraint);
                if (width > result)
                {
                    result = width;
                }
            }
            return result;
        }
        else
        {
            ReportError("Invalid getOrient in VirtualBox"); 
            return 0;
        }
    }


    int BoxLayouter::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        if (getOrient() == HORIZONTAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != numChildren(); ++idx)
            {
                int height = getChild(idx)->getHeight(inSizeConstraint);
                if (height > result)
                {
                    result = height;
                }
            }
            return result;
        }
        else if (getOrient() == VERTICAL)
        {
            int result = 0;
            for (size_t idx = 0; idx != numChildren(); ++idx)
            {
                result += getChild(idx)->getHeight(inSizeConstraint);
            }
            return result;
        }
        else
        {
            ReportError("Invalid getOrient in VirtualBox");
            return 0;
        }
    }


    BoxLayouter::BoxLayouter(Orient inOrient, Align inAlign) :
        mOrient(inOrient),
        mAlign(inAlign)
    {
    }

    
    Orient BoxLayouter::getOrient() const
    {
        return mOrient;
    }


    void BoxLayouter::setOrient(Orient inOrient)
    {
        mOrient = inOrient;
    }


    Align BoxLayouter::getAlign() const
    {
        return mAlign;
    }


    void BoxLayouter::setAlign(Align inAlign)
    {
        mAlign = inAlign;
    }


    bool BoxLayouter::initAttributeControllers()
    {
        setAttributeController("orient", static_cast<OrientController*>(this));
        setAttributeController("align", static_cast<AlignController*>(this));
        return true;
    }

    
    void BoxLayouter::rebuildLayout()
    {     
        Rect clientR(clientRect());   
        LinearLayoutManager layout(getOrient());
        bool horizontal = getOrient() == HORIZONTAL;
        std::vector<ExtendedSizeInfo> sizeInfos;

        for (size_t idx = 0; idx != numChildren(); ++idx)
        {
            ElementImpl * child = getChild(idx);
            std::string flex;
            if (child->owningElement())
            {
                flex = child->owningElement()->getAttribute("flex");
            }
            int flexValue = String2Int(flex, 0);
            int optSize = horizontal ? child->getWidth(Optimal) : child->getHeight(Optimal);
            int minSize = horizontal ? child->getWidth(Minimum) : child->getHeight(Minimum);
            int minSizeOpposite = horizontal ? child->getHeight(Minimum) : child->getWidth(Minimum);
            sizeInfos.push_back(
                ExtendedSizeInfo(FlexWrap(flexValue),
                                 MinSizeWrap(minSize),
                                 OptSizeWrap(optSize),
                                 MinSizeOppositeWrap(minSizeOpposite),
                                 child->expansive()));
        }

        std::vector<Rect> childRects;
        layout.getRects(clientR, getAlign(), sizeInfos, childRects);

        for (size_t idx = 0; idx != numChildren(); ++idx)
        {
            ElementImpl * child = getChild(idx);
            const Rect & rect = childRects[idx];
            child->move(rect.x(), rect.y(), rect.width(), rect.height());
        }

        rebuildChildLayouts();
    }
        

    NativeBox::NativeBox(ElementImpl * inParent, const AttributesMapping & inAttributesMapping, Orient inOrient, Align inAlign) :
        NativeControl(inParent, inAttributesMapping, TEXT("STATIC"), WS_EX_CONTROLPARENT, WS_TABSTOP),
        BoxLayouter(inOrient, inAlign)
    {
    }


    void NativeBox::setAttributeController(const std::string & inAttr, AttributeController * inController)
    {
        Super::setAttributeController(inAttr, inController);
    }
    
        
    bool NativeBox::initAttributeControllers()
    {
        BoxLayouter::initAttributeControllers();
        return Super::initAttributeControllers();
    }
    
    
    void NativeBox::rebuildLayout()
    {
        BoxLayouter::rebuildLayout();
    }

    
    int NativeBox::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return BoxLayouter::calculateWidth(inSizeConstraint);
    }

    
    int NativeBox::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return BoxLayouter::calculateHeight(inSizeConstraint);
    }
    
    
    Rect NativeBox::clientRect() const
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
    
    
    int NativeMenuList::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return Defaults::dropDownListMinimumWidth() + Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx;
    }


    int NativeMenuList::calculateHeight(SizeConstraint inSizeConstraint) const
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
        
        
    int NativeSeparator::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return 1;
    }

    
    int NativeSeparator::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return 1;
    }


    NativeSpacer::NativeSpacer(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualComponent(inParent, inAttributesMapping)
    {
    }

        
    int NativeSpacer::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return 0;
    }

     
    int NativeSpacer::calculateHeight(SizeConstraint inSizeConstraint) const
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
        
        
    int NativeMenuButton::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx + Defaults::textPadding()*2;
    }

    
    int NativeMenuButton::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return Defaults::buttonHeight();
    }


    VirtualGrid::VirtualGrid(ElementImpl * inParent,
                           const AttributesMapping & inAttributesMapping) :
        VirtualComponent(inParent, inAttributesMapping)
    {
    }
        
        
    int VirtualGrid::calculateWidth(SizeConstraint inSizeConstraint) const
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
                        result += col->getWidth(inSizeConstraint);
                    }
                }
            }
        }
        return result;
    }

    
    int VirtualGrid::calculateHeight(SizeConstraint inSizeConstraint) const
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
                        result += row->getHeight(inSizeConstraint);
                    }
                }
            }
        }
        return result;
    }


    void VirtualGrid::rebuildLayout()
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

        if (rows->children().empty())
        {
            ReportError("Grid has no rows!");
            return;
        }

        if (columns->children().empty())
        {
            ReportError("Grid has no columns!");
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
                colWidths.push_back(
                    SizeInfo(FlexWrap(String2Int(col->owningElement()->getAttribute("flex"), 0)),
                             MinSizeWrap(col->getWidth(Minimum)),
                             OptSizeWrap(col->getWidth(Optimal))));
            }
        }

        if (colWidths.empty())
        {
            ReportError("Grid has no columns!");
            return;
        }


        //
        // Get row size infos (min height and flex)
        //
        std::vector<SizeInfo> rowHeights;
        for (size_t rowIdx = 0; rowIdx != rows->children().size(); ++rowIdx)
        {
            if (NativeRow * row = rows->children()[rowIdx]->impl()->downcast<NativeRow>())
            {
                rowHeights.push_back(
                    SizeInfo(FlexWrap(String2Int(row->owningElement()->getAttribute("flex"), 0)),
                             MinSizeWrap(row->getHeight(Minimum)),
                             OptSizeWrap(row->getHeight(Optimal))));
            }
        }

        if (rowHeights.empty())
        {
            ReportError("Grid has no rows!");
            return;
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
                int rowHeight = row->getHeight();
                for (size_t colIdx = 0; colIdx != numCols; ++colIdx)
                {
                    if (NativeColumn * column = columns->children()[colIdx]->impl()->downcast<NativeColumn>())
                    {
                        if (colIdx < row->owningElement()->children().size())
                        {                            
                            ElementImpl * child = row->owningElement()->children()[colIdx]->impl();
                            widgetInfos.set(rowIdx, colIdx,
                                                     CellInfo(child->getWidth(),
                                                     child->getHeight(),
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


    NativeGrid::NativeGrid(ElementImpl * inParent,
                           const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping, TEXT("STATIC"), WS_EX_CONTROLPARENT, WS_TABSTOP)
    {
    }
        
        
    int NativeGrid::calculateWidth(SizeConstraint inSizeConstraint) const
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
                        result += col->getWidth(inSizeConstraint);
                    }
                }
            }
        }
        return result;
    }

    
    int NativeGrid::calculateHeight(SizeConstraint inSizeConstraint) const
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
                        result += row->getHeight(inSizeConstraint);
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

        if (rows->children().empty())
        {
            ReportError("Grid has no rows!");
            return;
        }

        if (columns->children().empty())
        {
            ReportError("Grid has no columns!");
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
                colWidths.push_back(
                    SizeInfo(FlexWrap(String2Int(col->owningElement()->getAttribute("flex"), 0)),
                             MinSizeWrap(col->getWidth(Minimum)),
                             OptSizeWrap(col->getWidth(Optimal))));
            }
        }

        if (colWidths.empty())
        {
            ReportError("Grid has no columns!");
            return;
        }


        //
        // Get row size infos (min height and flex)
        //
        std::vector<SizeInfo> rowHeights;
        for (size_t rowIdx = 0; rowIdx != rows->children().size(); ++rowIdx)
        {
            if (NativeRow * row = rows->children()[rowIdx]->impl()->downcast<NativeRow>())
            {
                rowHeights.push_back(
                    SizeInfo(FlexWrap(String2Int(row->owningElement()->getAttribute("flex"), 0)),
                             MinSizeWrap(row->getHeight(Minimum)),
                             OptSizeWrap(row->getHeight(Optimal))));
            }
        }

        if (rowHeights.empty())
        {
            ReportError("Grid has no rows!");
            return;
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
                int rowHeight = row->getHeight();
                for (size_t colIdx = 0; colIdx != numCols; ++colIdx)
                {
                    if (NativeColumn * column = columns->children()[colIdx]->impl()->downcast<NativeColumn>())
                    {
                        if (colIdx < row->owningElement()->children().size())
                        {                            
                            ElementImpl * child = row->owningElement()->children()[colIdx]->impl();
                            widgetInfos.set(rowIdx, colIdx,
                                                     CellInfo(child->getWidth(),
                                                     child->getHeight(),
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
        VirtualComponent(inParent, inAttributesMapping)
    {
    }


    NativeColumns::NativeColumns(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualComponent(inParent, inAttributesMapping)
    {
    }


    NativeRow::NativeRow(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualComponent(inParent, inAttributesMapping)
    {
    }

    
    int NativeRow::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        int res = 0;
        const Children & children = owningElement()->children();
        for (size_t idx = 0; idx != children.size(); ++idx)
        {
            ElementPtr child = children[idx];
            res += child->impl()->getWidth(inSizeConstraint);
        }
        return res;
    }


    int NativeRow::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        int res = 0;
        const Children & children = owningElement()->children();
        for (size_t idx = 0; idx != children.size(); ++idx)
        {
            ElementPtr child = children[idx];
            int h = child->impl()->getHeight(inSizeConstraint);
            if (h > res)
            {
                res = h;
            }
        }
        return res;
    }


    NativeColumn::NativeColumn(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualComponent(inParent, inAttributesMapping)
    {
    }

    
    int NativeColumn::calculateWidth(SizeConstraint inSizeConstraint) const
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
            int w = row->children()[ownIndex]->impl()->getWidth(inSizeConstraint);
            if (w > res)
            {
                res = w;
            }
        }
        return res;
    }


    int NativeColumn::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        int res = 0;
        const Children & children = owningElement()->children();
        for (size_t idx = 0; idx != children.size(); ++idx)
        {
            ElementPtr child = children[idx];
            res += child->impl()->getHeight(inSizeConstraint);
        }
        return res;
    }

    
    NativeRadioGroup::NativeRadioGroup(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualBox(inParent, inAttributesMapping, Defaults::Attributes::orient(), Defaults::Attributes::align())
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


    int NativeRadio::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return Defaults::radioButtonMinimumWidth() + Utils::getTextSize(handle(), Utils::getWindowText(handle())).cx;
    }

    
    int NativeRadio::calculateHeight(SizeConstraint inSizeConstraint) const
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


    int NativeProgressMeter::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return 1;
    }

    
    int NativeProgressMeter::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return Defaults::progressMeterHeight();
    }
    

    int NativeProgressMeter::getValue() const
    {
        return ::Utils::getProgressMeterProgress(handle());
    }


    void NativeProgressMeter::setValue(int inValue)
    {
        Utils::setProgressMeterProgress(handle(), inValue);
    }


    bool NativeProgressMeter::initAttributeControllers()
    {
        Super::setAttributeController("value", static_cast<IntValueController*>(this));
        return Super::initAttributeControllers();
    }

    
    NativeDeck::NativeDeck(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualComponent(inParent, inAttributesMapping),
        mSelectedIndex(0)
    {
    }
        
    
    int NativeDeck::getSelectedIndex() const
    {
        return mSelectedIndex;
    }


    void NativeDeck::setSelectedIndex(int inSelectedIndex)
    {
        mSelectedIndex = inSelectedIndex;
        rebuildLayout();
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


    int NativeDeck::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        int res = 0;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            int w = owningElement()->children()[idx]->impl()->getWidth(inSizeConstraint);
            if (w > res)
            {
                res = w;
            }
        }
        return res;
    }

    
    int NativeDeck::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        int res = 0;
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            int h = owningElement()->children()[idx]->impl()->getHeight(inSizeConstraint);
            if (h > res)
            {
                res = h;
            }
        }
        return res;
    }


    bool NativeDeck::initAttributeControllers()
    {
        //AttributeSetter selIndexSetter = boost::bind(&NativeDeck::setSelectedIndex, this, boost::bind(&String2Int, _1));
        //AttributeGetter selIndexGetter = boost::bind(&Int2String, boost::bind(&NativeDeck::selectedIndex, this));
        //setOldAttributeController("selectedIndex", OldAttributeController(selIndexGetter, selIndexSetter));
        Super::setAttributeController("selectedIndex", static_cast<SelectedIndexController*>(this));
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
        mEventListener(0),
        mIncrement(0)
    {
        mExpansive = true;
        Utils::setScrollInfo(handle(), 100, 10, 0);
    }


    int NativeScrollbar::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return Defaults::scrollbarWidth();
    }

    
    int NativeScrollbar::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return Defaults::scrollbarWidth();
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


    int NativeScrollbar::getCurrentPosition() const
    {
        return Utils::getScrollPos(handle());
    }


    void NativeScrollbar::setCurrentPosition(int inCurrentPosition)
    {
        int totalHeight = 0;
        int pageHeight = 0;
        int oldCurPos = 0;
        Utils::getScrollInfo(handle(), totalHeight, pageHeight, oldCurPos);

        // The order in which curpos, maxpos and pageincrement
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
        if (totalHeight < inCurrentPosition)
        {
            totalHeight = inCurrentPosition + 1;
        }
        Utils::setScrollInfo(handle(), totalHeight, pageHeight, inCurrentPosition);
        if ((oldCurPos != inCurrentPosition) && eventHandler())
        {
            eventHandler()->curposChanged(this, oldCurPos, inCurrentPosition);
        }
    }


    int NativeScrollbar::getMaxPosition() const
    {
        int totalHeight = 0;
        int pageHeight = 0;
        int curPos = 0;
        Utils::getScrollInfo(handle(), totalHeight, pageHeight, curPos);
        return totalHeight;
    }


    void NativeScrollbar::setMaxPosition(int inMaxPosition)
    {
        int dummy = 0;
        int pageHeight = 0;
        int curPos = 0;
        Utils::getScrollInfo(handle(), dummy, pageHeight, curPos);

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
        if (inMaxPosition <= pageHeight)
        {
            pageHeight = inMaxPosition - 1;
        }
        Utils::setScrollInfo(handle(), inMaxPosition, pageHeight, curPos);
    }


    void NativeScrollbar::setIncrement(int inIncrement)
    {
        mIncrement = inIncrement;
    }


    int NativeScrollbar::getIncrement() const
    {
        return mIncrement;
    }


    void NativeScrollbar::setPageIncrement(int inPageIncrement)
    {
        int totalHeight = 0;
        int dummy = 0;
        int curPos = 0;
        Utils::getScrollInfo(handle(), totalHeight, dummy, curPos);

        // The order in which setCurPos, setMaxPos and setPageIncrement
        // will be set (alphabetically by attribute name) can cause
        // impossible scrollbar states (i.e. currentpos or pageincrement
        // greater than maxpos). And we want to avoid that.
        // Our workaround is to detect such states here, and change invalid
        // values to valid ones.
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
        Utils::setScrollInfo(handle(), totalHeight, inPageIncrement, curPos);
    }


    int NativeScrollbar::getPageIncrement() const
    {
        int totalHeight = 0;
        int pageHeight = 0;
        int curPos = 0;
        Utils::getScrollInfo(handle(), totalHeight, pageHeight, curPos);
        return pageHeight;
    }


    bool NativeScrollbar::initAttributeControllers()
    {
        Super::setAttributeController("curpos", static_cast<ScrollbarCurrentPositionController*>(this));
        Super::setAttributeController("maxpos", static_cast<ScrollbarMaxPositionController*>(this));
        Super::setAttributeController("increment", static_cast<ScrollbarIncrementController*>(this));
        Super::setAttributeController("pageincrement", static_cast<ScrollbarPageIncrementController*>(this));
        return Super::initAttributeControllers();
    }


} // namespace XULWin
