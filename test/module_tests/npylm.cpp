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
#include "../../src/npylm/hash.h"
using namespace npylm;
using namespace npylm::lm;
using std::cout;
using std::flush;
using std::endl;

double compute_p_w_given_h(NPYLM* npylm, Sentence* sentence, int word_t_index){
	int substr_char_t_start = sentence->_start[word_t_index];
	int substr_char_t_end = sentence->_start[word_t_index] + sentence->_segments[word_t_index] - 1;
	int word_ids_length = sentence->get_num_segments();
	int character_ids_length = sentence->size();
	assert(word_t_index < word_ids_length);
	assert(substr_char_t_start >= 0);
	id word_id = sentence->_word_ids[word_t_index];
	if(word_t_index == word_ids_length - 1){
		assert(word_id == ID_EOS);
	}else{
		if(word_id != ID_EOS){
			assert(substr_char_t_end < character_ids_length);
			#ifdef __DEBUG__
			id a = hash_substring_ptr(sentence->_characters, substr_char_t_start, substr_char_t_end);
			assert(a == word_id);
			#endif
		}
	}
	Node<id>* node = npylm->find_node_by_tracing_back_context_from_time_t(sentence->_word_ids, word_ids_length, word_t_index, false, true);
	assert(node != NULL);
	double g0 = npylm->compute_g0_substring_at_time_t(sentence->_characters, sentence->size(), substr_char_t_start, substr_char_t_end, word_id);
	return node->compute_p_w(word_id, g0, npylm->_hpylm->_d_m, npylm->_hpylm->_theta_m);
}

void add_costmers(NPYLM* npylm, Sentence* sentence, int* segments_without_special_tokens){
	for(int repeat = 0;repeat < 5;repeat++){
		for(int i = 1;i <= sentence->_sentence_str.size() - 3;i++){
			for(int m = 1;m <= sentence->_sentence_str.size() - i - 2;m++){
				for(int k = 1;k <= sentence->_sentence_str.size() - i - m - 1;k++){
					int n = sentence->_sentence_str.size() - i - m - k;
					assert(i + m + k + n == sentence->_sentence_str.size());
					segments_without_special_tokens[0] = i;
					segments_without_special_tokens[1] = m;
					segments_without_special_tokens[2] = k;
					segments_without_special_tokens[3] = n;
					sentence->split(segments_without_special_tokens, 4);
					for(int t = 2;t < sentence->get_num_segments();t++){
						npylm->add_customer_at_time_t(sentence, t);
					}
				}
			}
		}
	}
}

void remove_costmers(NPYLM* npylm, Sentence* sentence, int* segments_without_special_tokens){
	for(int repeat = 0;repeat < 5;repeat++){
		for(int i = 1;i <= sentence->_sentence_str.size() - 3;i++){
			for(int m = 1;m <= sentence->_sentence_str.size() - i - 2;m++){
				for(int k = 1;k <= sentence->_sentence_str.size() - i - m - 1;k++){
					int n = sentence->_sentence_str.size() - i - m - k;
					assert(i + m + k + n == sentence->_sentence_str.size());
					segments_without_special_tokens[0] = i;
					segments_without_special_tokens[1] = m;
					segments_without_special_tokens[2] = k;
					segments_without_special_tokens[3] = n;
					sentence->split(segments_without_special_tokens, 4);
					for(int t = 2;t < sentence->get_num_segments();t++){
						npylm->remove_customer_at_time_t(sentence, t);
					}
				}
			}
		}
	}
}

void test_vpylm_add_customers(){
	VPYLM* vpylm = new VPYLM(0.001, 1000, 4, 1);
	NPYLM* npylm = new NPYLM(20, 10000, 0.001, 4, 1, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* token_ids = new wchar_t[sentence_str.size() + 2];

	double* parent_pw_cache = new double[sentence->size() + 2];
	Node<wchar_t>** path_nodes_cache = new Node<wchar_t>*[sentence->size() + 2];
	sampler::mt.seed(0);
	for(int n = 0;n < 100;n++){
		for(int t = 0;t < sentence->size();t++){
			wrap_bow_eow(sentence->_characters, 0, t, token_ids);
			for(int char_t = 0;char_t <= t + 2;char_t++){
				int depth = vpylm->sample_depth_at_time_t(token_ids, char_t, parent_pw_cache, path_nodes_cache);
				vpylm->add_customer_at_time_t(token_ids, char_t, depth);
			}
		}
	}

	sampler::mt.seed(0);
	for(int n = 0;n < 100;n++){
		for(int t = 0;t < sentence->size();t++){
			std::vector<int> prev_depths;
			npylm->vpylm_add_customers(sentence->_characters, 0, t, token_ids, prev_depths);
		}
	}
	assert(vpylm->get_num_nodes() == npylm->_vpylm->get_num_nodes());
	assert(vpylm->get_num_customers() == npylm->_vpylm->get_num_customers());
	assert(vpylm->get_num_tables() == npylm->_vpylm->get_num_tables());
	assert(vpylm->get_sum_stop_counts() == npylm->_vpylm->get_sum_stop_counts());
	assert(vpylm->get_sum_pass_counts() == npylm->_vpylm->get_sum_pass_counts());

	delete[] parent_pw_cache;
	delete[] path_nodes_cache;
	delete[] token_ids;
	delete sentence;
	delete vpylm;
	delete npylm;
}

void test_vpylm_remove_customers(){
	NPYLM* npylm = new NPYLM(20, 10000, 0.001, 4, 1, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* token_ids = new wchar_t[sentence_str.size() + 2];

	std::vector<std::vector<int>> prev_depths_list;
	for(int n = 0;n < 100;n++){
		for(int t = 0;t < sentence->size();t++){
			std::vector<int> prev_depths;
			npylm->vpylm_add_customers(sentence->_characters, 0, t, token_ids, prev_depths);
			prev_depths_list.push_back(prev_depths);
		}
	}

	auto itr = prev_depths_list.begin();
	for(int n = 0;n < 100;n++){
		for(int t = 0;t < sentence->size();t++){
			std::vector<int> &prev_depths = *itr;
			npylm->vpylm_remove_customers(sentence->_characters, 0, t, token_ids, prev_depths);
			itr++;
		}
	}

	assert(npylm->_vpylm->get_num_customers() == 0);
	assert(npylm->_vpylm->get_num_tables() == 0);
	assert(npylm->_vpylm->get_sum_stop_counts() == 0);
	assert(npylm->_vpylm->get_sum_pass_counts() == 0);

	delete[] token_ids;
	delete sentence;
	delete npylm;
}

void test_remove_customer_at_time_t(){
	NPYLM* npylm = new NPYLM(20, 10000, 0.001, 4, 1, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* token_ids = new wchar_t[sentence_str.size() + 2];
	int* segments_without_special_tokens = new int[4];
	add_costmers(npylm, sentence, segments_without_special_tokens);
	remove_costmers(npylm, sentence, segments_without_special_tokens);

	assert(npylm->_vpylm->get_num_customers() == 0);
	assert(npylm->_vpylm->get_num_tables() == 0);
	assert(npylm->_vpylm->get_sum_stop_counts() == 0);
	assert(npylm->_vpylm->get_sum_pass_counts() == 0);

	assert(npylm->_hpylm->get_num_customers() == 0);
	assert(npylm->_hpylm->get_num_tables() == 0);
	assert(npylm->_hpylm->get_sum_stop_counts() == 0);
	assert(npylm->_hpylm->get_sum_pass_counts() == 0);

	delete[] segments_without_special_tokens;
	delete npylm;
}

void test_find_node_by_tracing_back_context_from_time_t(){
	NPYLM* npylm = new NPYLM(20, 10000, 0.001, 4, 1, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* token_ids = new wchar_t[sentence_str.size() + 2];
	int* segments_without_special_tokens = new int[4];
	add_costmers(npylm, sentence, segments_without_special_tokens);
	double* parent_pw_cache = new double[3];

	for(int t = 2;t < sentence->get_num_segments();t++){
		Node<id>* node = npylm->find_node_by_tracing_back_context_from_time_t(sentence, t, parent_pw_cache, false, false);
		Node<id>* parent = node->_parent;
		id word_t_id = sentence->get_word_id_at(t);
		int substr_char_t_start = sentence->_start[t];
		int substr_char_t_end = sentence->_start[t] + sentence->_segments[t] - 1;
		double g0 = npylm->compute_g0_substring_at_time_t(sentence->_characters, sentence->size(), substr_char_t_start, substr_char_t_end, word_t_id);
		double pw = parent->compute_p_w(word_t_id, g0, npylm->_hpylm->_d_m, npylm->_hpylm->_theta_m);
		assert(pw == parent_pw_cache[2]);
	}

	delete[] parent_pw_cache;
	delete npylm;
}

void test_compute_p_w_given_h(){
	NPYLM* npylm = new NPYLM(20, 10000, 0.001, 4, 1, 4, 1);
	std::wstring sentence_str = L"本論文では, 教師データや辞書を必要とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する.";
	Sentence* sentence = new Sentence(sentence_str);
	wchar_t* token_ids = new wchar_t[sentence_str.size() + 2];
	int* segments_without_special_tokens = new int[4];
	add_costmers(npylm, sentence, segments_without_special_tokens);
	double* parent_pw_cache = new double[3];

	for(int t = 2;t < sentence->get_num_segments();t++){
		double p1 = compute_p_w_given_h(npylm, sentence, t);
		double p2 = npylm->compute_p_w_given_h(sentence, t);
		assert(p1 == p2);
	}

	delete[] parent_pw_cache;
	delete npylm;
}

int main(){
	setlocale(LC_CTYPE, "ja_JP.UTF-8");
	std::ios_base::sync_with_stdio(false);
	std::locale default_loc("ja_JP.UTF-8");
	std::locale::global(default_loc);
	std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype); //※
	std::wcout.imbue(ctype_default);
	std::wcin.imbue(ctype_default);

	test_vpylm_add_customers();
	cout << "OK" << endl;
	test_vpylm_remove_customers();
	cout << "OK" << endl;
	test_remove_customer_at_time_t();
	cout << "OK" << endl;
	test_find_node_by_tracing_back_context_from_time_t();
	cout << "OK" << endl;
	test_compute_p_w_given_h();
	cout << "OK" << endl;
	return 0;
}