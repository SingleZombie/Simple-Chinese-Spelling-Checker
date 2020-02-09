#ifndef NLP_ALGORITHM_H
#define NLP_ALGORITHM_H

#include "Dictionary.h"
#include "ConfusionSet.h"

namespace NLP
{
	// 用语言模型进行分词
	std::wstring languageModelSeparator(const Dictionary& dic, const std::wstring& str);
	
	// 用语言模型同时进行分词和词性标注
	// k为分词概率在总概率中的比例
	std::wstring languageModelSeparatorAndTagger(const Dictionary& dic, const std::wstring& str, double k = 0.5); 

	// 进行中文拼写错误矫正
	std::wstring correctSentence(const Dictionary& dic, const ConfusionSet& cSet, const std::wstring& str);

	// 计算字符串中每个空格的位置
	std::vector<int> getGapPos(const std::wstring& str);

	// 计算字符串中每个空格的位置以及每一个词的词性
	void getGapPosAndTag(const std::wstring& str, std::vector<int>& posArr, std::vector<NLP::Word::WordType>& typeArr);

	// 去掉一段训练数据中的标注
	std::wstring deleteAns(const std::wstring& str);

	// 分词结果评价代码
	// 通过比较空格的位置来计算P、R
	void cnSeparationEvaluator(const std::string& inFile, const std::string& inFile2);

	// 分词和词性标注结果评价代码
	// 通过比较空格的位置、对应词语的词性来计算P、R
	void cnTaggingEvaluator(const std::string& inFile, const std::string& inFile2);

	// 根据原来的串和修改过的串，得到修改的位置和修改的字符
	void getModifiedSet(const std::wstring& ord, const std::wstring& res, std::vector<unsigned>& posSet, std::vector<wchar_t>& charSet);

	// 中文拼写纠错评价代码
	void cnSpellingDetectEvaluator(const std::string& sourceFile, const std::string& resFile, const std::string& ansFile);
	void cnSpellingCorrectEvaluator(const std::string& sourceFile, const std::string& resFile, const std::string& ansFile);
}

#endif