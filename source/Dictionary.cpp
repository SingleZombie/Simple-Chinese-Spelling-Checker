#include "Dictionary.h"
#include <cassert>
#include <algorithm>

/* Word Message*/

namespace NLP
{


	WordMsg::WordMsg() : _content(L""), _totalCount(0), _totalCountWithType(0)
	{

	}

	WordMsg::WordMsg(const std::wstring& str) : _content(str), _totalCount(0), _totalCountWithType(0)
	{

	}

	void WordMsg::insert(const Word& word, const Word& prevWord)
	{
		_totalCount++;
		if (word.getType() != Word::WordType::empty)
		{
			_totalCountWithType++;
			_typeCount[word.getType()]++;
			_prevWords[prevWord]++;
		}
		_prevStrs[prevWord.getContent()]++;
	}

	/* Dictionary*/

	Dictionary::Dictionary()
	{
		memset(_typePairCount, 0, sizeof(_typePairCount));
	}

	void Dictionary::insertArticle(Wchar::WcharLoader& fileLoader)
	{
		std::wstring tmpStr;

		unsigned _rowCount = 0;
		int loadMark = 0;
		do
		{
			_rowCount++;

			tmpStr.clear();
			loadMark = fileLoader.getStr(tmpStr, L'\n');

			unsigned i = 0, len = tmpStr.size();
			while (i < len)
			{
				unsigned loc = std::wstring::npos;
				for (unsigned j = i; j < len; j++)
				{
					if (tmpStr[j] == L'¡£' || tmpStr[j] == L'£¬' || tmpStr[j] == L'£¿' || tmpStr[j] == L'£¡')
					{
						loc = j + 2;
						break;
					}
				}

				if (loc == std::wstring::npos)
				{
					insertSentence(tmpStr.substr(i, len - i));
					i = len;
				}
				else
				{
					insertSentence(tmpStr.substr(i, loc - i + 1));
					i = loc + 1;
				}

			}

		} while (loadMark != EOF);
	}

	void Dictionary::insertSentence(const std::wstring& str, const wchar_t& wordSeperator)
	{
		_sentenceCount++;

		Word preWord(L"S", Word::WordType::x);
		Word preWord2;
		Word crtWord;
		bool loadType = false;
		bool loadBracket = false;
		bool loadBracketOver = false;
		std::wstring tmpStr;
		std::wstring tmpStr2;
		std::wstring typeStr;

		auto insertWordProcedure = [&]()
		{
			if (!tmpStr.empty())
			{
				if (typeStr.empty())
				{ 
					crtWord = Word(tmpStr, Word::WordType::empty);
				}
				else
				{
					if (!Word::strToWordTypeMap.count(typeStr))
					{
						std::cout << "Wrong or undefined type code: ";
						for (auto ch : typeStr)
						{
							std::cout << Wchar::wcharToChar(ch).second;
						}
						std::cout << std::endl;
						crtWord = Word(tmpStr, Word::WordType::empty);
					}
					else
					{
						crtWord = Word(tmpStr, Word::strToWordTypeMap.at(typeStr));
					}
				}
				if (tmpStr.size() <= MAX_LENGTH)
				{
					insertWord(crtWord, preWord);
				}
				

				if (loadBracket)
				{
					tmpStr2 += tmpStr;
				}

				tmpStr.clear();
				typeStr.clear();
				if (tmpStr.size() <= MAX_LENGTH)
				{
					preWord = crtWord;
				}
				loadType = false;
			}
		};

		for (const auto& ch : str)
		{
			_characterSet.insert(ch);
			if (ch == wordSeperator)
			{
				if (loadBracketOver)
				{
					insertWord(Word(tmpStr2, Word::strToWordTypeMap.at(typeStr)), preWord2);
					loadBracketOver = false;
					loadType = false;
					tmpStr2.clear();
					typeStr.clear();
				}
				else
				{
					insertWordProcedure();
				}
			}
			else if (ch == L'/')
			{
				loadType = true;
			}
			else if (ch == L'[')
			{
				loadBracket = true;
				preWord2 = preWord;
			}
			else if (ch == L']')
			{
				insertWordProcedure();
				loadBracket = false;
				loadBracketOver = true;
				loadType = true;
			}
			else
			{
				if (loadType)
				{
					typeStr.push_back(ch);
				}
				else
				{
					tmpStr.push_back(ch);
				}
			}
		}

		insertWordProcedure();

		preWord = crtWord;
		crtWord = Word(L"E", Word::WordType::x);
		insertWord(crtWord, preWord);
	}
	void Dictionary::insertWord(const Word& word, const Word& prevWord)
	{
		if (word.getContent().empty())
		{
			return;
		}
		unsigned nodeIndex = insertWordIntoTrie(word.getContent());
		
		if (_trie.nodePool[nodeIndex].leaf.empty())
		{
			_trie.nodePool[nodeIndex].leaf = WordMsg(word.getContent());
			_wordCnt++;
		}
		WordMsg& wordMsg = _trie.nodePool[nodeIndex].leaf;
		wordMsg.insert(word, prevWord);
		if (word.getType() != Word::WordType::empty)
		{
			_totalCountWithType++;
			_typePairCount[(unsigned)word.getType()][(unsigned)prevWord.getType()]++;
			_typePairCount[(unsigned)word.getType()][(unsigned)Word::WordType::sum]++;
			_typePairCount[(unsigned)Word::WordType::sum][(unsigned)prevWord.getType()]++;
		}
		_totalCount++;
	}

	bool Dictionary::haveCharacter(wchar_t wch) const
	{
		return _characterSet.count(wch);
	}

	bool Dictionary::haveWord(const std::wstring& str) const
	{
		return getWordIndexInTrie(str) != 0;
	}
	const WordMsg& Dictionary::getWordMsg(const std::wstring& str) const
	{
		assert(haveWord(str));
		return _trie.nodePool[getWordIndexInTrie(str)].leaf;
	}

	double Dictionary::getPrevWordOccurence(const std::wstring& str,
		const std::wstring& prevStr) const
	{
		if (!haveWord(str))
		{
			if (str.size() == 1 || isEnglishWord(str) || isNumberWord(str))
			{
				return 1.0 / _wordCnt;
			}
			return 0;
		}
		auto& wordMsg = getWordMsg(str);
		unsigned cnt = 0;
		if (wordMsg.getPrevStrs().count(prevStr))
		{
			cnt = wordMsg.getPrevStrs().at(prevStr);
		}
		double numer = cnt + 1;
		double deno = _wordCnt + wordMsg.getTotalCount();
		return numer / deno;
	}

	double Dictionary::getPrevTypeOccurence(const Word::WordType& cType,
		const Word::WordType& pType) const
	{
		double numer = _typePairCount[(unsigned)cType][(unsigned)pType];
		double deno = _typePairCount[(unsigned)Word::WordType::sum][(unsigned)pType];
		if (deno == 0)
		{
			return 0;
		}
		return numer / deno;
	}

	bool Dictionary::isEnglishWord(const std::wstring& str) const
	{
		for (auto ch : str)
		{
			if (!(ch >= L'a' && ch <= L'z') && !(ch >= L'A' && ch <= L'Z') && !(ch >= L'£Á' && ch <= L'£Ú') && !(ch >= L'£á' && ch <= L'£ú') )
			{
				return false;
			}
		}
		return true;
	}
	bool Dictionary::isNumberWord(const std::wstring& str) const
	{
		for (auto ch : str)
		{
			if (!(ch >= L'£°' && ch <= L'£¹') && !(ch >= L'0' && ch <= L'9'))
			{
				return false;
			}
		}
		return true;
	}

	unsigned Dictionary::insertWordIntoTrie(const std::wstring& word)
	{
		unsigned cIndex = 0;
		for (wchar_t wch : word)
		{
			const auto& cNode = _trie.nodePool[cIndex];
			if (!cNode.next.count(wch))
			{
				unsigned newIndex = _trie.newNode();
				cIndex = _trie.nodePool[cIndex].next[wch] = newIndex;
			}
			else
			{
				cIndex = cNode.next.at(wch);
			}
		}
		return cIndex;
	}

	unsigned Dictionary::getWordIndexInTrie(const std::wstring& word) const
	{
		unsigned cIndex = 0;
		for (wchar_t wch : word)
		{
			auto& cNode = _trie.nodePool[cIndex];
			if (!cNode.next.count(wch))
			{
				cIndex = 0;
				break;
			}
			else
			{
				cIndex = cNode.next.at(wch);
			}
		}
		return cIndex;
	}

	unsigned Dictionary::getTrieNextNode(unsigned index, wchar_t wch) const
	{
		if (!_trie.nodePool[index].next.count(wch))
		{
			return 0;
		}
		return _trie.nodePool[index].next.at(wch);
	}
	const WordMsg& Dictionary::getTrieLeaf(unsigned index) const
	{
		return _trie.nodePool[index].leaf;
	}

	Dictionary::TrieNode::TrieNode() : next(), leaf()
	{

	}

	Dictionary::Trie::Trie() : tot(1)
	{
		nodePool.push_back(TrieNode());
	}

	unsigned Dictionary::Trie::newNode()
	{
		nodePool.push_back(TrieNode());
		return tot++;
	}
}