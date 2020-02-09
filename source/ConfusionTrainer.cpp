#include"LanguageModel.h"

static std::vector<std::wstring> strSeg; // ��split����ַ�������

static bool dfs(			// �����Ƿ�ɹ�
	bool hasModified,		// �ַ��Ƿ��Ѿ��޸�
	unsigned i,				// ��ǰ�ַ������ַ�������
	unsigned startWi,		// ��һ���ʵ�����
	unsigned wi,			// ��ǰ��������0,1,2)
	unsigned trieIndex,		// ��ǰ�����ֵ����ϵ�����
	const NLP::Dictionary& dic, // �ʵ�
	const NLP::ConfusionSet& cSet, // �����ּ���
	std::pair<std::wstring, std::wstring>& result // ����д𰸣��򴢴淵��ֵ
	)
{
	if (wi > 2 || startWi + wi >= strSeg.size())
	{
		return false;
	}
	// ���ö������һ�����ַ���
	if (i == strSeg[startWi + wi].size())
	{
		std::wstring leafStr = dic.getTrieLeaf(trieIndex).getContent();
		// ���ж��Ƿ��ҵ�һ���޸ĵ���
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
		// ������ǰdfs
		return dfs(hasModified, 0, startWi, wi + 1, trieIndex, dic, cSet, result);
	}

	
	wchar_t wch = strSeg[startWi + wi][i];

	// ��ǰ�ַ�������Ǻ���
	if (!cSet.characterExists(wch))
	{
		return false;
	}

	// ���޸ĵ�ǰ�ַ�����������
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
		// ��������޸ģ�����һ���ֵĻ�����
		for (auto pr : cSet.getCharConfustionSet(strSeg[startWi + wi][i]))
		{
			wch = pr.first;
			if (wch != strSeg[startWi + wi][i]) // �������ֲ��ǵ�ǰ����
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