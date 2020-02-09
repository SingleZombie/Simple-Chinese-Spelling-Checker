#include "NLPAlgorithm.h"
#include <algorithm>
#include <cassert>
using namespace NLP;

std::wstring NLP::languageModelSeparator(const Dictionary& dic, const std::wstring& str2)
{
	std::wstring str = L'S' + str2 + L'E';
	int len = str.length();

	std::vector<std::vector<std::pair<unsigned, unsigned>>> prevState(len); // 当前状态是由哪个最优上一状态转移
	std::vector<std::vector<double>> dp(len); // 当前状态的最大概率
	for (int i = 0; i < len; i++)
	{
		dp[i] = std::vector<double>(Dictionary::MAX_LENGTH + 1);
		prevState[i] = std::vector < std::pair<unsigned, unsigned> >(Dictionary::MAX_LENGTH + 1);
	}


	// main process
	{
		// str[1 ~ len - 2] = str2, str[0] = 'S', str[len - 1] = 'E'

		dp[0][1] = 1.0;
		for (int i = 1; i <= len - 1; i++)
		{
			for (int j = 1; j <= Dictionary::MAX_LENGTH && i - j >= 0; j++)
			{
				double tmp = dic.getPrevWordOccurence(str.substr(i + 1 - j, j), str.substr(i - j, 1));
				
				dp[i][j] = dp[i - j][1] * tmp;
				prevState[i][j] = { i - j, 1 };

				if (tmp == 0) // 子串无法分词，直接跳过
				{
					continue;
				}

				for (int k = 2; k <= Dictionary::MAX_LENGTH && i - j + 1 - k >= 0; k++)
				{
					double tmp = dp[i - j][k] * dic.getPrevWordOccurence(str.substr(i + 1 - j, j), str.substr(i - j + 1 - k, k));
					if (tmp > dp[i][j])
					{
						dp[i][j] = tmp;
						prevState[i][j] = { i - j, k };
					}
				}

				if (i == len - 1) // str[len - 1] = 'E'
				{
					break;
				}
			}
		}

		
	}
	// 逆序得到分词结果
	std::wstring res;
	std::pair<unsigned, unsigned> crtState = prevState[len - 1][1];
	while (crtState.first > 0)
	{
		for (int i = 0; i < crtState.second; i++)
		{
			res.push_back(str[crtState.first - i]);
		}
		res.push_back(L' ');
		crtState = prevState[crtState.first][crtState.second];
	}
	res.pop_back();
	std::reverse(res.begin(), res.end());

	return res;
}

std::wstring NLP::languageModelSeparatorAndTagger(const Dictionary& dic, const std::wstring& str2, double fac)
{
	std::wstring str = L'S' + str2 + L'E';
	int len = str.length();

	std::vector<std::vector<std::pair<unsigned, unsigned>>> prevState(len); // 当前状态是由哪个最优上一状态转移
	std::vector<std::vector<double>> dp(len); // 当前状态的最大概率
	std::vector<std::vector<Word::WordType>> bestType(len); // 当前状态的最优词性
	for (int i = 0; i < len; i++)
	{
		dp[i] = std::vector<double>(Dictionary::MAX_LENGTH + 1);
		prevState[i] = std::vector < std::pair<unsigned, unsigned> >(Dictionary::MAX_LENGTH + 1);
		bestType[i] = std::vector<Word::WordType>(Dictionary::MAX_LENGTH + 1);
	}

	// main process
	{
		// str[1 ~ len - 2] = str2, str[0] = 'S', str[len - 1] = 'E'

		dp[0][1] = 1.0;
		bestType[0][1] = Word::WordType::x;
		for (int i = 1; i <= len - 1; i++)
		{
			for (int j = 1; j <= Dictionary::MAX_LENGTH && i - j >= 0; j++)
			{
				std::vector<Word::WordType> validType; // 当前子串可能的类型
				double dp1 = dic.getPrevWordOccurence(str.substr(i + 1 - j, j), str.substr(i - j, 1));	// 分词概率
				double dp2 = -1; // 词性概率
				Word::WordType cType = Word::WordType::empty; // 当前状态最优词性

				if (dp1 != 0) // 如果当前子串能够计算分词概率
				{
					if (!dic.haveWord(str.substr(i + 1 - j, j))) // 如果子串不是字典中的词，默认它为名词
					{
						validType.push_back(Word::WordType::x);
					}
					else // 否则从字典中查找子串可能的词性
					{
						const auto& msg = dic.getWordMsg(str.substr(i + 1 - j, j));
						const auto& wordTypeMap = msg.getTypeCountMap();
						for (auto& pr : wordTypeMap)
						{
							validType.push_back(pr.first);
						}
					}

					// 默认上一词长度为1，求此时的最优词性
					for (auto type : validType)
					{
						double tmp = dic.getPrevTypeOccurence(type, bestType[i - j][1]);
						if (tmp > dp2)
						{
							dp2 = tmp;
							cType = type;
						}
					}
				}
				
				// 默认词为上一词时，设置当前状态的值
				dp[i][j] = dp[i - j][1] * (fac * dp1 + (1 - fac) * dp2); // 概率由分词概率和词性概率综合而成
				prevState[i][j] = { i - j, 1 };
				bestType[i][j] = cType;

				if (dp1 == 0) // 子串无法分词，直接跳过
				{
					continue;
				}

				for (int k = 2; k <= Dictionary::MAX_LENGTH && i - j + 1 - k >= 0; k++)
				{
					dp1 = dic.getPrevWordOccurence(str.substr(i + 1 - j, j), str.substr(i - j + 1 - k, k));

					if (dp1 != 0) // 如果当前子串能够计算分词概率
					{
						// 从字典中查找子串可能的词性，找到当前上一词下这一个词的最优词性
						dp2 = -1;
						cType = Word::WordType::empty;
						for (auto type : validType)
						{
							double tmp = dic.getPrevTypeOccurence(type, bestType[i - j][k]);
							if (tmp > dp2)
							{
								dp2 = tmp;
								cType = type;
							}
						}

						double tmp = dp[i - j][k] * (fac * dp1 + (1 - fac) * dp2);
						if (tmp > dp[i][j])
						{
							dp[i][j] = tmp;
							prevState[i][j] = { i - j, k };
							bestType[i][j] = cType;
						}
					}
				}

				if (i == len - 1) // str[len - 1] = 'E'
				{
					break;
				}
			}
		}


	}
	// 逆序得到分词结果
	std::wstring res;
	std::pair<unsigned, unsigned> crtState = prevState[len - 1][1];
	while (crtState.first > 0)
	{
		auto typeStr = (Word::wordTypeToStrMap.at(bestType[crtState.first][crtState.second]));
		for (auto it = typeStr.rbegin(); it != typeStr.rend(); it++)
		{
			res.push_back(*it);
		}
		res.push_back(L'/');
		
		for (int i = 0; i < crtState.second; i++)
		{
			res.push_back(str[crtState.first - i]);
		}
		res.push_back(L' ');
		crtState = prevState[crtState.first][crtState.second];
	}
	res.pop_back();
	std::reverse(res.begin(), res.end());

	return res;
}

// 计算字符串中每个空格的位置
std::vector<int> NLP::getGapPos(const std::wstring& str)
{
	std::vector<int> res;
	int pos = 0;
	int len = str.size();
	for (int i = 0; i < len; i++)
	{
		if (str[i] == L' ')
		{
			if (i > 0 && str[i - 1] == L' ')
			{
				continue;
			}
			if (pos != 0 && (!(!res.empty() && pos == res.back())))
			{
				res.push_back(pos);
			}
		}
		else
		{
			pos++;
		}
	}
	return res;
}

// 计算字符串中每个空格的位置以及每一个词的词性
void NLP::getGapPosAndTag(const std::wstring& str, std::vector<int>& posArr, std::vector<NLP::Word::WordType>& typeArr)
{
	posArr.clear();
	typeArr.clear();

	int pos = 0;
	int len = str.size();
	bool loadType = false;
	bool loadBracket = false;
	std::wstring typeStr;
	for (int i = 0; i < len; i++)
	{
		if (str[i] == L' ')
		{
			if (i > 0 && str[i - 1] == L' ')
			{
				continue;
			}
			if (pos != 0 && (!(!posArr.empty() && pos == posArr.back())))
			{
				posArr.push_back(pos);
				typeArr.push_back(NLP::Word::strToWordTypeMap.at(typeStr));
			}
			typeStr.clear();
			loadType = false;
			loadBracket = false;

		}
		else if (str[i] == L'/')
		{
			loadType = true;
		}
		else if (loadType)
		{
			if (str[i] == L']')
			{
				loadBracket = true;
			}
			if (!loadBracket)
			{
				typeStr.push_back(str[i]);
			}
		}
		else if (str[i] == L'[')
		{
			continue;
		}
		else if (str[i] == L']')
		{
			loadBracket = true;
		}
		else
		{
			pos++;
		}
	}
	if (loadType)
	{
		typeArr.push_back(NLP::Word::strToWordTypeMap.at(typeStr));
		typeStr.clear();
		loadType = false;
	}
}

// 去掉一段训练数据中的标注
std::wstring NLP::deleteAns(const std::wstring& str)
{
	std::wstring res;

	int len = str.size();
	bool loadType = false;
	bool loadBracket = false;
	for (int i = 0; i < len; i++)
	{
		if (str[i] == L' ')
		{
			if (i > 0 && str[i - 1] == L' ')
			{
				continue;
			}
			if (loadType)
			{
				loadType = false;
				loadBracket = false;
			}

		}
		else if (str[i] == L'/')
		{
			loadType = true;
		}
		else if (loadType)
		{
			continue;
		}
		else if (str[i] == L'[')
		{
			continue;
		}
		else if (str[i] == L']')
		{
			loadBracket = true;
		}
		else
		{
			res.push_back(str[i]);
		}
	}
	return res;
}

// 分词结果评价代码
// 通过比较空格的位置来计算P、R
void NLP::cnSeparationEvaluator(const std::string& inFile, const std::string& inFile2)
{
	Wchar::WcharLoader fin(inFile);
	Wchar::WcharLoader fin2(inFile2);
	std::wstring str1, str2;
	std::vector<int> v1, v2;

	double totalCnt = 0;
	double myCnt = 0;
	double correctCnt = 0;

	while (fin.getStr(str1, L'\n') != EOF)
	{
		fin2.getStr(str2, L'\n');
		v1 = getGapPos(str1);
		v2 = getGapPos(str2);

		totalCnt += v2.size();
		myCnt += v1.size();
		int j = 0;
		for (int i = 0; i < v2.size(); i++)
		{
			while (j < v1.size() && v1[j] < v2[i])
			{
				j++;
			}
			correctCnt += v1[j] == v2[i];
		}
	}
	double P = correctCnt / myCnt;
	double R = correctCnt / totalCnt;
	printf("P = %.4lf%%, R = %.4lf%%, F1 = %.4lf%%\n", P * 100, R * 100, 200 * P * R / (P + R));
}

// 分词和词性标注结果评价代码
// 通过比较空格的位置、对应词语的词性来计算P、R
void NLP::cnTaggingEvaluator(const std::string& inFile, const std::string& inFile2)
{
	Wchar::WcharLoader fin(inFile);
	Wchar::WcharLoader fin2(inFile2);

	std::wstring str1, str2;
	std::vector<int> v1, v2;
	std::vector<NLP::Word::WordType> t1, t2;
	unsigned lineCnt = 0;

	double totalCnt[2] = {};
	double myCnt[2] = {};
	double correctCnt[2] = {};

	while (fin.getStr(str1, L'\n') != EOF)
	{
		fin2.getStr(str2, L'\n');
		getGapPosAndTag(str1, v1, t1);
		getGapPosAndTag(str2, v2, t2);

		totalCnt[0] += v2.size();
		totalCnt[1] += t2.size();
		myCnt[0] += v1.size();
		myCnt[1] += t1.size();
		int j = 0;
		for (int i = 0; i < v2.size() && j < v1.size(); i++)
		{
			while (j < v1.size() && v1[j] < v2[i])
			{
				j++;
			}
			if (j < v1.size() && v1[j] == v2[i])
			{
				correctCnt[0]++;
				correctCnt[1] += t1[j] == t2[i];
			}
		}
		if (!t1.empty() && !t2.empty())
		{
			correctCnt[1] += t1.back() == t2.back();
			lineCnt++;
		}
	}
	double P = correctCnt[0] / myCnt[0];
	double R = correctCnt[0] / totalCnt[0];
	printf("Sperator: P = %.4lf%%, R = %.4lf%%, F1 = %.4lf%%\n", P * 100, R * 100, 200 * P * R / (P + R));
	if (myCnt[1] == 0 || totalCnt[1] == 0)
	{
		P = R = 0;
	}
	else
	{
		P = correctCnt[1] / myCnt[1];
		R = correctCnt[1] / totalCnt[1];
	}
	printf("Tagger: P = %.4lf%%, R = %.4lf%%, F1 = %.4lf%%\n", P * 100, R * 100, 200 * P * R / (P + R));
	printf("Accurancy of tagger when sperator is correct: %.4lf%%\n", 100 * correctCnt[1] / (correctCnt[0] + lineCnt));
}

// 
void NLP::getModifiedSet(const std::wstring& ord, const std::wstring& res, std::vector<unsigned>& posSet, std::vector<wchar_t>& charSet)
{
	posSet.clear();
	charSet.clear();
	size_t len = ord.size();
	assert(ord.size() == res.size());
	for (unsigned i = 0; i < len; i++)
	{
		if (ord[i] != res[i])
		{
			posSet.push_back(i);
			charSet.push_back(res[i]);
		}
	}
}

void NLP::cnSpellingDetectEvaluator(const std::string& sourceFile, const std::string& resFile, const std::string& ansFile)
{
	Wchar::WcharLoader finSrc(sourceFile);
	Wchar::WcharLoader finRes(resFile);
	Wchar::WcharLoader finAns(ansFile);

	unsigned totalCnt = 0;
	unsigned withoutErrorCnt = 0;
	unsigned withErrorCnt = 0;

	unsigned detectErrorCnt = 0;

	unsigned detectCorrectlyCnt = 0;
	unsigned detectErrorCorrectlyCnt = 0;
	unsigned detectFailureCnt = 0;

	unsigned locErrorCorrectlyCnt = 0;

	std::wstring str1, str2, str3;
	while (finSrc.getStr(str1, L'\n') != EOF)
	{
		finAns.getStr(str2, L'\n');
		totalCnt++;

		std::vector<unsigned> posAns;
		auto splitResult = Wchar::splitWstr(str2, L',');
		for (unsigned i = 1; i < splitResult.size(); i++)
		{
			const auto& numberStr = splitResult[i];
			unsigned pos = std::stoi(numberStr.substr(1, numberStr.size() - 1)) - 1;

			posAns.push_back(pos);
		}
		if (posAns.size() == 1 && posAns[0] == -1)
		{
			posAns.clear();
		}

		std::vector<unsigned> posRes;
		std::vector<wchar_t> correctCharRes;

		finRes.getStr(str3, L'\n');
		getModifiedSet(str1, str3, posRes, correctCharRes);

		if (posAns.empty())
		{
			withoutErrorCnt++;
			if (posRes.empty())
			{
				detectCorrectlyCnt++;
			}
			else
			{
				detectFailureCnt++;
			}
		}
		else
		{
			withErrorCnt++;
			if (!posRes.empty())
			{
				detectCorrectlyCnt++;
				detectErrorCorrectlyCnt++;
			}
		}

		if (!posRes.empty())
		{
			detectErrorCnt++;
		}

		bool locSuccess = true, correctSuccess = true;
		if (posAns.size() != posRes.size())
		{
			locSuccess = correctSuccess = false;
		}
		else
		{
			for (int i = 0; i < posAns.size(); i++)
			{
				if (posRes[i] != posAns[i])
				{
					locSuccess = false;
					correctSuccess = false;
					break;
				}
			}
		}
		if (locSuccess && !posAns.empty())
		{
			locErrorCorrectlyCnt++;
		}

	}

	double FAR = 100.0 * detectFailureCnt / withoutErrorCnt;
	double DA = 100.0 * detectCorrectlyCnt / totalCnt;
	double DP = 100.0 * detectErrorCorrectlyCnt / detectErrorCnt;
	double DR = 100.0 * detectErrorCorrectlyCnt / withErrorCnt;
	double DF1 = 2.0 * DP * DR / (DP + DR);
	double ELA = 100.0 * (locErrorCorrectlyCnt + detectCorrectlyCnt - detectErrorCorrectlyCnt) / totalCnt;
	double ELP = 100.0 * locErrorCorrectlyCnt / detectErrorCnt;
	double ELR = 100.0 * locErrorCorrectlyCnt / withErrorCnt;
	double ELF1 = 2.0 * ELP * ELR / (ELP + ELR);

	printf("FAR:%.4lf%%  DA:%.4lf%%  DP:%.4lf%%  DR:%.4lf%%  DF1:%.4lf%%\n"
		"ELA:%.4lf%%  ELP:%.4lf%%  ELR:%.4lf%%  ELF1:%.4lf%%\n",
		FAR, DA, DP, DR, DF1, ELA, ELP, ELR, ELF1);
}

void NLP::cnSpellingCorrectEvaluator(const std::string& sourceFile, const std::string& resFile, const std::string& ansFile)
{
	Wchar::WcharLoader finSrc(sourceFile);
	Wchar::WcharLoader finRes(resFile);
	Wchar::WcharLoader finAns(ansFile);

	unsigned totalCnt = 0;
	unsigned withoutErrorCnt = 0;
	unsigned withErrorCnt = 0;

	unsigned detectErrorCnt = 0;

	unsigned detectCorrectlyCnt = 0;
	unsigned detectErrorCorrectlyCnt = 0;
	unsigned detectFailureCnt = 0;

	unsigned locErrorCorrectlyCnt = 0;

	unsigned correctCorrectlyCnt = 0;

	std::wstring str1, str2, str3;
	while (finSrc.getStr(str1, L'\n') != EOF)
	{
		finAns.getStr(str2, L'\n');
		totalCnt++;

		std::vector<unsigned> posAns;
		std::vector<wchar_t> correctCharAns;
		auto splitResult = Wchar::splitWstr(str2, L',');
		for (unsigned i = 1; i + 1 < splitResult.size(); i += 2)
		{
			const auto& numberStr = splitResult[i];
			const auto& wordStr = splitResult[i + 1];
			unsigned pos = std::stoi(numberStr.substr(1, numberStr.size() - 1)) - 1;
			wchar_t ch = wordStr.back();

			posAns.push_back(pos);
			correctCharAns.push_back(ch);
		}

		
		std::vector<unsigned> posRes;
		std::vector<wchar_t> correctCharRes;

		finRes.getStr(str3, L'\n');
		getModifiedSet(str1, str3, posRes, correctCharRes);

		if (posAns.empty())
		{
			withoutErrorCnt++;
			if (posRes.empty())
			{
				detectCorrectlyCnt++;
			}
			else
			{
				detectFailureCnt++;
			}
		}
		else
		{
			withErrorCnt++;
			if (!posRes.empty())
			{
				detectCorrectlyCnt++;
				detectErrorCorrectlyCnt++;
			}
		}

		if (!posRes.empty())
		{
			detectErrorCnt++;
		}

		bool locSuccess = true, correctSuccess = true;
		if (posAns.size() != posRes.size())
		{
			locSuccess = correctSuccess = false;
		}
		else
		{
			for (int i = 0; i < posAns.size(); i++)
			{
				if (posRes[i] != posAns[i])
				{
					locSuccess = false;
					correctSuccess = false;
					break;
				}
				if (correctCharAns[i] != correctCharRes[i])
				{
					correctSuccess = false;
				}
			}
		}
		if (locSuccess)
		{
			locErrorCorrectlyCnt++;
		}
		if (correctSuccess)
		{
			correctCorrectlyCnt++;
		}
		
	}
	double ELA = 100.0 * (locErrorCorrectlyCnt + detectCorrectlyCnt - detectErrorCorrectlyCnt) / totalCnt;

	double CA = 100.0 * correctCorrectlyCnt / totalCnt;
	double CP = 100.0 * correctCorrectlyCnt / detectErrorCnt;
	printf("LA:%.4lf%%  CA:%.4lf%%  CP:%.4lf%%\n",
		ELA, CA, CP);
}