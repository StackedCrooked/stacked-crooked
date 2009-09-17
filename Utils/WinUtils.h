#ifndef WINUTILS_H_INCLUDED
#define WINUTILS_H_INCLUDED


#include <windows.h>


namespace Utils
{
    void addStringToComboBox(HWND inHandle, const TCHAR * inString);

    int getComboBoxItemCount(HWND inHandle);

    void selectComboBoxItem(HWND inHandle, int inItemIndex);

} // namespace Utils


#endif // WINUTILS_H_INCLUDED
