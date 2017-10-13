#include <boost/python.hpp>
#include <cassert>
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
	void Corpus::add_sentence(std::wstring sentence_str){
		_sentence_str_list.push_back(sentence_str);
	}
	int Corpus::get_num_sentences(){
		return _sentence_str_list.size();
	}
}