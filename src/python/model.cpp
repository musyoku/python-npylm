#include "model.h"

namespace npylm {
	Model::Model(Dataset* dataset, int max_word_length){
		_set_locale();
		int max_sentece_length = dataset->get_max_sentence_length();
		double vpylm_g0 = 1.0 / (double)dataset->_dict->get_num_characters();
	}
	Model::Model(std::string filename){
		_set_locale();
		_ithmm = new iTHMM();
		if(load(filename) == false){
			std::cout << filename << " not found." << std::endl;
			exit(0);
		}
	}
	Model::~Model(){
		delete _npylm;
	}
	// 日本語周り
	void Model::_set_locale(){
		setlocale(LC_CTYPE, "ja_JP.UTF-8");
		std::ios_base::sync_with_stdio(false);
		std::locale default_loc("ja_JP.UTF-8");
		std::locale::global(default_loc);
		std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype); //※
		std::wcout.imbue(ctype_default);
		std::wcin.imbue(ctype_default);
	}
}