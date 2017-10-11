#pragma once
#include "common.h"
#include "hash.h"

// <bos>と<eos>は長さが0文字であることに注意

namespace npylm {
	class Sentence {
	public:
		int _num_segments;	// <bos>2つと<eos>1つを含める
		int* _segments;		// 各単語の長さが入る. <bos>2つが先頭に来る
		int* _start;		// <bos>2つが先頭に来る
		wchar_t const* _character_ids; // _sentence_strの各文字
		id* _word_ids;		// <bos>2つと<eos>1つを含める
		wstring _sentence_str;	// 生の文データ
		Sentence(wstring sentence);
		~Sentence();
		Sentence* copy();
		int size();
		int get_num_segments();
		int get_num_segments_without_special_tokens();
		int get_word_length_at(int t);
		id get_word_id_at(int t);
		id get_substr_word_id(int start, int end);
		wstring get_substr_word_str(int start, int end);
		// <bos>を考慮
		wstring get_word_str_at(int t);
		void dump_characters();
		void dump_words();
		// num_segmentsには<bos>や<eos>の数は含めない
		void split(int* segments_without_special_tokens, int num_segments_without_special_tokens);
		void split(vector<int> &segments_without_special_tokens);
	};
} // namespace npylm