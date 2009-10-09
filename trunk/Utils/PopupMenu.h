#ifndef POPUPMENU_H_INCLUDED
#define POPUPMENU_H_INCLUDED


#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <map>
#include <string>
#include <vector>
#include <windows.h>
#include <gdiplus.h>


namespace Utils
{	
	class PopupMenu;		
	typedef boost::shared_ptr<PopupMenu> MenuPtr;


	class PopupMenuItem;
	typedef boost::shared_ptr<PopupMenuItem> PopupMenuItemPtr;

	class PopupMenuItem : public boost::noncopyable
	{
		public:

			PopupMenuItem();

			PopupMenuItem(int inId, const std::string & inText);

			const static int SeparatorID;
			static boost::shared_ptr<PopupMenuItem> Separator;
				
			int id() const;
			
			const std::string & text() const;
			
			bool isEnabled() const;
			
			bool isChecked() const;
			
			void setEnabled(bool inEnabled);
			
			void setChecked(bool inChecked);
			
			void setBitmap(HBITMAP inBitmap);
			
			HBITMAP bitmap() const;
			
            void setImage(boost::shared_ptr<Gdiplus::Bitmap> inImage);
			
			boost::shared_ptr<Gdiplus::Bitmap> image() const;
		
		private:
			int mId;
			std::string mText;
			bool mEnabled;
			bool mChecked;
			HBITMAP mBitmap;				
			boost::shared_ptr<Gdiplus::Bitmap> mImage;
	};
	
	
	class PopupMenu : public boost::noncopyable
	{
		public:
			PopupMenu();
			
			virtual ~PopupMenu();
			
			operator HMENU() const;
			
			virtual void append(boost::shared_ptr<PopupMenuItem> inMenuItem);

			void append(const std::string & inText, const MenuPtr & inSubmenu);
		
		protected:
		
			virtual void updateInfo(const PopupMenuItem & inMenuItem, MENUITEMINFO & ioItemInfo);
			
		private:			
			HMENU mHandle;
			bool mOwnerDraw;
			int mSize;
			std::vector<MenuPtr> mSubmenus;
	};


} // namespace Utils


#endif // POPUPMENU_H_INCLUDED
