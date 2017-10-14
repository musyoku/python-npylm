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
	for(int i = 1;i <= sentence_str.size() - 3;i++){
		for(int m = 1;m <= sentence_str.size() - i - 1;m++){
			for(int k = 1;k <= sentence_str.size() - i - m - 1;k++){
				int n = sentence_str.size() - i - m - k;
				assert(i + m + k + n == sentence_str.size());
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
	for(int i = 1;i <= sentence_str.size() - 3;i++){
		for(int m = 1;m <= sentence_str.size() - i - 1;m++){
			for(int k = 1;k <= sentence_str.size() - i - m - 1;k++){
				int n = sentence_str.size() - i - m - k;
				assert(i + m + k + n == sentence_str.size());
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
	return 0;
}