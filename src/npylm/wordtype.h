#pragma once
#include <wchar.h>
#include <string>

#define WORDTYPE_NUM_TYPES 9

#define WORDTYPE_ALPHABET 1
#define WORDTYPE_NUMBER 2
#define WORDTYPE_SYMBOL 3
#define WORDTYPE_HIRAGANA 4
#define WORDTYPE_KATAKANA 5
#define WORDTYPE_KANJI 6
#define WORDTYPE_KANJI_HIRAGANA 7
#define WORDTYPE_KANJI_KATAKANA 8
#define WORDTYPE_OTHER 9

namespace npylm {
	namespace wordtype {
		bool is_dash(wchar_t character);
		bool is_hiragana(wchar_t character);
		bool is_katakana(wchar_t character);
		bool is_kanji(wchar_t character);
		bool is_number(wchar_t character);
		bool is_alphabet(wchar_t character);
		bool is_symbol(wchar_t character);
		int detect_word_type(std::wstring &word);
		int detect_word_type_substr(wchar_t const* characters, int substr_start, int substr_end);
	}
}