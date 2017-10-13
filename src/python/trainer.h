#pragma once
#include <boost/python.hpp>
#include <vector>
#include <cassert>
#include "../npylm/lattice.h"
#include "dataset.h"
#include "model.h"
#include "dictionary.h"

namespace npylm {
	class Trainer{
	private:
		std::vector<int> _rand_indices_train;
		Dataset* _dataset;
		Dictionary* _dict;
		Model* _model;
		double** _forward_table;	// 前向き確率計算用
		double** _decode_table;		// viterbiデコーディング用
		Lattice* _lattice;			// forward filtering-backward sampling
		double* _vpylm_sampling_probability_table;
		wchar_t* _vpylm_sampling_id_table;
		bool _always_accept_new_segmentation;
		bool* _added_npylm_train;
		int _num_segmentation_rejection;
		int _num_segmentation_acceptance;
	public:
		Trainer(Dataset* dataset, Model* model, bool always_accept_new_segmentation);
		void remove_all_data();
		void gibbs();
		void sample_hpylm_vpylm_hyperparameters();
		void sample_lambda();
		wchar_t sample_word_from_vpylm_given_context(wchar_t* context_ids, int context_length, int sample_t, bool skip_eow = false);
		void update_p_k_given_vpylm();
	};
}