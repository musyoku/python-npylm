#include <cassert>
#include "ctype.h"
#include "wordtype.h"

namespace npylm {
	namespace wordtype {
		bool is_dash(wchar_t character){
			if(character == 0x30FC){
				return true;
			}
			return false;
		}
		bool is_hiragana(wchar_t character){
			int type = ctype::get_type(character);
			if(type == CTYPE_HIRAGANA){
				return true;
			}
			return is_dash(character);	// 長音はひらがなとカタカナ両方で使われる
		}
		bool is_katakana(wchar_t character){
			int type = ctype::get_type(character);
			if(type == CTYPE_KATAKANA){
				return true;
			}
			if(type == CTYPE_KATAKANA_PHONETIC_EXTENSIONS){
				return true;
			}
			return is_dash(character);	// 長音はひらがなとカタカナ両方で使われる
		}
		bool is_kanji(wchar_t character){
			int type = ctype::get_type(character);
			if(type == CTYPE_CJK_UNIFIED_IDEOGRAPHS){
				return true;
			}
			if(type == CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A){
				return true;
			}
			if(type == CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B){
				return true;
			}
			if(type == CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C){
				return true;
			}
			if(type == CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D){
				return true;
			}
			if(type == CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_E){
				return true;
			}
			if(type == CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_F){
				return true;
			}
			if(type == CTYPE_CJK_RADICALS_SUPPLEMENT){
				return true;
			}
			return false;

		}
		bool is_number(wchar_t character){
			int type = ctype::get_type(character);
			if(type == CTYPE_BASIC_LATIN){
				if(0x30 <= character && character <= 0x39){
					return true;
				}
				return false;
			}
			if(type == CTYPE_NUMBER_FORMS){
				return true;
			}
			if(type == CTYPE_COMMON_INDIC_NUMBER_FORMS){
				return true;
			}
			if(type == CTYPE_AEGEAN_NUMBERS){
				return true;
			}
			if(type == CTYPE_ANCIENT_GREEK_NUMBERS){
				return true;
			}
			if(type == CTYPE_COPTIC_EPACT_NUMBERS){
				return true;
			}
			if(type == CTYPE_SINHALA_ARCHAIC_NUMBERS){
				return true;
			}
			if(type == CTYPE_CUNEIFORM_NUMBERS_AND_PUNCTUATION){
				return true;
			}
			return false;
		}
		bool is_alphabet(wchar_t character){
			if(0x41 <= character && character <= 0x5a){
				return true;
			}
			if(0x61 <= character && character <= 0x7a){
				return true;
			}
			return false;
		}
		bool is_symbol(wchar_t character){
			if(is_alphabet(character)){
				return false;
			}
			if(is_number(character)){
				return false;
			}
			if(is_kanji(character)){
				return false;
			}
			if(is_hiragana(character)){
				return false;
			}
			return true;
		}
		int detect_word_type(std::wstring &word){
			return detect_word_type_substr(word.data(), 0, word.size() - 1);
		}
		// 文字列の指定範囲の単語種判定
		int detect_word_type_substr(wchar_t const* characters, int substr_start, int substr_end){
			assert(substr_end >= substr_start);
			int num_alphabet = 0;
			int num_number = 0;
			int num_symbol = 0;
			int num_hiragana = 0;
			int num_katakana = 0;
			int num_kanji = 0;
			int num_dash = 0;
			int size = substr_end - substr_start + 1;
			for(int i = substr_start;i <= substr_end;i++){
				const wchar_t target = characters[i];
				if(is_alphabet(target)){
					num_alphabet += 1;
					continue;
				}
				if(is_number(target)){
					num_number += 1;
					continue;
				}
				if(is_dash(target)){
					num_dash += 1;
					continue;
				}
				if(is_hiragana(target)){
					num_hiragana += 1;
					continue;
				}
				if(is_katakana(target)){
					num_katakana += 1;
					continue;
				}
				if(is_kanji(target)){
					num_kanji += 1;
					continue;
				}
				num_symbol += 1;
			}
			if(num_alphabet == size){
				return WORDTYPE_ALPHABET;
			}
			if(num_number == size){
				return WORDTYPE_NUMBER;
			}
			if(num_hiragana + num_dash == size){
				return WORDTYPE_HIRAGANA;
			}
			if(num_katakana + num_dash == size){
				return WORDTYPE_KATAKANA;
			}
			if(num_kanji == size){
				return WORDTYPE_KANJI;
			}
			if(num_symbol == size){
				return WORDTYPE_SYMBOL;
			}
			if(num_kanji > 0){
				if(num_hiragana + num_kanji == size){
					return WORDTYPE_KANJI_HIRAGANA;
				}
				if(num_hiragana > 0){
					if(num_hiragana + num_kanji + num_dash == size){
						return WORDTYPE_KANJI_HIRAGANA;
					}
				}
				if(num_katakana + num_kanji == size){
					return WORDTYPE_KANJI_KATAKANA;
				}
				if(num_katakana){
					if(num_katakana + num_kanji + num_dash == size){
						return WORDTYPE_KANJI_KATAKANA;
					}
				}
			}
			return WORDTYPE_OTHER;
		}
	}
}