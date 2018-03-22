#include <algorithm>
#include <iostream>
#include "hash.h"
#include "lattice.h"
#include "sampler.h"

// ＿人人人人人人人人人人人人人人人人人人人人人人人人人人人人＿
// ＞　Latticeでは文字のインデックスtが1から始まることに注意　＜
// ￣Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y￣

namespace npylm {
	namespace lattice {
		template<typename T>
		void _init_array(T* &array, int size_i){
			array = new T[size_i];
			assert(array != NULL);
		}
		template<typename T>
		void _init_array(T** &array, int size_i, int size_j){
			array = new T*[size_i];
			assert(array != NULL);
			for(int i = 0;i < size_i;i++){
				array[i] = new T[size_j];
				assert(array[i] != NULL);
			}
		}
		template<typename T>
		void _init_array(T*** &array, int size_i, int size_j, int size_k){
			array = new T**[size_i];
			assert(array != NULL);
			for(int i = 0;i < size_i;i++){
				array[i] = new T*[size_j];
				assert(array[i] != NULL);
				for(int j = 0;j < size_j;j++){
					array[i][j] = new T[size_k];
					assert(array[i][j] != NULL);
				}
			}
		}
		template<typename T>
		void _init_array(T**** &array, int size_i, int size_j, int size_k, int size_l){
			array = new T***[size_i];
			assert(array != NULL);
			for(int i = 0;i < size_i;i++){
				array[i] = new T**[size_j];
				assert(array[i] != NULL);
				for(int j = 0;j < size_j;j++){
					array[i][j] = new T*[size_k];
					assert(array[i][j] != NULL);
					for(int k = 0;k < size_k;k++){
						array[i][j][k] = new T[size_l];
						assert(array[i][j][k] != NULL);
					}
				}
			}
		}
		template<typename T>
		void _delete_array(T* &array, int size_i){
			if(array == NULL){
				return;
			}
			delete[] array;
		}
		template<typename T>
		void _delete_array(T** &array, int size_i, int size_j){
			if(array == NULL){
				return;
			}
			for(int i = 0;i < size_i;i++){
				delete[] array[i];
			}
			delete[] array;
		}
		template<typename T>
		void _delete_array(T*** &array, int size_i, int size_j, int size_k){
			if(array == NULL){
				return;
			}
			for(int i = 0;i < size_i;i++){
				for(int j = 0;j < size_j;j++){
					delete[] array[i][j];
				}
				delete[] array[i];
			}
			delete[] array;
		}
		template<typename T>
		void _delete_array(T**** &array, int size_i, int size_j, int size_k, int size_l){
			if(array == NULL){
				return;
			}
			for(int i = 0;i < size_i;i++){
				for(int j = 0;j < size_j;j++){
					for(int k = 0;k < size_j;k++){
						delete[] array[i][j][k];
					}
					delete[] array[i][j];
				}
				delete[] array[i];
			}
			delete[] array;
		}
		template<typename T>
		void _init_table(T*** &table, int size, int max_word_length){
			table = new T**[size];
			assert(table != NULL);
			for(int t = 0;t < size;t++){
				table[t] = new T*[max_word_length + 1];
				assert(table[t] != NULL);
				for(int k = 0;k < max_word_length + 1;k++){
					table[t][k] = new T[max_word_length + 1];
					assert(table[t][k] != NULL);
					for(int j = 0;j < max_word_length + 1;j++){
						table[t][k][j] = 0;
					}
				}
			}
		}
		template<typename T>
		void _delete_table(T*** &table, int size, int max_word_length){
			if(table == NULL){
				return;
			}
			for(int t = 0;t < size;t++){
				for(int k = 0;k < max_word_length + 1;k++){
					delete[] table[t][k];
				}
				delete[] table[t];
			}
			delete[] table;
			table = NULL;
		}
	}
	Lattice::Lattice(NPYLM* npylm, int max_word_length, int max_sentence_length){
		_npylm = npylm;
		_word_ids = new id[3];	// 3-gram
		_alpha = NULL;
		_pw_h = NULL;
		_scaling = NULL;
		_backward_sampling_table = NULL;
		_viterbi_backward = NULL;
		_substring_word_id_cache = NULL;
		_allocate_capacity(max_word_length, max_sentence_length);
	}
	Lattice::~Lattice(){
		delete[] _word_ids;
		_delete_capacity();
	}
	void Lattice::reserve(int max_word_length, int max_sentence_length){
		if(max_word_length <= _max_word_length && max_sentence_length <= _max_sentence_length){
			return;
		}
		_delete_capacity();
		_allocate_capacity(max_word_length, max_sentence_length);
		_max_word_length = max_word_length;
		_max_sentence_length = max_sentence_length;
	}
	void Lattice::_allocate_capacity(int max_word_length, int max_sentence_length){
		_max_word_length = max_word_length;
		_max_sentence_length = max_sentence_length;
		// 必要な配列の初期化
		int seq_capacity = max_sentence_length + 1;
		int word_capacity = max_word_length + 1;
		// 前向き確率のスケーリング係数
		lattice::_init_array(_scaling, seq_capacity + 1);
		// ビタビアルゴリズム用
		lattice::_init_array(_viterbi_backward, seq_capacity, word_capacity, word_capacity);
		// 後ろ向きアルゴリズムでkとjをサンプリングするときの確率表
		lattice::_init_array(_backward_sampling_table, word_capacity * word_capacity);
		// 前向き確率
		lattice::_init_array(_alpha, seq_capacity + 1, word_capacity, word_capacity);
		// 3-gram確率のキャッシュ
		lattice::_init_array(_pw_h, seq_capacity, word_capacity, word_capacity, word_capacity);
		// 部分文字列のIDのキャッシュ
		lattice::_init_array(_substring_word_id_cache, seq_capacity, word_capacity);
	}
	void Lattice::_delete_capacity(){
		int seq_capacity = _max_sentence_length + 1;
		int word_capacity = _max_word_length + 1;
		lattice::_delete_array(_scaling, seq_capacity + 1);
		lattice::_delete_array(_viterbi_backward, seq_capacity, word_capacity, word_capacity);
		lattice::_delete_array(_backward_sampling_table, word_capacity * word_capacity);
		lattice::_delete_array(_alpha, seq_capacity + 1, word_capacity, word_capacity);
		lattice::_delete_array(_pw_h, seq_capacity, word_capacity, word_capacity, word_capacity);
		lattice::_delete_array(_substring_word_id_cache, seq_capacity, word_capacity);
	}
	id Lattice::get_substring_word_id_at_t_k(Sentence* sentence, int t, int k){
		assert(0 <= t && t < _max_sentence_length + 1);
		assert(1 <= k && k < _max_sentence_length + 1);
		if(t == 0){
			return ID_BOS;
		}
		id word_id = _substring_word_id_cache[t][k];
		if(word_id == 0){
			word_id = sentence->get_substr_word_id(t - k, t - 1);
			_substring_word_id_cache[t][k] = word_id;
		}
		return word_id;
	}
	// alpha[t-k][j][i]自体は正規化されている場合があるが、alpha[t][k][j]の正規化はここでは行わない
	void Lattice::_sum_alpha_t_k_j(Sentence* sentence, double*** alpha, double**** pw_h_tkji, int t, int k, int j, double prod_scaling){
		id word_k_id = get_substring_word_id_at_t_k(sentence, t, k);
		wchar_t const* characters = sentence->_characters;
		int character_ids_length = sentence->size();
		assert(t <= _max_sentence_length + 1);
		assert(k <= _max_word_length);
		assert(j <= _max_word_length);
		assert(t - k >= 0);
		// <bos>から生成されている場合
		if(j == 0){
			_word_ids[0] = ID_BOS;
			_word_ids[1] = ID_BOS;
			_word_ids[2] = word_k_id;
			double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t - k, t - 1);
			assert(pw_h > 0);
			alpha[t][k][0] = pw_h * prod_scaling;
			pw_h_tkji[t][k][0][0] = pw_h;
			return;
		}
		// i=0に相当
		if(t - k - j == 0){
			id word_j_id = get_substring_word_id_at_t_k(sentence, t - k, j);
			_word_ids[0] = ID_BOS;
			_word_ids[1] = word_j_id;
			_word_ids[2] = word_k_id;
			double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t - k, t - 1);
			assert(pw_h > 0);
			assert(alpha[t - k][j][0] > 0);
			alpha[t][k][j] = pw_h * alpha[t - k][j][0] * prod_scaling;
			assert(alpha[t][k][j] > 0);
			pw_h_tkji[t][k][j][0] = pw_h;
			return;
		}
		// それ以外の場合は周辺化
		double sum = 0;
		for(int i = 1;i <= std::min(t - k - j, _max_word_length);i++){
			id word_i_id = get_substring_word_id_at_t_k(sentence, t - k - j, i);
			id word_j_id = get_substring_word_id_at_t_k(sentence, t - k, j);
			_word_ids[0] = word_i_id;
			_word_ids[1] = word_j_id;
			_word_ids[2] = word_k_id;
			double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t - k, t - 1);
			assert(i <= _max_word_length);
			assert(alpha[t - k][j][i] > 0);
			double value = pw_h * alpha[t - k][j][i];
			assert(pw_h > 0);

			#ifdef __DEBUG__
				if(value <= 0){
					std::cout << value << std::endl;
					std::cout << pw_h << std::endl;
					std::cout << alpha[t - k][j][i] << std::endl;
					std::cout << t << ", " << k << ", " << j << ", " << i << std::endl;
				}
			#endif

			assert(value > 0);
			sum += value;
			pw_h_tkji[t][k][j][i] = pw_h;
		}
		assert(sum > 0);
		alpha[t][k][j] = sum * prod_scaling;
	}
	void Lattice::_forward_filtering(Sentence* sentence, double*** alpha, double* scaling, double**** pw_h_tkji, bool use_scaling){
		alpha[0][0][0] = 1;
		for(int t = 1;t <= sentence->size();t++){
			double prod_scaling = 1;
			for(int k = 1;k <= std::min(t, _max_word_length);k++){
				if(use_scaling == true && k > 1){
					prod_scaling *= scaling[t - k + 1];
				}
				for(int j = (t - k == 0) ? 0 : 1;j <= std::min(t - k, _max_word_length);j++){
					alpha[t][k][j] = 0;
					_sum_alpha_t_k_j(sentence, alpha, pw_h_tkji, t, k, j, prod_scaling);
				}
			}
			// スケーリング
			if(use_scaling == true){
				double sum_alpha = 0;
				for(int k = 1;k <= std::min(t, _max_word_length);k++){
					for(int j = (t - k == 0) ? 0 : 1;j <= std::min(t - k, _max_word_length);j++){
						sum_alpha += alpha[t][k][j];
					}
				}
				assert(sum_alpha > 0);
				scaling[t] = 1.0 / sum_alpha;
				for(int k = 1;k <= std::min(t, _max_word_length);k++){
					for(int j = (t - k == 0) ? 0 : 1;j <= std::min(t - k, _max_word_length);j++){
						alpha[t][k][j] *= scaling[t];
					}
				}
			}
		}
	}
	void Lattice::_backward_sampling(Sentence* sentence, std::vector<int> &segments, double*** alpha, double**** pw_h_tkji){
		segments.clear();
		int k = 0;
		int j = 0;
		int sum = 0;
		int t = sentence->size();
		_sample_backward_k_and_j(sentence, alpha, pw_h_tkji, t, 1, k, j);
		assert(k <= _max_word_length);
		segments.push_back(k);
		if(j == 0 && k == t){	// 文章すべてが1単語になる場合
			return;
		}
		assert(k > 0 && j > 0);
		assert(j <= _max_word_length);
		segments.push_back(j);
		// std::cout << "<- " << k << std::endl;
		// std::cout << "<- " << j << std::endl;
		t -= k;
		t -= j;
		sum += k + j;
		int next_word_length = j;
		while(t > 0){
			if(t == 1){
				k = 1;
				j = 0;
			}else{
				_sample_backward_k_and_j(sentence, alpha, pw_h_tkji, t, next_word_length, k, j);
				assert(k > 0);
				assert(k <= _max_word_length);
			}
			segments.push_back(k);
			// std::cout << "<- " << k << std::endl;
			t -= k;
			if(j == 0){
				assert(t == 0);
			}else{
				assert(j <= _max_word_length);
				segments.push_back(j);
				// std::cout << "<- " << j << std::endl;
				t -= j;
			}
			sum += k + j;
			next_word_length = j;
		}
		assert(t == 0);
		assert(sum == sentence->size());
		reverse(segments.begin(), segments.end());
	}
	void Lattice::_sample_backward_k_and_j(Sentence* sentence, double*** alpha, double**** pw_h_tkji, int t, int next_word_length, int &sampled_k, int &sampled_j){
		int table_index = 0;
		wchar_t const* characters = sentence->_characters;
		int character_ids_length = sentence->size();
		double sum_p = 0;
		int limit_k = std::min(t, _max_word_length);
		for(int k = 1;k <= limit_k;k++){
			for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
				id word_j_id = get_substring_word_id_at_t_k(sentence, t - k, j);
				id word_k_id = get_substring_word_id_at_t_k(sentence, t, k);
				id word_t_id = ID_EOS;
				if(t < sentence->size()){
					assert(t + next_word_length <= sentence->size());
					assert(next_word_length > 0);
					word_t_id = get_substring_word_id_at_t_k(sentence, t + next_word_length, next_word_length);
					// id word_id = sentence->get_substr_word_id(t - 1, t + next_word_length - 2);
					// assert(word_t_id == word_id);
				}
				_word_ids[0] = word_j_id;
				_word_ids[1] = word_k_id;
				_word_ids[2] = word_t_id;
				double pw_h = 0;
				if(t == sentence->size()){	// <eos>に接続する確率からサンプリング
					pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t, t);
				}else{
					pw_h = pw_h_tkji[t + next_word_length][next_word_length][k][j];
					#ifdef __DEBUG__
						double pw_h2 = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t, t + next_word_length - 1);
						if(pw_h != pw_h2){
							std::cout << "t = " << t << ", k = " << k << ", j = " << j << std::endl;
							std::cout << "next_word_length = " << next_word_length << std::endl;
							std::cout << "size = " << sentence->size() << std::endl;
						}
						assert(pw_h == pw_h2);
					#endif
				}
				assert(alpha[t][k][j] > 0);
				double p = pw_h * alpha[t][k][j];
				assert(p > 0);
				sum_p += p;
				_backward_sampling_table[table_index] = p;
				table_index++;
				// p_k.push_back(p);
			}
			if(t - k == 0){
				id word_j_id = ID_BOS;
				id word_k_id = get_substring_word_id_at_t_k(sentence, t, k);
				id word_t_id = ID_EOS;
				if(t < sentence->size()){
					assert(t + next_word_length <= sentence->size());
					assert(next_word_length > 0);
					word_t_id = get_substring_word_id_at_t_k(sentence, t + next_word_length, next_word_length);
					// id word_id = sentence->get_substr_word_id(t - 1, t + next_word_length - 2);
					// assert(word_t_id == word_id);
				}
				_word_ids[0] = word_j_id;
				_word_ids[1] = word_k_id;
				_word_ids[2] = word_t_id;
				double pw_h = 0;
				if(t == sentence->size()){	// <eos>に接続する確率からサンプリング
					pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t, t);
				}else{
					pw_h = pw_h_tkji[t + next_word_length][next_word_length][k][0];
					#ifdef __DEBUG__
						double pw_h2 = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t, t + next_word_length - 1);
						if(pw_h != pw_h2){
							std::cout << "t = " << t << ", k = " << k << ", j = " << 0 << std::endl;
							std::cout << "next_word_length = " << next_word_length << std::endl;
							std::cout << "size = " << sentence->size() << std::endl;
						}
						assert(pw_h == pw_h2);
					#endif
				}
				assert(alpha[t][k][0] > 0);
				double p = pw_h * alpha[t][k][0];
				assert(p > 0);
				sum_p += p;
				_backward_sampling_table[table_index] = p;
				table_index++;
			}
		}
		assert(table_index > 0);
		assert(table_index <= _max_word_length * _max_word_length);
		double normalizer = 1.0 / sum_p;
		double r = sampler::uniform(0, 1);
		int i = 0;
		double stack = 0;
		for(int k = 1;k <= limit_k;k++){
			for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
				assert(i < table_index);
				assert(_backward_sampling_table[i] > 0);
				stack += _backward_sampling_table[i] * normalizer;
				if(r < stack){
					sampled_k = k;
					sampled_j = j;
					return;
				}
				i++;
			}
			if(t - k == 0){
				assert(i < table_index);
				assert(_backward_sampling_table[i] > 0);
				stack += _backward_sampling_table[i] * normalizer;
				if(r < stack){
					sampled_k = k;
					sampled_j = 0;
					return;
				}
				i++;
			}
		}
	}
	// Blocked Gibbs Samplingによる分割のサンプリング
	// 分割結果が確率的に決まる
	void Lattice::blocked_gibbs(Sentence* sentence, std::vector<int> &segments, bool use_scaling){
		assert(sentence->size() <= _max_sentence_length);
		int size = sentence->size() + 1;

		#ifdef __DEBUG__
			for(int t = 0;t < size;t++){
				for(int k = 0;k < _max_word_length + 1;k++){
					for(int j = 0;j < _max_word_length + 1;j++){
						_alpha[t][k][j] = -1;
					}
				}
			}
			for(int k = 0;k < _max_word_length;k++){
				for(int j = 0;j < _max_word_length;j++){
					_backward_sampling_table[k * _max_word_length + j] = -1;
				}
			}
		#endif

		for(int i = 0;i < size;i++){
			for(int j = 0;j < _max_word_length + 1;j++){
				_substring_word_id_cache[i][j] = 0;
			}
		}
		_forward_filtering(sentence, _alpha, _scaling, _pw_h, use_scaling);
		_backward_sampling(sentence, segments, _alpha, _pw_h);
	}
	// ビタビアルゴリズム用
	void Lattice::viterbi_argmax_alpha_t_k_j(Sentence* sentence, int t, int k, int j){
		id word_k_id = get_substring_word_id_at_t_k(sentence, t, k);
		wchar_t const* characters = sentence->_characters;
		int character_ids_length = sentence->size();
		assert(t <= _max_sentence_length + 1);
		assert(k <= _max_word_length);
		assert(j <= _max_word_length);
		assert(t - k >= 0);
		// <bos>から生成されている場合
		if(j == 0){
			_word_ids[0] = ID_BOS;
			_word_ids[1] = ID_BOS;
			_word_ids[2] = word_k_id;
			double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t - k, t - 1);
			assert(pw_h > 0);
			_alpha[t][k][0] = log(pw_h);
			_viterbi_backward[t][k][0] = 0;
			return;
		}
		// i=0に相当
		if(t - k - j == 0){
			id word_j_id = get_substring_word_id_at_t_k(sentence, t - k, j);
			_word_ids[0] = ID_BOS;
			_word_ids[1] = word_j_id;
			_word_ids[2] = word_k_id;
			double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t - k, t - 1);
			assert(pw_h > 0);
			assert(_alpha[t - k][j][0] != 0);
			_alpha[t][k][j] = log(pw_h) + _alpha[t - k][j][0];
			assert(_alpha[t][k][j] <= 0);
			_viterbi_backward[t][k][j] = 0;
			return;
		}
		// それ以外の場合は周辺化
		double max_log_p = 0;
		int argmax = 0;
		for(int i = 1;i <= std::min(t - k - j, _max_word_length);i++){
			id word_i_id = get_substring_word_id_at_t_k(sentence, t - k - j, i);
			id word_j_id = get_substring_word_id_at_t_k(sentence, t - k, j);
			_word_ids[0] = word_i_id;
			_word_ids[1] = word_j_id;
			_word_ids[2] = word_k_id;
			double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t - k, t - 1);
			assert(pw_h > 0);
			assert(i <= _max_word_length);
			assert(_alpha[t - k][j][i] <= 0);
			double value = log(pw_h) + _alpha[t - k][j][i];
			assert(value <= 0);
			if(argmax == 0 || value > max_log_p){
				argmax = i;
				max_log_p = value;
			}
		}
		assert(argmax > 0);
		_alpha[t][k][j] = max_log_p;
		_viterbi_backward[t][k][j] = argmax;
	}
	void Lattice::viterbi_forward(Sentence* sentence){
		for(int t = 1;t <= sentence->size();t++){
			for(int k = 1;k <= std::min(t, _max_word_length);k++){
				if(t - k == 0){
					viterbi_argmax_alpha_t_k_j(sentence, t, k, 0);
				}
				for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
					viterbi_argmax_alpha_t_k_j(sentence, t, k, j);
				}
			}
		}
	}
	// <eos>に繋がる確率でargmax
	void Lattice::viterbi_argmax_backward_k_and_j_to_eos(Sentence* sentence, int t, int next_word_length, int &argmax_k, int &argmax_j){
		assert(t == sentence->size());
		wchar_t const* characters = sentence->_characters;
		int character_ids_length = sentence->size();
		double max_log_p = 0;
		argmax_k = 0;
		argmax_j = 0;
		int limit_k = std::min(t, _max_word_length);
		for(int k = 1;k <= limit_k;k++){
			for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
				id word_j_id = get_substring_word_id_at_t_k(sentence, t - k, j);
				id word_k_id = get_substring_word_id_at_t_k(sentence, t, k);
				_word_ids[0] = word_j_id;
				_word_ids[1] = word_k_id;
				_word_ids[2] = ID_EOS;
				double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t, t);
				assert(_alpha[t][k][j] <= 0);
				double value = log(pw_h) + _alpha[t][k][j];
				assert(value <= 0);
				if(argmax_k == 0 || value > max_log_p){
					max_log_p = value;
					argmax_k = k;
					argmax_j = j;
				}
			}
			if(t - k == 0){
				id word_j_id = ID_BOS;
				id word_k_id = get_substring_word_id_at_t_k(sentence, t, k);
				id word_t_id = ID_EOS;
				_word_ids[0] = word_j_id;
				_word_ids[1] = word_k_id;
				_word_ids[2] = word_t_id;
				double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t, t);
				assert(_alpha[t][k][0] <= 0);
				double value = log(pw_h) + _alpha[t][k][0];
				assert(value <= 0);
				if(argmax_k == 0 || value > max_log_p){
					max_log_p = value;
					argmax_k = k;
					argmax_j = 0;
				}
			}
		}
	}
	void Lattice::viterbi_backward(Sentence* sentence, std::vector<int> &segments){
		segments.clear();
		int k = 0;
		int j = 0;
		int sum = 0;
		int t = sentence->size();
		viterbi_argmax_backward_k_and_j_to_eos(sentence, t, 1, k, j);
		assert(k <= _max_word_length);
		segments.push_back(k);
		sum += k;
		if(j == 0 && k == t){	// 文章すべてが1単語になる場合
			assert(sum == sentence->size());
			return;
		}
		assert(k > 0 && j > 0);
		assert(j <= _max_word_length);
		segments.push_back(j);
		int i = _viterbi_backward[t][k][j];
		assert(i >= 0);
		assert(i <= _max_word_length);
		t -= k;
		sum += j;
		sum += i;
		k = j;
		j = i;
		if(i == 0){
			assert(sum == sentence->size());
			return;
		}
		segments.push_back(i);
		while(t > 0){
			i = _viterbi_backward[t][k][j];
			assert(i >= 0);
			assert(i <= _max_word_length);
			if(i != 0){
				segments.push_back(i);
			}
			// std::cout << "<- " << k << std::endl;
			t -= k;
			k = j;
			j = i;
			sum += i;
		}
		assert(t == 0);
		assert(sum == sentence->size());
		assert(segments.size() > 0);
		reverse(segments.begin(), segments.end());
	}
	// ビタビアルゴリズムによる分割
	// 決定的に分割が決まる
	void Lattice::viterbi_decode(Sentence* sentence, std::vector<int> &segments){
		assert(sentence->size() <= _max_sentence_length);
		int size = sentence->size() + 1;

		#ifdef __DEBUG__
			for(int t = 0;t < size;t++){
				for(int k = 0;k < _max_word_length + 1;k++){
					for(int j = 0;j < _max_word_length + 1;j++){
						_alpha[t][k][j] = 1;
					}
				}
				for(int k = 0;k < _max_word_length;k++){
					for(int j = 0;j < _max_word_length;j++){
						_viterbi_backward[t][k][j] = -1;
					}
				}
			}
		#endif

		_alpha[0][0][0] = 0;
		for(int t = 0;t < size;t++){
			for(int k = 0;k < _max_word_length + 1;k++){
				_substring_word_id_cache[t][k] = 0;
			}
		}
		viterbi_forward(sentence);
		viterbi_backward(sentence, segments);
	}
	double Lattice::compute_log_forward_probability(Sentence* sentence, bool use_scaling){
		_enumerate_forward_variables(sentence, _alpha, _scaling, use_scaling);
		if(use_scaling == false){
			int t = sentence->size() + 1; // <eos>を指す
			int k = 1;	// ここでは<eos>の長さを1と考える
			double alpha_eos = 0;	
			for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
				assert(_alpha[t][k][j] > 0);
				alpha_eos += _alpha[t][k][j];
			}
			assert(alpha_eos > 0);
			return log(alpha_eos);
		}
		// スケーリング係数は全時刻の値の対数を足すと系列の確率になる
		int t = sentence->size() + 1; // <eos>を指す
		double log_px = 0;
		for(int m = 1;m <= t;m++){
			log_px += log(1.0 / _scaling[m]);
		}
		return log_px;
	}
	// 前向き確率とスケーリング係数を計算
	void Lattice::_enumerate_forward_variables(Sentence* sentence, double*** alpha, double* scaling, bool use_scaling){
		assert(sentence->size() <= _max_sentence_length);
		int size = sentence->size() + 1;
		for(int i = 0;i < size;i++){
			for(int j = 0;j < _max_word_length + 1;j++){
				_substring_word_id_cache[i][j] = 0;
			}
		}
		#ifdef __DEBUG__
			// 変な値を入れる
			for(int t = 0;t < size;t++){
				scaling[t] = -1;
				for(int k = 0;k < _max_word_length + 1;k++){
					for(int j = 0;j < _max_word_length + 1;j++){
						alpha[t][k][j] = -1;
					}
				}
			}
		#endif 

		wchar_t const* characters = sentence->_characters;
		int character_ids_length = sentence->size();
		// <eos>未満の前向き確率を計算
		_forward_filtering(sentence, alpha, scaling, _pw_h, use_scaling);
		// <eos>への接続を考える
		double alpha_eos = 0;
		int t = sentence->size() + 1; // <eos>を指す
		int k = 1;	// ここでは<eos>の長さを1と考える
		for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
			double sum_prob = 0;
			for(int i = (t - k - j == 0) ? 0 : 1;i <= std::min(t - k - j, _max_word_length);i++){
				_word_ids[0] = get_substring_word_id_at_t_k(sentence, t - k - j, i);
				_word_ids[1] = get_substring_word_id_at_t_k(sentence, t - k, j);
				_word_ids[2] = ID_EOS;
				double pw_h = _npylm->compute_p_w_given_h(characters, character_ids_length, _word_ids, 3, 2, t, t);
				assert(pw_h > 0);
				sum_prob += pw_h * alpha[t - k][j][i];
			}
			alpha[t][k][j] = sum_prob;
			alpha_eos += sum_prob;
		}
		if(use_scaling){
			scaling[t] = 1.0 / alpha_eos;
		}
	}
}