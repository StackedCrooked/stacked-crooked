#ifndef TOOLBAR_H_INCLUDED
#define TOOLBAR_H_INCLUDED


#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <map>
#include <string>
#include <vector>
#include <windows.h>


namespace Utils
{

	class AbstractToolbarItem;
	class ToolbarDropDown;
	class ToolbarSpring;

    class Toolbar
	{		
		public:
			class EventHandler
			{
			public:
				virtual void onRequestFocus() = 0;
			};
			typedef std::vector<boost::shared_ptr<AbstractToolbarItem> > ToolbarItems;
			
			typedef std::map<HWND, RECT> CustomWindowPositions;

			Toolbar(EventHandler * inEventHandler, HMODULE inModuleHandle, HWND inParentWindow, RECT inRect, int inID);

			~Toolbar();
			

			void add(AbstractToolbarItem * inToolbarItem);

			size_t size() const;

			bool empty() const;

			void remove(size_t inIndex);

			void clear();

			void enable(size_t inIndex);

			void disable(size_t inIndex);

			const AbstractToolbarItem * get(size_t inIndex) const;

			AbstractToolbarItem * get(size_t inIndex);

			const AbstractToolbarItem * getToolbarItemByCommandId(int inCommandID) const;

			AbstractToolbarItem * getToolbarItemByCommandId(int inCommandID);


			HMODULE moduleHandle() const;

			HWND handle() const;

			bool hasFocus() const;

			void setFocus();

			void rebuildLayout();

			void buildToolbar();

		private:
			friend class ToolbarDropDown;

			static LRESULT CALLBACK ToolbarProc(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

			static LRESULT CALLBACK ParentProc(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

			static void buildToolbar(HWND inToolbarHandle, HFONT hFont, const ToolbarItems & inToolbarItems, CustomWindowPositions & outCustomWindowPositions);

			static void updateToolbarButtonSizes(HWND inToolbarHandle, HFONT hFont, const ToolbarItems & inToolbarItems);

			static void applySpring(HWND inToolbarHandle, const ToolbarItems & inToolbarItems, int inSpringID);

			static ToolbarItems::const_iterator findByCommandID(const ToolbarItems & inToolbarItems, int inCommandID);

			void setActiveDropDownToNull();

			EventHandler * mEventHandler;
			
			HMODULE mModuleHandle;
			HWND mParentWindow;
			HWND mHandle;
			HFONT mFont;
			int mID;
			ToolbarDropDown * mActiveDropDown;

			WNDPROC mParentProc;
			WNDPROC mToolbarProc;
			
			typedef std::map<Toolbar*, HWND> ParentMapping;
			static ParentMapping sInstancesParent;

			typedef std::map<Toolbar*, HWND> InstanceMapping;
			static InstanceMapping sInstances;

			ToolbarItems mToolbarItems;
			CustomWindowPositions mCustomWindowPositions;
			std::vector<boost::signals::connection> mConnections;
	};

}


#endif // TOOLBAR_H_INCLUDED
