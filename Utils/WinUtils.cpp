#include "WinUtils.h"
#include "Poco/UnicodeConverter.h"
#include "ErrorReporter.h"
#include <boost/lexical_cast.hpp>
#include <commctrl.h>


namespace Utils
{
    
    std::string ToUTF8(const std::wstring & inText)
    {
        std::string result;
        Poco::UnicodeConverter::toUTF8(inText, result);
        return result;
    }


    std::wstring ToUTF16(const std::string & inText)
    {
        std::wstring result;
        Poco::UnicodeConverter::toUTF16(inText, result);
        return result;
    }


    CommonControlsInitializer::CommonControlsInitializer() :
        mSuccess(false)
    {
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC  = ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES;
		InitCommonControlsEx(&icex);
    }
    
        
    bool CommonControlsInitializer::ok() const
    {
        return mSuccess;
    }
    

    CurrentDirectoryChanger::CurrentDirectoryChanger(const std::string & inTargetDir)
	{
		::GetCurrentDirectoryW(MAX_PATH, mOldDir);
        std::wstring newDir = ToUTF16(inTargetDir);
		::SetCurrentDirectoryW(newDir.c_str());
	}


	CurrentDirectoryChanger::~CurrentDirectoryChanger()
	{
		::SetCurrentDirectoryW(mOldDir);
	}


    std::string getCurrentDirectory()
    {
        TCHAR buffer[MAX_PATH];
        ::GetCurrentDirectory(sizeof(buffer), &buffer[0]);
        return std::string(ToUTF8(buffer)) + "/";
    }
    
    
    SIZE GetSizeDifference_WindowRect_ClientRect(HWND inHandle)
    {
	    RECT rc;
	    GetClientRect(inHandle, &rc);
    	
	    RECT rw;
	    GetWindowRect(inHandle, &rw);
    	
	    int rc_w = rc.right-rc.left;
	    int rc_h = rc.bottom-rc.top;
	    int rw_w = rw.right-rw.left;
	    int rw_h = rw.bottom-rw.top;
    	
	    SIZE theDifference;
	    theDifference.cx = rw_w-rc_w;
	    theDifference.cy = rw_h-rc_h;
    	
	    return theDifference;
    }


    void addStringToComboBox(HWND inHandle, const std::string & inString)
    {
        ::SendMessage(inHandle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)ToUTF16(inString).c_str());
    }

    
    void deleteStringFromComboBox(HWND inHandle, int inIndex)
    {
        ::SendMessage(inHandle, CB_DELETESTRING, (WPARAM)inIndex, (LPARAM)0);
    }

    
    int findStringInComboBox(HWND inHandle, const std::string & inString, int inOffset)
    {
        return ::SendMessage(inHandle, CB_FINDSTRING, (WPARAM)inOffset, (LPARAM)(LPTSTR)ToUTF16(inString).c_str());
    }


    int getComboBoxItemCount(HWND inHandle)
    {
        return ::SendMessage(inHandle, CB_GETCOUNT, 0, 0);
    }


    int getWindowWidth(HWND inHandle)
    {
        RECT rw;
        ::GetWindowRect(inHandle, &rw);
        return rw.right - rw.left;
    }


    int getWindowHeight(HWND inHandle)
    {
        RECT rw;
        ::GetWindowRect(inHandle, &rw);
        return rw.bottom - rw.top;
    }


    void selectComboBoxItem(HWND inHandle, int inItemIndex)
    {
        ::SendMessage(inHandle, (UINT)CB_SETCURSEL, (WPARAM)inItemIndex, (LPARAM)0);
    }


    void setWindowWidth(HWND inHandle, int inWidth)
    {
        RECT rw;
        ::GetWindowRect(inHandle, &rw);
        
        int oldWidth = rw.right - rw.left;
        int x = rw.left - (inWidth - oldWidth)/2;
        if (inWidth < oldWidth)
        {
            x = rw.left + (inWidth - oldWidth)/2;
        }
        ::MoveWindow(inHandle, x, rw.top, inWidth, rw.bottom - rw.top, FALSE);
    }


    void setWindowHeight(HWND inHandle, int inHeight)
    {
        RECT rw;
        ::GetWindowRect(inHandle, &rw);
        
        int oldHeight = rw.bottom - rw.top;
        int y = rw.top - (inHeight - oldHeight)/2;
        if (inHeight < oldHeight)
        {
            y = rw.top + (inHeight - oldHeight)/2;
        }
        ::MoveWindow(inHandle, rw.left, y, rw.right - rw.left, inHeight, FALSE);
    }

    
    HFONT getFont(HWND inHandle)
    {
	    return (HFONT)SendMessage(inHandle, WM_GETFONT, 0, 0);
    }
    
    
    SIZE getTextSize(HWND inHandle, const std::string & inText)
    {
	    // get the size in pixels for the given text and font
        SIZE result = {0, 0};    	
	    HDC hDC = GetDC(inHandle);
	    SelectObject(hDC, getFont(inHandle));
        std::wstring utf16Text(ToUTF16(inText));
        ::GetTextExtentPoint32(hDC, utf16Text.c_str(), (int)utf16Text.size(), &result);
	    ReleaseDC(inHandle, hDC);
	    return result;
    }


    std::string getWindowText(HWND inHandle)
    {
		std::string result;
		int length = ::GetWindowTextLength(inHandle);
		if (length > 0)
		{
			TCHAR * buffer = new TCHAR[length+1];
			::GetWindowText(inHandle, buffer, length+1);
			result = ToUTF8(buffer);
			delete [] buffer;
		}
		return result;
    }

    
    void setWindowText(HWND inHandle, const std::string & inText)
    {
        ::SetWindowText(inHandle, ToUTF16(inText).c_str());
    }


    std::string getLastError(DWORD lastError)
    {
	    LPVOID lpMsgBuf;
	    ::FormatMessage
	    (
		    FORMAT_MESSAGE_ALLOCATE_BUFFER
		    | FORMAT_MESSAGE_FROM_SYSTEM
		    | FORMAT_MESSAGE_IGNORE_INSERTS,
		    NULL,
		    lastError,
		    MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		    (LPTSTR)&lpMsgBuf,
		    0,
		    NULL
	    );
	    if (lpMsgBuf)
	    {
            std::wstring errorString = (LPTSTR)lpMsgBuf;
		    LocalFree(lpMsgBuf);
		    if (errorString.empty())
		    {
			    try
			    {
                    errorString = boost::lexical_cast<std::wstring>(lastError);
			    }
			    catch(...)
			    {
				    ReportError("Bad cast!");
			    }
		    }
		    return ToUTF8(errorString);
	    }
	    else
	    {
		    return "";
	    }
    }

    
    bool isWindowDisabled(HWND inHandle)
    {
        return !::IsWindowEnabled(inHandle);
    }


    void disableWindow(HWND inHandle, bool inDisable)
    {
        ::EnableWindow(inHandle, inDisable ? FALSE : TRUE);
    }


    int getMultilineTextHeight(HWND inHandle)
    {   
        int result = 0;

        HDC deviceContext(::GetDC(inHandle));

        HFONT font = (HFONT)::SendMessage(inHandle, WM_GETFONT, 0, 0);
		if (font)
        {
			::SelectObject(deviceContext, font);
        }
        
        RECT rc;
        ::GetClientRect(inHandle, &rc);

        RECT bounds;
		bounds.left = 0;
		bounds.top = 0;
		bounds.right = rc.right - rc.left;
		bounds.bottom = INT_MAX;

        std::wstring textUTF16 = ToUTF16(getWindowText(inHandle));
        
		result = ::DrawText(deviceContext, textUTF16.c_str(), (int)textUTF16.size(), &bounds, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL);

        ::ReleaseDC(inHandle, deviceContext);

        return result;
    }


    void setCheckBoxState(HWND inHandle, CheckState inState)
    {
        ::SendMessage(inHandle, BM_SETCHECK, (WPARAM)inState, 0);
    }

    
    CheckState getCheckBoxState(HWND inHandle)
    {
        return static_cast<CheckState>(::SendMessage(inHandle, BM_GETCHECK, 0, 0));
    }


    bool isCheckBoxChecked(HWND inHandle)
    {
        return getCheckBoxState(inHandle) != UNCHECKED;
    }

    
    void setCheckBoxChecked(HWND inHandle, bool inChecked)
    {
        setCheckBoxState(inHandle, inChecked ? Utils::CHECKED : Utils::UNCHECKED);
    }

    
    void initializeProgressMeter(HWND inHandle, int inLimit)
    {
        ::SendMessage(inHandle, PBM_SETRANGE, 0, MAKELPARAM(0, inLimit));	
        ::SendMessage(inHandle, PBM_SETSTEP, 1, 0);
        ::SendMessage(inHandle, PBM_SETPOS, 0, 0);
    }


    void advanceProgressMeter(HWND inHandle)
	{
        ::SendMessage(inHandle, PBM_STEPIT, 0, 0);
	}
		

	void setProgressMeterProgress(HWND inHandle, int inProgress)
	{
        ::SendMessage(inHandle, PBM_SETPOS, (WPARAM)inProgress, (LPARAM)0);
	}


    int getProgressMeterProgress(HWND inHandle)
    {
        return (int)::SendMessage(inHandle, PBM_GETPOS, (WPARAM)0, (LPARAM)0);
    }


    void addWindowStyle(HWND inHandle, LONG inStyle) 
    {
        ::SetWindowLong(inHandle, GWL_STYLE, ::GetWindowLong(inHandle, GWL_STYLE) | inStyle);
    }


    LONG getWindowStyles(HWND inHandle)
    {
        return ::GetWindowLong(inHandle, GWL_STYLE);
    }


    void setWindowStyle(HWND inHandle, LONG inStyle) 
    {
        ::SetWindowLong(inHandle, GWL_STYLE, inStyle);
    }


    void removeWindowStyle(HWND inHandle, LONG inStyle)
    {
        ::SetWindowLong(inHandle, GWL_STYLE, ::GetWindowLong(inHandle, GWL_STYLE) & ~inStyle);
    }


    bool hasWindowStyle(HWND inHandle, LONG inStyle)
    {
        return (::GetWindowLong(inHandle, GWL_STYLE) & inStyle) != 0;
    }


    void setWindowVisible(HWND inHandle, bool inVisible)
    {
        ::ShowWindow(inHandle, inVisible ? SW_SHOW : SW_HIDE);
    }

    
    bool isWindowVisible(HWND inHandle)
    {
        return TRUE == ::IsWindowVisible(inHandle);
    }


    void setTextBoxReadOnly(HWND inHandle, bool inReadOnly)
    {
        ::SendMessage(inHandle, EM_SETREADONLY, inReadOnly ? TRUE : FALSE, 0);
    }


    bool isTextBoxReadOnly(HWND inHandle)
    {
        return hasWindowStyle(inHandle, ES_READONLY);
    }


    void setScrollInfo(HWND inHandle, int inTotalHeight, int inPageHeight, int inCurrentPosition)
    {
        SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = 0;
		si.nMax = inTotalHeight;
		si.nPage = inPageHeight;
		si.nPos = inCurrentPosition;
		si.nTrackPos = 0; // is ignored by SetScrollInfo
        ::SetScrollInfo(inHandle, SB_CTL, &si, TRUE);
    }


    void getScrollInfo(HWND inHandle, int & outTotalHeight, int & outPageHeight, int & outCurrentPosition)
    {
        SCROLLINFO si;
        ::ZeroMemory(&si, sizeof(si));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

        ::GetScrollInfo(inHandle, SB_CTL, &si);
        outTotalHeight = si.nMax;
        outPageHeight = si.nPage;
        outCurrentPosition = si.nPos;
    }


    int getScrollPos(HWND inHandle)
    {
        return (int)::GetScrollPos(inHandle, SB_CTL);
    }


    void setScrollPos(HWND inHandle, int inPos)
    {
        ::SetScrollPos(inHandle, SB_CTL, inPos, TRUE);
    }


    void appendTabPanel(HWND inHandle, const std::string & inTitle)
    {
        TCITEM tabItem;
        tabItem.mask = TCIF_TEXT | TCIF_IMAGE; 
        tabItem.iImage = -1; 
        std::wstring text = ToUTF16(inTitle);
        tabItem.pszText = const_cast<LPWSTR>(text.c_str());
        if (TabCtrl_InsertItem(inHandle, TabCtrl_GetItemCount(inHandle), &tabItem) == -1) 
        { 
            ReportError("Failed to insert a new tab.");
        }
    }
    
} // namespace Utils
