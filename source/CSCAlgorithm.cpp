#include "NLPAlgorithm.h"
#include "LanguageModel.h"
#include <algorithm>

static struct cscNode
{
	double probability;
	std::wstring word;
	unsigned prevState;
	cscNode(const std::wstring& _word, double _p, unsigned _s) : word(_word), probability(_p), prevState(_s)
	{

	}
};

static std::wstring str;
static unsigned len;
static std::vector<std::vector<cscNode>> vertices;

// 根据现有的字符串，找出使LM概率最高的上一词的位置
static std::pair<unsigned, double> getPrevState(
	const std::wstring& str,			// 现有字符串
	unsigned j,							// 上一词结束位置
	const NLP::Dictionary& dic			// 词典
);
// 返回上一词在其vertices数组的索引， 最大LM概率

//枚举从位置i开始所有可能的词
//把每个位置的字按混淆集替换，并且同时在字典树中查找该词是否存在
static void dfs(
	unsigned ci, // 当前位置
	unsigned clen, // 当前DFS的长度 - 1
	unsigned trieIndex, // 当前字典树节点
	double modifyCost, // 因为修改带来的概率损失
	const NLP::Dictionary& dic, // 词典
	const NLP::ConfusionSet& cSet // 总混淆集
);

std::wstring NLP::correctSentence(const Dictionary& dic, const ConfusionSet& cSet, const std::wstring& str2)
{
	str = L'S' + str2 + L'E';
	len = str.length();
	vertices = std::vector<std::vector<cscNode>>(len); // 存储以位置i结尾的词对应的节点
	vertices[0].push_back(cscNode(L"S", 1.0, 0));

	for (unsigned i = 1; i < len; i++)
	{
		//枚举从位置i开始所有可能的词
		//把每个位置的字按混淆集替换，并且同时在字典树中查找该词是否存在
		dfs(i, 0, 0, 1.0, dic, cSet);
	}

	// 倒序遍历得到结果字符串
	std::vector<std::pair<unsigned, unsigned>> resultNodes;
	unsigned vertexI = len - 1, vertexJ = 0;
	while (vertexI != 0)
	{
		resultNodes.push_back({ vertexI, vertexJ });
		unsigned nextI = vertexI - vertices[vertexI][vertexJ].word.size();
		unsigned nextJ = vertices[vertexI][vertexJ].prevState;
		vertexI = nextI;
		vertexJ = nextJ;
	}
	std::wstring result;
	for (unsigned i = resultNodes.size() - 1; i >= 1; i--)
	{
		result += vertices[resultNodes[i].first][resultNodes[i].second].word;
	}
	return result;
}

// 根据现有的字符串，找出使LM概率最高的上一词的位置
static std::pair<unsigned, double> getPrevState(
	const std::wstring& str,			// 现有字符串
	unsigned j,							// 上一词结束位置
	const NLP::Dictionary& dic			// 词典
)
// 返回上一词在其vertices数组的索引， 最大LM概率
{
	unsigned result = -1;
	double p = -1;
	for (unsigned i = 0; i < vertices[j].size(); i++)
	{
		const auto& node = vertices[j][i];
		double cp = node.probability * dic.getPrevWordOccurence(str, node.word);
		if (cp > p)
		{
			result = i;
			p = cp;
		}
	}
	return { result, p };
};

//枚举从位置i开始所有可能的词
//把每个位置的字按混淆集替换，并且同时在字典树中查找该词是否存在
static void dfs(
	unsigned ci, // 当前位置
	unsigned clen, // 当前DFS的长度 - 1
	unsigned trieIndex, // 当前字典树节点
	double modifyCost, // 因为修改带来的概率损失
	const NLP::Dictionary& dic, // 词典
	const NLP::ConfusionSet& cSet // 总混淆集
)
{
	if (clen + ci >= len || clen >= dic.MAX_LENGTH || (ci != len - 1 && ci + clen == len - 1)) //长度不合法直接返回
	{
		return;
	}

	if (ci == len - 1) // 当前正在访问'E'
	{
		auto res = getPrevState(L"E", len - 2, dic);
		vertices[len - 1].push_back(cscNode(L"E", res.second, res.first));
		return;
	}

	// 这个字没有混淆集
	if (!cSet.characterExists(str[ci + clen]))
	{
		if (clen == 0)
		{
			std::wstring cstr;
			cstr.push_back(str[ci + clen]);
			auto res = getPrevState(cstr, ci - 1, dic);
			vertices[ci + clen].push_back(cscNode(cstr, res.second, res.first));
		}
		return;
	}

	// 获取当前字的混淆集（集合中包括这个字自己）
	const auto& crtCharConfusionSet = cSet.getCharConfustionSet(str[ci + clen]);

	// 遍历混淆集中每一个字
	for (auto pr : crtCharConfusionSet)
	{
		wchar_t cChar = pr.first;
		double charSubstitudeCost = pr.second;
		// 根据当前字试图在字典树上移动
		unsigned crtTrieIndex = dic.getTrieNextNode(trieIndex, cChar);

		// 如果字典树上有这个节点，或者这个词只有一个字可能单独成词，进入判断语句
		if (crtTrieIndex || clen == 0)
		{
			std::wstring cstr;
			// 根据当前词的长度来决定是单字成词还是在字典树里找词
			if (clen == 0)
			{
				cstr.push_back(cChar);
			}
			else
			{
				cstr = dic.getTrieLeaf(crtTrieIndex).getContent();
			}

			// 当前修改字的概率
			double crtModifyCost = modifyCost;

			// 如果当前串在字典树上，但是字典树此处没有叶子，可能得到一个空的字符串
			// 这个空的字符串不能构成新的节点
			if (!cstr.empty())
			{
				auto res = getPrevState(cstr, ci - 1, dic);
				// 如果当前字修改了，减少句子生成的概率，也就是减小边权
				if (cChar != str[ci + clen])
				{
					crtModifyCost *= NLP::LanguageModel::CHARACTER_ERROR_P * charSubstitudeCost;
					//同时出现错误的概率更低
					if (modifyCost != 1.0f)
					{
						modifyCost *= 0.1f;
					}
				}
				else
				{
					crtModifyCost *= 1 - NLP::LanguageModel::CHARACTER_ERROR_P;
				}
				vertices[ci + clen].push_back(cscNode(cstr, res.second * crtModifyCost, res.first));
			}

			if (crtTrieIndex) // 如果当前串还在字典树上
			{
				dfs(ci, clen + 1, crtTrieIndex, crtModifyCost, dic, cSet);
			}
		}
	}
}