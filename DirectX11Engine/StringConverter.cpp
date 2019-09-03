#include "StringConverter.h"

std::wstring StringConverter::StringToWide(std::string str)
{
	std::wstring wstr(str.begin(), str.end());
	return wstr;
}