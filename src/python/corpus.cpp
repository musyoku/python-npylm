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
	int Corpus::get_num_true_segmentations(){
		return _word_sequence_list.size();
	}
	void Corpus::_before_add_true_segmentation(boost::python::list &py_word_str_list, std::vector<std::wstring> &word_str_vec){
		int num_words = boost::python::len(py_word_str_list);
		for(int i = 0;i < num_words;i++){
			std::wstring word = boost::python::extract<std::wstring>(py_word_str_list[i]);
			word_str_vec.push_back(word);
		}
	}
	void Corpus::python_add_true_segmentation(boost::python::list py_word_str_list){
		std::vector<std::wstring> word_str_vec;
		_before_add_true_segmentation(py_word_str_list, word_str_vec);
		assert(word_str_vec.size() > 0);
		add_true_segmentation(word_str_vec);
	}
	void Corpus::add_true_segmentation(std::vector<std::wstring> &word_str_vec){
		assert(word_str_vec.size() > 1);
		_word_sequence_list.push_back(word_str_vec);
	}
}