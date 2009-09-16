#include "FacebookConnection.h"
#include <algorithm>
#include <map>
#include <iostream>
#include <sstream>
#include <tchar.h>
#include "Poco/Bugcheck.h"
#include "Poco/Delegate.h"


// Todo
// - Poco Delegate instead of boost::bind may be nice as well
// - Tester with wxWidgets would be nice now :)
// - Implement remaining methods, and make tester for them -> this will reveal potential difficulties (photo upload params)
// - Make good sample application (friends explorer or something?)


using namespace Facebook;


HWND hWndCreateTokenResponse;
HWND hWndGetSessionResponse;
HWND hWndGetFriendsResponse;


class Tester
{
public:
	Tester():
		mConnection("959af3e7f629a891ca0b6342f4bfe1a2","c4a35cfe3b77e2ae538623aed8b70365"),
		mStarted(false)
	{
	}
	
		
	void createTokenSucceeded(const CreateTokenResponse & inResponse)
	{
		SendMessage(hWndCreateTokenResponse, WM_SETTEXT, 0, (LPARAM)Facebook::convertToUni16String("Create token: " + inResponse.token()).c_str());
		mConnection.login(inResponse.token());
		

		::DebugBreak(); // this stops the program from continuing before login
		mConnection.callMethod
		(
			GetSession
			(
				inResponse.token(),
				FbCallback<Tester, GetSession::ResponseType>(this, &Tester::getSessionSucceeded),
				FbCallback<Tester, ErrorResponse>(this, &Tester::getSessionFailed)
			)
		);
	}


	void createTokenFailed(const ErrorResponse & inErrorResponse)
	{
		MessageBox(0, Facebook::convertToUni16String(inErrorResponse.errorMessage()).c_str(), TEXT("CreateToken"), MB_OK);
	}
	
		
	void getSessionSucceeded(const GetSessionResponse & inResponse)
	{
		std::stringstream ss;
		ss << "uid: " << inResponse.uid()
			 << ", sessionKey: " << inResponse.sessionKey()
			 << ", secret: " << inResponse.secret()
			 << ", expires: " << inResponse.expires();
		SendMessage(hWndGetSessionResponse, WM_SETTEXT, 0, (LPARAM)Facebook::convertToUni16String(ss.str()).c_str());
		
		mConnection.callMethod
		(
			GetFriends
			(
				inResponse,
				FbCallback<Tester, GetFriends::ResponseType>(this, &Tester::getFriendsSucceeded),
				FbCallback<Tester, ErrorResponse>(this, &Tester::getFriendsFailed)
			)
		);
	}


	void getSessionFailed(const ErrorResponse & inErrorResponse)
	{
		MessageBox(0, Facebook::convertToUni16String(inErrorResponse.errorMessage()).c_str(), TEXT("GetSession"), MB_OK);
	}
	
	
	void getFriendsSucceeded(const GetFriendsResponse & inResponse)
	{
		std::stringstream ss;
		ss << "friends: ";
		std::vector<std::string>::const_iterator it = inResponse.uids().begin(), end = inResponse.uids().end();
		for(; it != end; ++it)
		{
			ss << *it << ", ";
		}
		SendMessage(hWndGetFriendsResponse, WM_SETTEXT, 0, (LPARAM)Facebook::convertToUni16String(ss.str()).c_str());
	}

	
	void getFriendsFailed(const ErrorResponse & inErrorResponse)
	{
		MessageBox(0, Facebook::convertToUni16String(inErrorResponse.errorMessage()).c_str(), TEXT("GetFriends"), MB_OK);
	}
	
	void start()
	{
		if(!mStarted)
		{
			mStarted = true;
			mConnection.callMethod
			(
				CreateToken
				(
					FbCallback<Tester, CreateToken::ResponseType>(this, &Tester::createTokenSucceeded),
					FbCallback<Tester, ErrorResponse>(this, &Tester::createTokenFailed)
				)
			);
		}
	}
private:
	bool mStarted;
	FacebookConnection mConnection;
};


Tester g_Tester;


LRESULT CALLBACK
WindowProc(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
void
CenterChildInParent( HWND inChild, HWND inParent )
{
	RECT rw_parent;
	GetWindowRect( inParent, &rw_parent );
	int rw_parent_w = rw_parent.right-rw_parent.left;
	int rw_parent_h = rw_parent.bottom-rw_parent.top;
	
	RECT rw_child;
	GetWindowRect( inChild, &rw_child );
	int rw_child_w = rw_child.right-rw_child.left;
	int rw_child_h = rw_child.bottom-rw_child.top;
	
	int new_child_x = rw_parent.left + (rw_parent_w - rw_child_w)/2;
	int new_child_y = rw_parent.top + (rw_parent_h - rw_child_h)/2;
	
	MoveWindow( inChild, new_child_x, new_child_y, rw_child_w, rw_child_h, TRUE );
}


INT_PTR WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
		WNDCLASSEX theWndClass;
		theWndClass.cbSize		= sizeof(WNDCLASSEX);
		theWndClass.style		 = 0;
		theWndClass.lpfnWndProc   = (WNDPROC)WindowProc;
		theWndClass.cbClsExtra	= 0;
		theWndClass.cbWndExtra	= 0;
		theWndClass.hInstance	 = hInstance;
		theWndClass.hIcon		 = 0;
		theWndClass.hCursor	   = LoadCursor(NULL, IDC_ARROW);
		theWndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
		theWndClass.lpszMenuName  = NULL;
		theWndClass.lpszClassName = _T("Tester");
		theWndClass.hIconSm	   = 0;
		if(!RegisterClassEx(&theWndClass))
		{
			MessageBox(NULL, _T("Window Registration Failed!"), _T("Error!"),
				MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
		HWND hWnd = CreateWindowEx(
			0,
			_T("Tester"), _T("Tester Window" ), 
			WS_OVERLAPPEDWINDOW | DS_CENTER,
			CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
			NULL, NULL, hInstance, NULL);
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)WindowProc);
	RECT rw;
	GetWindowRect( hWnd, &rw );
	RECT rc;
	GetClientRect(hWnd, &rc);
	MoveWindow( hWnd, rw.left, rw.top, 230, 240, TRUE );
	hWndCreateTokenResponse = CreateWindowEx(0,_T("STATIC"),_T("label1"),WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,0,0,300,24,hWnd,(HMENU)101,hInstance,0);
	hWndGetSessionResponse = CreateWindowEx(0,_T("STATIC"),_T("label2"),WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,0,30,300,24,hWnd,(HMENU)101,hInstance,0);
	hWndGetFriendsResponse = CreateWindowEx(0,_T("STATIC"),_T("label3"),WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,0,60,300,24,hWnd,(HMENU)101,hInstance,0);
	SendMessage( hWndCreateTokenResponse, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );
	SendMessage( hWndGetSessionResponse, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );
	SendMessage( hWndGetFriendsResponse, WM_SETFONT, (WPARAM)(HFONT)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0) );
	UpdateWindow( hWnd ); 
	ShowWindow(hWndCreateTokenResponse, SW_SHOW);
	ShowWindow( hWnd, SW_SHOW );

	MSG Msg;
	while( GetMessage(&Msg, NULL, 0, 0) > 0 )
	{
		HWND hActive = GetActiveWindow(); 
		if ( !IsWindow( hActive ) || !IsDialogMessage(hActive, &Msg) )
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

  return 0;
}


LRESULT CALLBACK
WindowProc(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam)
{
	switch(inMessage)
	{
		case WM_LBUTTONDOWN:
		{
			g_Tester.start();
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow( hWnd );
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		default:
		{
			return DefWindowProc(hWnd, inMessage, wParam, lParam);
		}
	}
	return 0;
}