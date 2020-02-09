#include "WcharLoader.h"

using namespace Wchar;

WcharLoader::WcharLoader(const std::string& fileName)
{
	fopen_s(&_fp, fileName.c_str(), "rb");
	if (_fp)
	{
		getWchar();
	}
}
WcharLoader::~WcharLoader()
{
	if (_fp)
	{
		fclose(_fp);
	}
}
int WcharLoader::getWchar()
{
	return fGetWchar(_fp);
}

int WcharLoader::getStr(std::wstring& str, wchar_t sperator)
{
	return fGetStr(_fp, str, sperator);
}

int WcharLoader::getDouble(double& num, wchar_t sperator)
{
	return fGetDouble(_fp, num, sperator);
}

int WcharLoader::getInt(int& num, wchar_t sperator)
{
	return fGetInt(_fp, num, sperator);
}