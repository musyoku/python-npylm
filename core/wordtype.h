#pragma once
#include <wchar.h>
#include <string>

// ハードコード感がすごいのでもっといいやり方に変えたい

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
		bool is_dash(wchar_t character){
			if(character == 12540){
				return true;
			}
			return false;
		}
		bool is_hiragana(wchar_t character){
			if(character > 12352){
				if(character < 12439){
					return true;
				}
			}
			return is_dash(character);	// 長音はひらがなとカタカナ両方で使われる
		}
		bool is_katakana(wchar_t character){
			if(character > 12448){
				if(character < 12539){
					return true;
				}
			}
			return is_dash(character);	// 長音はひらがなとカタカナ両方で使われる
		}
		// 割りと大雑把に判定
		bool is_kanji(wchar_t character){
			if(character > 11903){
				if(character < 12246){
					return true;
				}
			}
			if(character > 13311){
				if(character < 19894){
					return true;
				}
			}
			if(character > 19967){
				if(character < 40909){
					return true;
				}
			}
			if(character > 131071){
				if(character < 173783){

				}
			}
			return false;

		}
		bool is_number(wchar_t character){
			if(character < 48){
				return false;
			}
			if(character > 57){
				return false;
			}
			return true;
		}
		bool is_alphabet(wchar_t character){
			if(character > 64){
				if(character < 91){
					return true;
				}
			}
			if(character > 96){
				if(character < 123){
					return true;
				}
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
		int detect_word_type(wstring &word){
			int num_alphabet = 0;
			int num_number = 0;
			int num_symbol = 0;
			int num_hiragana = 0;
			int num_katakana = 0;
			int num_kanji = 0;
			int num_dash = 0;
			int size = word.size();
			for(wchar_t const target: word){
				if(is_alphabet(target)){
					num_alphabet += 1;
				}else if(is_number(target)){
					num_number += 1;
				}else if(is_dash(target)){
					num_dash += 1;
				}else if(is_hiragana(target)){
					num_hiragana += 1;
				}else if(is_katakana(target)){
					num_katakana += 1;
				}else if(is_kanji(target)){
					num_kanji += 1;
				}else{
					num_symbol += 1;
				}
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
			if(num_hiragana + num_kanji == size){
				return WORDTYPE_KANJI_HIRAGANA;
			}
			if(num_katakana + num_kanji == size){
				return WORDTYPE_KANJI_KATAKANA;
			}
			return WORDTYPE_OTHER;
		}
		// 文字列の指定範囲の単語種判定
		int detect_word_type_substr(wchar_t const* characters, int substr_start, int substr_end){
			int num_alphabet = 0;
			int num_number = 0;
			int num_symbol = 0;
			int num_hiragana = 0;
			int num_katakana = 0;
			int num_kanji = 0;
			int num_dash = 0;
			int size = substr_end - substr_start + 1;
			for(int i = substr_start;i <= substr_end;i++){
				wchar_t target = characters[i];
				if(is_alphabet(target)){
					num_alphabet += 1;
				}else if(is_number(target)){
					num_number += 1;
				}else if(is_dash(target)){
					num_dash += 1;
				}else if(is_hiragana(target)){
					num_hiragana += 1;
				}else if(is_katakana(target)){
					num_katakana += 1;
				}else if(is_kanji(target)){
					num_kanji += 1;
				}else{
					num_symbol += 1;
				}
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
			if(num_hiragana + num_kanji == size){
				return WORDTYPE_KANJI_HIRAGANA;
			}
			if(num_katakana + num_kanji == size){
				return WORDTYPE_KANJI_KATAKANA;
			}
			return WORDTYPE_OTHER;
		}
	}
}