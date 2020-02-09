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
	// �ʵ���ÿһ���ʴ������Ϣ
	class WordMsg
	{
	public:
		WordMsg();

		// �ôʵ������������
		WordMsg(const std::wstring& str);

		// ���ݵ�ǰ�ʺ���һ���ʣ����´���Ϣ
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
		std::wstring _content;			//�ʵ�����
		unsigned _totalCount;			//���ܹ����ִ���
		unsigned _totalCountWithType;	// ����˴��ԵĴʵĸ���
		std::unordered_map<Word::WordType, unsigned> _typeCount;		// ����ĳ�ִ��Գ��ֵĳ��ִ���
		std::unordered_map<Word, unsigned> _prevWords;					// �ô���һ���ʵļ���
		std::unordered_map<std::wstring, unsigned> _prevStrs;			// �ô���һ���ַ����ļ���
	};

	// �ʵ�
	class Dictionary
	{
	public:
		const static unsigned MAX_LENGTH = 8; // ����������������

		Dictionary();
		
		// ���´ʵ�
		void insertArticle(Wchar::WcharLoader& fileLoader);
		void insertSentence(const std::wstring& str, const wchar_t& wordSeperator = L' ');
		void insertWord(const Word& word, const Word& prevWord = Word());

		// ��ѯ�ʵ�
		bool haveCharacter(wchar_t wch) const;
		bool haveWord(const std::wstring& str) const;
		const WordMsg& getWordMsg(const std::wstring& str) const;

		unsigned getTrieNextNode(unsigned index, wchar_t wch) const;
		const WordMsg& getTrieLeaf(unsigned index) const;

		// ����һ��������һ����֮ǰ���ֵĸ���
		// �����һ���ʲ����ڣ�
		//		��Ϊ���֡�ȫӢ�ġ�ȫ���֣����һ�� 1 / �ܴ��� �ĸ���
		//		������һ����δ���ֵ��г��֣�����Ϊ0
		// �����һ���ʲ����ڣ���ʹ������ƽ����
		double getPrevWordOccurence(const std::wstring& str,
									const std::wstring& prevStr) const;	

		// ����һ����������һ������֮ǰ���ֵĸ���
		double getPrevTypeOccurence(const Word::WordType& cType,
									const Word::WordType& pType) const;
																	
		bool isEnglishWord(const std::wstring& str) const; 
		bool isNumberWord(const std::wstring& str) const;

		// getters

	private:
		//���дʴ���һ���ֵ�����
		//ֻ��Ҷ�Ӵ洢��ʵ�ʵĴ���Ϣ
		struct TrieNode
		{
			// ������ת������
			std::unordered_map<wchar_t, unsigned> next;
			// Ҷ���ϴ洢����Ϣ
			WordMsg leaf;

			TrieNode();
		};
		struct Trie
		{
			// ���ڵ������һ����0
			// ��̬����Ľڵ�ء����нڵ�ֻ�����ڽڵ�ص�����
			std::vector<TrieNode> nodePool;
			
			// �ڵ���
			unsigned tot;

			Trie();

			// ��ڵ���½�һ���ڵ㣬���ظõ��±�
			unsigned newNode(); 
		};
		Trie _trie;

		unsigned insertWordIntoTrie(const std::wstring& word); // �Ѵʲ����ֵ������������սڵ�����
		unsigned getWordIndexInTrie(const std::wstring& word) const; //���Ҵ��Ƿ����ֵ�����

		std::unordered_set<wchar_t> _characterSet; // ���ֵ�ÿһ����
		unsigned _sentenceCount; // ���������������ڶ����ʱ��debug

		// ֮ǰ�Ĵ���
		//std::unordered_map<std::wstring, WordMsg> _words; // �Դʵ�����Ϊ��ֵ���ҵ�һ���ʵľ�����Ϣ

		unsigned _typePairCount[Word::WORD_TYPE_COUNT + 3][Word::WORD_TYPE_COUNT + 3]; // ���δ���Ϊi����һ�δ���Ϊj��<i,j>����
		
		unsigned _wordCnt; //һ���м�����
		unsigned _totalCount; //���д��ܹ����ִ���
		unsigned _totalCountWithType; // ���б���˴��ԵĴʵĸ���
	};
}

#endif

