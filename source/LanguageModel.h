#ifndef LANGUAGE_MODEL_H
#define LANGUAGE_MDOEL_H

#include "Dictionary.h"
#include "ConfusionSet.h"

namespace NLP
{
	class LanguageModel
	{
	public:
		LanguageModel();

		void insertDictionary(const std::string& fileName);
		void insertWordsToDictionary(const std::string& fileName);

		void insertCSPronunciation(const std::string& fileName);
		void insertCSShape(const std::string& fileName);

		void insertConfusionPairSet(const std::string& fileName);

		void calPotentialConfusionPair(const std::string& inFile, const std::string& outFile);

		// �ӷֺôʵľ����У��ҳ����ܵĻ�����
		// �������ֻ��һ����
		// �����������������ʽ��п���
		// ����滻�����е�ĳ���ֺ��������������ʱ����һ���ʵ����еĴ�
		// ��ô������־��Ǵ��֡��Ѹ�֮ǰ�͸�֮����ַ���ƴ��pair�������
		void getConfusionPair(
			std::vector<std::pair<std::wstring, std::wstring>>& outputArr,
			std::vector<std::wstring>& preWordArr,
			const std::wstring& sentenceWithBlank
		);

		// �������ļ��ִ�
		// ֮����getConfusionPair�õ�������
		// ���������
		void outputConfusionPair(const std::string& inFile, const std::string& outFile);

		// �����Ѿ��ֺôʵĴ��ĸ���
		double calSentenceProbability(const std::wstring& str) const;

		// �ִ�ִ�д���
		// ��ȡ�ļ������NLP::languageModelSeparator
		void cnSeparation(const std::string& inFile, const std::string& outFile) const;

		// �ִʺʹ��Ա�עִ�д���
		// ��ȡ�ļ������NLP::languageModelSeparatorAndTagger
		void cnWordTagging(const std::string& inFile, const std::string& outFile) const;

		// ����ƴд����ִ�д���
		// ��ȡ�ļ������NLP::correctCSC
		void cnSpellingChecker(const std::string& inFile, const std::string& outFile) const;

		static const std::set<wchar_t> SEPARATOR_SET;
		static const double CHARACTER_ERROR_P; // �������ֳ�����ʣ�������
	private:
		Dictionary _dic;
		ConfusionSet _cSet;
	};
}

#endif