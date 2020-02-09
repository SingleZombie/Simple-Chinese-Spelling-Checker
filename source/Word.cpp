#include "Word.h"
#include "WcharIO.h"

namespace NLP
{

	const std::unordered_map<Word::WordType, std::wstring> Word::wordTypeToStrMap =
	{
	{ Word::WordType::empty, L""},
	{ Word::WordType::Ag, L"Ag"},
	{ Word::WordType::a, L"a"},
	{ Word::WordType::ad, L"ad"},
	{ Word::WordType::an, L"an"},
	{ Word::WordType::Bg, L"Bg"},
	{ Word::WordType::b, L"b"},
	{ Word::WordType::c, L"c"},
	{ Word::WordType::Dg, L"Dg"},
	{ Word::WordType::d, L"d"},
	{ Word::WordType::e, L"e"},
	{ Word::WordType::f, L"f"},
	{ Word::WordType::h, L"h"},
	{ Word::WordType::i, L"i"},
	{ Word::WordType::j, L"j"},
	{ Word::WordType::k, L"k"},
	{ Word::WordType::l, L"l"},
	{ Word::WordType::Mg, L"Mg"},
	{ Word::WordType::m, L"m"},
	{ Word::WordType::Ng, L"Ng"},
	{ Word::WordType::n, L"n"},
	{ Word::WordType::nr, L"nr"},
	{ Word::WordType::ns, L"ns"},
	{ Word::WordType::nt, L"nt"},
	{ Word::WordType::nx, L"nx"},
	{ Word::WordType::nz, L"nz"},
	{ Word::WordType::o, L"o"},
	{ Word::WordType::p, L"p"},
	{ Word::WordType::q, L"q"},
	{ Word::WordType::Rg, L"Rg"},
	{ Word::WordType::r, L"r"},
	{ Word::WordType::s, L"s"},
	{ Word::WordType::Tg, L"Tg"},
	{ Word::WordType::t, L"t"},
	{ Word::WordType::u, L"u"},
	{ Word::WordType::Vg, L"Vg"},
	{ Word::WordType::v, L"v"},
	{ Word::WordType::vd, L"vd"},
	{ Word::WordType::vn, L"vn"},
	{ Word::WordType::w, L"w"},
	{ Word::WordType::x, L"x"},
	{ Word::WordType::Yg, L"Yg"},
	{ Word::WordType::y, L"y"},
	{ Word::WordType::z, L"z"}
	};
	const std::unordered_map<std::wstring, Word::WordType> Word::strToWordTypeMap =
	{
	{L"", Word::WordType::empty},
	{L"Ag", Word::WordType::Ag},
	{L"a", Word::WordType::a},
	{L"ad", Word::WordType::ad},
	{L"an", Word::WordType::an},
	{L"Bg", Word::WordType::Bg},
	{L"b", Word::WordType::b},
	{L"c", Word::WordType::c},
	{L"Dg", Word::WordType::Dg},
	{L"d", Word::WordType::d},
	{L"e", Word::WordType::e},
	{L"f", Word::WordType::f},
	{L"h", Word::WordType::h},
	{L"i", Word::WordType::i},
	{L"j", Word::WordType::j},
	{L"k", Word::WordType::k},
	{L"l", Word::WordType::l},
	{L"Mg", Word::WordType::Mg},
	{L"m", Word::WordType::m},
	{L"Ng", Word::WordType::Ng},
	{L"n", Word::WordType::n},
	{L"nr", Word::WordType::nr},
	{L"ns", Word::WordType::ns},
	{L"nt", Word::WordType::nt},
	{L"nx", Word::WordType::nx},
	{L"nz", Word::WordType::nz},
	{L"o", Word::WordType::o},
	{L"p", Word::WordType::p},
	{L"q", Word::WordType::q},
	{L"Rg", Word::WordType::Rg},
	{L"r", Word::WordType::r},
	{L"s", Word::WordType::s},
	{L"Tg", Word::WordType::Tg},
	{L"t", Word::WordType::t},
	{L"u", Word::WordType::u},
	{L"Vg", Word::WordType::Vg},
	{L"v", Word::WordType::v},
	{L"vd", Word::WordType::vd},
	{L"vn", Word::WordType::vn},
	{L"w", Word::WordType::w},
	{L"x", Word::WordType::x},
	{L"Yg", Word::WordType::Yg},
	{L"y", Word::WordType::y},
	{L"z", Word::WordType::z}
	};


	Word::Word() : _content(L""), _type(WordType::empty)
	{

	}

	Word::Word(const Word& word) : _content(word._content), _type(word._type)
	{

	}

	Word::Word(const std::wstring& str, WordType type) : _content(str), _type(type)
	{

	}

	bool Word::operator==(const Word& rhs) const
	{
		return _content == rhs._content && _type == rhs._type;
	}
}