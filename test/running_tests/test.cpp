#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <chrono>
#include <fstream>
#include "src/hash.h"
#include "model.cpp"
using namespace std;

void test_npylm_find_node_by_tracing_back_context(){
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	string filename = "dataset/karen.txt";
	wifstream ifs(filename.c_str());
	wstring str;
	assert(ifs.fail() == false);
	int i = 0;
	while (getline(ifs, str) && !str.empty()){
		Sentence* sentence = new Sentence(str);
		for(int i = 2;i <= 3;i++){
			Node<id>* node = npylm->find_node_by_tracing_back_context_from_time_t(sentence->_word_ids, sentence->_num_segments, i, true, false);
			assert(node->_depth == 2);
			assert(node->_token_id == ID_BOS);
		}
		cout << "\r" << i << ": OK" << flush;
		i++;
		delete sentence;
	}
}

void test_npylm_find_node_by_tracing_back_context_and_store_pw(){
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	string filename = "dataset/karen.txt";
	wifstream ifs(filename.c_str());
	wstring str;
	assert(ifs.fail() == false);
	int i = 0;
	int* segments = new int[3];
	segments[0] = 0;
	segments[1] = 0;
	segments[2] = 0;
	int segment_index = 0;
	while (getline(ifs, str) && !str.empty()){
		Sentence* sentence = new Sentence(str);
		for(int t = 1;t <= str.size();t++){
			for(int k = 1;k <= std::min((size_t)t, str.size());k++){
				segment_index = 0;
				int seg = t - k;
				segments[segment_index] = seg;
				if(seg > 0){
					segment_index++;
				}
				segments[segment_index] = k;
				if(t < sentence->size()){
					segment_index++;
					segments[segment_index] = sentence->size() - t;
				}
				int sum = 0;
				for(int i = 0;i <= segment_index;i++){
					sum += segments[i];
				}
				assert(sum == sentence->size());
				sentence->split(segments, segment_index + 1);

				Node<id>* node = npylm->find_node_by_tracing_back_context_from_time_t(sentence, segment_index + 2, npylm->_hpylm_parent_pw_cache, true, false);
				if(segment_index < 2){
					assert(node->_token_id == ID_BOS);
				}else{
					wstring substr(str.begin(), str.begin() + segments[segment_index - 2]);
					id b = npylm::hash_wstring(substr);
					assert(node->_token_id == b);
				}
			}
		}
		cout << "\r" << i << ": OK" << flush;
		i++;
		delete sentence;
	}
}

bool npylm_add_customer_at_time_t(NPYLM* npylm, Sentence* sentence, int t){
	Node<id>* node = npylm->find_node_by_tracing_back_context_from_time_t(sentence, t, npylm->_hpylm_parent_pw_cache, true, false);
	assert(node != NULL);
	id const* token_ids = sentence->_word_ids;
	id token_t = token_ids[t];
	int num_tables_before = npylm->_hpylm->_root->_num_tables;
	int added_table_k = -1;
	int substr_start = sentence->_start[t];
	int substr_end = sentence->_start[t] + sentence->_segments[t] - 1;
	int substr_length = substr_end - substr_start + 1;
	id word_id = sentence->get_substr_word_id(substr_start, substr_end);
	double g0 = npylm->compute_g0_substring_at_time_t(sentence->_character_ids, sentence->size(), sentence->_start[t], sentence->_start[t] + sentence->_segments[t] - 1, word_id);
	node->add_customer(token_t, g0, npylm->_hpylm->_d_m, npylm->_hpylm->_theta_m, true, added_table_k);
	if(token_t == ID_EOS){
		int added_to_table_k;
		npylm->_vpylm->_root->add_customer(token_t, npylm->_vpylm->_g0, npylm->_vpylm->_d_m, npylm->_vpylm->_theta_m, true, added_to_table_k);
		return true;
	}
	int num_tables_after = npylm->_hpylm->_root->_num_tables;
	if(num_tables_before < num_tables_after){
		assert(added_table_k >= 0);
		vector<vector<int>> &depths = npylm->_prev_depths_for_token_at_table[token_t];
		assert(depths.size() <= added_table_k);	// 存在してはいけない
		vector<int> prev_depths;
		// 先頭に<bow>をつける
		wchar_t const* character_ids = sentence->_character_ids;
		int original_character_ids_length = sentence->size();
		wchar_t* wrapped_character_ids = npylm->_character_ids;
		bow_eow(character_ids, substr_start, substr_end, wrapped_character_ids);
		int wrapped_character_ids_length = substr_end - substr_start + 3;
		// 客を追加
		for(int char_t = 0;char_t < wrapped_character_ids_length;char_t++){
			int depth_t = npylm->_vpylm->sample_depth_at_time_t(wrapped_character_ids, char_t, npylm->_vpylm->_parent_pw_cache, npylm->_vpylm->_path_nodes);
			npylm->_vpylm->add_customer_at_time_t(wrapped_character_ids, char_t, depth_t, npylm->_vpylm->_parent_pw_cache, npylm->_vpylm->_path_nodes);	// キャッシュを使って追加
			prev_depths.push_back(depth_t);
		}
		npylm->_g0_cache.clear();
	}
	return true;
}
void test_npylm_add_customer_at_time_t(){
	string filename = "dataset/karen.txt";
	int* segments = new int[3];
	segments[0] = 0;
	segments[1] = 0;
	segments[2] = 0;
	NPYLM* npylm1 = new NPYLM(20, 1000, 0.00001);
	NPYLM* npylm2 = new NPYLM(20, 1000, 0.00001);
	int max_sentences = 10000;
	{
		int limit = max_sentences;
		sampler::mt.seed(0);
		wifstream ifs(filename.c_str());
		wstring str;
		assert(ifs.fail() == false);
		int segment_index = 0;
		while (getline(ifs, str) && !str.empty()){
			limit--;
			if(limit < 0){
				break;
			}
			Sentence* sentence = new Sentence(str);
			for(int t = 1;t <= str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, str.size());k++){
					segment_index = 0;
					int seg = t - k;
					segments[segment_index] = seg;
					if(seg > 0){
						segment_index++;
					}
					segments[segment_index] = k;
					if(t < sentence->size()){
						segment_index++;
						segments[segment_index] = sentence->size() - t;
					}
					int sum = 0;
					for(int i = 0;i <= segment_index;i++){
						sum += segments[i];
					}
					assert(sum == sentence->size());
					sentence->split(segments, segment_index + 1);
					for(int i = 0;i <= segment_index;i++){
						npylm1->add_customer_at_time_t(sentence, i + 2);
					}
				}
			}
			delete sentence;
			cout << "\r" << limit << flush;
		}
	}
	{
		int limit = max_sentences;
		sampler::mt.seed(0);
		wifstream ifs(filename.c_str());
		wstring str;
		assert(ifs.fail() == false);
		int segment_index = 0;
		while (getline(ifs, str) && !str.empty()){
			limit--;
			if(limit < 0){
				break;
			}
			Sentence* sentence = new Sentence(str);
			for(int t = 1;t <= str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, str.size());k++){
					segment_index = 0;
					int seg = t - k;
					segments[segment_index] = seg;
					if(seg > 0){
						segment_index++;
					}
					segments[segment_index] = k;
					if(t < sentence->size()){
						segment_index++;
						segments[segment_index] = sentence->size() - t;
					}
					int sum = 0;
					for(int i = 0;i <= segment_index;i++){
						sum += segments[i];
					}
					assert(sum == sentence->size());
					sentence->split(segments, segment_index + 1);
					for(int i = 0;i <= segment_index;i++){
						npylm_add_customer_at_time_t(npylm2, sentence, i + 2);
					}
				}
			}
			delete sentence;
			cout << "\r" << limit << flush;
		}
	}
	cout << npylm1->_vpylm->get_num_nodes() << " == " << npylm2->_vpylm->get_num_nodes() << endl;
	cout << npylm1->_vpylm->get_num_customers() << " == " << npylm2->_vpylm->get_num_customers() << endl;
	cout << npylm1->_vpylm->get_num_tables() << " == " << npylm2->_vpylm->get_num_tables() << endl;
	cout << npylm1->_vpylm->get_sum_stop_counts() << " == " << npylm2->_vpylm->get_sum_stop_counts() << endl;
	cout << npylm1->_vpylm->get_sum_pass_counts() << " == " << npylm2->_vpylm->get_sum_pass_counts() << endl;
	assert(npylm1->_vpylm->get_num_nodes() == npylm2->_vpylm->get_num_nodes());
	assert(npylm1->_vpylm->get_num_customers() == npylm2->_vpylm->get_num_customers());
	assert(npylm1->_vpylm->get_num_tables() == npylm2->_vpylm->get_num_tables());
	assert(npylm1->_vpylm->get_sum_stop_counts() == npylm2->_vpylm->get_sum_stop_counts());
	assert(npylm1->_vpylm->get_sum_pass_counts() == npylm2->_vpylm->get_sum_pass_counts());
}

void test_npylm_add_customer_at_time_t2(){
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	wstring str = L"あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをん";
	Sentence* sentence = new Sentence(str);
	wchar_t* wrapped_character_ids = new wchar_t[1000];
	bow_eow(sentence->_character_ids, 0, sentence->size() - 1, wrapped_character_ids);
	npylm->_vpylm->add_customer_at_time_t(wrapped_character_ids, str.size() - 1, str.size() - 1);
	npylm->_vpylm->sample_depth_at_time_t(wrapped_character_ids, str.size() - 1, npylm->_vpylm->_parent_pw_cache, npylm->_vpylm->_path_nodes);
	delete[] wrapped_character_ids;
}

void test_npylm_remove_customer_at_time_t(){
	string filename = "dataset/karen.txt";
	int* segments = new int[3];
	segments[0] = 0;
	segments[1] = 0;
	segments[2] = 0;
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	int max_sentences = 10000;
	int repeat = 10;
	wstring str;
	int segment_index = 0;
	{
		int limit = max_sentences;
		wifstream ifs(filename.c_str());
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			limit--;
			if(limit < 0){
				break;
			}
			Sentence* sentence = new Sentence(str);
			for(int t = 1;t <= str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, str.size());k++){
					segment_index = 0;
					int seg = t - k;
					segments[segment_index] = seg;
					if(seg > 0){
						segment_index++;
					}
					segments[segment_index] = k;
					if(t < sentence->size()){
						segment_index++;
						segments[segment_index] = sentence->size() - t;
					}
					int sum = 0;
					for(int i = 0;i <= segment_index;i++){
						sum += segments[i];
					}
					assert(sum == sentence->size());
					sentence->split(segments, segment_index + 1);
					for(int r = 0;r < repeat;r++){
						for(int i = 0;i <= segment_index;i++){
							npylm->add_customer_at_time_t(sentence, i + 2);
						}
					}
				}
			}
			delete sentence;
			cout << "\r" << limit << flush;
		}
	}
	{
		int limit = max_sentences;
		wifstream ifs(filename.c_str());
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			limit--;
			if(limit < 0){
				break;
			}
			Sentence* sentence = new Sentence(str);
			for(int t = 1;t <= str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, str.size());k++){
					segment_index = 0;
					int seg = t - k;
					segments[segment_index] = seg;
					if(seg > 0){
						segment_index++;
					}
					segments[segment_index] = k;
					if(t < sentence->size()){
						segment_index++;
						segments[segment_index] = sentence->size() - t;
					}
					int sum = 0;
					for(int i = 0;i <= segment_index;i++){
						sum += segments[i];
					}
					assert(sum == sentence->size());
					sentence->split(segments, segment_index + 1);
					for(int r = 0;r < repeat;r++){
						for(int i = 0;i <= segment_index;i++){
							npylm->remove_customer_at_time_t(sentence, i + 2);
						}
					}
				}
			}
			delete sentence;
			cout << "\r" << limit << flush;
		}
	}
	cout << endl;
	cout << npylm->_vpylm->get_num_nodes() << endl;
	cout << npylm->_vpylm->get_num_customers() << endl;
	cout << npylm->_vpylm->get_num_tables() << endl;
	cout << npylm->_vpylm->get_sum_stop_counts() << endl;
	cout << npylm->_vpylm->get_sum_pass_counts() << endl;
	assert(npylm->_vpylm->get_num_nodes() == 1);
	assert(npylm->_vpylm->get_num_customers() == 0);
	assert(npylm->_vpylm->get_num_tables() == 0);
	assert(npylm->_vpylm->get_sum_stop_counts() == 0);
	assert(npylm->_vpylm->get_sum_pass_counts() == 0);
	cout << npylm->_hpylm->get_num_nodes() << endl;
	cout << npylm->_hpylm->get_num_customers() << endl;
	cout << npylm->_hpylm->get_num_tables() << endl;
	cout << npylm->_hpylm->get_sum_stop_counts() << endl;
	cout << npylm->_hpylm->get_sum_pass_counts() << endl;
	assert(npylm->_hpylm->get_num_nodes() == 1);
	assert(npylm->_hpylm->get_num_customers() == 0);
	assert(npylm->_hpylm->get_num_tables() == 0);
	assert(npylm->_hpylm->get_sum_stop_counts() == 0);
	assert(npylm->_hpylm->get_sum_pass_counts() == 0);
}
void test_npylm_sample_lambda(){
	string filename = "dataset/karen.txt";
	int* segments = new int[3];
	segments[0] = 0;
	segments[1] = 0;
	segments[2] = 0;
	PyTrainer* model = new PyTrainer();
	model->add_textfile(filename, 0.95);
	model->compile();
	int repeat = 10;
	int segment_index = 0;
	for(const auto sentence: model->_dataset_train){
		for(int t = 1;t <= sentence->size();t++){
			for(int k = 1;k <= std::min(t, sentence->size());k++){
				segment_index = 0;
				int seg = t - k;
				segments[segment_index] = seg;
				if(seg > 0){
					segment_index++;
				}
				segments[segment_index] = k;
				if(t < sentence->size()){
					segment_index++;
					segments[segment_index] = sentence->size() - t;
				}
				int sum = 0;
				for(int i = 0;i <= segment_index;i++){
					sum += segments[i];
				}
				assert(sum == sentence->size());
				sentence->split(segments, segment_index + 1);
				for(int r = 0;r < repeat;r++){
					for(int i = 0;i <= segment_index;i++){
						model->_npylm->add_customer_at_time_t(sentence, i + 2);
					}
				}
			}
		}
	}
	model->sample_lambda();
	cout << model->_npylm->_vpylm->get_num_nodes() << endl;
	cout << model->_npylm->_vpylm->get_num_customers() << endl;
	cout << model->_npylm->_vpylm->get_num_tables() << endl;
	cout << model->_npylm->_vpylm->get_sum_stop_counts() << endl;
	cout << model->_npylm->_vpylm->get_sum_pass_counts() << endl;
}
double npylm_compute_Pw_h(NPYLM* npylm,
		wchar_t const* character_ids, int character_ids_length, 
		id const* word_ids, int word_ids_length, 
		int word_t, int char_t_start, int char_t_end){
	Node<id>* node = npylm->find_node_by_tracing_back_context_from_time_t(character_ids, character_ids_length, word_ids, word_ids_length, word_t, char_t_start, char_t_end, npylm->_hpylm_parent_pw_cache, false, true);
	id word_id = word_ids[word_t];
	double g0 = npylm->compute_g0_substring_at_time_t(character_ids, character_ids_length, char_t_start, char_t_end, word_id);
	return node->compute_Pw(word_id, g0, npylm->_hpylm->_d_m, npylm->_hpylm->_theta_m);
}
double npylm_compute_Pw_h(NPYLM* npylm, Sentence* sentence, int word_t){
	assert(word_t >= 2);
	assert(word_t < sentence->get_num_segments());
	assert(sentence->_segments[word_t] > 0);
	int substr_start = sentence->_start[word_t];
	int substr_end = sentence->_start[word_t] + sentence->_segments[word_t] - 1;
	return npylm_compute_Pw_h(npylm, sentence->_character_ids, sentence->size(), sentence->_word_ids, sentence->get_num_segments(), word_t, substr_start, substr_end);
}
void npylm_test_compute_Pw_h(){
	string filename = "dataset/karen.txt";
	int* segments = new int[3];
	segments[0] = 0;
	segments[1] = 0;
	segments[2] = 0;
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	int max_sentences = 10000;
	int repeat = 100;
	wstring str;
	int segment_index = 0;
	{
		int limit = max_sentences;
		wifstream ifs(filename.c_str());
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			limit--;
			if(limit < 0){
				break;
			}
			Sentence* sentence = new Sentence(str);
			for(int t = 1;t <= str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, str.size());k++){
					segment_index = 0;
					int seg = t - k;
					segments[segment_index] = seg;
					if(seg > 0){
						segment_index++;
					}
					segments[segment_index] = k;
					if(t < sentence->size()){
						segment_index++;
						segments[segment_index] = sentence->size() - t;
					}
					int sum = 0;
					for(int i = 0;i <= segment_index;i++){
						sum += segments[i];
					}
					assert(sum == sentence->size());
					sentence->split(segments, segment_index + 1);
					for(int r = 0;r < repeat;r++){
						for(int i = 0;i <= segment_index;i++){
							npylm->add_customer_at_time_t(sentence, i + 2);
						}
					}
				}
			}
			delete sentence;
			cout << "\r" << limit << flush;
		}
	}
	{
		int limit = max_sentences;
		wifstream ifs(filename.c_str());
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			limit--;
			if(limit < 0){
				break;
			}
			Sentence* sentence = new Sentence(str);
			for(int t = 1;t <= str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, str.size());k++){
					segment_index = 0;
					int seg = t - k;
					segments[segment_index] = seg;
					if(seg > 0){
						segment_index++;
					}
					segments[segment_index] = k;
					if(t < sentence->size()){
						segment_index++;
						segments[segment_index] = sentence->size() - t;
					}
					int sum = 0;
					for(int i = 0;i <= segment_index;i++){
						sum += segments[i];
					}
					assert(sum == sentence->size());
					sentence->split(segments, segment_index + 1);
					double a = npylm->compute_Pw_h(sentence, 2);
					double b = npylm_compute_Pw_h(npylm, sentence, 2);
					assert(a == b);
				}
			}
			delete sentence;
			cout << "\r" << limit << flush;
		}
	}
}		
void test_lattice_perform_blocked_gibbs_sampling(){
	string filename = "dataset/karen.txt";
	int* segments = new int[3];
	segments[0] = 0;
	segments[1] = 0;
	segments[2] = 0;
	PyTrainer* model = new PyTrainer();
	model->add_textfile(filename, 0.95);
	model->compile();
	int repeat = 10;
	int segment_index = 0;
	for(const auto sentence: model->_dataset_train){
		for(int t = 1;t <= sentence->size();t++){
			for(int k = 1;k <= std::min(t, sentence->size());k++){
				segment_index = 0;
				int seg = t - k;
				segments[segment_index] = seg;
				if(seg > 0){
					segment_index++;
				}
				segments[segment_index] = k;
				if(t < sentence->size()){
					segment_index++;
					segments[segment_index] = sentence->size() - t;
				}
				int sum = 0;
				for(int i = 0;i <= segment_index;i++){
					sum += segments[i];
				}
				assert(sum == sentence->size());
				sentence->split(segments, segment_index + 1);
				for(int r = 0;r < repeat;r++){
					for(int i = 0;i <= segment_index;i++){
						model->_npylm->add_customer_at_time_t(sentence, i + 2);
					}
				}
			}
		}
	}
	cout << model->_npylm->_vpylm->get_num_nodes() << endl;
	cout << model->_npylm->_vpylm->get_num_customers() << endl;
	cout << model->_npylm->_vpylm->get_num_tables() << endl;
	cout << model->_npylm->_vpylm->get_sum_stop_counts() << endl;
	cout << model->_npylm->_vpylm->get_sum_pass_counts() << endl;

	vector<int> seg;
	for(int i = 0;i < 100;i++){
		for(const auto sentence: model->_dataset_train){
			model->_lattice->perform_blocked_gibbs_sampling(sentence, seg);
			sentence->split(seg);
			sentence->dump_words();
		}
	}
}		
void test_npylm_update_pk_vpylm(){
	string filename = "dataset/karen.txt";
	int* segments = new int[3];
	segments[0] = 0;
	segments[1] = 0;
	segments[2] = 0;
	PyTrainer* model = new PyTrainer();
	model->add_textfile(filename, 0.95);
	model->compile();
	int repeat = 10;
	int segment_index = 0;
	for(const auto sentence: model->_dataset_train){
		for(int t = 1;t <= sentence->size();t++){
			for(int k = 1;k <= std::min(t, sentence->size());k++){
				segment_index = 0;
				int seg = t - k;
				segments[segment_index] = seg;
				if(seg > 0){
					segment_index++;
				}
				segments[segment_index] = k;
				if(t < sentence->size()){
					segment_index++;
					segments[segment_index] = sentence->size() - t;
				}
				int sum = 0;
				for(int i = 0;i <= segment_index;i++){
					sum += segments[i];
				}
				assert(sum == sentence->size());
				sentence->split(segments, segment_index + 1);
				for(int r = 0;r < repeat;r++){
					for(int i = 0;i <= segment_index;i++){
						model->_npylm->add_customer_at_time_t(sentence, i + 2);
					}
				}
			}
		}
	}
	cout << model->_npylm->_vpylm->get_num_nodes() << endl;
	cout << model->_npylm->_vpylm->get_num_customers() << endl;
	cout << model->_npylm->_vpylm->get_num_tables() << endl;
	cout << model->_npylm->_vpylm->get_sum_stop_counts() << endl;
	cout << model->_npylm->_vpylm->get_sum_pass_counts() << endl;

	model->update_Pk_vpylm();
}		
void test_npylm_perform_gibbs_sampling(){
	string filename = "dataset/karen.txt";
	PyTrainer* model = new PyTrainer();
	model->_max_word_length = 30;
	model->add_textfile(filename, 0.95);
	model->compile();
	model->_always_accept_new_segmentation = true;
	double ppl = model->compute_perplexity_test();
	cout << ppl << endl;
	// for(int i = 0;i < 50;i++){
	// 	model->pretrain_vpylm();
	// 	cout << model->vpylm_compute_perplexity_train() << " ppl(train) - ";
	// 	cout << model->vpylm_compute_perplexity_test() << " ppl(test)" << endl;
	// }
	// cout << "VPYLM:" << endl;		
	// printf("	# of nodes: %d\n", model->_npylm->_vpylm->get_num_nodes());
	// printf("	# of customers: %d\n", model->_npylm->_vpylm->get_num_customers());
	// printf("	# of tables: %d\n", model->_npylm->_vpylm->get_num_tables());
	// printf("	stop count: %d\n", model->_npylm->_vpylm->get_sum_stop_counts());
	// printf("	pass count: %d\n", model->_npylm->_vpylm->get_sum_pass_counts());
	for(int i = 0;i < 1000;i++){
		auto start = std::chrono::system_clock::now();
		model->perform_gibbs_sampling();
		auto end = std::chrono::system_clock::now();
		auto diff = end - start;
		cout << model->_dataset_train.size() / (double)std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() * 1000.0 << " sentences/sec" << endl;
		if(i > 0){
			model->_always_accept_new_segmentation = true;
			model->update_Pk_vpylm();
			model->sample_lambda();
			model->sample_pitman_yor_hyperparameters();
		}
		model->show_viterbi_segmentation_train(10);
		model->show_viterbi_segmentation_test(10);
		model->dump_hpylm();
		model->dump_vpylm();
		cout << model->get_acceptance_ratio_of_new_segmentation();
		model->reset_acceptance_ratio();
	}
	delete model;
}
void test_npylm_remove_all_data(){
	string filename = "dataset/karen.txt";
	PyTrainer* model = new PyTrainer();
	model->add_textfile(filename, 0.95);
	model->compile();
	for(int i = 0;i < 5;i++){
		auto start = std::chrono::system_clock::now();
		model->perform_gibbs_sampling();
		auto end = std::chrono::system_clock::now();
		auto diff = end - start;
		cout << model->_dataset_train.size() / (double)std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() * 1000.0 << " sentences/sec" << endl;
		model->update_Pk_vpylm();
		model->sample_lambda();
		model->sample_pitman_yor_hyperparameters();
		for(int j = 0;j < 10;j++){
			int index = model->_rand_indices_train[j];
			Sentence* sentence = model->_dataset_train[index];
			sentence->dump_words();
		}
	}
	model->remove_all_data();
	cout << "HPYLM:" << endl;		
	printf("	# of nodes: %d\n", model->_npylm->_hpylm->get_num_nodes());
	printf("	# of customers: %d\n", model->_npylm->_hpylm->get_num_customers());
	printf("	# of tables: %d\n", model->_npylm->_hpylm->get_num_tables());
	printf("	stop count: %d\n", model->_npylm->_hpylm->get_sum_stop_counts());
	printf("	pass count: %d\n", model->_npylm->_hpylm->get_sum_pass_counts());
	cout << "VPYLM:" << endl;		
	printf("	# of nodes: %d\n", model->_npylm->_vpylm->get_num_nodes());
	printf("	# of customers: %d\n", model->_npylm->_vpylm->get_num_customers());
	printf("	# of tables: %d\n", model->_npylm->_vpylm->get_num_tables());
	printf("	stop count: %d\n", model->_npylm->_vpylm->get_sum_stop_counts());
	printf("	pass count: %d\n", model->_npylm->_vpylm->get_sum_pass_counts());
	delete model;
}
void test_npylm_pw(){
	string filename = "dataset/karen.txt";
	PyTrainer* model = new PyTrainer();
	model->add_textfile(filename, 0.95);
	model->compile();
	model->_always_accept_new_segmentation = false;
	for(int i = 0;i < 2;i++){
		model->perform_gibbs_sampling();
	}
	delete model;
}
void test_npylm_viterbi(){
	string filename = "dataset/karen.txt";
	PyNPYLM* npylm = new PyNPYLM("out");
	wifstream ifs(filename.c_str());
	wstring str;
	assert(ifs.fail() == false);
	while (getline(ifs, str) && !str.empty()){
		python::list words = npylm->parse(str);
	}
	delete npylm;
}
void test_train(){
	string filename = "dataset/test.txt";
	PyTrainer* model = new PyTrainer();
	model->_max_word_length = 16;
	model->add_textfile(filename, 0.99);
	model->_always_accept_new_segmentation = true;
	for(int i = 0;i < 100;i++){
		auto start = std::chrono::system_clock::now();
		model->perform_gibbs_sampling();
		auto end = std::chrono::system_clock::now();
		auto diff = end - start;
		if(i > 0){
			model->_always_accept_new_segmentation = true;
			model->update_Pk_vpylm();
			model->sample_lambda();
			model->sample_pitman_yor_hyperparameters();
		}
		model->show_viterbi_segmentation_train(10);
		model->show_viterbi_segmentation_test(10);
		model->dump_hpylm();
		model->dump_vpylm();
	}
	delete model;
}
void test_hash_collision(){
	string filename = "dataset/karen.txt";
	wstring str;
	int i = 0;
	int max_word_length = 20;
	hashmap<id, wstring> pool;
	wifstream ifs(filename.c_str());
	assert(ifs.fail() == false);
	while (getline(ifs, str) && !str.empty()){
		Sentence* sentence = new Sentence(str);
		for(int t = 1;t <= sentence->size();t++){
			for(int k = 1;k <= std::min(t, max_word_length);k++){
				id word_id = sentence->get_substr_word_id(t - k, t - 1);
				wstring word = sentence->get_substr_word_str(t - k, t - 1);
				assert(word_id == hash_wstring(word));
				auto itr = pool.find(word_id);
				if(itr == pool.end()){
					pool[word_id] = word;
				}else{
					assert(itr->second == word);
				}
			}
		}
		delete sentence;
		i++;
		cout << "\r" << i << flush;
	}
	cout << "OK" << endl;
}

void split_word_by(const wstring &str, wchar_t delim, vector<wstring> &elems){
	elems.clear();
	wstring item;
	for(wchar_t ch: str){
		if (ch == delim){
			if (!item.empty()){
				elems.push_back(item);
			}
			item.clear();
		}
		else{
			item += ch;
		}
	}
	if (!item.empty()){
		elems.push_back(item);
	}
}

void test_vpylm_equiv(){
	sampler::mt.seed(0);
	vector<Sentence*> dataset_train;
	vector<Sentence*> dataset_test;
	double train_split_ratio = 0.87;
	vector<int> rand_indices;
	VPYLM* vpylm = new VPYLM(1000);
	vpylm->set_g0(1.0 / 9999.0);

	unordered_map<wstring, wchar_t> vocab;
	vocab[L"<bow>"] = ID_BOW;
	vocab[L"<bos>"] = ID_BOS;
	vocab[L"<eow>"] = ID_EOW;
	vocab[L"<eos>"] = ID_EOS;

	string filename = "dataset/karen.txt";
	{
		wifstream ifs(filename.c_str());
		wstring str;
		if (ifs.fail()){
			return;
		}
		vector<wstring> lines;
		while (getline(ifs, str) && !str.empty()){
			if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
				return;
			}
			lines.push_back(str);
		}
		for(int i = 0;i < lines.size();i++){
			rand_indices.push_back(i);
		}
		int train_split = lines.size() * train_split_ratio;
		shuffle(rand_indices.begin(), rand_indices.end(), sampler::mt);	// データをシャッフル
		for(int i = 0;i < rand_indices.size();i++){
			wstring &str = lines[rand_indices[i]];

			vector<wstring> words;
			split_word_by(str, L' ', words);	// スペースで分割
			if(words.size() > 0){
				wstring token_ids = L"";
				for(auto word: words){
					if(word.size() == 0){
						continue;
					}
					auto itr = vocab.find(word);
					wchar_t token_id = 0;
					if(itr == vocab.end()){
						token_id = vocab.size();
						vocab[word] = token_id;
					}else{
						token_id = itr->second;
					}
					token_ids += token_id;
				}
				Sentence* sentence = new Sentence(token_ids);
				if(i < train_split){
					dataset_train.push_back(sentence);
				}else{
					dataset_test.push_back(sentence);
				}
			}
		}
	}

	rand_indices.clear();
	for(int i = 0;i < dataset_train.size();i++){
		rand_indices.push_back(i);
	}

	vector<vector<int>> _prev_depths_for_data;
	for(int data_index = 0;data_index < dataset_train.size();data_index++){
		Sentence* sentence = dataset_train[data_index];
		vector<int> prev_depths(sentence->size() + 2, -1);
		_prev_depths_for_data.push_back(prev_depths);
	}
	cout << dataset_train.size() << endl;
	cout << dataset_test.size() << endl;
	wchar_t* wrapped_character_ids = new wchar_t[1000];
	shuffle(rand_indices.begin(), rand_indices.end(), sampler::mt);	// データをシャッフル
	for(int epoch = 1;epoch <= 100;epoch++){
		cout << "epoch: " << epoch << endl;
		for(int itr = 0;itr < dataset_train.size();itr++){
			if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
				return;
			}
			int data_index = rand_indices[itr];
			Sentence* sentence = dataset_train[data_index];
			vector<int> &prev_depths = _prev_depths_for_data[data_index];
			bow_eow(sentence->_character_ids, 0, sentence->size() - 1, wrapped_character_ids);
			for(int token_t_index = 1;token_t_index < sentence->size() + 2;token_t_index++){
				if(epoch > 1){
					int prev_depth = prev_depths[token_t_index];
					assert(prev_depth >= 0);
					vpylm->remove_customer_at_time_t(wrapped_character_ids, token_t_index, prev_depth);
				}
				int new_depth = vpylm->sample_depth_at_time_t(wrapped_character_ids, token_t_index, vpylm->_parent_pw_cache, vpylm->_path_nodes);
				vpylm->add_customer_at_time_t(wrapped_character_ids, token_t_index, new_depth);
				prev_depths[token_t_index] = new_depth;
			}
		}
		if(epoch % 10 == 0){
			double log_Pdataset = 0;
			for(int data_index = 0;data_index < dataset_test.size();data_index++){
				if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
					return;
				}
				Sentence* sentence = dataset_test[data_index];
				bow_eow(sentence->_character_ids, 0, sentence->size() - 1, wrapped_character_ids);
				log_Pdataset += vpylm->compute_log_Pw(wrapped_character_ids, sentence->size() + 2) / (sentence->size() + 2);
			}
			cout << exp(-log_Pdataset / (double)dataset_test.size()) << endl;
			printf("# of nodes: %d\n", vpylm->get_num_nodes());
			printf("# of customers: %d\n", vpylm->get_num_customers());
			printf("# of tables: %d\n", vpylm->get_num_tables());
			printf("stop count: %d\n", vpylm->get_sum_stop_counts());
			printf("pass count: %d\n", vpylm->get_sum_pass_counts());
		}
		vpylm->sample_hyperparams();
	}
	delete[] wrapped_character_ids;
}

int main(int argc, char *argv[]){
	// 日本語周り
	setlocale(LC_CTYPE, "");
	ios_base::sync_with_stdio(false);
	locale default_loc("");
	locale::global(default_loc);
	locale ctype_default(locale::classic(), default_loc, locale::ctype);
	wcout.imbue(ctype_default);
	wcin.imbue(ctype_default);

	// test_vpylm_compute_Pw_given_h();
	// test_vpylm_compute_Pw_substr();
	// test_npylm_compute_g0_of_substring();
	// test_vpylm_add_customer();
	// test_npylm_find_node_by_tracing_back_context();
	// test_sentence_split();
	// test_npylm_find_node_by_tracing_back_context_and_store_pw();
	// test_npylm_remove_customer_at_time_t();
	// test_vpylm_sample_depth_at_timestep();
	// test_npylm_add_customer_at_time_t();
	// test_npylm_add_customer_at_time_t2();
	// test_npylm_sample_lambda();
	// npylm_test_compute_Pw_h();
	// test_lattice_perform_blocked_gibbs_sampling();
	// test_npylm_update_pk_vpylm();
	// test_npylm_remove_all_data();
	// test_npylm_perform_gibbs_sampling();
	// test_npylm_pw();
	// test_npylm_viterbi();
	// test_hash_collision();
	// test_vpylm_equiv();
	exit(0);

	// test_wchar();
	for(int i = 0;i < 3;i++){
		test_train();
		// test_train2();
	}
}
