#include "ErrorLogger.h"
#include <comdef.h>

void ErrorLogger::Log(std::string msg)
{
	std::string error_message = "Error: " + msg;
	MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::string msg)
{
	_com_error error(hr);
	std::wstring error_message = L"Error: " + StringHelper::StringToWide(msg) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring msg)
{
	_com_error error(hr);
	std::wstring error_message = L"Error: " + msg + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(COMException & exception)
{
	std::wstring error_message = exception.what();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}
