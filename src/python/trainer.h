#pragma once
#include <boost/python.hpp>
#include <vector>
#include <cassert>
#include "dataset.h"
#include "model.h"
#include "dictionary.h"

namespace npylm {
	class Trainer{
	private:
		std::vector<int> _rand_indices_train;
		std::vector<int> _rand_indices_dev;
		Dataset* _dataset;
		Dictionary* _dict;
		Model* _model;
		double* _vpylm_sampling_probability_table;
		wchar_t* _vpylm_sampling_id_table;
		bool _always_accept_new_segmentation;
		bool* _added_to_npylm_train;
		int _num_segmentation_rejection;
		int _num_segmentation_acceptance;
		void _print_segmentation(int num_to_print, std::vector<Sentence*> &dataset, std::vector<int> &rand_indices);
		double _compute_perplexity(std::vector<Sentence*> &dataset);
	public:
		Trainer(Dataset* dataset, Model* model, bool always_accept_new_segmentation);
		void remove_all_data();
		void gibbs();
		void sample_hpylm_vpylm_hyperparameters();
		void sample_lambda();
		wchar_t sample_word_from_vpylm_given_context(wchar_t* context_ids, int context_length, int sample_t, bool skip_eow = false);
		void update_p_k_given_vpylm();
		double compute_perplexity_train();
		double compute_perplexity_dev();
		void print_segmentation_train(int num_to_print);
		void print_segmentation_dev(int num_to_print);
	};
}