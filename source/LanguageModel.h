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

		// 从分好词的句子中，找出可能的混淆对
		// 假设错误只是一个字
		// 对相邻两个或三个词进行考虑
		// 如果替换了其中的某个字后，这两个、三个词变成了一个词典中有的词
		// 那么，这个字就是错字。把改之前和改之后的字符串拼成pair加入输出
		void getConfusionPair(
			std::vector<std::pair<std::wstring, std::wstring>>& outputArr,
			std::vector<std::wstring>& preWordArr,
			const std::wstring& sentenceWithBlank
		);

		// 把输入文件分词
		// 之后用getConfusionPair得到混淆对
		// 输出混淆对
		void outputConfusionPair(const std::string& inFile, const std::string& outFile);

		// 计算已经分好词的串的概率
		double calSentenceProbability(const std::wstring& str) const;

		// 分词执行代码
		// 读取文件后调用NLP::languageModelSeparator
		void cnSeparation(const std::string& inFile, const std::string& outFile) const;

		// 分词和词性标注执行代码
		// 读取文件后调用NLP::languageModelSeparatorAndTagger
		void cnWordTagging(const std::string& inFile, const std::string& outFile) const;

		// 中文拼写纠错执行代码
		// 读取文件后调用NLP::correctCSC
		void cnSpellingChecker(const std::string& inFile, const std::string& outFile) const;

		static const std::set<wchar_t> SEPARATOR_SET;
		static const double CHARACTER_ERROR_P; // 单个汉字出错概率，超参数
	private:
		Dictionary _dic;
		ConfusionSet _cSet;
	};
}

#endif