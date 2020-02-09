#include "WcharIO.h"

namespace Wchar
{
	inline wchar_t charToWchar(char ch1, char ch2)
	{
		return ((int)(unsigned char)ch1 << 8) + (int)(unsigned char)ch2;
	}
	inline std::pair<char, char> wcharToChar(wchar_t wch)
	{
		char ch1 = wch >> 8, ch2 = wch & ((1 << 8) - 1);
		return { ch1, ch2 };
	}

	int fGetWchar(FILE* file)
	{
		int ch1, ch2;
		ch1 = fgetc(file);
		if (ch1 == EOF)
		{
			return EOF;
		}
		ch2 = fgetc(file);
		if (ch2 == EOF)
		{
			return EOF;
		}
		return charToWchar(ch1, ch2);
	}

	void fPutWchar(FILE* file, wchar_t wch)
	{
		auto charPr = wcharToChar(wch);
		fputc(charPr.first, file);
		fputc(charPr.second, file);
	}

	int fGetStr(FILE* file, std::wstring& str, wchar_t sperator)
	{
		str.clear();

		int wch;
		while ((wch = fGetWchar(file)) != EOF)
		{
			if (wch == sperator)
			{
				break;
			}
			if (wch != L'\r')
			{
				str.push_back(wch);
			}
		}

		if (str.empty() && wch == EOF)
		{
			return EOF;
		}
		
		return 0;
	}
	int fGetInt(FILE* file, int& num, wchar_t sperator)
	{
		num = 0;
		int op = 1;

		bool flag = false;

		int wch, ch;
		while ((wch = fGetWchar(file)) != EOF)
		{
			if (wch == sperator)
			{
				break;
			}
			flag = true;

			ch = wcharToChar(wch).second;
			if (ch == '-')
			{
				op = -1;
			}
			else
			{
				num = num * 10 + ch - '0';
			}
		}
		num = op == 1 ? num : -num;

		if (!flag && wch == EOF)
		{
			return EOF;
		}

		return 0;
	}
	int fGetDouble(FILE* file, double& num, wchar_t sperator)
	{
		num = 0;
		double p10 = 1.0;
		bool dotFlag = false;
		int op = 1;

		bool flag = false;

		int wch, ch;
		while ((wch = fGetWchar(file)) != EOF)
		{
			if (wch == sperator)
			{
				break;
			}
			flag = true;

			ch = wcharToChar(wch).second;
			if (ch == '-')
			{
				op = -1;
			}
			else if (ch == '.')
			{
				dotFlag = true;
			}
			else
			{	
				if (dotFlag)
				{
					p10 /= 10.0;
					num = num + (ch - '0') * p10;
				}
				else
				{
					num = num * 10 + ch - '0';
				}
			}
		}
		num = op == 1 ? num : -num;

		if (!flag && wch == EOF)
		{
			return EOF;
		}
	}

	void fPutStr(FILE*  file, const std::wstring& str)
	{
		for (auto wch : str)
		{
			fPutWchar(file, wch);
		}
	}

	int stdGetWchar()
	{
		return fGetWchar(stdin);
	}
	void stdPutWchar(wchar_t wch)
	{
		fPutWchar(stdout, wch);
	}
	int stdGetStr(std::wstring& str, wchar_t seperator)
	{
		return fGetStr(stdin, str, seperator);
	}
	void stdPutStr(const std::wstring& str)
	{
		fPutStr(stdout, str);
	}

	std::vector<std::wstring> splitWstr(const std::wstring& str, wchar_t separator)
	{
		std::vector<std::wstring> result;
		size_t pos = 0, pPos = 0;
		while ((pos = str.find_first_of(separator, pPos)) != std::wstring::npos)
		{
			result.push_back(str.substr(pPos, pos - pPos));
			pPos = pos + 1;
		}
		pos = str.size() + 1;
		result.push_back(str.substr(pPos, pos - pPos));
		return result;
	}

	std::vector<std::wstring> splitWstr(const std::wstring& str, std::set<wchar_t> separatorSet)
	{
		unsigned len = str.size();
		std::vector<std::wstring> result;
		std::wstring cStr;
		size_t pos = 0, pPos = 0;
		for (; pos < len; pos++)
		{
			if (separatorSet.count(str[pos]))
			{
				result.push_back(str.substr(pPos, pos - pPos));
				pPos = pos + 1;
			}
		}
		pos = str.size() + 1;
		if (pos != pPos)
		{
			result.push_back(str.substr(pPos, pos - pPos));
		}
		return result;
	}
}

