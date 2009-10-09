#ifndef TOOLBARMENUITEM_H_INCLUDED
#define TOOLBARMENUITEM_H_INCLUDED


#include "PopupMenu.h"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>


namespace Utils
{
	
	class ToolbarMenuItem;
	
	typedef boost::shared_ptr<ToolbarMenuItem> TbMenuItemPtr;
	
    class ToolbarMenuItem : public PopupMenuItem
	{
		public:
			ToolbarMenuItem();
			
			ToolbarMenuItem(int inId, const std::string & inText, const boost::function<void()> & inAction);

			const boost::function<void()> action() const { return mAction; }

		private:
			boost::function<void()> mAction;
	};

} // namespace Utils


#endif // TOOLBARMENUITEM_H_INCLUDED
