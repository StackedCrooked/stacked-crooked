#include "Element.h"
#include "Window.h"
#include <windows.h>


using namespace XULWin;


LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam)
{
    switch (inMessage)
    {
        case WM_COMMAND:
        {
            break;
        }
        case WM_CLOSE:
        {
            ::PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
        {

            break;
        }
        default:
        {
            break;
        }

    }
    return DefWindowProc(hWnd, inMessage, wParam, lParam);
}


void registerTypes(HMODULE inModule)
{
    NativeWindow::Register(inModule, &MessageHandler);
}


void runSample()
{
    ElementPtr window(new Window(Element::ID("Test")));
    ElementPtr button1(new Button(window, Element::ID("btn1")));
    ElementPtr button2(new Button(window, Element::ID("btn2")));

    ::ShowWindow(window->nativeComponent()->handle(), SW_SHOW);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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
    return 0;
}
