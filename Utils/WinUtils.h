#ifndef WINUTILS_H_INCLUDED
#define WINUTILS_H_INCLUDED


#include <windows.h>
#include <string>


namespace Utils
{
    void addStringToComboBox(HWND inHandle, const std::string & inString);

    void deleteStringFromComboBox(HWND inHandle, int inIndex);

    // returns CB_ERR (-1) if not found
    int findStringInComboBox(HWND inHandle, const std::string & inString, int inOffset = -1);

    int getComboBoxItemCount(HWND inHandle);

    void selectComboBoxItem(HWND inHandle, int inItemIndex);

    int getWindowWidth(HWND inHandle);

    int getWindowHeight(HWND inHandle);

    void setWindowWidth(HWND inHandle, int inWidth);

    void setWindowHeight(HWND inHandle, int inHeight);

    HFONT getFont(HWND inHandle);

    SIZE getTextSize(HWND inHandle, const std::string & inText);

    std::string getWindowText(HWND inHandle);

    void setWindowText(HWND inHandle, const std::string & inText);

} // namespace Utils


#endif // WINUTILS_H_INCLUDED
