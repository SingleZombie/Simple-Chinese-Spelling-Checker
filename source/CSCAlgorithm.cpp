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

// �������е��ַ������ҳ�ʹLM������ߵ���һ�ʵ�λ��
static std::pair<unsigned, double> getPrevState(
	const std::wstring& str,			// �����ַ���
	unsigned j,							// ��һ�ʽ���λ��
	const NLP::Dictionary& dic			// �ʵ�
);
// ������һ������vertices����������� ���LM����

//ö�ٴ�λ��i��ʼ���п��ܵĴ�
//��ÿ��λ�õ��ְ��������滻������ͬʱ���ֵ����в��Ҹô��Ƿ����
static void dfs(
	unsigned ci, // ��ǰλ��
	unsigned clen, // ��ǰDFS�ĳ��� - 1
	unsigned trieIndex, // ��ǰ�ֵ����ڵ�
	double modifyCost, // ��Ϊ�޸Ĵ����ĸ�����ʧ
	const NLP::Dictionary& dic, // �ʵ�
	const NLP::ConfusionSet& cSet // �ܻ�����
);

std::wstring NLP::correctSentence(const Dictionary& dic, const ConfusionSet& cSet, const std::wstring& str2)
{
	str = L'S' + str2 + L'E';
	len = str.length();
	vertices = std::vector<std::vector<cscNode>>(len); // �洢��λ��i��β�Ĵʶ�Ӧ�Ľڵ�
	vertices[0].push_back(cscNode(L"S", 1.0, 0));

	for (unsigned i = 1; i < len; i++)
	{
		//ö�ٴ�λ��i��ʼ���п��ܵĴ�
		//��ÿ��λ�õ��ְ��������滻������ͬʱ���ֵ����в��Ҹô��Ƿ����
		dfs(i, 0, 0, 1.0, dic, cSet);
	}

	// ��������õ�����ַ���
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

// �������е��ַ������ҳ�ʹLM������ߵ���һ�ʵ�λ��
static std::pair<unsigned, double> getPrevState(
	const std::wstring& str,			// �����ַ���
	unsigned j,							// ��һ�ʽ���λ��
	const NLP::Dictionary& dic			// �ʵ�
)
// ������һ������vertices����������� ���LM����
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

//ö�ٴ�λ��i��ʼ���п��ܵĴ�
//��ÿ��λ�õ��ְ��������滻������ͬʱ���ֵ����в��Ҹô��Ƿ����
static void dfs(
	unsigned ci, // ��ǰλ��
	unsigned clen, // ��ǰDFS�ĳ��� - 1
	unsigned trieIndex, // ��ǰ�ֵ����ڵ�
	double modifyCost, // ��Ϊ�޸Ĵ����ĸ�����ʧ
	const NLP::Dictionary& dic, // �ʵ�
	const NLP::ConfusionSet& cSet // �ܻ�����
)
{
	if (clen + ci >= len || clen >= dic.MAX_LENGTH || (ci != len - 1 && ci + clen == len - 1)) //���Ȳ��Ϸ�ֱ�ӷ���
	{
		return;
	}

	if (ci == len - 1) // ��ǰ���ڷ���'E'
	{
		auto res = getPrevState(L"E", len - 2, dic);
		vertices[len - 1].push_back(cscNode(L"E", res.second, res.first));
		return;
	}

	// �����û�л�����
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

	// ��ȡ��ǰ�ֵĻ������������а���������Լ���
	const auto& crtCharConfusionSet = cSet.getCharConfustionSet(str[ci + clen]);

	// ������������ÿһ����
	for (auto pr : crtCharConfusionSet)
	{
		wchar_t cChar = pr.first;
		double charSubstitudeCost = pr.second;
		// ���ݵ�ǰ����ͼ���ֵ������ƶ�
		unsigned crtTrieIndex = dic.getTrieNextNode(trieIndex, cChar);

		// ����ֵ�����������ڵ㣬���������ֻ��һ���ֿ��ܵ����ɴʣ������ж����
		if (crtTrieIndex || clen == 0)
		{
			std::wstring cstr;
			// ���ݵ�ǰ�ʵĳ����������ǵ��ֳɴʻ������ֵ������Ҵ�
			if (clen == 0)
			{
				cstr.push_back(cChar);
			}
			else
			{
				cstr = dic.getTrieLeaf(crtTrieIndex).getContent();
			}

			// ��ǰ�޸��ֵĸ���
			double crtModifyCost = modifyCost;

			// �����ǰ�����ֵ����ϣ������ֵ����˴�û��Ҷ�ӣ����ܵõ�һ���յ��ַ���
			// ����յ��ַ������ܹ����µĽڵ�
			if (!cstr.empty())
			{
				auto res = getPrevState(cstr, ci - 1, dic);
				// �����ǰ���޸��ˣ����پ������ɵĸ��ʣ�Ҳ���Ǽ�С��Ȩ
				if (cChar != str[ci + clen])
				{
					crtModifyCost *= NLP::LanguageModel::CHARACTER_ERROR_P * charSubstitudeCost;
					//ͬʱ���ִ���ĸ��ʸ���
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

			if (crtTrieIndex) // �����ǰ�������ֵ�����
			{
				dfs(ci, clen + 1, crtTrieIndex, crtModifyCost, dic, cSet);
			}
		}
	}
}