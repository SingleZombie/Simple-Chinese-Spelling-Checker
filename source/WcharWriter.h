#ifndef WCHAR_WRITER_H
#define WCHAR_WRITER_H

#include "WcharIO.h"

// ���ڴ��ļ��ж�ȡ���ַ�
// ������ʱ�Զ��ͷ��ļ�ָ��

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