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
		const static unsigned int WORD_TYPE_COUNT	= 43;	// ���ʴ�������

		// ���ʴ���ö����
		enum class WordType
		{
			empty,		// ������
			Ag,			// ������
			a,			// ���ݴ�
			ad,			// ���δ�
			an,			// ���δ�
			Bg,			// ��������
			b,			// �����
			c,			// ����
			Dg,			// ������
			d,			// ����
			e,			// ̾��
			f,			// ��λ��
			h,			// ǰ�ӳɷ�
			i,			// ����
			j,			// ���
			k,			// ��ӳɷ�
			l,			// ϰ����
			Mg,			// ������
			m,			// ����
			Ng,			// ������
			n,			// ����
			nr,			// ����
			ns,			// ����
			nt,			// ��������
			nx,			// �����ַ�
			nz,			// ��������
			o,			// ������
			p,			// ���
			q,			// ����
			Rg,			// ������
			r,			// ����
			s,			// ������
			Tg,			// ʱ����
			t,			// ʱ���
			u,			// ����
			Vg,			// ������
			v,			// ����
			vd,			// ������
			vn,			// ������
			w,			// ���
			x,			// ������
			Yg,			// ��������
			y,			// ������
			z,			// ״̬��
			sum			// �ܺ�
		};

		// ���������ö�ٻ���ת����ӳ���
		const static std::unordered_map<WordType, std::wstring> wordTypeToStrMap;
		const static std::unordered_map<std::wstring, WordType> strToWordTypeMap;
		
		// Ĭ�Ϲ��캯��
		Word();
		Word(const Word& word);
		// �øôʵ����ݡ�����������һ��Word
		Word(const std::wstring& str, WordType type);

		// ���������
		bool operator==(const Word& rhs) const;

		// getters
		std::wstring getContent() const { return _content; }
		WordType getType() const { return _type; }

	private:
		std::wstring _content;	// �ʵ�����
		WordType _type;			// �ʵĴ���
	};

	
	
}

// �������Ĺ�ϣ����
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