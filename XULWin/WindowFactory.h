#ifndef WINDOW_FACTORY_H_INCLUDED
#define WINDOW_FACTORY_H_INCLUDED


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

		HWND create(HWND inParent, const std::string & inType);

		HWND get(const std::string & inID) const;

	private:
		WindowFactory(HMODULE inModuleHandle);

		struct Params
		{
			DWORD dwExStyle;
			LPCWSTR lpClassName;
			DWORD dwStyle;
		};

		bool get(const std::string & inType, WindowFactory::Params & outParams) const;
		
				
		HMODULE mModuleHandle;
		std::map<std::string, HWND> mHandles;
		
		typedef std::map<std::string, Params> ParamsMapping;
		ParamsMapping mParams;

		static WindowFactory * sInstance;
	};

} // XULWin


#endif // WINDOW_FACTORY_H_INCLUDED
