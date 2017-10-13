#pragma once
#include <vector> 
#include "common.h"
#include "lm/node.h"
#include "lm/vpylm.h"
#include "lm/hpylm.h"

namespace npylm {
	// character_idsのsubstr_char_t_startからsubstr_char_t_endまでの文字列を<bow>と<eow>で挟んでwrapped_character_idsの先頭に格納
	void wrap_bow_eow(wchar_t const* character_ids, int substr_char_t_start, int substr_char_t_end, wchar_t* wrapped_character_ids);
	double factorial(double n);
	class NPYLM {
	private:
		void _init_cache(int max_word_length, int max_sentence_length);
		void _delete_cache();
	public:
		lm::HPYLM* _hpylm;	// 単語n-gram
		lm::VPYLM* _vpylm;	// 文字n-gram
		hashmap<id, std::vector<std::vector<int>>> _prev_depths_for_token_at_table;
		hashmap<id, double> _g0_cache;
		hashmap<id, double> _vpylm_g0_cache;
		double* _lambda_for_type;
		double* _pk_vpylm;	// 文字n-gramから長さkの単語が生成される確率
		int _max_word_length;
		int _max_sentence_length;
		double _lambda_a;
		double _lambda_b;
		// 計算高速化用
		double* _hpylm_parent_pw_cache;
		wchar_t* _character_ids;
		NPYLM(){}
		NPYLM(int max_word_length, int max_sentence_length, double g0, double initial_lambda_a, double initial_lambda_b);
		~NPYLM();
		void set_vpylm_g0(double g0);
		void set_lambda_prior(double a, double b);
		void sample_lambda_with_initial_params();
		bool add_customer_at_time_t(Sentence* sentence, int t);
		void vpylm_add_customers(wchar_t const* character_ids, int substr_char_t_start, int substr_char_t_end, wchar_t* wrapped_character_ids, std::vector<int> &prev_depths);
		bool remove_customer_at_time_t(Sentence* sentence, int t);
		void vpylm_remove_customers(wchar_t const* character_ids, int substr_char_t_start, int substr_char_t_end, wchar_t* wrapped_character_ids, std::vector<int> &prev_depths);
		lm::Node<id>* find_node_by_tracing_back_context_from_time_t(id const* word_ids, int word_ids_length, int word_t_index, bool generate_node_if_needed, bool return_middle_node);
		lm::Node<id>* find_node_by_tracing_back_context_from_time_t(Sentence* sentence, int word_t_index, double* parent_pw_cache, int generate_node_if_needed, bool return_middle_node);
		lm::Node<id>* find_node_by_tracing_back_context_from_time_t(
			wchar_t const* character_ids, int character_ids_length, 
			id const* word_ids, int word_ids_length, 
			int word_t_index, int substr_char_t_start, int substr_char_t_end, 
			double* parent_pw_cache, bool generate_node_if_needed, bool return_middle_node);
		// word_idは既知なので再計算を防ぐ
		double compute_g0_substring_at_time_t(wchar_t const* character_ids, int character_ids_length, int substr_char_t_start, int substr_char_t_end, id word_t_id);
		double compute_poisson_k_lambda(unsigned int k, double lambda);
		double compute_Pk_vpylm(int k);
		void sample_pitman_yor_hyperparameters();
		double compute_log_Pw(Sentence* sentence);
		double compute_Pw(Sentence* sentence);
		double compute_Pw_h(Sentence* sentence, int word_t_index);
		double compute_Pw_h(
			wchar_t const* character_ids, int character_ids_length, 
			id const* word_ids, int word_ids_length, 
			int word_t_index, int substr_char_t_start, int substr_char_t_end);
		template <class Archive>
		void serialize(Archive &archive, unsigned int version);
	};
} // namespace npylm

namespace boost { namespace serialization {
	template<class Archive>
	void save(Archive &archive, const npylm::NPYLM &npylm, unsigned int version);
	template<class Archive>
	void load(Archive &archive, npylm::NPYLM &npylm, unsigned int version);
}} // namespace boost::serialization