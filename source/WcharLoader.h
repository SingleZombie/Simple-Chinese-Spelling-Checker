#ifndef WCHAR_LOADER_H
#define WCHAR_LOADER_H

#include <string>
#include "WcharIO.h"

// 用于向文件中写入宽字符
// 在析构时自动释放文件指针

namespace Wchar
{
	class WcharLoader
	{
	public:
		WcharLoader(const std::string& fileName);
		~WcharLoader();
		int getWchar();

		int getStr(std::wstring& str, wchar_t sperator = L' ');
		int getDouble(double& num, wchar_t sperator = L' ');
		int getInt(int& num, wchar_t sperator = L' ');

	private:
		FILE* _fp;
	};

}



#endif