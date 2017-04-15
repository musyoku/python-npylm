#pragma once
#include <algorithm>
#include "common.h"
#include "npylm.h"
#include "sampler.h"
#include "hash.h"
using namespace std;

// ＿人人人人人人人人人人人人人人人人人人人人人人人人人人人人＿
// ＞　Latticeでは文字のインデックスtが1から始まることに注意　＜
// ￣Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y￣

namespace npylm {
	namespace lattice {
		void init_alpha(double*** &alpha, int size, int max_word_length){
			alpha = new double**[size];
			assert(alpha != NULL);
			for(int t = 0;t < size;t++){
				alpha[t] = new double*[max_word_length + 1];
				assert(alpha[t] != NULL);
				for(int k = 0;k < max_word_length + 1;k++){
					alpha[t][k] = new double[max_word_length + 1];
					assert(alpha[t][k] != NULL);
					for(int j = 0;j < max_word_length + 1;j++){
						alpha[t][k][j] = 0;
					}
				}
			}
		}
		void delete_alpha(double*** &alpha, int size, int max_word_length){
			if(alpha == NULL){
				return;
			}
			for(int t = 0;t < size;t++){
				for(int k = 0;k < max_word_length + 1;k++){
					delete[] alpha[t][k];
				}
				delete[] alpha[t];
			}
			delete[] alpha;
			alpha = NULL;
		}
	}
	class Lattice {
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
		bool _is_ready;
		Lattice(NPYLM* npylm){
			_npylm = npylm;
			_word_ids = new id[4];	// 3-gramなので<bos><bos>単語<eos>の最低4つ
			_is_ready = false;
			_alpha = NULL;
			_normalized_alpha = NULL;
			_pw_h = NULL;
			_log_z = NULL;
			_backward_sampling_table = NULL;
			_viterbi_backward = NULL;
			_substring_word_id_cache = NULL;
		}
		~Lattice(){
			delete[] _word_ids;
			_delete_cache();
		}
		void _init_cache(int max_word_length, int max_sentence_length){
			_delete_cache();
			_max_word_length = max_word_length;
			_max_sentence_length = max_sentence_length;
			int size = max_sentence_length + 1;
			// 前向き確率の正規化定数
			_log_z = new double[size];
			assert(_log_z != NULL);
			// ビタビアルゴリズム用
			_viterbi_backward = new int**[size];
			assert(_viterbi_backward != NULL);
			for(int t = 0;t < size;t++){
				_viterbi_backward[t] = new int*[max_word_length + 1];
				assert(_viterbi_backward[t] != NULL);
				for(int k = 0;k < max_word_length + 1;k++){
					_viterbi_backward[t][k] = new int[max_word_length + 1];
					assert(_viterbi_backward[t][k] != NULL);
				}
			}
			// 後ろ向きアルゴリズムでkとjをサンプリングするときの確率表
			_backward_sampling_table = new double[max_word_length * max_word_length];
			assert(_backward_sampling_table != NULL);
			// 前向き確率
			lattice::init_alpha(_alpha, size, max_word_length);
			// 正規化後の前向き確率
			lattice::init_alpha(_normalized_alpha, size, max_word_length);
			// 3-gram確率のキャッシュ
			_pw_h = new double***[size];
			assert(_pw_h != NULL);
			for(int t = 0;t < size;t++){
				_pw_h[t] = new double**[max_word_length + 1];
				assert(_pw_h[t] != NULL);
				for(int k = 0;k < max_word_length + 1;k++){
					_pw_h[t][k] = new double*[max_word_length + 1];
					assert(_pw_h[t][k] != NULL);
					for(int j = 0;j < max_word_length + 1;j++){
						_pw_h[t][k][j] = new double[max_word_length + 1];
						assert(_pw_h[t][k] != NULL);
						for(int i = 0;i < max_word_length + 1;i++){
							_pw_h[t][k][j][i] = 0;
						}
					}
				}
			}
			// 部分文字列のIDのキャッシュ
			_substring_word_id_cache = new id*[size];
			assert(_substring_word_id_cache != NULL);
			for(int i = 0;i < size;i++){
				_substring_word_id_cache[i] = new id[max_word_length + 1];
				assert(_substring_word_id_cache[i] != NULL);
				for(int j = 0;j < max_word_length + 1;j++){
					_substring_word_id_cache[i][j] = 0;
				}
			}
			_is_ready = true;
		}
		void _delete_cache(){
			if(_log_z != NULL){
				delete[] _log_z;
				_log_z = NULL;
			}
			int size = _max_sentence_length + 1;
			lattice::delete_alpha(_alpha, size, _max_word_length);
			lattice::delete_alpha(_normalized_alpha, size, _max_word_length);
			if(_substring_word_id_cache != NULL){
				for(int t = 0;t < size;t++){
					delete[] _substring_word_id_cache[t];
				}
				delete[] _substring_word_id_cache;
				_substring_word_id_cache = NULL;
			}
			if(_viterbi_backward != NULL){
				for(int t = 0;t < size;t++){
					for(int k = 0;k < _max_word_length + 1;k++){
						delete[] _viterbi_backward[t][k];
					}
					delete[] _viterbi_backward[t];
				}
				delete[] _viterbi_backward;
				_viterbi_backward = NULL;
			}
			if(_backward_sampling_table != NULL){
				delete[] _backward_sampling_table;
				_backward_sampling_table = NULL;
			}
			if(_pw_h != NULL){
				for(int t = 0;t < size;t++){
					for(int k = 0;k < _max_word_length + 1;k++){
						for(int j = 0;j < _max_word_length + 1;j++){
							delete[] _pw_h[t][k][j];
						}
						delete[] _pw_h[t][k];
					}
					delete[] _pw_h[t];
				}
				delete[] _pw_h;
				_pw_h = NULL;
			}
		}
		id get_substring_word_id_at_t_k(Sentence* sentence, int t, int k){
			assert(t < _max_sentence_length + 1);
			assert(k < _max_sentence_length + 1);
			id word_id = _substring_word_id_cache[t][k];
			if(word_id == 0){
				word_id = sentence->get_substr_word_id(t - k, t - 1);
				_substring_word_id_cache[t][k] = word_id;
			}
			return word_id;
		}
		// 正規化定数をここでは掛けないことに注意
		void sum_alpha_t_k_j(Sentence* sentence, int t, int k, int j, double*** normalized_alpha){
			double*** forward_alpha = (normalized_alpha == NULL) ? _alpha : normalized_alpha;
			id word_k_id = get_substring_word_id_at_t_k(sentence, t, k);
			wchar_t const* character_ids = sentence->_character_ids;
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
				_word_ids[3] = ID_EOS;
				double pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t - k, t - 1);
				assert(pw_h > 0);
				_alpha[t][k][0] = pw_h;
				_pw_h[t][k][0][0] = pw_h;
				return;
			}
			// i=0に相当
			if(t - k - j == 0){
				id word_j_id = get_substring_word_id_at_t_k(sentence, t - k, j);
				_word_ids[0] = ID_BOS;
				_word_ids[1] = word_j_id;
				_word_ids[2] = word_k_id;
				_word_ids[3] = ID_EOS;
				double pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t - k, t - 1);
				assert(pw_h > 0);
				assert(forward_alpha[t - k][j][0] > 0);
				_alpha[t][k][j] = pw_h * forward_alpha[t - k][j][0];
				assert(_alpha[t][k][j] > 0);
				_pw_h[t][k][j][0] = pw_h;
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
				_word_ids[3] = ID_EOS;
				double pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t - k, t - 1);
				assert(i <= _max_word_length);
				assert(_alpha[t - k][j][i] > 0);
				double value = pw_h * forward_alpha[t - k][j][i];
				assert(pw_h > 0);

				#ifdef __DEBUG__
				if(value == 0){
					cout << pw_h << endl;
					if(normalized_alpha == NULL){
						cout << _alpha[t - k][j][i] << endl;
					}else{
						cout << normalized_alpha[t - k][j][i] << " (normalized)" << endl;
					}
					cout << t << ", " << k << ", " << j << ", " << i << endl;
				}
				#endif

				assert(value > 0);
				sum += value;
				_pw_h[t][k][j][i] = pw_h;
			}
			assert(sum > 0);
			_alpha[t][k][j] = sum;
		}
		void forward_filtering(Sentence* sentence, double*** normalized_alpha){
			for(int t = 1;t <= sentence->size();t++){
				for(int k = 1;k <= std::min(t, _max_word_length);k++){
					if(t - k == 0){
						sum_alpha_t_k_j(sentence, t, k, 0, normalized_alpha);
					}
					for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
						sum_alpha_t_k_j(sentence, t, k, j, normalized_alpha);
					}
				}
				// 正規化
				if(normalized_alpha != NULL){
					// アンダーフローを防ぐためlogsumexpを経由して正規化後の前向き確率テーブルを計算
					double log_sum = 0;
					// 最大値を求める
					double max_log_z = 0;
					for(int k = 1;k <= std::min(t, _max_word_length);k++){
						if(t - k == 0){
							assert(_alpha[t][k][0] > 0);
							double tmp = log(_alpha[t][k][0]) + _log_z[t - k];
							if(max_log_z == 0 || tmp > max_log_z){
								max_log_z = tmp;
							}
						}
						for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
							assert(_alpha[t][k][j] > 0);
							double tmp = log(_alpha[t][k][j]) + _log_z[t - k];
							if(max_log_z == 0 || tmp > max_log_z){
								max_log_z = tmp;
							}
						}
					}
					// 求めた最大値をもとにlogsumexp
					for(int k = 1;k <= std::min(t, _max_word_length);k++){
						if(t - k == 0){
							log_sum += exp(log(_alpha[t][k][0]) + _log_z[t - k] - max_log_z);
							continue;
						}
						for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
							assert(_alpha[t][k][j] > 0);
							log_sum += exp(log(_alpha[t][k][j]) + _log_z[t - k] - max_log_z);
						}
					}
					log_sum = log(log_sum) + max_log_z;
					// 正規化
					assert(log_sum != 0);
					for(int k = 1;k <= std::min(t, _max_word_length);k++){
						if(t - k == 0){
							normalized_alpha[t][k][0] = exp(log(_alpha[t][k][0]) + _log_z[t - k] - log_sum);
							assert(normalized_alpha[t][k][0] > 0);
							continue;
						}
						for(int j = 1;j <= std::min(t - k, _max_word_length);j++){
							normalized_alpha[t][k][j] = exp(log(_alpha[t][k][j]) + _log_z[t - k] - log_sum);
							assert(normalized_alpha[t][k][j] > 0);
						}
					}
					assert(t <= _max_sentence_length + 1);
					_log_z[t] = log_sum;
				}else{
					_log_z[t] = 1;
				}
			}
		}
		void backward_sampling(Sentence* sentence, vector<int> &segments, double*** backward_alpha){
			segments.clear();
			int k = 0;
			int j = 0;
			int sum = 0;
			int t = sentence->size();
			sample_backward_k_and_j(sentence, t, 1, k, j, backward_alpha);
			assert(k <= _max_word_length);
			segments.push_back(k);
			if(j == 0 && k == t){	// 文章すべてが1単語になる場合
				return;
			}
			assert(k > 0 && j > 0);
			assert(j <= _max_word_length);
			segments.push_back(j);
			// cout << "<- " << k << endl;
			// cout << "<- " << j << endl;
			t -= k;
			t -= j;
			sum += k + j;
			int next_word_length = j;
			while(t > 0){
				if(t == 1){
					k = 1;
					j = 0;
				}else{
					sample_backward_k_and_j(sentence, t, next_word_length, k, j, backward_alpha);
					assert(k > 0);
					assert(k <= _max_word_length);
				}
				segments.push_back(k);
				// cout << "<- " << k << endl;
				t -= k;
				if(j == 0){
					assert(t == 0);
				}else{
					assert(j <= _max_word_length);
					segments.push_back(j);
					// cout << "<- " << j << endl;
					t -= j;
				}
				sum += k + j;
				next_word_length = j;
			}
			assert(t == 0);
			assert(sum == sentence->size());
			reverse(segments.begin(), segments.end());
		}
		void sample_backward_k_and_j(Sentence* sentence, int t, int next_word_length, int &sampled_k, int &sampled_j, double*** backward_alpha){
			int table_index = 0;
			wchar_t const* character_ids = sentence->_character_ids;
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
					_word_ids[3] = ID_EOS;
					double pw_h = 0;
					if(t == sentence->size()){	// <eos>に接続する確率からサンプリング
						pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t, t);
					}else{
						pw_h = _pw_h[t + next_word_length][next_word_length][k][j];
						#ifdef __DEBUG__
						double pw_h2 = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t, t + next_word_length - 1);
						if(pw_h != pw_h2){
							cout << "t = " << t << ", k = " << k << ", j = " << j << endl;
							cout << "next_word_length = " << next_word_length << endl;
							cout << "size = " << sentence->size() << endl;
						}
						assert(pw_h == pw_h2);
						#endif
					}
					assert(backward_alpha[t][k][j] > 0);
					double p = pw_h * backward_alpha[t][k][j];
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
					_word_ids[3] = ID_EOS;
					double pw_h = 0;
					if(t == sentence->size()){	// <eos>に接続する確率からサンプリング
						pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t, t);
					}else{
						pw_h = _pw_h[t + next_word_length][next_word_length][k][0];
						#ifdef __DEBUG__
						double pw_h2 = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t, t + next_word_length - 1);
						if(pw_h != pw_h2){
							cout << "t = " << t << ", k = " << k << ", j = " << 0 << endl;
							cout << "next_word_length = " << next_word_length << endl;
							cout << "size = " << sentence->size() << endl;
						}
						assert(pw_h == pw_h2);
						#endif
					}
					assert(backward_alpha[t][k][0] > 0);
					double p = pw_h * backward_alpha[t][k][0];
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
		void perform_blocked_gibbs_sampling(Sentence* sentence, vector<int> &segments, bool normalize = true){
			assert(sentence->size() <= _max_sentence_length);
			int size = sentence->size() + 1;

			#ifdef __DEBUG__
			for(int t = 0;t < size;t++){
				_log_z[t] = 0;
				for(int k = 0;k < _max_word_length + 1;k++){
					for(int j = 0;j < _max_word_length + 1;j++){
						_alpha[t][k][j] = -1;
						_normalized_alpha[t][k][j] = -1;
					}
				}
			}
			for(int k = 0;k < _max_word_length;k++){
				for(int j = 0;j < _max_word_length;j++){
					_backward_sampling_table[k * _max_word_length + j] = -1;
				}
			}
			#endif

			_alpha[0][0][0] = 1;
			_log_z[0] = 0;
			for(int i = 0;i < size;i++){
				for(int j = 0;j < _max_word_length + 1;j++){
					_substring_word_id_cache[i][j] = 0;
				}
			}
			double*** normalized_alpha = normalize ? _normalized_alpha : NULL;
			double*** backward_alpha = normalize ? _normalized_alpha : _alpha;
			this->forward_filtering(sentence, normalized_alpha);
			this->backward_sampling(sentence, segments, backward_alpha);
		}
		// ビタビアルゴリズム用
		void viterbi_argmax_alpha_t_k_j(Sentence* sentence, int t, int k, int j){
			id word_k_id = get_substring_word_id_at_t_k(sentence, t, k);
			wchar_t const* character_ids = sentence->_character_ids;
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
				_word_ids[3] = ID_EOS;
				double pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t - k, t - 1);
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
				_word_ids[3] = ID_EOS;
				double pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t - k, t - 1);
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
				_word_ids[3] = ID_EOS;
				double pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t - k, t - 1);
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
		void viterbi_forward(Sentence* sentence){
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
		void viterbi_argmax_backward_k_and_j_to_eos(Sentence* sentence, int t, int next_word_length, int &argmax_k, int &argmax_j){
			assert(t == sentence->size());
			wchar_t const* character_ids = sentence->_character_ids;
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
					_word_ids[3] = ID_EOS;
					double pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t, t);
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
					_word_ids[3] = ID_EOS;
					double pw_h = _npylm->compute_Pw_h(character_ids, character_ids_length, _word_ids, 4, 2, t, t);
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
		void viterbi_backward(Sentence* sentence, vector<int> &segments){
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
				// cout << "<- " << k << endl;
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
		void viterbi_decode(Sentence* sentence, vector<int> &segments){
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
			_log_z[0] = 0;
			for(int t = 0;t < size;t++){
				for(int k = 0;k < _max_word_length + 1;k++){
					_substring_word_id_cache[t][k] = 0;
				}
			}
			this->viterbi_forward(sentence);
			this->viterbi_backward(sentence, segments);
		}
	};
} // namespace npylm