#ifndef TOOLBARITEM_H_INCLUDED
#define TOOLBARITEM_H_INCLUDED


#include "Utils/PopupMenu.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/utility.hpp>
#include <windows.h>
#include <gdiplus.h>


namespace Utils
{

	class Toolbar;


	class ToolbarItem : public boost::noncopyable
	{
	public:			
		friend class Toolbar;
		ToolbarItem
		(
			boost::weak_ptr<Toolbar> inToolbar,
			int inCommandID,
			const std::string & inText,
			const std::string & inTooltipText,
			boost::shared_ptr<Gdiplus::Bitmap> inImage,
			int inMenuId
		);
		
		virtual ~ToolbarItem();
		
		virtual int flags() const = 0;

		virtual void performCommand() = 0;

		bool noHover() const;

		void setNoHover(bool inNoHover);
		
		const std::string & text() const;

		const std::string & tooltipText() const;

		boost::shared_ptr<Gdiplus::Bitmap> image() const;
		
		int commandId() const;

		int menuId() const;

		void setText( const std::string & inText);

		void setImage(boost::shared_ptr<Gdiplus::Bitmap> inImage);

		int getLeftMargin() const;

		int getRightMargin() const;

		void setLeftMargin(int inLeftMargin);

		void setRightMargin(int inRightMargin);

		int minimumWidth() const;

		void setMinimumWidth(int inMinimumWidth);

		int maximumWidth() const;

		void setMaximumWidth(int inMaximumWidth);

		int maxIconHeight() const;

		void setMaxIconHeight(int inMaxIconHeight);

		// gets the position of this button in its parent toolbar
		// returns false if not found
		bool getIndex(size_t & outIndex) const;

		RECT getRect() const;

		bool isVisible() const;

		void setVisible(bool inVisible);

	protected:
		virtual void draw(HDC inHDC, RECT inRect, HFONT hFont, SIZE inTextSize);

		virtual void onClientRectChanged(const RECT & inNewClientRect) {}

		virtual void onPostRebuildLayout() {}

		boost::weak_ptr<Toolbar> mToolbar;
		
	private:
		int mCommandId;
		std::string mText;
		std::string mTooltipText;
		boost::shared_ptr<Gdiplus::Bitmap> mImage;
		int mMenuId;
		bool mNoHover;
		int mLeftMargin;
		int mRightMargin;
		int mMinimumWidth;
		int mMaximumWidth;
		int mMaxIconHeight;
	};
	
	class ToolbarButton : public ToolbarItem
	{
	public:
		ToolbarButton
		(
			boost::weak_ptr<Toolbar> inToolbar,
			int inCommandID,
			const boost::function<void()> & inAction,
			const std::string & inText,
			const std::string & inTooltipText,
			boost::shared_ptr<Gdiplus::Bitmap> inImage
		);

		virtual ~ToolbarButton();

		virtual int flags() const;

		virtual void performCommand();
	private:
		boost::function<void()> mAction;
	};
	
	class ToolbarDropDown : public ToolbarItem
	{
	public:
		ToolbarDropDown
		(
			boost::weak_ptr<Toolbar> inToolbar,
			int inCommandID,
			const std::string & inText,
			const std::string & inTooltipText,
			boost::shared_ptr<Gdiplus::Bitmap> inImage,
			int inMenuID,
			bool inIsButton
		);

		virtual ~ToolbarDropDown();

		virtual int flags() const;

		bool isButton() const;

		virtual void performCommand() {}

		virtual void performMenuCommand(int inMenuId) = 0;

		void showMenu();

	protected:
		virtual boost::shared_ptr<PopupMenu> getMenu() = 0;
		
	private:
		bool mIsButton;

		//void disposeActiveDropDown();
		//static int sTimerID_DisposeActiveDropDown;
	};
	
	// Only one spring can be added to the toolbar.
	class ToolbarSpring : public ToolbarItem
	{
	public:
		ToolbarSpring
		(
			boost::weak_ptr<Toolbar> inToolbar,
			int inCommandID // only serves as id, no command can be associated with it
		);

		virtual ~ToolbarSpring() {}

		virtual int flags() const;

		virtual void performCommand() {}
	};

	/**
	 * Interface for toolbar items that can
	 * have focus.
	 */
	class IFocusableToolbarItem
	{
	public:
		virtual ~IFocusableToolbarItem() {}

		virtual bool hasFocus() const = 0;

		virtual void setFocus() const = 0;
	};
	
	class IECustomWindow : public ToolbarItem,
						   public IFocusableToolbarItem
	{
	public:
		IECustomWindow
		(
			boost::weak_ptr<Toolbar> inToolbar,
			int inCommandID
		);

		virtual ~IECustomWindow();

		virtual int flags() const;

		virtual bool hasFocus() const = 0;

		virtual void setFocus() const = 0;

		virtual HWND handle() const = 0;

		virtual void performCommand() {}
	};
	
	class ToolbarSeparator : public ToolbarItem //IECustomWindow
	{
	public:
		ToolbarSeparator
		(
			boost::weak_ptr<Toolbar> inToolbar,
			int inCommandID // only serves as id, no command can be associated with it
		);

		virtual ~ToolbarSeparator();

		virtual int flags() const;

		virtual void performCommand() {}

	private:
		virtual void draw(HDC inHDC, RECT inRect, HFONT hFont, SIZE inTextSize);
	};

} // namespace Utils


#endif // TOOLBARITEM_H_INCLUDED
