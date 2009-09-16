#include "Element.h"
#include "Window.h"
#include "ElementFactory.h"
#include "WindowFactory.h"
#include <windows.h>


using namespace XULWin;


LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, inMessage, wParam, lParam);
}


void registerTypes(HMODULE inModule)
{
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = 0;
	wndClass.lpfnWndProc = &MessageHandler;
	wndClass.cbClsExtra	= 0;
	wndClass.cbWndExtra	= 0;
	wndClass.hInstance	= inModule;
	wndClass.hIcon = 0;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = 0; // covered by content pane so no color needed (reduces flicker)
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("XULWin::Window");
	wndClass.hIconSm = 0;
	if (! RegisterClassEx(&wndClass))
	{
		throw std::runtime_error(std::string("Could not register Windows class."));
	}


	//WindowFactory::Instance().registerType(
	//	Element::Type("window"),
	//	WindowFactory::Params(
	//		0,
	//		TEXT("XULWin::NativeComponent"),
	//		WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW));

	//WindowFactory::Instance().registerType(
	//	Element::Type("button"),
	//	WindowFactory::Params(
	//		0,
	//		TEXT("BUTTON"),
	//		WS_TABSTOP | BS_PUSHBUTTON | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE));

	//WindowFactory::Instance().registerType(
	//	Element::Type("hbox"),
	//	WindowFactory::Params(
	//		0,
	//		TEXT("STATIC"),
	//		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE));
}


void runSample()
{
	Element * window = new Window(Element::ID("Test"));
	Element * btn1 = new Button(window, Element::ID("btn1"));

	::ShowWindow(window->window()->handle(), SW_SHOW);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ElementFactory::Initialize();

	registerTypes(hInstance);
	runSample();

	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		HWND hActive = GetActiveWindow();
		if (! IsDialogMessage(hActive, &message))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	ElementFactory::Finalize();
	return 0;
}
