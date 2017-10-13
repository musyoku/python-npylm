#include <iostream>
#include <fstream>
#include <unordered_set>
#include "dataset.h"
#include "../ithmm/utils.h"
#include "../ithmm/sampler.h"

namespace ithmm {
	Dataset::Dataset(Corpus* corpus, double train_split, int seed){
		_dict = new Dictionary();
		_corpus = corpus;
		std::vector<int> rand_indices;
		for(int i = 0;i < corpus->get_num_sentences();i++){
			rand_indices.push_back(i);
		}
		sampler::set_seed(seed);
		shuffle(rand_indices.begin(), rand_indices.end(), sampler::mt);	// データをシャッフル
		train_split = std::min(1.0, std::max(0.0, train_split));
		int num_train_data = corpus->get_num_sentences() * train_split;
		for(int i = 0;i < rand_indices.size();i++){
			std::wstring &sentence_str = corpus->_sentence_str_list[rand_indices[i]];
			if(i < num_train_data){
				_add_words_to_dataset(sentence_str, _sentence_sequences_train, corpus);
			}else{
				_add_words_to_dataset(sentence_str, _sentence_sequences_dev, corpus);
			}
		}
	}
	Dataset::~Dataset(){
		for(int n = 0;n < _sentence_sequences_train.size();n++){
			Sentence* sentence = _sentence_sequences_train[n];
			delete sentence;
		}
		for(int n = 0;n < _sentence_sequences_dev.size();n++){
			Sentence* sentence = _sentence_sequences_dev[n];
			delete sentence;
		}
		delete _dict;
	}
	void Dataset::_add_words_to_dataset(std::wstring &sentence_str, std::vector<Sentence*> &dataset){
		assert(sentence_str.size() > 0);
		for(wchar_t character: sentence_str){
			_dict->add_character(character);
		}
		Sentence* sentence = new Sentence(sentence_str);
		dataset.push_back(words);
	}
	int Dataset::get_max_sentence_length(){
		return _corpus->get_max_sentence_length();
	}
	Dictionary &Dataset::get_dict_obj(){
		return *_dict;
	}
}