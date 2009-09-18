#include "WinUtils.h"


namespace Utils
{


    void addStringToComboBox(HWND inHandle, const TCHAR * inString)
    {
        ::SendMessage(inHandle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)inString);
    }

    
    void deleteStringFromComboBox(HWND inHandle, int inIndex)
    {
        ::SendMessage(inHandle, CB_DELETESTRING, (WPARAM)inIndex, (LPARAM)0);
    }

    
    int findStringInComboBox(HWND inHandle, const TCHAR * inString, int inOffset)
    {
        return ::SendMessage(inHandle, CB_FINDSTRING, (WPARAM)inOffset, (LPARAM)(LPTSTR)inString);
    }


    int getComboBoxItemCount(HWND inHandle)
    {
        return ::SendMessage(inHandle, CB_GETCOUNT, 0, 0);
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

    
    HFONT GetFont(HWND inHandle)
    {
	    return (HFONT)SendMessage(inHandle, WM_GETFONT, 0, 0);
    }
    
    
    SIZE GetTextSize(HWND inHandle, const TCHAR * inText, size_t inTextLength)
    {
	    // get the size in pixels for the given text and font
        SIZE result = {0, 0};    	
	    HDC hDC = GetDC(inHandle);
	    SelectObject(hDC, GetFont(inHandle));
	    GetTextExtentPoint32(hDC, inText, (int)inTextLength, &result);
	    ReleaseDC(inHandle, hDC);
	    return result;
    }


} // namespace Utils
