#include "StringHelper.h"
#include <algorithm>

std::wstring StringHelper::StringToWide(std::string str)
{
	std::wstring wstr(str.begin(), str.end());
	return wstr;
}

std::string StringHelper::GetDirectoryFromPath(const std::string & filePath)
{
	size_t off1 = filePath.find_last_of('\\');
	size_t off2 = filePath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) //If no slash or backslash in path?
	{
		return "";
	}
	if (off1 == std::string::npos)
	{
		return filePath.substr(0, off2);
	}
	if (off2 == std::string::npos)
	{
		return filePath.substr(0, off1);
	}
	//If both exists, need to use the greater offset
	return filePath.substr(0, std::max(off1, off2));
}

std::string StringHelper::GetFileExtension(const std::string & fileName)
{
	size_t offset = fileName.find_last_of('.');
	if (offset == std::string::npos)
	{
		return {};
	}
	return fileName.substr(offset + 1);
}
