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
		double*** _normalized_alpha;	// 正規化後の前向き確率
		double**** _pw_h;		// キャッシュ
		double* _log_z;			// 正規化定数
		double* _backward_sampling_table;
		int*** _viterbi_backward;
		int _max_word_length;
		int _max_sentence_length;
		Lattice(NPYLM* npylm, int max_word_length, int max_sentence_length);
		~Lattice();
		void reserve(int max_word_length, int max_sentence_length);
		id get_substring_word_id_at_t_k(Sentence* sentence, int t, int k);
		void sum_alpha_t_k_j(Sentence* sentence, int t, int k, int j, double*** forward_table);
		void forward_filtering(Sentence* sentence, bool normalize);
		void backward_sampling(Sentence* sentence, std::vector<int> &segments, double*** backward_alpha);
		void sample_backward_k_and_j(Sentence* sentence, int t, int next_word_length, int &sampled_k, int &sampled_j, double*** backward_alpha);
		void blocked_gibbs(Sentence* sentence, std::vector<int> &segments, bool normalize = true);
		void viterbi_argmax_alpha_t_k_j(Sentence* sentence, int t, int k, int j);
		void viterbi_forward(Sentence* sentence);
		void viterbi_argmax_backward_k_and_j_to_eos(Sentence* sentence, int t, int next_word_length, int &argmax_k, int &argmax_j);
		void viterbi_backward(Sentence* sentence, std::vector<int> &segments);
		void viterbi_decode(Sentence* sentence, std::vector<int> &segments);
		double compute_forward_probability(Sentence* sentence, bool normalize = true);
	};
} // namespace npylm