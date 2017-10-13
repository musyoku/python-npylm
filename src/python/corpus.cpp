#include <fstream>
#include "corpus.h"

namespace npylm {
	void Corpus::add_textfile(std::string filename){
		std::wifstream ifs(filename.c_str());
		std::wstring sentence_str;
		assert(ifs.fail() == false);
		while (getline(ifs, sentence_str) && !sentence_str.empty()){
			if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
				return;
			}
			add_sentence(sentence_str);
		}
	}
	void Corpus::add_sentence(std::wstring sentence){
		if(_max_sentence_length == 0 || sentence.size() > _max_sentence_length){
			_max_sentence_length = sentence.size();
		}
		_sentence_str_list.push_back(sentence_str);
	}
	int Corpus::get_num_sentences(){
		return _sentence_str_list.size();
	}
	int Corpus::get_max_sentence_length(){
		return _max_sentence_length;
	}
}