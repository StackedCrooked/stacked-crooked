#include "NativeComponent.h"
#include "Defaults.h"
#include "Element.h"
#include "Layout.h"
#include "Utils/ErrorReporter.h"
#include "Utils/WinUtils.h"
#include <boost/lexical_cast.hpp>
#include <string>


using namespace Utils;


namespace XULWin
{
    int CommandID::sID = 101; // start command IDs at 101 to avoid conflicts with Windows predefined values
    
    NativeComponent::Components NativeComponent::sComponents;
    NativeComponent::ComponentsByID NativeComponent::sComponentsByID;

    NativeComponent::NativeComponent(NativeComponentPtr inParent, CommandID inCommandID) :
        mParent(inParent ? inParent.get() : 0),
        mHandle(0),
        mModuleHandle(::GetModuleHandle(0)), // TODO: Fix this hacky thingy!
        mCommandID(inCommandID),
        mMinimumWidth(0),
        mMinimumHeight(0)
    {
    }


    NativeComponent::~NativeComponent()
    {
        Components::iterator it = sComponents.find(mHandle);
        bool found = it != sComponents.end();
        assert(found);
        if (found)
        {
            sComponents.erase(it);
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
    

    void NativeComponent::applyAttribute(const std::string & inName, const std::string & inValue)
    {
        if (inName == "label")
        {
            ::SetWindowTextA(handle(), inValue.c_str());
        }
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
            throw std::runtime_error(std::string("Could not register Windows class."));
        }
    }


    NativeWindow::NativeWindow(NativeComponentPtr inParent) :
        NativeComponent(inParent, CommandID())
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
        sComponents.insert(std::make_pair(mHandle, this));
        sComponentsByID.insert(std::make_pair(mCommandID.intValue(), this));
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

    
    void NativeWindow::applyAttribute(const std::string & inName, const std::string & inValue)
    {
        NativeComponent::applyAttribute(inName, inValue);
        if (inName == "width")
        {
            try
            {
                Utils::setWindowWidth(handle(), boost::lexical_cast<int>(inValue));
            }
            catch (boost::bad_lexical_cast & e)
            {
                ReportError(std::string("Failed to apply 'width' attribute to Window. Reason: ") + e.what());
            }
        }
        else if (inName == "height")
        {
            try
            {
                Utils::setWindowHeight(handle(), boost::lexical_cast<int>(inValue));
            }
            catch (boost::bad_lexical_cast & e)
            {
                ReportError(std::string("Failed to apply 'width' attribute to Window. Reason: ") + e.what());
            }
        }
    }


    LRESULT NativeWindow::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        switch(inMessage)
        {
            case WM_COMMAND:
            {
                assert(false); // should prob not come here
                break;
            }
            case WM_SIZE:
            {
                Children::const_iterator it = owningElement()->children().begin();
                if (it != owningElement()->children().end())
                {
                    RECT rc;
                    ::GetClientRect(handle(), &rc);
                    HWND childHandle = (*it)->nativeComponent()->handle();
                    ::MoveWindow(childHandle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
                    ::InvalidateRect(childHandle, 0, FALSE);
                    ::UpdateWindow(childHandle);
                    rebuildChildLayouts();
                }
                ::InvalidateRect(handle(), 0, FALSE);
                ::UpdateWindow(handle());
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
        Components::iterator it = sComponents.find(hWnd);
        if (it != sComponents.end())
        {
            return it->second->handleMessage(inMessage, wParam, lParam);
        }
        return ::DefWindowProc(hWnd, inMessage, wParam, lParam);
    }


    NativeControl::NativeControl(NativeComponentPtr inParent, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle) :
        NativeComponent(inParent, CommandID())
    {
        if (!mParent)
        {
            throw std::runtime_error("Invalid mParent");
        }

        RECT rc;
        ::GetClientRect(inParent->handle(), &rc);
        
        mHandle = ::CreateWindowEx
        (
            0, 
            inClassName,
            TEXT(""),
            inStyle | WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
            0, 0, rc.right - rc.left, rc.bottom - rc.top,
            mParent ? mParent->handle() : 0,
            (HMENU)mCommandID.intValue(),
            mModuleHandle,
            0
        );

        // set default font
        ::SendMessage(mHandle, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));

        // subclass
        mOrigProc = (WNDPROC)(LONG_PTR)::SetWindowLongPtr(mHandle, GWL_WNDPROC, (LONG)(LONG_PTR)&NativeControl::MessageHandler);

        sComponents.insert(std::make_pair(mHandle, this));
        sComponentsByID.insert(std::make_pair(mCommandID.intValue(), this));
    }


    NativeControl::~NativeControl()
    {
        if (mOrigProc)
        {
            ::SetWindowLongPtr(mHandle, GWL_WNDPROC, (LONG)(LONG_PTR)mOrigProc);
            mOrigProc = 0;
        }
    }


    LRESULT NativeControl::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        switch(inMessage)
        {
            case WM_COMMAND:
            {					
                ComponentsByID::iterator it = sComponentsByID.find(LOWORD(wParam));
                if (it != sComponentsByID.end())
                {
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        TCHAR str[1024];
                        ::GetWindowTextA(it->second->handle(), &str[0], 1024);
                        it->second->owningElement()->setAttribute("value", str);
                    }
                    it->second->owningElement()->handleEvent("command");
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
        Components::iterator it = sComponents.find(hWnd);
        if (it != sComponents.end())
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
        
        
    void NativeTextBox::applyAttribute(const std::string & inName, const std::string & inValue)
    {
        NativeComponent::applyAttribute(inName, inValue);
        if (inName == "value")
        {
            ::SetWindowTextA(handle(), inValue.c_str());
        }
    }


    NativeTextBox::NativeTextBox(NativeComponentPtr inParent) :
        NativeControl(inParent,
                      TEXT("EDIT"),
                      WS_EX_CLIENTEDGE, // exStyle
                      ES_AUTOHSCROLL)
    {
    }
        
        
    void NativeLabel::applyAttribute(const std::string & inName, const std::string & inValue)
    {
        NativeComponent::applyAttribute(inName, inValue);
        if (inName == "value")
        {
            ::SetWindowTextA(handle(), inValue.c_str());
        }
    }
    
    
    void NativeBox::applyAttribute(const std::string & inName, const std::string & inValue)
    {
        NativeComponent::applyAttribute(inName, inValue);
        if (inName == "orientation")
        {
            if (inValue == "horizontal")
            {
                mOrientation = HORIZONTAL;
            }
            else if (inValue == "vertical")
            {
                mOrientation = VERTICAL;
            }
            else
            {
                ReportError("Invalid orientation: " + inValue);
            }
        }
    }
    
    
    void NativeBox::rebuildLayout()
    {
        RECT rc;
        ::GetClientRect(handle(), &rc);

        LinearLayoutManager layoutManager(mOrientation);
        
        //
        // Obtain the flex values
        //
        std::vector<int> flexValues;
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            std::string flex = mElement->children()[idx]->getAttribute("flex");

            int flexValue = Defaults::Attributes::flex();
            try
            {
                if (!flex.empty())
                {
                    flexValue = boost::lexical_cast<int>(flex);
                }
            }
            catch (const boost::bad_lexical_cast & )
            {
                ReportError("Lexical cast failed for value: " + flex + ".");
                // continue program flow
            }
            flexValues.push_back(flexValue);
        }
        
        //
        // Use the flex values to obtain the child rectangles
        //
        std::vector<Rect> childRects;
        layoutManager.getRects(
            Rect(rc.left,
                 rc.top,
                 rc.right-rc.left,
                 rc.bottom-rc.top),
            flexValues,
            childRects);

        //
        // Apply the new child rectangles
        //
        for (size_t idx = 0; idx != mElement->children().size(); ++idx)
        {
            Rect & rect = childRects[idx];
            int width = rect.width();
            if (width < mElement->children()[idx]->nativeComponent()->minimumWidth())
            {
                width = mElement->children()[idx]->nativeComponent()->minimumWidth();
            }

            int height = rect.height();
            if (height < mElement->children()[idx]->nativeComponent()->minimumHeight())
            {
                height = mElement->children()[idx]->nativeComponent()->minimumHeight();
            }
            HWND childHandle = mElement->children()[idx]->nativeComponent()->handle();
            ::MoveWindow(childHandle, rect.x(), rect.y(), width, height, FALSE);
            ::InvalidateRect(childHandle, 0, FALSE);
            ::UpdateWindow(childHandle);
        }
        rebuildChildLayouts();
        ::InvalidateRect(handle(), 0, FALSE);
        ::UpdateWindow(handle());
    }


    void NativeMenuList::addMenuItem(const std::string & inText)
    {
        Utils::addStringToComboBox(handle(), inText.c_str());
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
        int idx = Utils::findStringInComboBox(handle(), inText.c_str());
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
