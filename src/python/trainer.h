#pragma once
#include <boost/python.hpp>
#include <cassert>
#include "../npylm/lattice.h"
#include "dataset.h"
#include "model.h"
#include "dictionary.h"

namespace ithmm {
	class Trainer{
	private:
		void _before_viterbi_decode(std::vector<Node*> &nodes);
		void _after_viterbi_decode();
		void _before_compute_log_p_dataset(std::vector<Node*> &nodes);
		void _after_compute_log_p_dataset();
		double _compute_log_p_dataset(std::vector<std::vector<Word*>> &dataset);
		double _compute_log2_p_dataset(std::vector<std::vector<Word*>> &dataset);
		double _compute_perplexity(std::vector<std::vector<Word*>> &dataset);
		std::vector<int> _rand_indices;
		Dataset* _dataset;
		Dictionary* _dict;
		Model* _model;
		double** _forward_table;	// 前向き確率計算用
		double** _decode_table;		// viterbiデコーディング用
		Lattice* _lattice;			// forward filtering-backward sampling
	public:
		Trainer(Dataset* dataset, Model* model);
		void remove_all_data();
		void gibbs();
		double compute_log_p_dataset_train();
		double compute_log_p_dataset_dev();
		double compute_log2_p_dataset_train();
		double compute_log2_p_dataset_dev();
		double compute_perplexity_train();
		double compute_perplexity_dev();
		void update_hyperparameters();
		void set_model(Model* model);
	};
}