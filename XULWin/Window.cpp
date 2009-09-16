#include "Window.h"
#include <string>


namespace XULWin
{
	int CommandID::sID = 101; // start command IDs at 101 to avoid conflicts with Windows predefined values

	NativeComponent::NativeComponent(NativeComponent * inParent, CommandID inCommandID, LPCWSTR inClassName, DWORD inExStyle, DWORD inStyle) :
		mParent(inParent),
		mModuleHandle(::GetModuleHandle(0)),
		mCommandID(inCommandID)
	{
		int x = 100;
		int y = 100;
		int w = 200;
		int h = 300;

		if (mParent)
		{
			RECT rc;
			::GetClientRect(mParent->handle(), &rc);
			x = 0;
			y = 0;
			w = rc.right - rc.left;
			h = rc.bottom - rc.top;
		}
		else
		{
			x = (GetSystemMetrics(SM_CXSCREEN) - w)/2;
			y = (GetSystemMetrics(SM_CYSCREEN) - h)/2;
		}

		
		mHandle = ::CreateWindowEx
		(
			inExStyle, 
			inClassName,
			L"",			// title attribute can be set later
			inStyle,
			x, y, w, h,
			inParent ? inParent->handle() : 0,
			(HMENU)inCommandID.intValue(),
			mModuleHandle,
			0
		);
	}


	NativeComponent * NativeComponent::parent() const
	{
		return mParent;
	}


	HWND NativeComponent::handle() const
	{
		return mHandle;
	}

} // namespace XULWin
