#include "PopupMenu.h"
#include "ErrorReporter.h"
#include "WinUtils.h"
#include <sstream>


namespace Utils
{

    boost::shared_ptr<PopupMenuItem> PopupMenuItem::Separator(new PopupMenuItem(0, ""));

    const int PopupMenuItem::SeparatorID = 0;


    PopupMenuItem::PopupMenuItem():
	    mId(0),
	    mEnabled(true),
	    mChecked(false),
	    mBitmap(0)
    {
    }


    PopupMenuItem::PopupMenuItem(int inId, const std::string & inText):
	    mId(inId),
	    mText(inText),
	    mEnabled(true),
	    mChecked(false),
	    mBitmap(0)
    {
    }


    int PopupMenuItem::id() const
    {
	    return mId;
    }


    const std::string & PopupMenuItem::text() const
    {
	    return mText;
    }


    bool PopupMenuItem::isEnabled() const
    {
	    return mEnabled;
    }


    bool PopupMenuItem::isChecked() const
    {
	    return mChecked;
    }


    void PopupMenuItem::setEnabled(bool inEnabled)
    {
	    mEnabled = inEnabled;
    }
    				

    void PopupMenuItem::setChecked(bool inChecked)
    {
	    mChecked = inChecked;
    }


    void PopupMenuItem::setBitmap(HBITMAP inBitmap)
    {
	    mBitmap = inBitmap;
    }


    HBITMAP PopupMenuItem::bitmap() const
    {
	    return mBitmap;
    }
    				

    void PopupMenuItem::setImage(boost::shared_ptr<Gdiplus::Bitmap> inImage)
    {
	    mImage = inImage;
    }


    boost::shared_ptr<Gdiplus::Bitmap> PopupMenuItem::image() const
    {
	    return mImage;
    }


    PopupMenu::PopupMenu():
	    mHandle(0), mOwnerDraw(false), mSize(0)
    {
	    mHandle = CreatePopupMenu();
    }
    		

    PopupMenu::~PopupMenu()
    {
	    DestroyMenu(mHandle);
	    mHandle = 0;
    }


    PopupMenu::operator HMENU() const
    {
	    return mHandle;
    }


    void PopupMenu::append(boost::shared_ptr<PopupMenuItem> inMenuItem)
    {
	    MENUITEMINFO mii;
	    memset(&mii, 0, sizeof(mii));
	    mii.cbSize = sizeof(mii);
    	
	    updateInfo(*inMenuItem, mii);
	    BOOL result = InsertMenuItem(mHandle, mSize++, TRUE, &mii);
	    ::EnableMenuItem(mHandle, inMenuItem->id(), MF_BYCOMMAND | (inMenuItem->isEnabled() ? MF_ENABLED : MF_DISABLED | MF_GRAYED));
	    ::CheckMenuItem(mHandle, inMenuItem->id(), MF_BYCOMMAND | (inMenuItem->isChecked() ? MF_CHECKED : MF_UNCHECKED));
	    if (!result)
	    {
            std::stringstream ss;
            ss << "PopupMenu::appendMenuItem failed. Reason: " << Utils::getLastError(::GetLastError());
		    ReportError(ss.str());
	    }
    }


    void PopupMenu::append(const std::string & inText, const MenuPtr & inSubmenu)
    {	
	    PopupMenuItem item(0, inText);
	    MENUITEMINFO mii;
	    memset(&mii, 0, sizeof(mii));
	    mii.cbSize = sizeof(mii);
	    mii.fMask = MIIM_SUBMENU | MIIM_STRING;
    	
	    // copy string into dwTypeData field
	    {
		    std::wstring text = ToUTF16(inText);
		    std::size_t length = text.length();
		    TCHAR * buffer = new TCHAR[length + 1];
		    for (std::size_t i = 0; i < length; i++)
		    {
			    buffer[i] = text[i];
		    }
		    buffer[length] = 0;
		    mii.dwTypeData = (LPTSTR)buffer;
	    }
    		
	    mii.cch = (UINT)inText.size();
	    mii.hSubMenu = inSubmenu->operator HMENU();

	    BOOL result = InsertMenuItem(mHandle, mSize++, TRUE, &mii);
	    mSubmenus.push_back(inSubmenu);
    			
	    if (!result)
	    {
            std::stringstream ss;
            ss << "PopupMenu::appendMenuItem failed. Reason: " << Utils::getLastError(::GetLastError());
		    ReportError(ss.str());
	    }
    }


    void PopupMenu::updateInfo(const PopupMenuItem & inMenuItem, MENUITEMINFO & ioItemInfo)
    {
	    ioItemInfo.fMask = MIIM_FTYPE | MIIM_ID | MIIM_STRING;
	    ioItemInfo.fType = (inMenuItem.id() == 0) ? MFT_SEPARATOR : 0;
    	
	    // copy string into dwTypeData field
	    {
		    std::wstring text = ToUTF16(inMenuItem.text());
		    std::size_t length = text.length();
		    TCHAR * buffer = new TCHAR[length + 1];
		    for (std::size_t i = 0; i < length; i++)
		    {
			    buffer[i] = text[i];
		    }
		    buffer[length] = 0;
		    ioItemInfo.dwTypeData = (LPTSTR)buffer;
	    }
    		
	    ioItemInfo.cch = (UINT)inMenuItem.text().size();
	    ioItemInfo.wID = inMenuItem.id();
	    if (inMenuItem.bitmap())
	    {
		    ioItemInfo.fMask |= MIIM_CHECKMARKS;
		    ioItemInfo.hbmpChecked = inMenuItem.bitmap();
		    ioItemInfo.hbmpUnchecked = inMenuItem.bitmap();
	    }
    }

} // namespace Utils
