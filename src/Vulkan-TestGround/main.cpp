#include <Windows.h>

#ifndef NDEBUG
int main()
{
    return 0;
}
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    return 0;
}
#endif