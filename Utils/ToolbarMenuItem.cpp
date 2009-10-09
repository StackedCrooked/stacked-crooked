#include "ToolbarMenuItem.h"


namespace Utils
{


	ToolbarMenuItem::ToolbarMenuItem()
	{
	}


    ToolbarMenuItem::ToolbarMenuItem(int inId, const std::string & inText, const boost::function<void()> & inAction) :
        mAction(inAction)
	{
	}


} // namespace Utils
