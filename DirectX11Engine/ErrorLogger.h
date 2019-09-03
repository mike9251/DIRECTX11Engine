#pragma once
#include "StringConverter.h"
#include <Windows.h>

class ErrorLogger
{
public:
	static void Log(std::string msg);
	static void Log(HRESULT hr, std::string msg);
	static void Log(HRESULT hr, std::wstring msg);
};