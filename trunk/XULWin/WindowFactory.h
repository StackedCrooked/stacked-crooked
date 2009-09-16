//#ifndef WINDOW_FACTORY_H_INCLUDED
//#define WINDOW_FACTORY_H_INCLUDED
//
//
//#include "Element.h"
//#include <string>
//#include <map>
//#include <windows.h>
//
//
//namespace XULWin
//{
//
//	class WindowFactory
//	{
//	public:
//		static void Initialize(HMODULE inModuleHandle);
//
//		static WindowFactory & Instance();
//
//		static void Finalize();
//
//		NativeComponent * create(NativeComponent * inParent, const Element::Type & inType);
//
//		struct Params
//		{
//			Params() :
//				dwExStyle(0), lpClassName(TEXT("")), dwStyle(0) {}
//
//			Params(DWORD i_dwExStyle, LPCWSTR i_lpClassName, DWORD i_dwStyle) :
//				dwExStyle(i_dwExStyle), lpClassName(i_lpClassName), dwStyle(i_dwStyle) {}
//
//			DWORD dwExStyle;
//			LPCWSTR lpClassName;
//			DWORD dwStyle;
//		};
//
//		void registerType(const Element::Type & inType, const Params & inParams);
//
//	private:
//		WindowFactory(HMODULE inModuleHandle);
//
//		bool get(const Element::Type & inType, WindowFactory::Params & outParams) const;
//		
//				
//		HMODULE mModuleHandle;		
//		typedef std::map<Element::Type, Params> ParamsMapping;
//		ParamsMapping mParams;
//
//		static WindowFactory * sInstance;
//	};
//
//} // XULWin
//
//
//#endif // WINDOW_FACTORY_H_INCLUDED
