#ifndef WINUTILS_H_INCLUDED
#define WINUTILS_H_INCLUDED


#include <windows.h>


namespace Utils
{
    void addStringToComboBox(HWND inHandle, const TCHAR * inString);

    void deleteStringFromComboBox(HWND inHandle, int inIndex);

    // returns CB_ERR if not found
    int findStringInComboBox(HWND inHandle, const TCHAR * inString, int inOffset = -1);

    int getComboBoxItemCount(HWND inHandle);

    void selectComboBoxItem(HWND inHandle, int inItemIndex);

    void setWindowWidth(HWND inHandle, int inWidth);

    void setWindowHeight(HWND inHandle, int inHeight);

    HFONT GetFont(HWND inHandle);

    SIZE GetTextSize(HWND inHandle, const TCHAR * inText, size_t inTextLength);

} // namespace Utils


#endif // WINUTILS_H_INCLUDED
