#ifndef ELEMENT_FACTORY_H_INCLUDED
#define ELEMENT_FACTORY_H_INCLUDED


#include "Element.h"
#include <string>
#include <map>
#include <windows.h>


namespace XULWin
{

	class ElementFactory
	{
	public:
		static void Initialize();

		static ElementFactory & Instance();

		static void Finalize();

		Element * create(Element * inParent, const Element::Type & inType, const Element::ID & inID);

		Element * get(const Element::ID & inID) const;

	private:
		static ElementFactory * sInstance;
		std::map<Element::ID, HWND> mHandles;
	};

} // XULWin


#endif // ELEMENT_FACTORY_H_INCLUDED
