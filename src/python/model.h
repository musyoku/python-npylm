#pragma once
#include <boost/python.hpp>
#include "../npylm/npylm.h"
#include "../npylm/lattice.h"
#include "dataset.h"
#include "dictionary.h"

namespace npylm {
	class Model{
	private:
		void _set_locale();
	public:
		NPYLM* _npylm;
		Lattice* _lattice;			// forward filtering-backward sampling
		Model(Dataset* dataset, int max_word_length);
		Model(Dataset* dataset, 
			int max_word_length, 
			double initial_lambda_a, 
			double initial_lambda_b, 
			double vpylm_beta_stop, 
			double vpylm_beta_pass);
		Model(std::string filename);
		~Model();
		int get_max_word_length();
		void set_initial_lambda_a(double lambda);
		void set_initial_lambda_b(double lambda);
		void set_vpylm_beta_stop(double stop);
		void set_vpylm_beta_pass(double pass);
		double compute_log_forward_probability(std::wstring sentence_str, bool use_scaling = true);
		bool load(std::string filename);
		bool save(std::string filename);
		void parse(std::wstring sentence_str, std::vector<std::wstring> &words);
		boost::python::list python_parse(std::wstring sentence_str);
        boost::python::list python_get_lambda();
	};
}