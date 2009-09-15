#include "WindowFactory.h"
#include <assert.h>


namespace XULWin
{

	WindowFactory * WindowFactory::sInstance = 0;

	
	void WindowFactory::Initialize(HMODULE inModuleHandle)
	{
		assert (!sInstance);
		sInstance = new WindowFactory(inModuleHandle);
	}


	WindowFactory & WindowFactory::Instance()
	{
		return *sInstance;
	}


	void WindowFactory::Finalize()
	{
		assert(sInstance);
		delete sInstance;
	}


	WindowFactory::WindowFactory(HMODULE inModuleHandle) :
		mModuleHandle(inModuleHandle)
	{
	}

	
	HWND WindowFactory::create(HWND inParent, const Element::Type & inType, const Element::ID & inID)
	{
		static int fID = 101;
		Params p;
		if (!get(inType, p))
		{
			// no params found for this one
			return 0;
		}

		HWND handle = ::CreateWindowEx
		(
			p.dwExStyle, 
			p.lpClassName,
			L"",			// title attribute can be set later
			p.dwStyle,
			0, 0, 1, 1,		// coords can be set later
			inParent,
			(HMENU)fID++,
			mModuleHandle,
			0
		);
		mHandles.insert(std::make_pair(inID, handle));
		return handle;
	}

	
	HWND WindowFactory::get(const Element::ID & inID) const
	{
		return 0;
	}
	
	
	bool WindowFactory::get(const Element::Type & inType, WindowFactory::Params & outParams) const
	{
		bool result = false;
		ParamsMapping::const_iterator it = mParams.find(inType);
		if (it != mParams.end())
		{
			outParams = it->second;
			result = true;
		}
		return result;
	}

} // namespace XULWin
