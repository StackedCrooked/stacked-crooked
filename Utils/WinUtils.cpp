#include "WinUtils.h"


namespace Utils
{


    void addStringToComboBox(HWND inHandle, const String & inString)
    {
        ::SendMessage(inHandle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)inString.c_str());
    }

    
    void deleteStringFromComboBox(HWND inHandle, int inIndex)
    {
        ::SendMessage(inHandle, CB_DELETESTRING, (WPARAM)inIndex, (LPARAM)0);
    }

    
    int findStringInComboBox(HWND inHandle, const String & inString, int inOffset)
    {
        return ::SendMessage(inHandle, CB_FINDSTRING, (WPARAM)inOffset, (LPARAM)(LPTSTR)inString.c_str());
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
    
    
    SIZE getTextSize(HWND inHandle, const String & inText)
    {
	    // get the size in pixels for the given text and font
        SIZE result = {0, 0};    	
	    HDC hDC = GetDC(inHandle);
	    SelectObject(hDC, getFont(inHandle));
        ::GetTextExtentPoint32(hDC, inText.c_str(), (int)inText.size(), &result);
	    ReleaseDC(inHandle, hDC);
	    return result;
    }


    String getWindowText(HWND inHandle)
    {
		String result;
		int length = ::GetWindowTextLength(inHandle);
		if (length > 0)
		{
			TCHAR * buffer = new TCHAR[length+1];
			::GetWindowText(inHandle, buffer, length+1);
			result = String(buffer);
			delete [] buffer;
		}
		return result;
    }

    
    void setWindowText(HWND inHandle, const String & inText)
    {
        ::SetWindowText(inHandle, inText.c_str());
    }


} // namespace Utils
