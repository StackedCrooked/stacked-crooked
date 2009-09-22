#include "WinUtils.h"
#include "Poco/UnicodeConverter.h"
#include "ErrorReporter.h"
#include <boost/lexical_cast.hpp>


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
        
        HDC deviceContext(::GetDC(inHandle));

        HFONT font = (HFONT)::SendMessage(inHandle, WM_GETFONT, 0, 0);
		if (font)
        {
			::SelectObject(deviceContext, font);
        }
        
        RECT rw;
        ::GetWindowRect(inHandle, &rw);

        RECT bounds;
		bounds.left = 0;
		bounds.top = 0;
		bounds.right = rw.right - rw.left;
		bounds.bottom = 1000;

        std::wstring textUTF16 = ToUTF16(getWindowText(inHandle));
        
        return ::DrawText(deviceContext, textUTF16.c_str(), (int)textUTF16.size(), &bounds, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL);
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


} // namespace Utils
