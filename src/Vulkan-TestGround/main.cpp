#include "Renderer.h"

#include <windows.h>

void StartEngine()
{
	Renderer engine;

	engine.init();	
	
	engine.run();	

	engine.cleanup();	
}

#ifndef NDEBUG
int main()
{
	StartEngine();
	return 0;
}
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	StartEngine();
	return 0;
}
#endif