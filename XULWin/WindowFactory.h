#ifndef WINDOW_FACTORY_H_INCLUDED
#define WINDOW_FACTORY_H_INCLUDED


#include "Element.h"
#include <string>
#include <map>
#include <windows.h>


namespace XULWin
{

	class WindowFactory
	{
	public:
		static void Initialize(HMODULE inModuleHandle);

		static WindowFactory & Instance();

		static void Finalize();

		HWND create(HWND inParent, const Element::Type & inType, const Element::ID & inID);

		HWND get(const Element::ID & inID) const;

	private:
		WindowFactory(HMODULE inModuleHandle);

		struct Params
		{
			DWORD dwExStyle;
			LPCWSTR lpClassName;
			DWORD dwStyle;
		};

		bool get(const Element::Type & inType, WindowFactory::Params & outParams) const;
		
				
		HMODULE mModuleHandle;
		std::map<Element::ID, HWND> mHandles;
		
		typedef std::map<Element::Type, Params> ParamsMapping;
		ParamsMapping mParams;

		static WindowFactory * sInstance;
	};

} // XULWin


#endif // WINDOW_FACTORY_H_INCLUDED
