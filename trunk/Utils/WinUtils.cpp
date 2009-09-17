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


} // namespace Utils
