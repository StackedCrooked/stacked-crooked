#include "Element.h"
#include "ElementFactory.h"
#include "WindowFactory.h"
#include <windows.h>


using namespace XULWin;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WindowFactory::Initialize(hInstance);
	ElementFactory::Initialize();

	

	ElementFactory::Finalize();
	WindowFactory::Finalize();	
	return 0;
}
