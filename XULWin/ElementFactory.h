#ifndef ELEMENT_FACTORY_H_INCLUDED
#define ELEMENT_FACTORY_H_INCLUDED


#include <string>
#include <map>
#include <windows.h>


namespace XULWin
{
	class Element;

	class ElementFactory
	{
	public:
		static void Initialize();

		static ElementFactory & Instance();

		static void Finalize();

		Element * create(const std::string & inType, const std::string & inID);

		Element * get(const std::string & inID) const;

	private:
		static ElementFactory * sInstance;
		std::map<std::string, HWND> mHandles;
	};

} // XULWin


#endif // ELEMENT_FACTORY_H_INCLUDED
