#pragma once
#include <comdef.h>
#include "StringHelper.h"

#define COM_ERROR_IF_FAILED(hr, msg) if(FAILED(hr)) throw COMException(hr, msg, __FILE__, __FUNCTION__, __LINE__)

class COMException
{
public:
	COMException(HRESULT hr, const std::string &msg, const std::string &file, const std::string &func, int line)
	{
		_com_error error(hr);
		whatmsg += L"Message: " + StringHelper::StringToWide(msg) + L"\n";
		whatmsg += error.ErrorMessage();
		whatmsg += L"\nFile: " + StringHelper::StringToWide(file);
		whatmsg += L"\nFunction: " + StringHelper::StringToWide(func);
		whatmsg += L"\nLine: " + StringHelper::StringToWide(std::to_string(line));
	}

	const wchar_t * what()
	{
		return this->whatmsg.c_str();
	}

private:
	std::wstring whatmsg;
};