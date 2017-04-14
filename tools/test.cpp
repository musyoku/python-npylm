#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <chrono>
#include "core/hash.h"
#include "npylm.cpp"
using namespace std;

void test_wchar(){
	wstring sentence_str = L"こんにちは";
	boost::hash<wstring> boost_hash_func;
	std::hash<wstring> std_hash_func;
	cout << std_hash_func(sentence_str) << endl;
	cout << boost_hash_func(sentence_str) << endl;
	{
		hashmap<id, int> count;
		auto start = std::chrono::system_clock::now();
		for(int n = 0;n < 10000000;n++){
			for(int t = 1;t <= sentence_str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, sentence_str.size());k++){
					wstring substr(sentence_str.begin() + t - k, sentence_str.begin() + t);
					id word_id = std_hash_func(substr);
					count[word_id] += 1;
				}
			}
		}
		auto end = std::chrono::system_clock::now();
		auto diff = end - start;
		cout << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << endl;
	}

	{
		hashmap<id, int> count;
		auto start = std::chrono::system_clock::now();
		for(int n = 0;n < 10000000;n++){
			for(int t = 1;t <= sentence_str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, sentence_str.size());k++){
					wstring substr(sentence_str.begin() + t - k, sentence_str.begin() + t);
					id word_id = boost_hash_func(substr);
					count[word_id] += 1;
				}
			}
		}
		auto end = std::chrono::system_clock::now();
		auto diff = end - start;
		cout << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << endl;
	}
	{
		hashmap<id, int> count;
		auto start = std::chrono::system_clock::now();
		wchar_t const* c = sentence_str.data();
		for(int n = 0;n < 10000000;n++){
			for(int t = 1;t <= sentence_str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, sentence_str.size());k++){
					id word_id = npylm::hash_bytes(c + t, k);
					count[word_id] += 1;
				}
			}
		}
		auto end = std::chrono::system_clock::now();
		auto diff = end - start;
		cout << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << endl;
	}
	{
		hashmap<id, int> count;
		auto start = std::chrono::system_clock::now();
		for(int n = 0;n < 10000000;n++){
			for(int t = 1;t <= sentence_str.size();t++){
				for(int k = 1;k <= std::min((size_t)t, sentence_str.size());k++){
					id word_id = npylm::hash_substring(sentence_str, t - k, t);
					count[word_id] += 1;
				}
			}
		}
		auto end = std::chrono::system_clock::now();
		auto diff = end - start;
		cout << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << endl;
	}
	exit(0);
}

void test_wchar2(){
	wstring sentence_str = L"こんにちは";
	wcout << sentence_str << endl;
	wchar_t const* c = sentence_str.data();
	wchar_t* bytes = new wchar_t[sentence_str.size()];
	for(int t = 1;t <= sentence_str.size();t++){
		for(int k = 1;k <= std::min((size_t)t, sentence_str.size());k++){
			cout << t << ", " << k << endl;
			cout << npylm::hash_substring(sentence_str, t - k, t) << endl;
			wstring substr(sentence_str.begin() + t - k, sentence_str.begin() + t);
			cout << npylm::hash_wstring(substr) << endl;
		}
	}
	exit(0);
}

void test_hash(){
	string filename = "dataset/test.txt";
	wifstream ifs(filename);
	wstring sentence;
	assert(ifs.fail() == false);
	std::hash<wstring> hash_func;
	unordered_set<id> hash_table;
	while (getline(ifs, sentence) && !sentence.empty()){
		if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
			exit(0);
		}
		id word_id = hash_func(sentence);
		cout << word_id << endl;
		hash_table.insert(word_id);
	}
	cout << hash_table.size() << endl;
	exit(0);
}

// void test_train(){
// 	string dirname = "out";
// 	PyTrainer* model = new PyTrainer();
// 	model->load_textfile("dataset/alice/alice.txt", 1300);
// 	model->set_max_word_length(16);
// 	model->init_lambda(1, 1);
// 	model->set_always_use_new_segmentation(true);
// 	model->compile();
// 	printf("# of sentences (train): %d\n", model->get_num_sentences_train());
// 	printf("# of sentences (test): %d\n", model->get_num_sentences_test());

// 	for(int epoch = 1;epoch < 10000;epoch++){
// 		auto start = std::chrono::system_clock::now();
// 		model->perform_gibbs_sampling();
// 		model->sample_pitman_yor_hyperparameters();
// 		model->sample_lambda();
// 		model->update_Pk_vpylm();
// 		auto end = std::chrono::system_clock::now();
// 		auto diff = end - start;
// 		double elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
// 		cout << model->get_num_sentences_train() / elapsed_time << " sentences/sec" << endl;
// 		// cout << model->compute_perplexity_train() << ", " << model->compute_log_Pdataset_train() << endl;
// 		// cout << model->compute_perplexity_test() << ", " << model->compute_log_Pdataset_test() << endl;
// 		if(epoch % 5 == 0){
// 			model->show_random_segmentation_result_test(10);
// 		}
// 	}
// 	printf("# of nodes: %d\n", model->_vpylm->get_num_nodes());
// 	printf("# of customers: %d\n", model->_vpylm->get_num_customers());
// 	printf("# of tables: %d\n", model->_vpylm->get_num_tables());
// 	printf("stop count: %d\n", model->_vpylm->get_sum_stop_counts());
// 	printf("pass count: %d\n", model->_vpylm->get_sum_pass_counts());
// 	model->save(dirname);
// 	model->load(dirname);
// 	printf("# of nodes: %d\n", model->_vpylm->get_num_nodes());
// 	printf("# of customers: %d\n", model->_vpylm->get_num_customers());
// 	printf("# of tables: %d\n", model->_vpylm->get_num_tables());
// 	printf("stop count: %d\n", model->_vpylm->get_sum_stop_counts());
// 	printf("pass count: %d\n", model->_vpylm->get_sum_pass_counts());

// 	model->remove_all_data();
// 	printf("# of nodes: %d\n", model->_vpylm->get_num_nodes());
// 	printf("# of customers: %d\n", model->_vpylm->get_num_customers());
// 	printf("# of tables: %d\n", model->_vpylm->get_num_tables());
// 	printf("stop count: %d\n", model->_vpylm->get_sum_stop_counts());
// 	printf("pass count: %d\n", model->_vpylm->get_sum_pass_counts());
// 	delete model;
// }

void test_train2(){
	string dirname = "out";
	PyTrainer* model = new PyTrainer();
	model->add_textfile("dataset/alice/alice.txt", 0.9);
	model->compile();
	delete model;
}

void bow_eow(wchar_t const* character_ids, int char_t_start, int char_t_end, wchar_t* wrapped_character_ids){
	wrapped_character_ids[0] = ID_BOW;
	int i = 0;
	for(;i < char_t_end - char_t_start + 1;i++){
		wrapped_character_ids[i + 1] = character_ids[i + char_t_start];
	}
	wrapped_character_ids[i + 1] = ID_EOW;
}
void test2(){
	string filename = "dataset/kemono.txt";
	wifstream ifs(filename.c_str());
	wstring str;
	assert(ifs.fail() == false);
	int i = 0;
	while (getline(ifs, str) && !str.empty()){
		Sentence* sentence = new Sentence(str);
		for(int t = 1;t <= str.size();t++){
			for(int k = 1;k <= std::min((size_t)t, str.size());k++){
				id a = sentence->get_substr_word_id(t - k, t - 1);
				wstring substr(str.begin() + t - k, str.begin() + t);
				id b = npylm::hash_wstring(substr);
				if(a != b){
					cout << a << endl;
					cout << b << endl;
					wcout << substr << endl;
				}
				assert(a == b);
			}
		}
		cout << "\r" << i << ": OK" << flush;
		i++;
		delete sentence;
	}
}

double vpylm_compute_Pw_given_h(VPYLM* vpylm, wchar_t const* character_ids, int context_start, int context_end){
	int context_size = context_end - context_start + 1;
	Node<wchar_t>* node = vpylm->_root;
	wchar_t target_id = character_ids[context_end + 1];
	assert(node != NULL);
	double parent_pass_probability = 1;
	double p = 0;
	double parent_pw = vpylm->_g0;
	for(int i = 0;i < context_size;i++){
		// ノードがない場合親の確率とベータ事前分布から計算
		if(node == NULL){
			double p_stop = (vpylm->_beta_stop) / (vpylm->_beta_pass + vpylm->_beta_stop) * parent_pass_probability;
			p += parent_pw * p_stop;
			parent_pass_probability *= (vpylm->_beta_pass) / (vpylm->_beta_pass + vpylm->_beta_stop);
		}else{
			wchar_t context_token_id = character_ids[context_end - i];
			double pw = node->compute_Pw(target_id, vpylm->_g0, vpylm->_d_m, vpylm->_theta_m);
			double p_stop = node->stop_probability(vpylm->_beta_stop, vpylm->_beta_pass);
			p += pw * p_stop;
			Node<wchar_t>* child = node->find_child_node(context_token_id);
			double p_pass = node->pass_probability(vpylm->_beta_stop, vpylm->_beta_pass);
			parent_pass_probability *= p_pass;
			parent_pw = pw;
			node = child;
		}
	}
	assert(p > 0);
	return p;
}
void test_vpylm_compute_Pw_given_h(){
	VPYLM* vpylm = new VPYLM(1000);
	vpylm->set_g0(0.00001);
	string filename = "dataset/kemono.txt";
	wifstream ifs(filename.c_str());
	wstring str;
	assert(ifs.fail() == false);
	int i = 0;
	wchar_t* wrapped_character_ids = new wchar_t[1000];
	while (getline(ifs, str) && !str.empty()){
		Sentence* sentence = new Sentence(str);
		bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
		for(int i = 0;i < sentence->size();i++){
			for(int j = 0;j <= i;j++){
				vpylm->add_customer_at_time_t(wrapped_character_ids, i, j);
			}
		}
		for(int end = 1;end < sentence->size() - 1;end++){
			for(int start = 0;start < end;start++){
				double a = vpylm->compute_Pw_given_h(wrapped_character_ids, start, end);
				double b = vpylm_compute_Pw_given_h(vpylm, wrapped_character_ids, start, end);
				if(abs(a - b) > 1e-8){
					printf("%.32e != %.32e\n", a, b);
					assert(a == b);
				}
			}
		}
		cout << "\r" << i << ": OK" << flush;
		i++;
		delete sentence;
	}
	delete[] wrapped_character_ids;
}
void test_vpylm_compute_Pw_substr(){
	VPYLM* vpylm = new VPYLM(1000);
	vpylm->set_g0(0.00001);
	string filename = "dataset/kemono.txt";
	wifstream ifs(filename.c_str());
	wstring str;
	wchar_t* wrapped_character_ids = new wchar_t[1000];
	assert(ifs.fail() == false);
	int i = 0;
	while (getline(ifs, str) && !str.empty()){
		Sentence* sentence = new Sentence(str);
		bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
		for(int i = 0;i < sentence->size();i++){
			for(int j = 0;j <= i;j++){
				vpylm->add_customer_at_time_t(wrapped_character_ids, i, j);
			}
		}
		for(int end = 1;end < sentence->size() - 1;end++){
			for(int start = 0;start < end;start++){
				bow_eow(sentence->_character_ids, start, end, wrapped_character_ids);
				double a = vpylm->compute_Pw(wrapped_character_ids, end - start  + 1);
			}
		}
		cout << "\r" << i << ": OK" << flush;
		i++;
		delete sentence;
	}
	delete[] wrapped_character_ids;
}
void test_npylm_compute_g0_of_substring(){
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	string filename = "dataset/kemono.txt";
	wifstream ifs(filename.c_str());
	wstring str;
	assert(ifs.fail() == false);
	int i = 0;
	wchar_t* wrapped_character_ids = new wchar_t[1000];
	while (getline(ifs, str) && !str.empty()){
		Sentence* sentence = new Sentence(str);
		bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
		for(int i = 0;i < sentence->size();i++){
			for(int j = 0;j <= i;j++){
				npylm->_vpylm->add_customer_at_time_t(wrapped_character_ids, i, j);
			}
		}
		cout << "\r" << i << ": OK" << flush;
		i++;
		delete sentence;
	}
	delete[] wrapped_character_ids;
}
bool vpylm_add_customer_at_time_t(VPYLM* vpylm, wchar_t const* token_ids, int t, int depth_t){
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
void test_vpylm_add_customer(){
	sampler::mt.seed(0);
	VPYLM* vpylm1 = new VPYLM(1000);
	vpylm1->set_g0(0.00001);
	wchar_t* wrapped_character_ids = new wchar_t[1000];
	string filename = "dataset/kemono.txt";
	{
		wifstream ifs(filename.c_str());
		wstring str;
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			Sentence* sentence = new Sentence(str);
			bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
			for(int i = 0;i < sentence->size();i++){
				for(int j = 0;j <= i;j++){
					vpylm1->add_customer_at_time_t(wrapped_character_ids, i, j);
				}
			}
			delete sentence;
		}
	}
	sampler::mt.seed(0);
	VPYLM* vpylm2 = new VPYLM(1000);
	vpylm2->set_g0(0.00001);
	{
		wifstream ifs(filename.c_str());
		wstring str;
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			Sentence* sentence = new Sentence(str);
			bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
			for(int i = 0;i < sentence->size();i++){
				for(int j = 0;j <= i;j++){
					vpylm_add_customer_at_time_t(vpylm2, wrapped_character_ids, i, j);
				}
			}
			delete sentence;
		}
	}
	cout << vpylm1->get_num_nodes() << " == " << vpylm2->get_num_nodes() << endl;
	cout << vpylm1->get_num_customers() << " == " << vpylm2->get_num_customers() << endl;
	cout << vpylm1->get_num_tables() << " == " << vpylm2->get_num_tables() << endl;
	cout << vpylm1->get_sum_stop_counts() << " == " << vpylm2->get_sum_stop_counts() << endl;
	cout << vpylm1->get_sum_pass_counts() << " == " << vpylm2->get_sum_pass_counts() << endl;
	assert(vpylm1->get_num_nodes() == vpylm2->get_num_nodes());
	assert(vpylm1->get_num_customers() == vpylm2->get_num_customers());
	assert(vpylm1->get_num_tables() == vpylm2->get_num_tables());
	assert(vpylm1->get_sum_stop_counts() == vpylm2->get_sum_stop_counts());
	assert(vpylm1->get_sum_pass_counts() == vpylm2->get_sum_pass_counts());
	{
		int i = 0;
		wifstream ifs(filename.c_str());
		wstring str;
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			Sentence* sentence = new Sentence(str);
			for(int end = 1;end < sentence->size() - 1;end++){
				for(int start = 0;start < end;start++){
					bow_eow(sentence->_character_ids, start, end, wrapped_character_ids);
					double a = vpylm1->compute_Pw(wrapped_character_ids, end - start + 1);
					double b = vpylm2->compute_Pw(wrapped_character_ids, end - start + 1);
					assert(a == b);
				}
			}
			cout << "\r" << i << ": OK" << flush;
			i++;
			delete sentence;
		}
	}
	delete[] wrapped_character_ids;
}
void test_npylm_find_node_by_tracing_back_context(){
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	string filename = "dataset/kemono.txt";
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
void test_sentence_split(){
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	string filename = "dataset/kemono.txt";
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

				int start = 0;
				for(int i = 0;i <= segment_index;i++){
					int end = start + segments[i];
					wstring substr(str.begin() + start, str.begin() + end);
					assert(substr.size() == segments[i]);
					id a = sentence->get_word_id_at(i + 2);
					id b = npylm::hash_wstring(substr);
					assert(a == b);
					start = end;
				}
			}
		}
		cout << "\r" << i << ": OK" << flush;
		i++;
		delete sentence;
	}
}
void test_npylm_find_node_by_tracing_back_context_and_store_pw(){
	NPYLM* npylm = new NPYLM(20, 1000, 0.00001);
	string filename = "dataset/kemono.txt";
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
int vpylm_sample_depth_at_timestep(VPYLM* vpylm, wchar_t const* token_ids, int t){
	if(t == 0){
		return 0;
	}
	wchar_t token_t = token_ids[t];

	// この値を下回れば打ち切り
	double eps = 1e-8;
	
	double sum = 0;
	double p_pass = 0;
	double parent_pw = vpylm->_g0;
	int sampling_table_size = 0;
	Node<wchar_t>* node = vpylm->_root;
	for(int n = 0;n <= t;n++){
		if(node){
			double pw = node->compute_Pw(token_t, vpylm->_g0, vpylm->_d_m, vpylm->_theta_m);
			double p_stop = node->stop_probability(vpylm->_beta_stop, vpylm->_beta_pass);
			p_pass = node->pass_probability(vpylm->_beta_stop, vpylm->_beta_pass);
			double p = pw * p_stop;
			parent_pw = pw;
			vpylm->_sampling_table[n] = p;
			sampling_table_size += 1;
			sum += p;
			if(p < eps){
				break;
			}
			if(n < t){
				id context_token_id = token_ids[t - n - 1];
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
			if(p < eps){
				break;
			}
		}
	}
	// assert(sampling_table_size == t + 1);
	double normalizer = 1.0 / sum;
	uniform_real_distribution<double> rand(0, 1);
	double bernoulli = rand(sampler::mt);
	double stack = 0;
	for(int n = 0;n < sampling_table_size;n++){
		stack += vpylm->_sampling_table[n] * normalizer;
		if(bernoulli < stack){
			return n;
		}
	}
	return vpylm->_sampling_table[sampling_table_size - 1];
}
void test_vpylm_sample_depth_at_timestep(){
	sampler::mt.seed(0);
	VPYLM* vpylm1 = new VPYLM(1000);
	vpylm1->set_g0(0.00001);
	string filename = "dataset/kemono.txt";
	wchar_t* wrapped_character_ids = new wchar_t[1000];
	{
		wifstream ifs(filename.c_str());
		wstring str;
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			Sentence* sentence = new Sentence(str);
			bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
			for(int i = 0;i < sentence->size();i++){
				for(int j = 0;j <= i;j++){
					vpylm1->add_customer_at_time_t(wrapped_character_ids, i, j);
				}
			}
			delete sentence;
		}
	}
	sampler::mt.seed(0);
	VPYLM* vpylm2 = new VPYLM(1000);
	vpylm2->set_g0(0.00001);
	{
		wifstream ifs(filename.c_str());
		wstring str;
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			Sentence* sentence = new Sentence(str);
			bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
			for(int i = 0;i < sentence->size();i++){
				for(int j = 0;j <= i;j++){
					vpylm_add_customer_at_time_t(vpylm2, wrapped_character_ids, i, j);
				}
			}
			delete sentence;
		}
	}
	cout << vpylm1->get_num_nodes() << " == " << vpylm2->get_num_nodes() << endl;
	cout << vpylm1->get_num_customers() << " == " << vpylm2->get_num_customers() << endl;
	cout << vpylm1->get_num_tables() << " == " << vpylm2->get_num_tables() << endl;
	cout << vpylm1->get_sum_stop_counts() << " == " << vpylm2->get_sum_stop_counts() << endl;
	cout << vpylm1->get_sum_pass_counts() << " == " << vpylm2->get_sum_pass_counts() << endl;
	assert(vpylm1->get_num_nodes() == vpylm2->get_num_nodes());
	assert(vpylm1->get_num_customers() == vpylm2->get_num_customers());
	assert(vpylm1->get_num_tables() == vpylm2->get_num_tables());
	assert(vpylm1->get_sum_stop_counts() == vpylm2->get_sum_stop_counts());
	assert(vpylm1->get_sum_pass_counts() == vpylm2->get_sum_pass_counts());
	{
		int i = 0;
		wifstream ifs(filename.c_str());
		wstring str;
		assert(ifs.fail() == false);
		while (getline(ifs, str) && !str.empty()){
			Sentence* sentence = new Sentence(str);
			bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
			for(int start = 0;start < sentence->size();start++){
				sampler::mt.seed(start);
				int a = vpylm1->sample_depth_at_time_t(wrapped_character_ids, start, vpylm1->_parent_pw_cache, vpylm1->_path_nodes);
				sampler::mt.seed(start);
				int b = vpylm_sample_depth_at_timestep(vpylm2, wrapped_character_ids, start);
				assert(a == b);
			}
			cout << "\r" << i << ": OK" << flush;
			i++;
			delete sentence;
		}
	}
	delete[] wrapped_character_ids;
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
	string filename = "dataset/kemono.txt";
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
	bow_eow(sentence->_character_ids, 0, sentence->size(), wrapped_character_ids);
	npylm->_vpylm->add_customer_at_time_t(wrapped_character_ids, str.size() - 1, str.size() - 1);
	npylm->_vpylm->sample_depth_at_time_t(wrapped_character_ids, str.size() - 1, npylm->_vpylm->_parent_pw_cache, npylm->_vpylm->_path_nodes);
	delete[] wrapped_character_ids;
}
void test_npylm_remove_customer_at_time_t(){
	string filename = "dataset/kemono.txt";
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
	string filename = "dataset/kemono.txt";
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
	string filename = "dataset/kemono.txt";
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
	string filename = "dataset/kemono.txt";
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
	string filename = "dataset/kemono.txt";
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
	string filename = "dataset/test.txt";
	PyTrainer* model = new PyTrainer();
	model->_max_word_length = 20;
	model->add_textfile(filename, 0.95);
	model->compile();
	model->_always_accept_new_segmentation = true;
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
		model->show_sampled_segmentation_train(10);
		model->show_sampled_segmentation_test(10);
		model->dump_hpylm();
		model->dump_vpylm();
		cout << model->get_acceptance_ratio_of_new_segmentation();
		model->reset_acceptance_ratio();
	}
	delete model;
}
void test_npylm_remove_all_data(){
	string filename = "dataset/kemono.txt";
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
	string filename = "dataset/kemono.txt";
	PyTrainer* model = new PyTrainer();
	model->add_textfile(filename, 0.95);
	model->compile();
	model->_always_accept_new_segmentation = false;
	for(int i = 0;i < 2;i++){
		model->perform_gibbs_sampling();
	}
	delete model;
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


	// test2();
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
	test_npylm_perform_gibbs_sampling();
	// test_npylm_pw();
	// test_npylm_save_load();
	exit(0);

	// test_wchar();
	for(int i = 0;i < 2;i++){
		test_npylm_perform_gibbs_sampling();
		// test_train2();
	}
}
