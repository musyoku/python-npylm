#pragma once
#include <boost/python.hpp>
#include "../ithmm/ithmm.h"
#include "dataset.h"
#include "dictionary.h"

namespace ithmm {
	class Model{
	private:
		void _set_locale();
	public:
		NPYLM* _npylm;
		Model(Dataset* dataset, 
			int max_word_length, 
			double initial_lambda_a, 
			double initial_lambda_b, 
			double vpylm_beta_stop, 
			double vpylm_beta_pass);
		Model(std::string filename);
		~Model();
		bool load(std::string filename);
		bool save(std::string filename);
	};
}