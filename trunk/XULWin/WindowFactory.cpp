//#include "WindowFactory.h"
//#include <assert.h>
//
//
//namespace XULWin
//{
//
//	LPTSTR GetLastError(DWORD lastError)
//	{
//		LPVOID lpMsgBuf;
//		::FormatMessage
//		(
//			FORMAT_MESSAGE_ALLOCATE_BUFFER
//			| FORMAT_MESSAGE_FROM_SYSTEM
//			| FORMAT_MESSAGE_IGNORE_INSERTS,
//			NULL,
//			lastError,
//			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
//			(LPTSTR)&lpMsgBuf,
//			0,
//			NULL
//		);
//		return (LPTSTR)lpMsgBuf;
//	}
//
//	WindowFactory * WindowFactory::sInstance = 0;
//
//	
//	void WindowFactory::Initialize(HMODULE inModuleHandle)
//	{
//		assert (!sInstance);
//		sInstance = new WindowFactory(inModuleHandle);
//	}
//
//
//	WindowFactory & WindowFactory::Instance()
//	{
//		return *sInstance;
//	}
//
//
//	void WindowFactory::Finalize()
//	{
//		assert(sInstance);
//		delete sInstance;
//	}
//
//
//	WindowFactory::WindowFactory(HMODULE inModuleHandle) :
//		mModuleHandle(inModuleHandle)
//	{
//	}
//
//
//	void WindowFactory::registerType(const Element::Type & inType, const Params & inParams)
//	{
//		mParams.insert(std::make_pair(inType, inParams));
//	}
//	
//	
//	bool WindowFactory::get(const Element::Type & inType, WindowFactory::Params & outParams) const
//	{
//		bool result = false;
//		ParamsMapping::const_iterator it = mParams.find(inType);
//		if (it != mParams.end())
//		{
//			outParams = it->second;
//			result = true;
//		}
//		return result;
//	}
//
//} // namespace XULWin
