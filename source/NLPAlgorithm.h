#ifndef NLP_ALGORITHM_H
#define NLP_ALGORITHM_H

#include "Dictionary.h"
#include "ConfusionSet.h"

namespace NLP
{
	// ������ģ�ͽ��зִ�
	std::wstring languageModelSeparator(const Dictionary& dic, const std::wstring& str);
	
	// ������ģ��ͬʱ���зִʺʹ��Ա�ע
	// kΪ�ִʸ������ܸ����еı���
	std::wstring languageModelSeparatorAndTagger(const Dictionary& dic, const std::wstring& str, double k = 0.5); 

	// ��������ƴд�������
	std::wstring correctSentence(const Dictionary& dic, const ConfusionSet& cSet, const std::wstring& str);

	// �����ַ�����ÿ���ո��λ��
	std::vector<int> getGapPos(const std::wstring& str);

	// �����ַ�����ÿ���ո��λ���Լ�ÿһ���ʵĴ���
	void getGapPosAndTag(const std::wstring& str, std::vector<int>& posArr, std::vector<NLP::Word::WordType>& typeArr);

	// ȥ��һ��ѵ�������еı�ע
	std::wstring deleteAns(const std::wstring& str);

	// �ִʽ�����۴���
	// ͨ���ȽϿո��λ��������P��R
	void cnSeparationEvaluator(const std::string& inFile, const std::string& inFile2);

	// �ִʺʹ��Ա�ע������۴���
	// ͨ���ȽϿո��λ�á���Ӧ����Ĵ���������P��R
	void cnTaggingEvaluator(const std::string& inFile, const std::string& inFile2);

	// ����ԭ���Ĵ����޸Ĺ��Ĵ����õ��޸ĵ�λ�ú��޸ĵ��ַ�
	void getModifiedSet(const std::wstring& ord, const std::wstring& res, std::vector<unsigned>& posSet, std::vector<wchar_t>& charSet);

	// ����ƴд�������۴���
	void cnSpellingDetectEvaluator(const std::string& sourceFile, const std::string& resFile, const std::string& ansFile);
	void cnSpellingCorrectEvaluator(const std::string& sourceFile, const std::string& resFile, const std::string& ansFile);
}

#endif