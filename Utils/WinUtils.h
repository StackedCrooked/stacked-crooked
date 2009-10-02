#ifndef WINUTILS_H_INCLUDED
#define WINUTILS_H_INCLUDED


#include <windows.h>
#include <string>


namespace Utils
{
    std::string ToUTF8(const std::wstring & inText);

    std::wstring ToUTF16(const std::string & inText);

    class CurrentDirectoryChanger
    {
    public:
	    CurrentDirectoryChanger(const std::string & inTargetDir);

	    ~CurrentDirectoryChanger();

    private:
        CurrentDirectoryChanger(const CurrentDirectoryChanger &);
        CurrentDirectoryChanger & operator=(const CurrentDirectoryChanger &);
	    TCHAR mOldDir[MAX_PATH];
    };


    // Initializes Windows Common Controls
    class CommonControlsInitializer
    {
    public:
        CommonControlsInitializer();

        bool ok() const;

    private:
        bool mSuccess;
    };

    std::string getCurrentDirectory();

    std::string getLastError(DWORD lastError);

    SIZE GetSizeDifference_WindowRect_ClientRect(HWND inHandle);

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

    bool isWindowDisabled(HWND inHandle);

    void disableWindow(HWND inHandle, bool inDisable);

    int getMultilineTextHeight(HWND inHandle);

    enum CheckState
    {
        CHECKED = BST_CHECKED,
        UNCHECKED = BST_UNCHECKED,
        PARTIALLY_CHECKED = BST_INDETERMINATE
    };

    void setCheckBoxState(HWND inHandle, CheckState inState);

    CheckState getCheckBoxState(HWND inHandle);

    bool isCheckBoxChecked(HWND inHandle);

    void setCheckBoxChecked(HWND inHandle, bool inChecked);

    void initializeProgressMeter(HWND inHandle, int inLimit);
    
    void advanceProgressMeter(HWND inHandle);
		
	void setProgressMeterProgress(HWND inHandle, int inProgress);

    int getProgressMeterProgress(HWND inHandle);

    void addWindowStyle(HWND inHandle, LONG inStyle);

    LONG getWindowStyles(HWND inHandle);

    void setWindowStyle(HWND inHandle, LONG inStyle);

    void removeWindowStyle(HWND inHandle, LONG inStyle);

    bool hasWindowStyle(HWND inHandle, LONG inStyle);

    void setWindowVisible(HWND inHandle, bool inVisible);

    bool isWindowVisible(HWND inHandle);

    void setTextBoxReadOnly(HWND inHandle, bool inReadOnly);

    bool isTextBoxReadOnly(HWND inHandle);

    void setScrollInfo(HWND inHandle, int inTotalHeight, int inPageHeight, int inCurrentPosition);

    void getScrollInfo(HWND inHandle, int & outTotalHeight, int & outPageHeight, int & outCurrentPosition);

    int getScrollPos(HWND inHandle);

    void setScrollPos(HWND inHandle, int inPos);    

    void appendTabPanel(HWND inHandle, const std::string & inTitle);

} // namespace Utils


#endif // WINUTILS_H_INCLUDED
