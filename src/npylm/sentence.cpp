#include <iostream>
#include "sentence.h"

// <bos>と<eos>は長さが0文字であることに注意

namespace npylm {
	Sentence::Sentence(wstring sentence){
		_sentence_str = sentence;
		_character_ids = _sentence_str.data();
		_word_ids = new id[size() + 3];
		_segments = new int[size() + 3];
		_start = new int[size() + 3];
		for(int i = 0;i < size() + 3;i++){
			_word_ids[i] = 0;
			_segments[i] = 0;
		}
		_word_ids[0] = ID_BOS;
		_word_ids[1] = ID_BOS;
		_word_ids[2] = get_substr_word_id(0, size() - 1);
		_word_ids[3] = ID_EOS;
		_segments[0] = 1;
		_segments[1] = 1;
		_segments[2] = _sentence_str.size();
		_segments[3] = 1;
		_start[0] = 0;
		_start[1] = 0;
		_start[2] = 0;
		_start[3] = _sentence_str.size();
		_num_segments = 4;
	}
	Sentence::~Sentence(){
		delete[] _segments;
		delete[] _start;
		delete[] _word_ids;
	}
	Sentence* Sentence::copy(){
		Sentence* sentence = new Sentence(_sentence_str);
		return sentence;
	}
	int Sentence::size(){
		return _sentence_str.size();
	}
	int Sentence::get_num_segments(){
		return _num_segments;
	}
	int Sentence::get_num_segments_without_special_tokens(){
		return _num_segments - 3;
	}
	int Sentence::get_word_length_at(int t){
		assert(t < _num_segments);
		return _segments[t];
	}
	id Sentence::get_word_id_at(int t){
		assert(t < _num_segments);
		return _word_ids[t];
	}
	id Sentence::get_substr_word_id(int start, int end){
		return hash_substring_ptr(_character_ids, start, end);
	}
	wstring Sentence::get_substr_word_str(int start, int end){
		wstring str(_sentence_str.begin() + start, _sentence_str.begin() + end + 1);
		return str;
	}
	// <bos>を考慮
	wstring Sentence::get_word_str_at(int t){
		assert(t < _num_segments);
		if(t < 2){
			return L"<bos>";
		}
		assert(t < _num_segments - 1);
		wstring str(_sentence_str.begin() + _start[t], _sentence_str.begin() + _start[t] + _segments[t]);
		return str;
	}
	void Sentence::dump_characters(){
		for(int i = 0;i < size();i++){
			cout << _character_ids[i] << ",";
		}
		cout << endl;
	}
	void Sentence::dump_words(){
		wcout << L" / ";
		for(int i = 2;i < _num_segments - 1;i++){
			for(int j = 0;j < _segments[i];j++){
				wcout << _character_ids[j + _start[i]];
			}
			wcout << L" / ";
		}
		wcout << endl;
	}
	// num_segmentsには<bos>や<eos>の数は含めない
	void Sentence::split(int* segments_without_special_tokens, int num_segments_without_special_tokens){
		int start = 0;
		int n = 0;
		int sum = 0;
		for(;n < num_segments_without_special_tokens;n++){
			if(segments_without_special_tokens[n] == 0){
				assert(n > 0);
				break;
			}
			sum += segments_without_special_tokens[n];
			_segments[n + 2] = segments_without_special_tokens[n];
			_word_ids[n + 2] = get_substr_word_id(start, start + segments_without_special_tokens[n] - 1);
			_start[n + 2] = start;
			start += segments_without_special_tokens[n];
		}
		assert(sum == _sentence_str.size());
		_segments[n + 2] = 1;
		_word_ids[n + 2] = ID_EOS;
		_start[n + 2] = _start[n + 1];
		n++;
		for(;n < _sentence_str.size();n++){
			_segments[n + 2] = 0;
			_start[n + 2] = 0;
		}
		_num_segments = num_segments_without_special_tokens + 3;
	}
	void Sentence::split(vector<int> &segments_without_special_tokens){
		int num_segments_without_special_tokens = segments_without_special_tokens.size();
		int start = 0;
		int n = 0;
		int sum = 0;
		for(;n < num_segments_without_special_tokens;n++){
			assert(segments_without_special_tokens[n] > 0);
			sum += segments_without_special_tokens[n];
			_segments[n + 2] = segments_without_special_tokens[n];
			_word_ids[n + 2] = get_substr_word_id(start, start + segments_without_special_tokens[n] - 1);
			_start[n + 2] = start;
			start += segments_without_special_tokens[n];
		}
		assert(sum == _sentence_str.size());
		_segments[n + 2] = 1;
		_word_ids[n + 2] = ID_EOS;
		_start[n + 2] = _start[n + 1];
		n++;
		for(;n < _sentence_str.size();n++){
			_segments[n + 2] = 0;
			_start[n + 2] = 0;
		}
		_num_segments = num_segments_without_special_tokens + 3;
	}
} // namespace npylm