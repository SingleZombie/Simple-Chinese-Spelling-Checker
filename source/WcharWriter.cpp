#include "WcharWriter.h"

using namespace Wchar;

WcharWriter::WcharWriter(const std::string& fileName)
{
	fopen_s(&_fp, fileName.c_str(), "wb");
	if (_fp)
	{
		putWchar(charToWchar(-2, -1));
	}
}
WcharWriter::~WcharWriter()
{
	if (_fp)
	{
		fclose(_fp);
	}
}
void WcharWriter::putWchar(wchar_t wch)
{
	fPutWchar(_fp, wch);
}
void WcharWriter::putStr(const std::wstring& str, wchar_t suffix)
{
	fPutStr(_fp, str);
	if (suffix)
	{
		putWchar(suffix);
	}
}