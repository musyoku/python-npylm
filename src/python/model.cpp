#include <iostream>
#include "model.h"

namespace npylm {
	Model::Model(Dataset* dataset, 
				int max_word_length, 		// 可能な単語長の最大値. 英語24, 日本語16程度
				double initial_lambda_a, 	// 単語長のポアソン分布のλの事前分布のハイパーパラメータ
				double initial_lambda_b,  	// 単語長のポアソン分布のλの事前分布のハイパーパラメータ
				double vpylm_beta_stop, 	// VPYLMのハイパーパラメータ
				double vpylm_beta_pass){	// VPYLMのハイパーパラメータ
		_set_locale();
		int max_sentence_length = dataset->get_max_sentence_length();
		double vpylm_g0 = 1.0 / (double)dataset->_dict->get_num_characters();
		_npylm = new NPYLM(max_word_length, max_sentence_length, vpylm_g0, initial_lambda_a, initial_lambda_b, vpylm_beta_stop, vpylm_beta_pass);
	}
	Model::Model(std::string filename){
		_set_locale();
		_npylm = new NPYLM();
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
	int Model::get_max_word_length(){
		return _npylm->_max_word_length;
	}
	bool Model::load(std::string filename){
		bool success = false;
		std::ifstream ifs(filename);
		if(ifs.good()){
			boost::archive::binary_iarchive iarchive(ifs);
			iarchive >> *_npylm;
			success = true;
		}
		ifs.close();
		return success;
	}
	bool Model::save(std::string filename){
		bool success = false;
		std::ofstream ofs(filename);
		if(ofs.good()){
			boost::archive::binary_oarchive oarchive(ofs);
			oarchive << *_npylm;
			success = true;
		}
		ofs.close();
		return success;
	}
}