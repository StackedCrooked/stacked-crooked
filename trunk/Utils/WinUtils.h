#ifndef WINUTILS_H_INCLUDED
#define WINUTILS_H_INCLUDED


#include <windows.h>
#include <string>


namespace Utils
{

#ifdef UNICODE
    typedef std::wstring String;
#else
    typedef std::string String;
#endif

    void addStringToComboBox(HWND inHandle, const String & inString);

    void deleteStringFromComboBox(HWND inHandle, int inIndex);

    // returns CB_ERR if not found
    int findStringInComboBox(HWND inHandle, const String & inString, int inOffset = -1);

    int getComboBoxItemCount(HWND inHandle);

    void selectComboBoxItem(HWND inHandle, int inItemIndex);

    void setWindowWidth(HWND inHandle, int inWidth);

    void setWindowHeight(HWND inHandle, int inHeight);

    HFONT GetFont(HWND inHandle);

    SIZE GetTextSize(HWND inHandle, const String & inText);

} // namespace Utils


#endif // WINUTILS_H_INCLUDED
