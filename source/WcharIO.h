#ifndef WCHAR_IO_H
#define WCHAR_IO_H

#include <string>
#include <algorithm>
#include <vector>
#include <set>

// ÓÃÓÚ¶ÁÈ¡¿í×Ö·û

namespace Wchar
{
	inline wchar_t charToWchar(char ch1, char ch2);
	inline std::pair<char, char> wcharToChar(wchar_t wch);

	int fGetWchar(FILE* file);
	void fPutWchar(FILE* file, wchar_t  wch);

	int fGetStr(FILE* file, std::wstring& str, wchar_t sperator = L' ');
	int fGetInt(FILE* file, int& num, wchar_t sperator = L' ');
	int fGetDouble(FILE* file, double& num, wchar_t sperator = L' ');
	void fPutStr(FILE*  file, const std::wstring& str);

	inline int stdGetWchar();
	inline void stdPutWchar(wchar_t wch);
	inline int stdGetStr(std::wstring& str, wchar_t seperator = L' ');
	inline void stdPutStr(const std::wstring& str);

	std::vector<std::wstring> splitWstr(const std::wstring& str, wchar_t separator = L' ');
	std::vector<std::wstring> splitWstr(const std::wstring& str, std::set<wchar_t> separatorSet);
}

#endif

