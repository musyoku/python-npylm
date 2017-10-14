#include <boost/functional/hash.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <unordered_set>
#include <string>
#include "../../src/npylm/common.h"
#include "../../src/npylm/lm/vpylm.h"
#include "../../src/npylm/npylm.h"
#include "../../src/npylm/sentence.h"
using namespace npylm;
using namespace npylm::lm;
using std::cout;
using std::flush;
using std::endl;

int sample_depth_at_time_t(VPYLM* vpylm, wchar_t const* token_ids, int t){
	if(t == 0){
		return 0;
	}
	wchar_t token_t = token_ids[t];

	// この値を下回れば打ち切り
	double eps = VPYLM_EPS;
	
	double sum = 0;
	double p_pass = 0;
	double parent_pw = vpylm->_g0;
	int sampling_table_size = 0;
	Node<wchar_t>* node = vpylm->_root;
	for(int n = 0;n <= t;n++){
		if(node){
			double pw = node->compute_p_w(token_t, vpylm->_g0, vpylm->_d_m, vpylm->_theta_m);
			double p_stop = node->stop_probability(vpylm->_beta_stop, vpylm->_beta_pass);
			p_pass = node->pass_probability(vpylm->_beta_stop, vpylm->_beta_pass);
			double p = pw * p_stop;
			parent_pw = pw;
			vpylm->_sampling_table[n] = p;
			sampling_table_size += 1;
			sum += p;
			if(p_stop < eps){
				break;
			}
			if(n < t){
				wchar_t context_token_id = token_ids[t - n - 1];
				node = node->find_child_node(context_token_id);
			}
		}else{
			double p_stop = p_pass * vpylm->_beta_stop / (vpylm->_beta_stop + vpylm->_beta_pass);
			double p = parent_pw * p_stop;
			// probs.push_back(p);
			vpylm->_sampling_table[n] = p;
			sampling_table_size += 1;
			sum += p;
			p_pass *= vpylm->_beta_pass / (vpylm->_beta_stop + vpylm->_beta_pass);
			if(p_stop < eps){
				break;
			}
		}
	}
	// assert(sampling_table_size == t + 1);
	double normalizer = 1.0 / sum;
	double bernoulli = sampler::uniform(0, 1);
	double stack = 0;
	for(int n = 0;n < sampling_table_size;n++){
		stack += vpylm->_sampling_table[n] * normalizer;
		if(bernoulli < stack){
			return n;
		}
	}
	return vpylm->_sampling_table[sampling_table_size - 1];
}

bool add_customer_at_time_t(VPYLM* vpylm, wchar_t const* token_ids, int t, int depth_t){
	assert(0 <= depth_t && depth_t <= t);
	Node<wchar_t>* node = vpylm->find_node_by_tracing_back_context(token_ids, t, depth_t, true, false);
	assert(node != NULL);
	assert(node->_depth == depth_t);
	if(depth_t > 0){	// ルートノードは特殊なので無視
		assert(node->_token_id == token_ids[t - depth_t]);
	}
	id token_t = token_ids[t];
	int tabke_k;
	return node->add_customer(token_t, vpylm->_g0, vpylm->_d_m, vpylm->_theta_m, true, tabke_k);
}

double compute_p_w_given_h(VPYLM* vpylm, wchar_t const* character_ids, int context_start, int context_end){
	int context_size = context_end - context_start + 1;
	Node<wchar_t>* node = vpylm->_root;
	wchar_t target_id = character_ids[context_end + 1];
	assert(node != NULL);
	double parent_pass_probability = 1;
	double p = 0;
	double eps = VPYLM_EPS;		// 停止確率がこの値を下回れば打ち切り
	double parent_pw = vpylm->_g0;
	double p_stop = 1;
	int depth = 0;
	while(p_stop > eps){
		// ノードがない場合親の確率とベータ事前分布から計算
		if(node == NULL){
			p_stop = (vpylm->_beta_stop) / (vpylm->_beta_pass + vpylm->_beta_stop) * parent_pass_probability;
			p += parent_pw * p_stop;
			parent_pass_probability *= (vpylm->_beta_pass) / (vpylm->_beta_pass + vpylm->_beta_stop);
		}else{
			assert(context_end - depth + 1 >= 0);
			assert(node->_depth == depth);
			wchar_t context_token_id = character_ids[context_end - depth];
			double pw = node->compute_p_w(target_id, vpylm->_g0, vpylm->_d_m, vpylm->_theta_m);
			p_stop = node->stop_probability(vpylm->_beta_stop, vpylm->_beta_pass);
			p += pw * p_stop;
			Node<wchar_t>* child = node->find_child_node(context_token_id);
			parent_pass_probability = node->pass_probability(vpylm->_beta_stop, vpylm->_beta_pass);
			parent_pw = pw;
			node = child;
			if(depth > 0 && node){
				assert(node->_token_id == context_token_id);
			}
		}
		depth++;
	}
	assert(p > 0);
	return p;
}
void test_compute_p_w_given_h(){
	VPYLM* vpylm = new VPYLM(0.001, 1000, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* wrapped_character_ids = new wchar_t[sentence_str.size() + 2];
	wrap_bow_eow(sentence->_character_ids, 0, sentence->size() - 1, wrapped_character_ids);
	for(int t = 0;t < sentence->size();t++){
		for(int depth_t = 0;depth_t <= t;depth_t++){
			vpylm->add_customer_at_time_t(wrapped_character_ids, t, depth_t);
		}
	}
	for(int end = 0;end < sentence->size() - 1;end++){
		for(int start = 0;start < end;start++){
			double a = vpylm->compute_p_w_given_h(wrapped_character_ids, start, end);
			double b = compute_p_w_given_h(vpylm, wrapped_character_ids, start, end);
			assert(std::abs(a - b) < 1e-16);
		}
	}
	delete sentence;
	delete vpylm;
	delete[] wrapped_character_ids;
}

void test_find_node_by_tracing_back_context(){
	VPYLM* vpylm = new VPYLM(0.001, 1000, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* character_ids = new wchar_t[sentence->size() + 2];
	wrap_bow_eow(sentence->_character_ids, 0, sentence->size() - 1, character_ids);
	for(int t = 0;t < sentence->size() + 2;t++){
		for(int depth_t = 0;depth_t <= t;depth_t++){
			vpylm->add_customer_at_time_t(character_ids, t, depth_t);
		}
	}

	double* parent_pw_cache = new double[sentence->size() + 2];
	for(int t = 1;t < sentence->size() + 2;t++){
		for(int depth_t = 1;depth_t <= t;depth_t++){
			Node<wchar_t>* node0 = vpylm->find_node_by_tracing_back_context(character_ids, t, depth_t - 1);
			assert(node0 != NULL);
			if(depth_t > 1){
				assert(node0->_token_id == character_ids[t - depth_t + 1]);
			}
			Node<wchar_t>* node1 = vpylm->find_node_by_tracing_back_context(character_ids, t, depth_t, parent_pw_cache);
			assert(node1->_token_id == character_ids[t - depth_t]);

			double p = node0->compute_p_w(character_ids[t], vpylm->_g0, vpylm->_d_m, vpylm->_theta_m);
			assert(parent_pw_cache[depth_t] == p);
		}
	}

	Node<wchar_t>** path_nodes_cache = new Node<wchar_t>*[sentence->size() + 2];
	for(int t = 1;t < sentence->size() + 2;t++){
		vpylm->sample_depth_at_time_t(character_ids, t, parent_pw_cache, path_nodes_cache);

		for(int depth_t = 1;depth_t <= t;depth_t++){
			Node<wchar_t>* node0 = vpylm->find_node_by_tracing_back_context(character_ids, t, depth_t - 1);
			assert(node0 != NULL);
			if(depth_t > 1){
				assert(node0->_token_id == character_ids[t - depth_t + 1]);
			}
			double p = node0->compute_p_w(character_ids[t], vpylm->_g0, vpylm->_d_m, vpylm->_theta_m);
			assert(parent_pw_cache[depth_t] == p);
			assert(node0 == path_nodes_cache[depth_t - 1]);
		}

		for(int depth_t = 1;depth_t <= t;depth_t++){
			path_nodes_cache[t - depth_t] = NULL;
			Node<wchar_t>* node0 = vpylm->find_node_by_tracing_back_context(character_ids, t, depth_t - 1);
			Node<wchar_t>* node1 = vpylm->find_node_by_tracing_back_context(character_ids, t, depth_t - 1, path_nodes_cache);
			assert(node0 != NULL);
			assert(node1 != NULL);
			assert(node0 == node1);
		}
	}

	delete sentence;
	delete vpylm;
	delete[] character_ids;
	delete[] path_nodes_cache;
	delete[] parent_pw_cache;
}

void test_add_customer(){
	sampler::mt.seed(0);
	VPYLM* vpylm1 = new VPYLM(0.001, 1000, 4, 1);
	VPYLM* vpylm2 = new VPYLM(0.001, 1000, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* character_ids = new wchar_t[sentence->size() + 2];
	wrap_bow_eow(sentence->_character_ids, 0, sentence->size() - 1, character_ids);
	int limit = 100;
	for(int t = 0;t < sentence->size();t++){
		for(int depth_t = 0;depth_t <= t;depth_t++){
			vpylm1->add_customer_at_time_t(character_ids, t, depth_t);
		}
	}
	sampler::mt.seed(0);
	for(int t = 0;t < sentence->size();t++){
		for(int depth_t = 0;depth_t <= t;depth_t++){
			add_customer_at_time_t(vpylm2, character_ids, t, depth_t);
		}
	}
	assert(vpylm1->get_num_nodes() == vpylm2->get_num_nodes());
	assert(vpylm1->get_num_customers() == vpylm2->get_num_customers());
	assert(vpylm1->get_num_tables() == vpylm2->get_num_tables());
	assert(vpylm1->get_sum_stop_counts() == vpylm2->get_sum_stop_counts());
	assert(vpylm1->get_sum_pass_counts() == vpylm2->get_sum_pass_counts());
	for(int end = 1;end < sentence->size() + 2;end++){
		for(int start = 0;start < end;start++){
			double a = vpylm1->compute_p_w(character_ids, end - start + 1);
			double b = vpylm2->compute_p_w(character_ids, end - start + 1);
			assert(a == b);
		}
	}
	delete sentence;
	delete vpylm1;
	delete vpylm2;
	delete[] character_ids;
}

void test_sample_depth_at_timestep(){
	sampler::mt.seed(0);
	VPYLM* vpylm = new VPYLM(0.001, 1000, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* character_ids = new wchar_t[sentence->size() + 2];
	wrap_bow_eow(sentence->_character_ids, 0, sentence->size() - 1, character_ids);
	int limit = 100;
	for(int t = 0;t < sentence->size();t++){
		for(int depth_t = 0;depth_t <= t;depth_t++){
			vpylm->add_customer_at_time_t(character_ids, t, depth_t);
		}
	}
	for(int t = 0;t < sentence->size();t++){
		for(int seed = 0;seed < 256;seed++){
			sampler::mt.seed(seed);
			int a = vpylm->sample_depth_at_time_t(character_ids, t, vpylm->_parent_pw_cache, vpylm->_path_nodes);
			sampler::mt.seed(seed);
			int b = sample_depth_at_time_t(vpylm, character_ids, t);
			assert(a == b);
		}
	}
	delete sentence;
	delete vpylm;
	delete[] character_ids;
}

int main(){
	setlocale(LC_CTYPE, "ja_JP.UTF-8");
	std::ios_base::sync_with_stdio(false);
	std::locale default_loc("ja_JP.UTF-8");
	std::locale::global(default_loc);
	std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype); //※
	std::wcout.imbue(ctype_default);
	std::wcin.imbue(ctype_default);

	test_compute_p_w_given_h();
	cout << "OK" << endl;
	test_find_node_by_tracing_back_context();
	cout << "OK" << endl;
	test_add_customer();
	cout << "OK" << endl;
	test_sample_depth_at_timestep();
	cout << "OK" << endl;
	return 0;
}