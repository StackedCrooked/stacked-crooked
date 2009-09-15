#ifndef WINDOW_FACTORY_H_INCLUDED
#define WINDOW_FACTORY_H_INCLUDED


#include <string>
#include <map>


namespace XULWin
{

	class WindowFactory
	{
	public:
		static void Initialize();

		static WindowFactory & Instance();

		static void Finalize();

		HWND create(const std::string & inType) const;

		HWND get(const std::string & inID) const;

	private:
		static WindowFactory * sInstance;
		std::map<std::string, HWND> mHandles;
	};

} // XULWin


#endif // WINDOW_FACTORY_H_INCLUDED
