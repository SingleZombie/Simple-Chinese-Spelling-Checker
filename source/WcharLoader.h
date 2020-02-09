#ifndef WCHAR_LOADER_H
#define WCHAR_LOADER_H

#include <string>
#include "WcharIO.h"

// �������ļ���д����ַ�
// ������ʱ�Զ��ͷ��ļ�ָ��

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