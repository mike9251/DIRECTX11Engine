#pragma once
#include "ErrorLogger.h"

class WindowContainer;//forward declaration to eliminate cyclic includes


class RenderWindow
{
public:
	bool Initialize(WindowContainer *pWindowContainner, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
	bool ProcessMessages();
	HWND GetHandle() const;
	~RenderWindow();

private:
	void RegisterWindowClass();
	HWND handle = NULL;
	HINSTANCE hInstance = NULL;
	std::string windowTitle = "";
	std::wstring windowTitleWide = L"";
	std::string windowClass = "";
	std::wstring windowClassWide = L"";
	int width = 0;
	int height = 0;
};