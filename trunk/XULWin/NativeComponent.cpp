#include "NativeComponent.h"
#include "Defaults.h"
#include "Element.h"
#include "Layout.h"
#include "Utils/ErrorReporter.h"
#include "Utils/WinUtils.h"
#include "Poco/UnicodeConverter.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>


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
            boost::lexical_cast<int>(inValue);
        }
        catch (std::exception &)
        {
            ReportError("Int2String: lexical cast failed");
        }
        return result;
    }


    std::string WString2String(const std::wstring & inText)
    {
        std::string result;
        Poco::UnicodeConverter::toUTF8(inText, result);
        return result;
    }


    std::wstring String2WString(const std::string & inText)
    {
        std::wstring result;
        Poco::UnicodeConverter::toUTF16(inText, result);
        return result;
    }


    int CommandId::sId = 101; // start command Ids at 101 to avoid conflicts with Windows predefined values
    
    NativeComponent::Components NativeComponent::sComponentsByHandle;
    
    NativeControl::ControlsById NativeControl::sControlsById;

    NativeComponent::NativeComponent(NativeComponentPtr inParent, CommandId inCommandId) :
        mParent(inParent ? inParent.get() : 0),
        mHandle(0),
        mModuleHandle(::GetModuleHandle(0)), // TODO: Fix this hacky thingy!
        mCommandId(inCommandId),
        mMinimumWidth(Defaults::componentMinimumWidth()),
        mMinimumHeight(Defaults::componentMinimumHeight())
    {
        {
            AttributeGetter heightGetter = boost::bind(&Int2String, boost::bind(&Utils::getWindowHeight, handle()));
            AttributeSetter heightSetter = boost::bind(&Utils::setWindowHeight, handle(), boost::bind(&String2Int, _1));
            mAttributeControllers.insert(std::make_pair("height", AttributeController(heightGetter, heightSetter)));
        }

        {
            AttributeGetter widthGetter = boost::bind(&Int2String, boost::bind(&Utils::getWindowWidth, handle()));
            AttributeSetter widthSetter = boost::bind(&Utils::setWindowWidth, handle(), boost::bind(&String2Int, _1));
            mAttributeControllers.insert(std::make_pair("width", AttributeController(widthGetter, widthSetter)));
        }

        {
            AttributeGetter labelGetter = boost::bind(&WString2String, boost::bind(&Utils::getWindowText, handle()));
            AttributeSetter labelSetter = boost::bind(&Utils::setWindowText, handle(), boost::bind(&String2WString, _1));
            mAttributeControllers.insert(std::make_pair("label", AttributeController(labelGetter, labelSetter)));
        }
    }


    NativeComponent::~NativeComponent()
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
    
    
    int NativeComponent::minimumWidth() const
    {
        return mMinimumWidth;
    }

    
    int NativeComponent::minimumHeight() const
    {
        return mMinimumHeight;
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
    

    //bool NativeComponent::applyAttribute(const std::string & inName, const std::string & inValue)
    //{
    //    if (inName == "label")
    //    {
    //        std::wstring utf16Text;
    //        Poco::UnicodeConverter::toUTF16(inValue, utf16Text);
    //        ::SetWindowText(handle(), utf16Text.c_str());
    //        return true;
    //    }
    //    return false;
    //}
    
    
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
        ::InvalidateRect(handle(), 0, FALSE);
    }

    
    void NativeComponent::rebuildChildLayouts()
    {
        Children::const_iterator it = mElement->children().begin(), end = mElement->children().end();
        for (; it != end; ++it)
        {
            NativeComponent * nativeComp = (*it)->nativeComponent().get();
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
        wndClass.hbrBackground = 0; // covered by content pane so no color needed (reduces flicker)
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = TEXT("XULWin::Window");
        wndClass.hIconSm = 0;
        if (! RegisterClassEx(&wndClass))
        {
            ReportError("Could not register XUL::Window class.");
        }
    }


    NativeWindow::NativeWindow(NativeComponentPtr inParent) :
        NativeComponent(inParent, CommandId())
    {
        int w = Defaults::windowWidth();
        int h = Defaults::windowHeight();
        int x = (GetSystemMetrics(SM_CXSCREEN) - w)/2;
        int y = (GetSystemMetrics(SM_CYSCREEN) - h)/2;
        
        mHandle = ::CreateWindowEx
        (
            0, 
            TEXT("XULWin::Window"),
            TEXT(""),
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
            x, y, w, h,
            mParent ? mParent->handle() : 0,
            (HMENU)0,
            mModuleHandle,
            0
        );

        // set default font
        ::SendMessage(mHandle, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
        sComponentsByHandle.insert(std::make_pair(mHandle, this));
    }


    void NativeWindow::showModal()
    {        
        ::ShowWindow(handle(), SW_SHOW);

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

    
    //bool NativeWindow::applyAttribute(const std::string & inName, const std::string & inValue)
    //{
    //    if (inName == "width")
    //    {
    //        try
    //        {
    //            Utils::setWindowWidth(handle(), boost::lexical_cast<int>(inValue));
    //            return true;
    //        }
    //        catch (boost::bad_lexical_cast & e)
    //        {
    //            ReportError(std::string("Failed to apply 'width' attribute to Window. Reason: ") + e.what());
    //            return false;
    //        }
    //    }
    //    else if (inName == "height")
    //    {
    //        try
    //        {
    //            Utils::setWindowHeight(handle(), boost::lexical_cast<int>(inValue));
    //            return true;
    //        }
    //        catch (boost::bad_lexical_cast & e)
    //        {
    //            ReportError(std::string("Failed to apply 'width' attribute to Window. Reason: ") + e.what());
    //            return false;
    //        }
    //    }
    //    return NativeComponent::applyAttribute(inName, inValue);
    //}


    LRESULT NativeWindow::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        switch(inMessage)
        {
            case WM_SIZE:
            {
                Children::const_iterator it = owningElement()->children().begin();
                if (it != owningElement()->children().end())
                {
                    RECT rc;
                    ::GetClientRect(handle(), &rc);
                    HWND childHandle = (*it)->nativeComponent()->handle();
                    ::MoveWindow(childHandle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
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


    NativeControl::NativeControl(NativeComponentPtr inParent, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle) :
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
                    it->second->owningElement()->OnCommand(0);
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


    NativeLabel::NativeLabel(NativeComponentPtr inParent) :
        NativeControl(inParent,
                      TEXT("STATIC"),
                      0, // exStyle
                      0)
    {
    }


    NativeTextBox::NativeTextBox(NativeComponentPtr inParent) :
        NativeControl(inParent,
                      TEXT("EDIT"),
                      WS_EX_CLIENTEDGE, // exStyle
                      ES_AUTOHSCROLL)
    {
        
        {
            AttributeGetter valueGetter = boost::bind(&WString2String, boost::bind(&Utils::getWindowText, handle()));
            AttributeSetter valueSetter = boost::bind(&Utils::setWindowText, handle(), boost::bind(&String2WString, _1));
            mAttributeControllers.insert(std::make_pair("value", AttributeController(valueGetter, valueSetter)));
        }
    }
        
        
    //bool NativeTextBox::applyAttribute(const std::string & inName, const std::string & inValue)
    //{
    //    if (inName == "value")
    //    {
    //        std::wstring text;
    //        Poco::UnicodeConverter::toUTF16(inValue, text);
    //        ::SetWindowText(handle(), text.c_str());
    //        return true;
    //    }
    //    return NativeComponent::applyAttribute(inName, inValue);
    //}


    void NativeTextBox::handleCommand(WPARAM wParam, LPARAM lParam)
    {
        //if (HIWORD(wParam) == EN_CHANGE)
        //{
        //    Utils::String utf16Text = Utils::getWindowText(handle());
        //    std::string utf8Text;
        //    Poco::UnicodeConverter::toUTF8(utf16Text, utf8Text);
        //    owningElement()->setAttribute("value", utf8Text, false);

        //    static_cast<TextBox*>(owningElement())->OnChanged(0);
        //}
    }
        
        
    //bool NativeLabel::applyAttribute(const std::string & inName, const std::string & inValue)
    //{
    //    if (inName == "value")
    //    {
    //        std::wstring utf16Text;
    //        Poco::UnicodeConverter::toUTF16(inValue, utf16Text);
    //        ::SetWindowText(handle(), utf16Text.c_str());
    //        return true;
    //    }
    //    return NativeComponent::applyAttribute(inName, inValue);
    //}


    int NativeLabel::minimumWidth() const
    {
        std::string text = owningElement()->getAttribute("value");
        std::wstring utf16Text;
        Poco::UnicodeConverter::toUTF16(text, utf16Text);
        SIZE size = Utils::getTextSize(mHandle, utf16Text);
        return size.cx;
    }

    
    int NativeLabel::minimumHeight() const
    {
        return Defaults::controlHeight();
    }
    
    
    NativeHBox::NativeHBox(NativeComponentPtr inParent) :
        NativeBox(inParent, HORIZONTAL)
    {   
    }
        
        
    NativeBox::NativeBox(NativeComponentPtr inParent, Orientation inOrientation) :
        NativeControl(inParent,
                      TEXT("STATIC"),
                      0, // exStyle
                      0),
        mOrientation(inOrientation)
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
        };
        AttributeSetter orientationSetter = boost::bind(&NativeBox::setOrientation, this, boost::bind(&Helper::String2Orientation, _1));
        AttributeGetter orientationGetter = boost::bind(&Helper::Orientation2String, boost::bind(&NativeBox::getOrientation, this));
        mAttributeControllers.insert(std::make_pair("orientation", AttributeController(orientationGetter, orientationSetter)));
    }


    void NativeBox::setOrientation(Orientation inOrientation)
    {
        mOrientation = inOrientation;
    }

    
    Orientation NativeBox::getOrientation()
    {
        return mOrientation;
    }

    
    void NativeBox::rebuildLayout()
    {

        LinearLayoutManager layoutManager(mOrientation);
        
        //
        // Obtain the flex values
        //
        std::vector<int> allFlexValues;
        std::vector<int> nonZeroFlexValues;
        RECT rc;
        ::GetClientRect(handle(), &rc);
        int availableSpace = rc.right - rc.left;
        if (mOrientation == VERTICAL)
        {
            availableSpace = rc.bottom - rc.top;
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
                nonZeroFlexValues.push_back(flexValue);
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
        layoutManager.GetPortions(availableSpace, nonZeroFlexValues.size(), portions);

        //
        // Apply the new child rectangles
        //
        int offsetX = 0;
        int offsetY = 0;
        int portionIdx = 0;
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            ElementPtr child = mElement->children()[idx];
            HWND childHandle = child->nativeComponent()->handle();
            int width = child->nativeComponent()->minimumWidth();
            int height = child->nativeComponent()->minimumHeight();
            if (allFlexValues[idx] != 0)
            {
                if (mOrientation == HORIZONTAL)
                {
                    width = portions[portionIdx];
                }
                else
                {
                    height = portions[portionIdx];
                }
                portionIdx++;
            }
            ::MoveWindow(childHandle, offsetX, offsetY, width, height, FALSE);

            if (mOrientation == HORIZONTAL)
            {
                offsetX += width;
            }
            if (mOrientation == VERTICAL)
            {
                offsetY += height;
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


    void NativeMenuList::addMenuItem(const std::string & inText)
    {
        std::wstring utf16Text;
        Poco::UnicodeConverter::toUTF16(inText, utf16Text);
        Utils::addStringToComboBox(handle(), utf16Text.c_str());
        int count = Utils::getComboBoxItemCount(handle());		
		if (count == 1)
		{
            Utils::selectComboBoxItem(handle(), 0);
		}
		
		// The height of a combobox defines the height of the dropdown menu + the height of the widget itself.
        mMinimumHeight = Defaults::controlHeight() + count*Defaults::dropDownListItemHeight();
    }


    void NativeMenuList::removeMenuItem(const std::string & inText)
    {
        std::wstring utf16String;
        Poco::UnicodeConverter::toUTF16(inText, utf16String);
        int idx = Utils::findStringInComboBox(handle(), utf16String.c_str());
        if (idx == CB_ERR)
        {
            ReportError("MenuList: remove failed because item not found: '" + inText + "'.");
            return;
        }

        Utils::deleteStringFromComboBox(handle(), idx);
		
		// The height of a combobox defines the height of the dropdown menu + the height of the widget itself.
        mMinimumHeight = Defaults::controlHeight() + Utils::getComboBoxItemCount(handle())*Defaults::dropDownListItemHeight();
    }


} // namespace XULWin
