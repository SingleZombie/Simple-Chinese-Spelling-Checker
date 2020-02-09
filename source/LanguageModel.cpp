#include "LanguageModel.h"
#include "NLPAlgorithm.h"
#include "WcharWriter.h"
#include "WcharLoader.h"

#include <ctime>
#include <cassert>

namespace NLP
{
	const std::set<wchar_t> NLP::LanguageModel::SEPARATOR_SET = 
	{
		L',', L'!', L'.', L';',L'?',L' ',
		L'，', L'。', L'！', L'；', L'？', L'「', L'」', L'…', L'~'
	};

	const double NLP::LanguageModel::CHARACTER_ERROR_P = 0.0003;

	LanguageModel::LanguageModel()
	{

	}
	void LanguageModel::insertDictionary(const std::string& fileName)
	{
		Wchar::WcharLoader loader(fileName);
		_dic.insertArticle(loader);
	}
	void LanguageModel::insertWordsToDictionary(const std::string& fileName)
	{
		Wchar::WcharLoader loader(fileName);
		std::wstring str;
		Word startMark(L"S", Word::WordType::x);
		while (loader.getStr(str, L'\n') != EOF)
		{
			_dic.insertWord(Word(str, Word::WordType::empty), startMark);
		}
	}

	void LanguageModel::insertCSPronunciation(const std::string& fileName)
	{
		Wchar::WcharLoader fin(fileName);
		std::wstring str;
		fin.getStr(str, L'\n');
		while (fin.getStr(str, L'\n') != EOF)
		{
			std::vector<std::wstring> substr = Wchar::splitWstr(str, ' ');
			wchar_t crtChar = substr[0][0];
			for (unsigned i = 0; i < substr.size(); i++)
			{
				for (auto ch : substr[i])
				{
					_cSet.insert(crtChar, ch, 1.0);
				}
			}
		}
	}
	void LanguageModel::insertCSShape(const std::string& fileName)
	{
		Wchar::WcharLoader fin(fileName);
		std::wstring str;
		while (fin.getStr(str, L'\n') != EOF)
		{
			wchar_t crtChar = str[0];
			for (unsigned i = 2; i < str.size(); i++)
			{
				_cSet.insert(crtChar, str[i], 1.0);
			}
		}
	}

	void LanguageModel::insertConfusionPairSet(const std::string& fileName)
	{
		Wchar::WcharLoader fin(fileName);
		std::wstring str1, str2;
		double cosValue;
		int cnt;
		while(fin.getStr(str1, L' ') != EOF)
		{
			fin.getStr(str2, L' ');
			fin.getDouble(cosValue, L' ');
			fin.getInt(cnt, L'\n');

			if (cosValue >= 0.2 && cnt >= 2)
			{
				unsigned len = str1.size();
				for (unsigned i = 0; i < len; i++)
				{
					if (str1[i] != str2[i])
					{
						_cSet.insertConfusionPair(str1[i], str2[i]);
						break;
					}
				}
			}
		}
		_cSet.calProbability();
	}

	double LanguageModel::calSentenceProbability(const std::wstring& str) const
	{
		double res = 1;
		std::wstring prevStr = L"S";
		std::wstring tmpStr;
		for (auto ch : str)
		{
			if (ch == L' ')
			{
				if (!tmpStr.empty())
				{
					res = res * _dic.getPrevWordOccurence(tmpStr, prevStr);
					prevStr = tmpStr;
				}
			}
		}
		if (!tmpStr.empty())
		{
			res = res * _dic.getPrevWordOccurence(tmpStr, prevStr);
			prevStr = tmpStr;
		}
		tmpStr = L"E";
		res = res * _dic.getPrevWordOccurence(tmpStr, prevStr);
		return res;
	}

	// 分词执行代码
	// 读取文件后调用NLP::languageModelSeparator
	void LanguageModel::cnSeparation(const std::string& inFile, const std::string& outFile) const
	{
		Wchar::WcharLoader fin(inFile);
		Wchar::WcharWriter fout(outFile);
		std::wstring str;
		while (fin.getStr(str, L'\n') != EOF)
		{
			std::vector<std::wstring> strArr = Wchar::splitWstr(str, SEPARATOR_SET);
			std::wstring result;
			unsigned clen = 0;
			for (auto strSeg : strArr)
			{
				if (!strSeg.empty())
				{
					result += NLP::languageModelSeparator(_dic, strSeg);
				}
				clen += strSeg.size();
				if (clen < str.size())
				{
					result.push_back(str[clen]);
				}
				clen++;
			}
			fout.putStr(result);
		}
	}

	// 分词和词性标注执行代码
	// 读取文件后调用NLP::languageModelSeparatorAndTagger
	void LanguageModel::cnWordTagging(const std::string& inFile, const std::string& outFile) const
	{
		Wchar::WcharLoader fin(inFile);
		Wchar::WcharWriter fout(outFile);
		std::wstring str;
		while (fin.getStr(str, L'\n') != EOF)
		{
			std::wstring result = NLP::languageModelSeparatorAndTagger(_dic, str);
			fout.putStr(result);
		}

	}
	// 中文拼写纠错执行代码
	// 读取文件后调用NLP::correctCSC
	void LanguageModel::cnSpellingChecker(const std::string& inFile, const std::string& outFile) const
	{
		Wchar::WcharLoader fin(inFile);
		Wchar::WcharWriter fout(outFile);
		std::wstring str;
		while (fin.getStr(str, L'\n') != EOF)
		{
			std::vector<std::wstring> strArr = Wchar::splitWstr(str, SEPARATOR_SET);
			std::wstring result;
			for (auto strSeg : strArr)
			{
				if (!strSeg.empty())
				{
					result += NLP::correctSentence(_dic, _cSet, strSeg);
				}
				if (result.size() < str.size())
				{
					result.push_back(str[result.size()]);
				}
			}
			fout.putStr(result);
		}
	}
	void LanguageModel::outputConfusionPair(const std::string& inFile, const std::string& outFile)
	{
		Wchar::WcharLoader fin(inFile);
		Wchar::WcharWriter fout(outFile);
		std::wstring str;
		while (fin.getStr(str, L'\n') != EOF)
		{
			std::vector<std::wstring> strArr = Wchar::splitWstr(str, SEPARATOR_SET);
			std::wstring result;
			unsigned clen = 0;
			for (auto strSeg : strArr)
			{
				if (!strSeg.empty())
				{
					result = NLP::languageModelSeparator(_dic, strSeg);
					std::vector<std::pair<std::wstring, std::wstring>> outputArr;
					std::vector<std::wstring> preWords;
					getConfusionPair(outputArr, preWords, result);
					for (unsigned i = 0; i < preWords.size(); i++)
					{
						fout.putStr(preWords[i], L' ');
						fout.putStr(outputArr[i].first, L' ');
						fout.putStr(outputArr[i].second, L'\n');
					}
				}
				
			}
			
		}
	}
	void LanguageModel::calPotentialConfusionPair(const std::string& inFile, const std::string& outFile)
	{
		// 假设字符串A被误写成字符串B，且它们前面是字符串C的个数
		// wordVector[A][B][C] = cnt
		std::unordered_map<std::wstring,
			std::unordered_map<std::wstring,
			std::unordered_map<std::wstring, unsigned> > > wordVector;

		// 从文本中提取（上一字符串，可能错误字符串，正确字符串）三元组
		Wchar::WcharLoader fin(inFile);
		std::wstring str;
		while (fin.getStr(str, L'\n') != EOF)
		{
			std::vector<std::wstring> strSeg = Wchar::splitWstr(str, L' ');
			assert(strSeg.size() == 3);
			std::wstring preWord = strSeg[0];
			wordVector[strSeg[2]][strSeg[1]][preWord]++;
		}

		Wchar::WcharWriter fout(outFile);
		char buf[100] = {};
		// 对每个正确字符串
		for (auto& pr1 : wordVector)
		{
			auto& correctStr = pr1.first;
			auto& correctStrMsg = _dic.getWordMsg(correctStr);

			// 获取该正确字符串在字典中的前一字符串向量
			auto& correctStrVec = correctStrMsg.getPrevStrs();
			double correctStrLen = correctStrMsg.getTotalCount();

			// 每个正确字符串的可能错误字符串
			for (auto& pr2 : pr1.second)
			{
				auto& errorStr = pr2.first;
				unsigned errorStrLen = 0;
				double cosValue = 0;

				// 枚举每个错误字符串前一字符串向量的分量
				for (auto& pr3 : pr2.second)
				{
					auto& prevStr = pr3.first;
					unsigned occurrence = pr3.second;
					errorStrLen += occurrence;

					// 若有相同字符串，说明向量维度相同，统计进cos值里
					if (correctStrVec.count(prevStr))
					{
						cosValue += occurrence * correctStrVec.at(prevStr);
					}
				}

				if (cosValue > 0)
				{
					// cos值除以向量长度
					cosValue /= correctStrLen * errorStrLen;
					sprintf_s(buf, sizeof(buf), "%.4lf %u\n", cosValue, errorStrLen);


					fout.putStr(correctStr, L' ');
					fout.putStr(errorStr, L' ');
					for (unsigned i = 0; buf[i]; i++)
					{
						fout.putWchar(Wchar::charToWchar('\0', buf[i]));
					}
				}
			}
		}
	}
}

