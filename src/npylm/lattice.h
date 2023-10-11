#pragma once
#include <vector>
#include "common.h"
#include "npylm.h"

namespace npylm {
	namespace lattice {
		void _init_alpha(double*** &alpha, int size, int max_word_length);
		void _delete_alpha(double*** &alpha, int size, int max_word_length);
	}
	class Lattice {
	private:
		void _allocate_capacity(int max_word_length, int max_sentence_length);
		void _delete_capacity();
	public:
		NPYLM* _npylm;
		id* _word_ids;
		id** _substring_word_id_cache;
		double*** _alpha;		// 前向き確率
		double**** _pw_h;		// キャッシュ
		double* _log_z;			// 正規化定数
		double* _scaling;		// スケーリング係数
		double* _backward_sampling_table;
		int*** _viterbi_backward;
		int _max_word_length;
		int _max_sentence_length;
		Lattice(NPYLM* npylm, int max_word_length, int max_sentence_length);
		~Lattice();
		void reserve(int max_word_length, int max_sentence_length);
		id get_substring_word_id_at_t_k(Sentence* sentence, int t, int k);
		double blocked_gibbs(Sentence* sentence, std::vector<int> &segments, bool use_scaling = true);
		void viterbi_argmax_alpha_t_k_j(Sentence* sentence, int t, int k, int j);
		void viterbi_forward(Sentence* sentence);
		void viterbi_argmax_backward_k_and_j_to_eos(Sentence* sentence, int t, int next_word_length, int &argmax_k, int &argmax_j);
		void viterbi_backward(Sentence* sentence, std::vector<int> &segments);
		void viterbi_decode(Sentence* sentence, std::vector<int> &segments);
		double compute_log_forward_probability(Sentence* sentence, bool use_scaling);
		void _enumerate_forward_variables(Sentence* sentence, double*** alpha, double* scaling, bool use_scaling);
		void _sum_alpha_t_k_j(Sentence* sentence, double*** alpha, double**** pw_h_t_k_j_i, int t, int k, int j, double prod_scaling);
		void _forward_filtering(Sentence* sentenfloatce, double*** alpha, double* scaling, double**** pw_h_t_k_j_i, bool use_scaling = true);
		double _backward_sampling(Sentence* sentence, std::vector<int> &segments, double*** alpha, double**** pw_h_t_k_j_i);
		double _sample_backward_k_and_j(Sentence* sentence, double*** alpha, double**** pw_h_t_k_j_i, int t, int next_word_length, int &sampled_k, int &sampled_j);
	};
} // namespace npylm