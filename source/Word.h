#ifndef WORD_H
#define WORD_H

#include <string>
#include <iostream>
#include <unordered_map>

namespace NLP
{
	class Word
	{
	public:
		const static unsigned int WORD_TYPE_COUNT	= 43;	// 单词词性总数

		// 单词词性枚举型
		enum class WordType
		{
			empty,		// 空类型
			Ag,			// 形语素
			a,			// 形容词
			ad,			// 副形词
			an,			// 名形词
			Bg,			// 区别语素
			b,			// 区别词
			c,			// 连词
			Dg,			// 副语素
			d,			// 副词
			e,			// 叹词
			f,			// 方位词
			h,			// 前接成分
			i,			// 成语
			j,			// 简称
			k,			// 后接成分
			l,			// 习用语
			Mg,			// 数语素
			m,			// 数词
			Ng,			// 名语素
			n,			// 名词
			nr,			// 人名
			ns,			// 地名
			nt,			// 机构团体
			nx,			// 外文字符
			nz,			// 其他名词
			o,			// 拟声词
			p,			// 介词
			q,			// 量词
			Rg,			// 代语素
			r,			// 代词
			s,			// 处所词
			Tg,			// 时语素
			t,			// 时间词
			u,			// 助词
			Vg,			// 动语素
			v,			// 动词
			vd,			// 副动词
			vn,			// 名动词
			w,			// 标点
			x,			// 非语素
			Yg,			// 语气词素
			y,			// 语气词
			z,			// 状态词
			sum			// 总和
		};

		// 词性与词性枚举互相转换的映射表
		const static std::unordered_map<WordType, std::wstring> wordTypeToStrMap;
		const static std::unordered_map<std::wstring, WordType> strToWordTypeMap;
		
		// 默认构造函数
		Word();
		Word(const Word& word);
		// 用该词的内容、类型来构造一个Word
		Word(const std::wstring& str, WordType type);

		// 运算符重载
		bool operator==(const Word& rhs) const;

		// getters
		std::wstring getContent() const { return _content; }
		WordType getType() const { return _type; }

	private:
		std::wstring _content;	// 词的内容
		WordType _type;			// 词的词性
	};

	
	
}

// 定义该类的哈希函数
namespace std {
	template <> 
	class hash<NLP::Word> {
	public:
		size_t operator()(const NLP::Word &word) const
		{
			return hash<std::wstring>()(word.getContent()) * (unsigned)word.getType();
		}
	};
};

#endif