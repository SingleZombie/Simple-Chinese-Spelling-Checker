#ifndef WCHAR_WRITER_H
#define WCHAR_WRITER_H

#include "WcharIO.h"

// 用于从文件中读取宽字符
// 在析构时自动释放文件指针

namespace Wchar
{
	class WcharWriter
	{
	public:
		WcharWriter(const std::string& fileName);
		~WcharWriter();
		void putWchar(wchar_t wch);
		void putStr(const std::wstring& str, wchar_t suffix = L'\n');

	private:
		FILE* _fp;
	};
}

#endif