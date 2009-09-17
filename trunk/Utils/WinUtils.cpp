#include "WinUtils.h"


namespace Utils
{


    void addStringToComboBox(HWND inHandle, const TCHAR * inString)
    {
        ::SendMessage(inHandle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)inString);
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
