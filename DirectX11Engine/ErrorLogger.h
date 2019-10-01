#pragma once
#include "COMException.h"
#include <Windows.h>

class ErrorLogger
{
public:
	static void Log(std::string msg);
	static void Log(HRESULT hr, std::string msg);
	static void Log(HRESULT hr, std::wstring msg);
	static void Log(COMException &exception);
};