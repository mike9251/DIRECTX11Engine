#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	//ErrorLogger::Log(S_OK, "Test Message");
	/*HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to CoInitialize!\n");
		return -1;
	}*/

	Engine eng;
	eng.Initialize(hInstance, "Test Window", "RenderWindow", 1280, 720);
	while (eng.ProcessMessages())
	{
		eng.Update();
		eng.RenderFrame();
	}
	return 0;
}