#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED


#include <windows.h>


namespace XULWin
{
	class CommandID
	{
	public:
		CommandID() : mID(sID++) {}

		CommandID(int inID) : mID(inID) {}

		int intValue() const { return mID; }

	private:
		int mID;
		static int sID;
	};


	/**
	 * NativeComponent is base class for all native UI elements.
	 */
	class NativeComponent
	{
	public:
		NativeComponent(NativeComponent * inParent, CommandID inCommandID, LPCWSTR inClassName, DWORD inExStyle, DWORD inStyle);

		NativeComponent * parent() const;

		HWND handle() const;		

	private:
		NativeComponent * mParent;
		CommandID mCommandID;
		HMODULE mModuleHandle;
		HWND mHandle;
	};


	class NativeWindow : public NativeComponent
	{
	public:
		NativeWindow() :
			NativeComponent(
				0,
				CommandID(0), // command id is hmenu id for Window
				TEXT("XULWin::Window"),
				0, // exStyle
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
			)
		{
		}
	};


	class NativeButton : public NativeComponent
	{
	public:
		NativeButton(NativeComponent * inParent) :
			NativeComponent(
				inParent,
				CommandID(),
				TEXT("BUTTON"),
				0, // exStyle
				WS_TABSTOP | BS_PUSHBUTTON | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
			)
		{
		}
	};

} // namespace XULWin


#endif // WINDOW_H_INCLUDED
