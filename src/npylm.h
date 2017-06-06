#pragma once
#include <boost/format.hpp>
#include <vector> 
#include <algorithm>
#include <iostream> 
#include <cmath> 
#include <cassert> 
#include "common.h"
#include "node.h"
#include "vpylm.h"
#include "hpylm.h"
#include "sampler.h"
#include "wordtype.h"

namespace npylm{
	// character_idsのsubstr_char_t_startからsubstr_char_t_endまでの文字列を<bow>と<eow>で挟んでwrapped_character_idsの先頭に格納
	void wrap_bow_eow(wchar_t const* character_ids, int substr_char_t_start, int substr_char_t_end, wchar_t* wrapped_character_ids){
		wrapped_character_ids[0] = ID_BOW;
		int i = 0;
		for(;i < substr_char_t_end - substr_char_t_start + 1;i++){
			wrapped_character_ids[i + 1] = character_ids[i + substr_char_t_start];
		}
		wrapped_character_ids[i + 1] = ID_EOW;
	}
	double factorial(double n) {
		if (n == 0){
			return 1;
		}
		return n * factorial(n - 1);
	}
	class NPYLM{
	public:
		HPYLM* _hpylm;	// 単語n-gram
		VPYLM* _vpylm;	// 文字n-gram
		hashmap<id, vector<vector<int>>> _prev_depths_for_token_at_table;
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
		bool _is_ready;
		NPYLM(){
			_init();
		}
		NPYLM(int max_word_length, int max_sentence_length, double g0){
			_init();
			_init_cache(max_word_length, max_sentence_length);
			_vpylm->set_g0(g0);
		}
		~NPYLM(){
			if(_hpylm_parent_pw_cache != NULL){
				delete[] _hpylm_parent_pw_cache;
			}
			if(_lambda_for_type != NULL){
				delete[] _lambda_for_type;
			}
			if(_hpylm != NULL){
				delete _hpylm;
			}
			if(_vpylm != NULL){
				delete _vpylm;
			}
			_delete_cache();
		}
		void _init(){
			_hpylm = new HPYLM(3);	// 3-gram以外を指定すると動かないので注意
			_vpylm = new VPYLM();
			_lambda_for_type = new double[WORDTYPE_NUM_TYPES + 1];
			_hpylm_parent_pw_cache = new double[3];					// 3-gram
			_pk_vpylm = NULL;
			_character_ids = NULL;
			_is_ready = false;
			set_lambda_prior(4, 1);
		}
		void _init_cache(int max_word_length, int max_sentence_length){
			_delete_cache();
			_max_sentence_length = max_sentence_length;
			_max_word_length = max_word_length;
			_character_ids = new wchar_t[max_sentence_length + 2]; 	// <bow>と<eow>を含める
			_pk_vpylm = new double[max_word_length + 2]; 			// kが1スタート、かつk > max_word_length用の領域も必要なので+2
			for(int k = 1;k < max_word_length + 2;k++){
				_pk_vpylm[k] = 0;
			}
			_vpylm->_init_cache(max_sentence_length);
			_is_ready = true;
		}
		void _delete_cache(){
			if(_character_ids != NULL){
				delete[] _character_ids;
			}
			if(_pk_vpylm != NULL){
				delete[] _pk_vpylm;
			}
		}
		void set_vpylm_g0(double g0){
			_vpylm->set_g0(g0);
		}
		void set_lambda_prior(double a, double b){
			_lambda_a = a;
			_lambda_b = b;
			sample_lambda_with_initial_params();
		}
		void sample_lambda_with_initial_params(){
			for(int type = 1;type <= WORDTYPE_NUM_TYPES;type++){
				_lambda_for_type[type] = sampler::gamma(_lambda_a, _lambda_b);
			}
		}
		bool add_customer_at_time_t(Sentence* sentence, int t){
			assert(_character_ids != NULL);
			assert(t >= 2);
			id token_t = sentence->get_word_id_at(t);
			Node<id>* node = find_node_by_tracing_back_context_from_time_t(sentence, t, _hpylm_parent_pw_cache, true, false);
			assert(node != NULL);
			int num_tables_before = _hpylm->_root->_num_tables;
			int added_table_k = -1;
			int substr_char_t_start = sentence->_start[t];
			int substr_char_t_end = sentence->_start[t] + sentence->_segments[t] - 1;
			node->add_customer(token_t, _hpylm_parent_pw_cache, _hpylm->_d_m, _hpylm->_theta_m, true, added_table_k);
			if(token_t == ID_EOS){
				int added_to_table_k;
				_vpylm->_root->add_customer(token_t, _vpylm->_g0, _vpylm->_d_m, _vpylm->_theta_m, true, added_to_table_k);
				return true;
			}
			int num_tables_after = _hpylm->_root->_num_tables;
			if(num_tables_before < num_tables_after){
				assert(added_table_k != -1);
				vector<vector<int>> &depths = _prev_depths_for_token_at_table[token_t];
				assert(depths.size() <= added_table_k);	// 存在してはいけない
				vector<int> prev_depths;
				vpylm_add_customers(sentence->_character_ids, substr_char_t_start, substr_char_t_end, _character_ids, prev_depths);
				assert(prev_depths.size() == substr_char_t_end - substr_char_t_start + 3);
				depths.push_back(prev_depths);
				_g0_cache.clear();
			}
			return true;
		}
		void vpylm_add_customers(wchar_t const* character_ids, int substr_char_t_start, int substr_char_t_end, wchar_t* wrapped_character_ids, vector<int> &prev_depths){
			assert(prev_depths.size() == 0);
			// 先頭に<bow>をつける
			assert(substr_char_t_end < _max_sentence_length);
			wrap_bow_eow(character_ids, substr_char_t_start, substr_char_t_end, wrapped_character_ids);
			int wrapped_character_ids_length = substr_char_t_end - substr_char_t_start + 3;	// <bow>と<eow>を考慮
			// 客を追加
			for(int char_t = 0;char_t < wrapped_character_ids_length;char_t++){
				int depth_t = _vpylm->sample_depth_at_time_t(wrapped_character_ids, char_t, _vpylm->_parent_pw_cache, _vpylm->_path_nodes);
				_vpylm->add_customer_at_time_t(wrapped_character_ids, char_t, depth_t, _vpylm->_parent_pw_cache, _vpylm->_path_nodes);	// キャッシュを使って追加
				prev_depths.push_back(depth_t);
			}
		}
		bool remove_customer_at_time_t(Sentence* sentence, int t){
			assert(_character_ids != NULL);
			assert(t >= 2);
			id token_t = sentence->get_word_id_at(t);
			Node<id>* node = find_node_by_tracing_back_context_from_time_t(sentence->_word_ids, sentence->get_num_segments(), t, false, false);
			assert(node != NULL);
			int num_tables_before = _hpylm->_root->_num_tables;
			int removed_from_table_k = -1;
			int substr_char_t_start = sentence->_start[t];
			int substr_char_t_end = sentence->_start[t] + sentence->_segments[t] - 1;
			node->remove_customer(token_t, true, removed_from_table_k);
			int num_tables_after = _hpylm->_root->_num_tables;
			if(token_t == ID_EOS){
				_vpylm->_root->remove_customer(token_t, true, removed_from_table_k);
				return true;
			}
			if(num_tables_before > num_tables_after){
				assert(removed_from_table_k != -1);
				auto itr = _prev_depths_for_token_at_table.find(token_t);
				assert(itr != _prev_depths_for_token_at_table.end());
				vector<vector<int>> &depths = itr->second;
				assert(removed_from_table_k < depths.size());
				// 客を除外
				vector<int> &prev_depths = depths[removed_from_table_k];
				assert(prev_depths.size() > 0);
				vpylm_remove_customers(sentence->_character_ids, substr_char_t_start, substr_char_t_end, _character_ids, prev_depths);
				// シフト
				depths.erase(depths.begin() + removed_from_table_k);
				_g0_cache.clear();
			}
			if(node->need_to_remove_from_parent()){
				node->remove_from_parent();
			}
			return true;
		}
		void vpylm_remove_customers(wchar_t const* character_ids, int substr_char_t_start, int substr_char_t_end, wchar_t* wrapped_character_ids, vector<int> &prev_depths){
			assert(prev_depths.size() > 0);
			// 先頭に<bow>をつける
			assert(substr_char_t_end < _max_sentence_length);
			wrap_bow_eow(character_ids, substr_char_t_start, substr_char_t_end, wrapped_character_ids);
			int wrapped_character_ids_length = substr_char_t_end - substr_char_t_start + 3;	// <bow>と<eow>を考慮
			// 客を除外
			assert(prev_depths.size() == wrapped_character_ids_length);
			auto prev_depth_t = prev_depths.begin();
			for(int char_t = 0;char_t < wrapped_character_ids_length;char_t++){
				_vpylm->remove_customer_at_time_t(wrapped_character_ids, char_t, *prev_depth_t);
				prev_depth_t++;
			}
		}
		Node<id>* find_node_by_tracing_back_context_from_time_t(id const* word_ids, int word_ids_length, int word_t_index, bool generate_node_if_needed, bool return_middle_node){
			assert(word_t_index >= 2);
			assert(word_t_index < word_ids_length);
			Node<id>* node = _hpylm->_root;
			for(int depth = 1;depth <= 2;depth++){
				id context_id = ID_BOS;
				if(word_t_index - depth >= 0){
					context_id = word_ids[word_t_index - depth];
				}
				Node<id>* child = node->find_child_node(context_id, generate_node_if_needed);
				if(child == NULL){
					if(return_middle_node){
						return node;
					}
					return NULL;
				}
				node = child;
			}
			return node;
		}
		// add_customer用
		Node<id>* find_node_by_tracing_back_context_from_time_t(Sentence* sentence, int word_t_index, double* parent_pw_cache, int generate_node_if_needed, bool return_middle_node){
			assert(word_t_index >= 2);
			assert(word_t_index < sentence->get_num_segments());
			assert(sentence->_segments[word_t_index] > 0);
			int substr_char_t_start = sentence->_start[word_t_index];
			int substr_char_t_end = sentence->_start[word_t_index] + sentence->_segments[word_t_index] - 1;
			return find_node_by_tracing_back_context_from_time_t(
				sentence->_character_ids, sentence->size(), 
				sentence->_word_ids, sentence->get_num_segments(), 
				word_t_index, substr_char_t_start, substr_char_t_end, 
				parent_pw_cache, generate_node_if_needed, return_middle_node);
		}
		// 効率のためノードを探しながら確率も計算する
		Node<id>* find_node_by_tracing_back_context_from_time_t(
				wchar_t const* character_ids, int character_ids_length, 
				id const* word_ids, int word_ids_length, 
				int word_t_index, int substr_char_t_start, int substr_char_t_end, 
				double* parent_pw_cache, bool generate_node_if_needed, bool return_middle_node){
			assert(word_t_index >= 2);
			assert(word_t_index < word_ids_length);
			assert(substr_char_t_start >= 0);
			assert(substr_char_t_end >= substr_char_t_start);
			Node<id>* node = _hpylm->_root;
			id word_t_id = word_ids[word_t_index];
			double parent_pw = compute_g0_substring_at_time_t(character_ids, character_ids_length, substr_char_t_start, substr_char_t_end, word_t_id);
			parent_pw_cache[0] = parent_pw;
			for(int depth = 1;depth <= 2;depth++){
				id context_id = ID_BOS;
				if(word_t_index - depth >= 0){
					context_id = word_ids[word_t_index - depth];
				}
				// 事前に確率を計算
				double pw = node->compute_Pw_with_parent_Pw(word_t_id, parent_pw, _hpylm->_d_m, _hpylm->_theta_m);
				parent_pw_cache[depth] = pw;
				Node<id>* child = node->find_child_node(context_id, generate_node_if_needed);
				if(child == NULL && return_middle_node == true){
					return node;
				}
				assert(child != NULL);
				parent_pw = pw;
				node = child;
			}
			return node;
		}
		// word_idは既知なので再計算を防ぐ
		double compute_g0_substring_at_time_t(wchar_t const* character_ids, int character_ids_length, int substr_char_t_start, int substr_char_t_end, id word_t_id){
			assert(_character_ids != NULL);
			if(word_t_id == ID_EOS){
				return _vpylm->_g0;
			}

			#ifdef __DEBUG__
			id a = hash_substring_ptr(character_ids, substr_char_t_start, substr_char_t_end);
			assert(a == word_t_id);
			#endif

			assert(substr_char_t_end < _max_sentence_length);
			assert(substr_char_t_start >= 0);
			assert(substr_char_t_end >= substr_char_t_start);
			int word_length = substr_char_t_end - substr_char_t_start + 1;
			// if(word_length > _max_word_length){
			// 	return 0;
			// }
			auto itr = _g0_cache.find(word_t_id);
			if(itr == _g0_cache.end()){
				// 先頭に<bow>をつける
				wchar_t* wrapped_character_ids = _character_ids;
				wrap_bow_eow(character_ids, substr_char_t_start, substr_char_t_end, wrapped_character_ids);
				int wrapped_character_ids_length = substr_char_t_end - substr_char_t_start + 3;
				// g0を計算
				double pw = _vpylm->compute_Pw(wrapped_character_ids, wrapped_character_ids_length);
				if(word_length > _max_word_length){
					_g0_cache[word_t_id] = pw;
					return pw;
				}
				double p_k_vpylm = compute_Pk_vpylm(word_length);
				if(p_k_vpylm == 0){
					_g0_cache[word_t_id] = pw;
					return pw;
				}
				int type = wordtype::detect_word_type_substr(character_ids, substr_char_t_start, substr_char_t_end);
				assert(type <= WORDTYPE_NUM_TYPES);
				assert(type > 0);
				double lambda = _lambda_for_type[type];
				double poisson = compute_poisson_k_lambda(word_length, lambda);
				assert(poisson > 0);
				double g0 = pw * poisson / p_k_vpylm;
				if(!(0 < g0 && g0 < 1)){
					for(int u = 0;u < character_ids_length;u++){
						wcout << character_ids[u];
					}
					wcout << endl;
					cout << pw << endl;
					cout << poisson << endl;
					cout << p_k_vpylm << endl;
					cout << g0 << endl;
					cout << word_length << endl;
				}
				assert(0 < g0 && g0 < 1);
				_g0_cache[word_t_id] = g0;
				return g0;
			}
			return itr->second;
		}
		double compute_poisson_k_lambda(unsigned int k, double lambda){
			return pow(lambda, k) * exp(-lambda) / factorial(k);
		}
		double compute_Pk_vpylm(int k){
			assert(k > 0);
			if(k > _max_word_length){
				return 0;
			}
			return _pk_vpylm[k];
		}
		void sample_pitman_yor_hyperparameters(){
			_hpylm->sample_hyperparams();
			_vpylm->sample_hyperparams();
		}
		double compute_log_Pw(Sentence* sentence){
			double pw = 0;
			for(int t = 2;t < sentence->get_num_segments();t++){
				pw += log(compute_Pw_h(sentence, t));
			}
			return pw;
		}
		double compute_Pw(Sentence* sentence){
			double pw = 1;
			for(int t = 2;t < sentence->get_num_segments();t++){
				pw *= compute_Pw_h(sentence, t);
			}
			return pw;
		}
		double compute_Pw_h(Sentence* sentence, int word_t_index){
			assert(word_t_index >= 2);
			assert(word_t_index < sentence->get_num_segments());
			assert(sentence->_segments[word_t_index] > 0);
			int substr_char_t_start = sentence->_start[word_t_index];
			int substr_char_t_end = sentence->_start[word_t_index] + sentence->_segments[word_t_index] - 1;
			return compute_Pw_h(sentence->_character_ids, sentence->size(), sentence->_word_ids, sentence->get_num_segments(), word_t_index, substr_char_t_start, substr_char_t_end);
		}
		double compute_Pw_h(
				wchar_t const* character_ids, int character_ids_length, 
				id const* word_ids, int word_ids_length, 
				int word_t_index, int substr_char_t_start, int substr_char_t_end){
			assert(word_t_index < word_ids_length);
			assert(substr_char_t_start >= 0);
			id word_id = word_ids[word_t_index];

			if(word_t_index == word_ids_length - 1){
				assert(word_id == ID_EOS);
			}else{
				if(word_id != ID_EOS){
					assert(substr_char_t_end < character_ids_length);
					#ifdef __DEBUG__
					id a = hash_substring_ptr(character_ids, substr_char_t_start, substr_char_t_end);
					assert(a == word_id);
					#endif
				}
			}
			// ノードを探しながら_hpylm_parent_pw_cacheをセット
			Node<id>* node = find_node_by_tracing_back_context_from_time_t(character_ids, character_ids_length, word_ids, word_ids_length, word_t_index, substr_char_t_start, substr_char_t_end, _hpylm_parent_pw_cache, false, true);
			assert(node != NULL);
			double parent_pw = _hpylm_parent_pw_cache[node->_depth];
			// 効率のため親の確率のキャッシュから計算
			return node->compute_Pw_with_parent_Pw(word_id, parent_pw, _hpylm->_d_m, _hpylm->_theta_m);
		}
		template <class Archive>
		void serialize(Archive &archive, unsigned int version)
		{
			boost::serialization::split_free(archive, *this, version);
		}
	};
} // namespace npylm

namespace boost { namespace serialization {
template<class Archive>
	void save(Archive &archive, const npylm::NPYLM &npylm, unsigned int version) {
		archive & npylm._hpylm;
		archive & npylm._vpylm;
		archive & npylm._max_word_length;
		archive & npylm._max_sentence_length;
		archive & npylm._lambda_a;
		archive & npylm._lambda_b;

		for(int type = 1;type <= WORDTYPE_NUM_TYPES;type++){
			archive & npylm._lambda_for_type[type];
		}

		for(int k = 0;k <= npylm._max_word_length + 1;k++){
			archive & npylm._pk_vpylm[k];
		}
	}
template<class Archive>
	void load(Archive &archive, npylm::NPYLM &npylm, unsigned int version) {
		archive & npylm._hpylm;
		archive & npylm._vpylm;
		archive & npylm._max_word_length;
		archive & npylm._max_sentence_length;
		archive & npylm._lambda_a;
		archive & npylm._lambda_b;

		npylm._pk_vpylm = new double[npylm._max_word_length + 2];
		npylm._lambda_for_type = new double[WORDTYPE_NUM_TYPES + 1];

		for(int type = 1;type <= WORDTYPE_NUM_TYPES;type++){
			archive & npylm._lambda_for_type[type];
		}

		for(int k = 0;k <= npylm._max_word_length + 1;k++){
			archive & npylm._pk_vpylm[k];
		}
	}
}} // namespace boost::serialization