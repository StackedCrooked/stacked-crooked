#include "ToolbarItem.h"
#include "Toolbar.h"
#include "Utils/PopupMenu.h"
#include "Utils/WinUtils.h"
#include <boost/bind.hpp>
#include <gdiplus.h>
#include <commctrl.h>

#ifdef _DEBUGFREE // detect the access to freed memory
#undef free
#define free(p) _free_dbg(p, _NORMAL_BLOCK); *(int*)&p = 0x666;
#endif


namespace Utils
{
	extern const int cDownArrowWidth;
	extern const int cMarginForCustomWindow;
	extern const int cSpacingBetweenIconAndText;

	static boost::shared_ptr<Gdiplus::Bitmap> nullImage;

    RECT getTextRect(const ConcreteToolbarItem * inItem, const RECT & inRect, size_t inIconWidth, size_t inIconHeight, SIZE inTextSize)
	{
		RECT result;
		result.left = inRect.left + inItem->getLeftMargin();;
		if (inIconWidth != 0)
		{
			result.left += inIconWidth + cSpacingBetweenIconAndText;
		}		 
		result.top = inRect.top + ((inRect.bottom - inRect.top) - inTextSize.cy)/2;
		result.right = inRect.right - inItem->getRightMargin();;
		result.bottom = result.top + inTextSize.cy;
		return result;
	}


	ConcreteToolbarItem::ConcreteToolbarItem
	(
		boost::weak_ptr<Toolbar> inToolbar,
		int inCommandID,
		const std::string & inText,
		const std::string & inTooltipText,
        boost::shared_ptr<Gdiplus::Bitmap> inImage,
		int inMenuId
	):
		mToolbar(inToolbar),
		mCommandId(inCommandID),
		mText(inText),
		mTooltipText(inTooltipText),
		mImage(inImage),
		mMenuId(inMenuId),
		mNoHover(false),
		mLeftMargin(4),
		mRightMargin(4),
		mMinimumWidth(0),
		mMaximumWidth(INT_MAX),
		mMaxIconHeight(18)
	{
	}


	ConcreteToolbarItem::~ConcreteToolbarItem()
	{
	}


	bool ConcreteToolbarItem::noHover() const
	{
		return mNoHover;
	}


	int ConcreteToolbarItem::getLeftMargin() const
	{
		return mLeftMargin;
	}


	void ConcreteToolbarItem::setLeftMargin(int inLeftMargin)
	{
		mLeftMargin = inLeftMargin;
	}


	void ConcreteToolbarItem::setRightMargin(int inRightMargin)
	{
		mRightMargin = inRightMargin;
	}


	int ConcreteToolbarItem::getRightMargin() const
	{
		return mRightMargin;
	}


	int ConcreteToolbarItem::minimumWidth() const
	{
		return mMinimumWidth;
	}


	void ConcreteToolbarItem::setMinimumWidth(int inMinimumWidth)
	{
		mMinimumWidth = inMinimumWidth;
	}

	
	int ConcreteToolbarItem::maximumWidth() const
	{
		return mMaximumWidth;
	}


	void ConcreteToolbarItem::setMaximumWidth(int inMaximumWidth)
	{
		mMaximumWidth = inMaximumWidth;
	}

	
	int ConcreteToolbarItem::maxIconHeight() const
	{
		return mMaxIconHeight;
	}

	
	void ConcreteToolbarItem::setMaxIconHeight(int inMaxIconHeight)
	{
		mMaxIconHeight = inMaxIconHeight;
	}


	void ConcreteToolbarItem::setNoHover(bool inNoHover)
	{
		mNoHover = inNoHover;
	}


	bool ConcreteToolbarItem::getIndex(size_t & outIndex) const
	{
		if (boost::shared_ptr<Toolbar> ieToolbar = mToolbar.lock())
		{
			for (size_t idx = 0; idx != ieToolbar->size(); ++idx)
			{
				if (ieToolbar->get(idx)->commandId() == commandId())
				{
					outIndex = idx;
					return true;
				}
			}
		}
		return false;
	}


	RECT ConcreteToolbarItem::getRect() const
	{
		RECT result = {0, 0, 0, 0};
		if (boost::shared_ptr<Toolbar> ieToolbar = mToolbar.lock())
		{
			size_t index;
			if (getIndex(index))
			{
				SendMessage(ieToolbar->handle(), TB_GETITEMRECT, (WPARAM)(INT)index, (LPARAM)&result);
			}
		}
		return result;
	}


	void ConcreteToolbarItem::setVisible(bool inVisible)
	{
		if (boost::shared_ptr<Toolbar> ieToolbar = mToolbar.lock())
		{
			SendMessage(ieToolbar->handle(), TB_HIDEBUTTON, (WPARAM)(INT)commandId(), (LPARAM)MAKELONG(inVisible ? FALSE : TRUE, 0));
		}
	}


	bool ConcreteToolbarItem::isVisible() const
	{
		if (boost::shared_ptr<Toolbar> ieToolbar = mToolbar.lock())
		{
			return 0 == SendMessage(ieToolbar->handle(), TB_ISBUTTONHIDDEN, (WPARAM)(INT)commandId(), (LPARAM)0);
		}
		return false;
	}


	int ConcreteToolbarItem::commandId() const
	{
		return mCommandId;
	}


	int ConcreteToolbarItem::menuId() const
	{
		return mMenuId;
	}


	const std::string & ConcreteToolbarItem::text() const
	{
		return mText;
	}


	const std::string & ConcreteToolbarItem::tooltipText() const
	{
		return mTooltipText;
	}


	void ConcreteToolbarItem::setText( const std::string & inText)
	{
		mText = inText;
		boost::shared_ptr<Toolbar> toolbar = mToolbar.lock();
		if (toolbar)
		{
			toolbar->rebuildLayout();

			// HACK!
			// This code forces the layout to update but does not belong here.
			RECT rc;
			GetClientRect(toolbar->handle(), &rc);
			for (size_t idx = 0; idx != toolbar->size(); ++idx)
			{
				toolbar->get(idx)->onClientRectChanged(rc);
			}
			// END HACK
		}
	}


	boost::shared_ptr<Gdiplus::Bitmap> ConcreteToolbarItem::image() const
	{
		return mImage;
	}


	void ConcreteToolbarItem::setImage(boost::shared_ptr<Gdiplus::Bitmap> inImage)
	{
		mImage = inImage;
		if (boost::shared_ptr<Toolbar> toolbar = mToolbar.lock())
		{
			toolbar->rebuildLayout();
		}
	}


	void ConcreteToolbarItem::draw(HDC inHDC, RECT rect, HFONT hFont, SIZE inTextSize)
	{
		size_t imageWidth = 0;
		size_t imageHeight = 0;
		if (mImage)
		{
			double resizeFactor = static_cast<double>(std::min<size_t>(maxIconHeight(), mImage->GetHeight()))/static_cast<double>(mImage->GetHeight());
			size_t h = static_cast<size_t>(static_cast<double>(mImage->GetHeight() * resizeFactor) + 0.5);
			size_t w = static_cast<size_t>(static_cast<double>(mImage->GetWidth() * resizeFactor) + 0.5);
			int x = rect.left + mLeftMargin;
			int y = rect.top + ((rect.bottom - rect.top) - h)/2;
			Gdiplus::Graphics g(inHDC);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.DrawImage
			(
				mImage.get(),
				x,
				y,
				w,
				h
			);
			imageWidth = w;
			imageHeight = h;
		}

		// draw text
        std::wstring unicodeText(Utils::ToUTF16(mText));
		RECT textRect = getTextRect(this, rect, imageWidth, imageHeight, inTextSize);
		int oldBkMode = ::SetBkMode(inHDC, TRANSPARENT);
		DWORD fgColor = GetSysColor(COLOR_MENUTEXT);
		int oldTextColor = ::SetTextColor(inHDC, fgColor);
		HGDIOBJ oldFont = SelectObject(inHDC, hFont);
		::DrawText(inHDC, unicodeText.c_str(), unicodeText.length(), &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		
		// Release stuff
		::SetTextColor(inHDC, oldTextColor);
		::SetBkMode(inHDC, oldBkMode);
		::SelectObject(inHDC, oldFont);
	}


	ToolbarButton::ToolbarButton
	(
		boost::weak_ptr<Toolbar> inToolbar,
		int inCommandID,
		const boost::function<void()> & inAction,
		const std::string & inText,
		const std::string & inTooltipText,
		boost::shared_ptr<Gdiplus::Bitmap> inImage
	):
		ConcreteToolbarItem
		(
			inToolbar,
			inCommandID,
			inText,
			inTooltipText,
			inImage,
			0
		),
		mAction(inAction)
	{
	}

	
	ToolbarButton::~ToolbarButton()
	{
	}


	int ToolbarButton::flags() const
	{
		return BTNS_BUTTON | (text().empty() ? 0 : BTNS_SHOWTEXT);
	}


	void ToolbarButton::performCommand()
	{
		if (mAction)
		{
			mAction();
		}
	}


	ToolbarDropDown::ToolbarDropDown
	(
		boost::weak_ptr<Toolbar> inToolbar,
		int inCommandID,
		const std::string & inText,
		const std::string & inTooltipText,
		boost::shared_ptr<Gdiplus::Bitmap> inImage,
		int inMenuId,
		bool inIsButton
	):
		ConcreteToolbarItem
		(
			inToolbar,
			inCommandID,
			inText,
			inTooltipText,
			inImage,
			inMenuId
		),
		mIsButton(inIsButton)
	{
		//if (mIsButton)
		//{
		//	setLeftMargin(2);
		//	setRightMargin(12);
		//}
	}
		
		
	ToolbarDropDown::~ToolbarDropDown()
	{
		//if (sTimerID_DisposeActiveDropDown)
		//{
		//	disposeActiveDropDown();
		//}
	}


	int ToolbarDropDown::flags() const
	{
		return BTNS_BUTTON | (isButton() ? BTNS_DROPDOWN : BTNS_WHOLEDROPDOWN) | (text().empty() ? 0 : BTNS_SHOWTEXT);
	}


	bool ToolbarDropDown::isButton() const
	{
		return mIsButton;
	}


	void ToolbarDropDown::showMenu()
	{
		if (boost::shared_ptr<Toolbar> toolbar = mToolbar.lock())
		{
			// Get the coordinates of the button.
			RECT rc;
			::SendMessage(toolbar->handle(), TB_GETRECT, (WPARAM)commandId(), (LPARAM)&rc);

			// Convert to screen coordinates.            
			MapWindowPoints(toolbar->handle(), HWND_DESKTOP, (LPPOINT)&rc, 2);
			
			boost::shared_ptr<PopupMenu> menu = getMenu();
			
			// Set up the popup menu.
			// Set rcExclude equal to the button rectangle so that if the toolbar 
			// is too close to the bottom of the screen, the menu will appear above 
			// the button rather than below it. 
			TPMPARAMS tpm;
			tpm.cbSize = sizeof(TPMPARAMS);
			tpm.rcExclude = rc;

			// Show the menu and wait for input. 
			// If the user selects an item, its WM_COMMAND is sent.
			toolbar->mActiveDropDown = this;

			//if (sTimerID_DisposeActiveDropDown)
			//{
			//	Utils::StopTimer(sTimerID_DisposeActiveDropDown);
			//}
			//TrackPopupMenuEx(menu->operator HMENU(), TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.left, rc.bottom, toolbar->handle(), &tpm);
			//sTimerID_DisposeActiveDropDown = Utils::StartTimer(boost::bind(&ToolbarDropDown::disposeActiveDropDown, this), 1);
		}
	}
	
	//int ToolbarDropDown::sTimerID_DisposeActiveDropDown = 0;
	//void ToolbarDropDown::disposeActiveDropDown()
	//{
	//	assert(sTimerID_DisposeActiveDropDown);
	//	if (boost::shared_ptr<Toolbar> toolbar = mToolbar.lock())
	//	{
	//		toolbar->setActiveDropDownToNull();
	//	}
	//	Utils::StopTimer(sTimerID_DisposeActiveDropDown);
	//	sTimerID_DisposeActiveDropDown = 0;
	//}


	ToolbarSeparator::ToolbarSeparator
	(
		boost::weak_ptr<Toolbar> inToolbar,
		int inCommandID
	):
		ConcreteToolbarItem
		(
			inToolbar,
			inCommandID,
			"",
			"",
			nullImage,
			0
		)
	{
		setNoHover(true);
	}


	ToolbarSeparator::~ToolbarSeparator()
	{
	}


	int ToolbarSeparator::flags() const
	{
		return BTNS_BUTTON;
	}


	void ToolbarSeparator::draw(HDC inHDC, RECT inRect, HFONT hFont, SIZE inTextSize)
	{
		Gdiplus::Graphics g(inHDC);
		Gdiplus::Pen pen(Gdiplus::Color::DarkGray, 1.0);
		const static int cMarginTopBottom = 2;
		int x1 = inRect.left + (inRect.right-inRect.left)/2;
		int x2 = x1;
		int y1 = inRect.top + cMarginTopBottom;
		int y2 = inRect.bottom - 2*cMarginTopBottom;
		g.DrawLine(&pen, x1, y1, x2, y2);
	}


	ToolbarSpring::ToolbarSpring
	(
		boost::weak_ptr<Toolbar> inToolbar,
		int inCommandID
	):
		ConcreteToolbarItem
		(
			inToolbar,
			inCommandID,
			"",
			"",
			nullImage,
			0
		)
	{
		setNoHover(true);
	}


	int ToolbarSpring::flags() const
	{
		return BTNS_SEP;
	}


	IECustomWindow::IECustomWindow()
	{
	}


	IECustomWindow::~IECustomWindow()
	{
	}


	int IECustomWindow::flags() const
	{
		return BTNS_SEP;
	}


} // namespace Utils
