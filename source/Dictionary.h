#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "Word.h"
#include "WcharLoader.h"

namespace NLP
{
	// 词典中每一个词储存的信息
	class WordMsg
	{
	public:
		WordMsg();

		// 用词的内容来构造词
		WordMsg(const std::wstring& str);

		// 根据当前词和上一个词，更新词信息
		void insert(const Word& word, const Word& prevWord);

		// getters
		bool empty() const { return _content.empty(); }
		const std::wstring& getContent() const { return _content; }
		unsigned getTotalCount() const { return _totalCount; }
		unsigned getTotalCountWithType() const { return _totalCountWithType; }
		const std::unordered_map<Word::WordType, unsigned>& getTypeCountMap() const { return _typeCount; }
		const std::unordered_map<Word, unsigned>& getPrevWords() const { return _prevWords; }
		const std::unordered_map<std::wstring, unsigned>& getPrevStrs() const { return _prevStrs; }

	private:
		std::wstring _content;			//词的内容
		unsigned _totalCount;			//词总共出现次数
		unsigned _totalCountWithType;	// 标记了词性的词的个数
		std::unordered_map<Word::WordType, unsigned> _typeCount;		// 词以某种词性出现的出现次数
		std::unordered_map<Word, unsigned> _prevWords;					// 该词上一个词的集合
		std::unordered_map<std::wstring, unsigned> _prevStrs;			// 该词上一个字符串的集合
	};

	// 词典
	class Dictionary
	{
	public:
		const static unsigned MAX_LENGTH = 8; // 词中最多包含几个字

		Dictionary();
		
		// 更新词典
		void insertArticle(Wchar::WcharLoader& fileLoader);
		void insertSentence(const std::wstring& str, const wchar_t& wordSeperator = L' ');
		void insertWord(const Word& word, const Word& prevWord = Word());

		// 查询词典
		bool haveCharacter(wchar_t wch) const;
		bool haveWord(const std::wstring& str) const;
		const WordMsg& getWordMsg(const std::wstring& str) const;

		unsigned getTrieNextNode(unsigned index, wchar_t wch) const;
		const WordMsg& getTrieLeaf(unsigned index) const;

		// 计算一个词在另一个词之前出现的概率
		// 如果这一个词不存在：
		//		若为单字、全英文、全数字，则给一个 1 / 总词数 的概率
		//		否则这一个词未在字典中出现，概率为0
		// 如果上一个词不存在，则使用数据平滑法
		double getPrevWordOccurence(const std::wstring& str,
									const std::wstring& prevStr) const;	

		// 计算一个词性在另一个词性之前出现的概率
		double getPrevTypeOccurence(const Word::WordType& cType,
									const Word::WordType& pType) const;
																	
		bool isEnglishWord(const std::wstring& str) const; 
		bool isNumberWord(const std::wstring& str) const;

		// getters

	private:
		//所有词存在一棵字典树上
		//只有叶子存储了实际的词信息
		struct TrieNode
		{
			// 向下跳转的数组
			std::unordered_map<wchar_t, unsigned> next;
			// 叶子上存储词信息
			WordMsg leaf;

			TrieNode();
		};
		struct Trie
		{
			// 根节点的索引一定是0
			// 动态分配的节点池。所有节点只储存在节点池的索引
			std::vector<TrieNode> nodePool;
			
			// 节点数
			unsigned tot;

			Trie();

			// 向节点池新建一个节点，返回该点下标
			unsigned newNode(); 
		};
		Trie _trie;

		unsigned insertWordIntoTrie(const std::wstring& word); // 把词插入字典树，返回最终节点索引
		unsigned getWordIndexInTrie(const std::wstring& word) const; //查找词是否在字典树上

		std::unordered_set<wchar_t> _characterSet; // 出现的每一个字
		unsigned _sentenceCount; // 句子数量，用于在读入的时候debug

		// 之前的代码
		//std::unordered_map<std::wstring, WordMsg> _words; // 以词的内容为键值来找到一个词的具体信息

		unsigned _typePairCount[Word::WORD_TYPE_COUNT + 3][Word::WORD_TYPE_COUNT + 3]; // 本次词性为i，上一次词性为j的<i,j>对数
		
		unsigned _wordCnt; //一共有几个词
		unsigned _totalCount; //所有词总共出现次数
		unsigned _totalCountWithType; // 所有标记了词性的词的个数
	};
}

#endif

