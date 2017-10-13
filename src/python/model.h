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
		iTHMM* _ithmm;
		Model(Dataset* dataset);
		Model(Dataset* dataset, int depth_limit);
		Model(Dataset* dataset, double alpha, double gamma, double lambda_alpha, double lambda_gamma, double conc_h, double conc_v, double tau0, double tau1, int depth_limit);
		Model(std::string filename);
		~Model();
		double get_alpha();
		double get_gamma();
		double get_lambda_alpha();
		double get_lambda_gamma();
		double get_concentration_v();
		double get_concentration_h();
		double get_tau0();
		double get_tau1();
		void set_alpha(double alpha);
		void set_gamma(double gamma);
		void set_lambda_alpha(double lambda_alpha);
		void set_lambda_gamma(double lambda_gamma);
		void set_concentration_v(double concentration);
		void set_concentration_h(double concentration);
		void set_tau0(double tau0);
		void set_tau1(double tau1);
		bool load(std::string filename);
		bool save(std::string filename);
		void enumerate_all_states(std::vector<Node*> &nodes);
		void precompute_all_stick_lengths(std::vector<Node*> &all_states);
		boost::python::list python_viterbi_decode(boost::python::list py_word_ids);
		boost::python::list python_get_tags();
		void viterbi_decode(std::vector<Word*> &data, std::vector<Node*> &all_states, std::vector<Node*> &sampled_state_sequence, double** forward_table, double** decode_table);
		double compute_p_sentence(std::vector<Word*> &data, std::vector<Node*> &states, double** forward_table);
		void show_assigned_words_for_each_tag(Dictionary* dict, int number_to_show_for_each_tag, bool show_probability = true);
		void show_assigned_words_and_probability_for_each_tag(Dictionary* dict, int number_to_show_for_each_tag);
		void show_hpylm_for_each_tag(Dictionary* dict);
		void show_sticks();
		void _show_stick(Node* node_in_structure);
		void update_hyperparameters();
	};
}