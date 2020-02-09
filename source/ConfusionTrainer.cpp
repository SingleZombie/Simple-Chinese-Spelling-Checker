#include"LanguageModel.h"

static std::vector<std::wstring> strSeg; // 被split后的字符串数组

static bool dfs(			// 返回是否成功
	bool hasModified,		// 字符是否已经修改
	unsigned i,				// 当前字符在子字符串索引
	unsigned startWi,		// 第一个词的索引
	unsigned wi,			// 当前词索引（0,1,2)
	unsigned trieIndex,		// 当前词在字典树上的索引
	const NLP::Dictionary& dic, // 词典
	const NLP::ConfusionSet& cSet, // 混淆字集合
	std::pair<std::wstring, std::wstring>& result // 如果有答案，则储存返回值
	)
{
	if (wi > 2 || startWi + wi >= strSeg.size())
	{
		return false;
	}
	// 如果枚举完了一个子字符串
	if (i == strSeg[startWi + wi].size())
	{
		std::wstring leafStr = dic.getTrieLeaf(trieIndex).getContent();
		// 先判断是否找到一个修改的字
		if (hasModified && wi == 1 && !leafStr.empty())
		{
			result = { strSeg[startWi] + strSeg[startWi + 1], leafStr };
			return true;
		}
		else if (hasModified && wi == 2 && !leafStr.empty())
		{
			result = { strSeg[startWi] + strSeg[startWi + 1] + strSeg[startWi + 2], leafStr };
			return true;
		}
		// 否则向前dfs
		return dfs(hasModified, 0, startWi, wi + 1, trieIndex, dic, cSet, result);
	}

	
	wchar_t wch = strSeg[startWi + wi][i];

	// 当前字符如果不是汉字
	if (!cSet.characterExists(wch))
	{
		return false;
	}

	// 不修改当前字符，继续搜索
	unsigned nxtIndex = dic.getTrieNextNode(trieIndex, wch);
	if (nxtIndex != 0)
	{
		if (dfs(hasModified, i + 1, startWi, wi, nxtIndex, dic, cSet, result))
		{
			return true;
		}
	}
	if (!hasModified)
	{
		// 如果可以修改，遍历一个字的混淆字
		for (auto pr : cSet.getCharConfustionSet(strSeg[startWi + wi][i]))
		{
			wch = pr.first;
			if (wch != strSeg[startWi + wi][i]) // 如果这个字不是当前的字
			{
				nxtIndex = dic.getTrieNextNode(trieIndex, wch);
				if (nxtIndex != 0)
				{
					if (dfs(true, i + 1, startWi, wi, nxtIndex, dic, cSet, result))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void NLP::LanguageModel::getConfusionPair(
	std::vector<std::pair<std::wstring, std::wstring>>& outputArr,
	std::vector<std::wstring>& preWordArr,
	const std::wstring& str
)
{
	strSeg = Wchar::splitWstr(str, L' ');
	for (unsigned i = 0; i + 1 < strSeg.size(); i++)
	{
		std::pair<std::wstring, std::wstring> tmpPr;
		if (dfs(false, 0, i, 0, 0, _dic, _cSet, tmpPr))
		{
			outputArr.push_back(tmpPr);
			if (i == 0)
			{
				preWordArr.push_back(L"S");
			}
			else
			{
				preWordArr.push_back(strSeg[i - 1]);
			}
		}
	}
}