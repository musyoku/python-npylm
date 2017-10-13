#include <boost/python.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "../npylm/wordtype.h"
#include "../npylm/hash.h"
#include "../npylm/sentence.h"
#include "../npylm/npylm.h"
#include "../npylm/lattice.h"
using namespace boost;
using namespace npylm;
using std::cout;
using std::wcout;
using std::endl;

void show_progress(int step, int total){
	double progress = step / (double)(total - 1);
	int barWidth = 30;

	cout << "\r" << step << "/" << total << " [";
	int pos = barWidth * progress;
	for(int i = 0; i < barWidth; ++i){
		if (i < pos) cout << "=";
		else if (i == pos) cout << ">";
		else cout << " ";
	}
	cout << "] " << int(progress * 100.0) << "%";
	cout.flush();
}

class PyTrainer{
public:
	NPYLM* _npylm;
	Lattice* _lattice;
	std::unordered_set<wchar_t> _all_characters;	// すべての文字
	std::vector<Sentence*> _dataset_train;
	std::vector<Sentence*> _dataset_test;
	bool* _added_npylm_train;
	std::vector<int> _rand_indices_train;
	std::vector<int> _rand_indices_test;
	double* _vpylm_sampling_probability_table;
	wchar_t* _vpylm_sampling_id_table;
	int _max_word_length;
	int _num_updates_pk_vpylm;
	bool _always_accept_new_segmentation;
	// 統計
	int _max_sentence_length;
	int _min_sentence_length;
	int _num_ws_acceptance;
	int _num_ws_rejection;
	double _average_sentence_length;
	PyTrainer(){
		#ifdef __DEBUG__
		cout << "\x1b[47mDEBUG\x1b[1m" << endl;
		#endif
		// 日本語周り
		setlocale(LC_CTYPE, "ja_JP.UTF-8");
		std::ios_base::sync_with_stdio(false);
		std::locale default_loc("ja_JP.UTF-8");
		std::locale::global(default_loc);
		std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype); //※
		std::wcout.imbue(ctype_default);
		std::wcin.imbue(ctype_default);

		_npylm = new NPYLM();
		_lattice = new Lattice(_npylm);
		_vpylm_sampling_probability_table = NULL;
		_vpylm_sampling_id_table = NULL;
		_added_npylm_train = NULL;
		_max_word_length = 16;
		_max_sentence_length = 0;
		_min_sentence_length = 0;
		_average_sentence_length = 0;
		_num_updates_pk_vpylm = 0;
		_always_accept_new_segmentation = true;
		_num_ws_acceptance = 0;
		_num_ws_rejection = 0;
	}
	~PyTrainer(){
		for(auto sentence: _dataset_train){
			delete sentence;
		}
		for(auto sentence: _dataset_test){
			delete sentence;
		}
		if(_vpylm_sampling_probability_table != NULL){
			delete[] _vpylm_sampling_probability_table;
		}
		if(_vpylm_sampling_id_table != NULL){
			delete[] _vpylm_sampling_id_table;
		}
		if(_npylm != NULL){
			delete _npylm;
		}
		if(_lattice != NULL){
			delete _lattice;
		}
		if(_added_npylm_train != NULL){
			delete _added_npylm_train;
		}
	}
	void set_seed(int seed){
		sampler::mt.seed(seed);
	}
	void compile(){
		assert(_dataset_train.size() > 0);
		_added_npylm_train = new bool[_dataset_train.size()];
		// 統計
		for(int data_index = 0;data_index < _dataset_train.size();data_index++){
			_rand_indices_train.push_back(data_index);
			_added_npylm_train[data_index] = false;
		}
		for(int data_index = 0;data_index < _dataset_test.size();data_index++){
			_rand_indices_test.push_back(data_index);
		}
		_average_sentence_length /= _dataset_train.size() + _dataset_test.size();
		// NPYLMの初期化
		_npylm->_init_cache(_max_word_length, _max_sentence_length);
		double g0 = 1.0 / (double)get_num_characters();
		_npylm->set_vpylm_g0(g0);
		// forward-filtering backward-sampling
		_lattice->_init_cache(_max_word_length, _max_sentence_length);
		// VPYLMからの単語のサンプリング用
		_vpylm_sampling_probability_table = new double[get_num_characters() + 1];
		_vpylm_sampling_id_table = new wchar_t[get_num_characters() + 1];
	}
	void compile_if_needed(){
		if(_npylm->_is_ready){
			return;
		}
		if(_lattice->_is_ready){
			return;
		}
		compile();
	}
	// ハッシュが衝突していないかチェック
	int detect_collision(){
		int step = 0;
		hashmap<id, std::wstring> pool;
		for(Sentence* sentence: _dataset_train){
			if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
				return 0;
			}
			_detect_collision_of_sentence(sentence, pool);
			step++;
			show_progress(step, _dataset_train.size() + _dataset_test.size());
		}
		for(Sentence* sentence: _dataset_test){
			if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
				return 0;
			}
			_detect_collision_of_sentence(sentence, pool);
			step++;
			show_progress(step, _dataset_train.size() + _dataset_test.size());
		}
		return pool.size();
	}
	void _detect_collision_of_sentence(Sentence* sentence, hashmap<id, std::wstring> &pool){
		for(int t = 1;t <= sentence->size();t++){
			for(int k = 1;k <= std::min(t, _max_word_length);k++){
				id word_id = sentence->get_substr_word_id(t - k, t - 1);
				std::wstring word = sentence->get_substr_word_str(t - k, t - 1);
				assert(word_id == hash_wstring(word));
				auto itr = pool.find(word_id);
				if(itr == pool.end()){
					pool[word_id] = word;
				}else{
					assert(itr->second == word);
				}
			}
		}
	}
	bool add_textfile(std::string filename, double train_split_ratio){
		std::wifstream ifs(filename.c_str());
		std::wstring sentence;
		assert(ifs.fail() == false);
		std::vector<std::wstring> sentence_array;
		while (getline(ifs, sentence) && !sentence.empty()){
			if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
				return false;
			}
			sentence_array.push_back(sentence);
			// 統計
			if(sentence.size() > _max_sentence_length){
				_max_sentence_length = sentence.size();
			}
			if(_min_sentence_length == 0 || sentence.size() < _min_sentence_length){
				_min_sentence_length = sentence.size();
			}
			_average_sentence_length += sentence.size();
		}
		int train_split = (double)sentence_array.size() * train_split_ratio;
		assert(sentence_array.size() >= train_split);
		std::vector<int> rand_indices;
		for(int i = 0;i < sentence_array.size();i++){
			rand_indices.push_back(i);
		}
		shuffle(rand_indices.begin(), rand_indices.end(), sampler::mt);	// データをシャッフル
		for(int i = 0;i < rand_indices.size();i++){
			std::wstring &sentence = sentence_array[rand_indices[i]];
			if(train_split == -1){
				add_train_data(sentence);
				continue;
			}
			if(i < train_split){
				add_train_data(sentence);
				continue;
			}
			add_test_data(sentence);
		}
		return true;
	}
	void add_train_data(std::wstring sentence){
		_add_data_to(sentence, _dataset_train);
	}
	void add_test_data(std::wstring sentence){
		_add_data_to(sentence, _dataset_test);
	}
	void _add_data_to(std::wstring &sentence_str, std::vector<Sentence*> &dataset){
		if(sentence_str.size() > 0){
			for(wchar_t character: sentence_str){
				add_character(character);
			}
			Sentence* sentence = new Sentence(sentence_str);
			dataset.push_back(sentence);
		}
	}
	void add_character(wchar_t character){
		_all_characters.insert(character);
	}
	int get_num_characters(){
		return _all_characters.size();
	}
	double get_acceptance_ratio_of_new_segmentation(){
		return _num_ws_acceptance / (double)(_num_ws_acceptance + _num_ws_rejection);
	}
	int get_num_sentences_train(){
		return _dataset_train.size();
	}
	int get_num_sentences_test(){
		return _dataset_test.size();
	}
	void set_always_accept_new_segmentation(bool accept){
		_always_accept_new_segmentation = accept;
	}
	void set_max_word_length(int length){
		_max_word_length = length;
	}
	void reset_acceptance_ratio(){
		_num_ws_acceptance = 0;
		_num_ws_rejection = 0;
	}
	void set_lambda_prior(double a, double b){
		assert(_npylm != NULL);
		_npylm->set_lambda_prior(a, b);
	}
	// HPYLM,VPYLMのdとthetaをサンプリング
	void sample_pitman_yor_hyperparameters(){
		_npylm->sample_pitman_yor_hyperparameters();
	}
	// 文字種ごとにλのサンプリング
	void sample_lambda(){
		std::vector<double> a_for_type(WORDTYPE_NUM_TYPES + 1, 0.0);
		std::vector<double> b_for_type(WORDTYPE_NUM_TYPES + 1, 0.0);
		std::unordered_set<id> words;
		for(int type = 1;type <= WORDTYPE_NUM_TYPES;type++){
			a_for_type[type] = _npylm->_lambda_a;
			b_for_type[type] = _npylm->_lambda_b;
		}
		for(auto sentence: _dataset_train){
			// <bos>と<eos>は除外
			for(int t = 2;t < sentence->get_num_segments() - 1;t++){
				std::wstring word = sentence->get_word_str_at(t);
				id word_id = sentence->get_word_id_at(t);
				int word_length = sentence->get_word_length_at(t);
				if(word_length > _max_word_length){
					continue;
				}
				if(words.find(word_id) == words.end()){
					std::vector<int> &tables = _npylm->_hpylm->_root->_arrangement[word_id];
					int t_w = tables.size();
					int type = wordtype::detect_word_type(word);
					a_for_type[type] += t_w * word_length;
					b_for_type[type] += t_w;
					words.insert(word_id);
				}
			}
		}
		for(int type = 1;type <= WORDTYPE_NUM_TYPES;type++){
			double lambda = sampler::gamma(a_for_type[type], b_for_type[type]);
			_npylm->_lambda_for_type[type] = lambda;
		}
	}
	// VPYLMに文脈を渡し次の文字を生成
	wchar_t sample_word_from_vpylm_given_context(wchar_t* context_ids, int context_length, int sample_t, bool skip_eow = false){
		double sum_probs = 0;
		lm::VPYLM* vpylm = _npylm->_vpylm;
		int table_index = 0;
		for(wchar_t character_id: _all_characters){
			assert(table_index < get_num_characters());
			double pw = vpylm->compute_Pw_given_h(character_id, context_ids, 0, context_length - 1);
			sum_probs += pw;
			_vpylm_sampling_probability_table[table_index] = pw;
			_vpylm_sampling_id_table[table_index] = character_id;
			table_index++;
		}
		if(skip_eow == false){
			assert(table_index < get_num_characters() + 1);
			double pw = vpylm->compute_Pw_given_h(ID_EOW, context_ids, 0, context_length - 1);
			sum_probs += pw;
			_vpylm_sampling_probability_table[table_index] = pw;
			_vpylm_sampling_id_table[table_index] = ID_EOW;
		}

		double normalizer = 1.0 / sum_probs;
		double r = sampler::uniform(0, 1);
		double stack = 0;
		for(int i = 0;i <= table_index;i++){
			stack += _vpylm_sampling_probability_table[i] * normalizer;
			if(r <= stack){
				return _vpylm_sampling_id_table[i];
			}
		}
		return _vpylm_sampling_id_table[table_index];
	}
	// VPYLMから長さkの単語が出現する確率をキャッシュする
	void update_Pk_vpylm(){
		int num_samples = 20000;
		int early_stopping_threshold = 10;
		_num_updates_pk_vpylm += 1;
		int max_word_length = _max_word_length + 1;
		double* pk_vpylm = _npylm->_pk_vpylm;
		int* num_words_of_k = new int[max_word_length + 1];	// 1つ余分に確保しておき、k > max_word_lengthの確率をすべてそこに格納
		for(int i = 0;i <= max_word_length;i++){
			pk_vpylm[i] = 0;
			num_words_of_k[i] = 0;
		}
		wchar_t* wrapped_character_ids = new wchar_t[max_word_length + 2];
		int k;
		double sum_words = 0;
		double sum_probs = 0;
		for(int m = 0;m < num_samples;m++){
			// wcout << "m = " << m << endl;
			wrapped_character_ids[0] = ID_BOW;
			int k = 0;
			for(int j = 0;j < max_word_length;j++){
				bool skip_eow = (j == 0) ? true : false;
				wchar_t token_char = sample_word_from_vpylm_given_context(wrapped_character_ids, j + 1, j + 1, skip_eow);
				wrapped_character_ids[j + 1] = token_char;
				if(token_char == ID_EOW){
					break;
				}
				k++;
			}
			sum_words += 1;
			if(k == 0){	// <bow><eow>
				continue;
			}
			assert(k <= max_word_length);
			num_words_of_k[k] += 1;
			// すべてのkが生成されているかをチェック
			if(m % 100 == 99){
				bool stop = true;
				for(int k = 1;k <= max_word_length;k++){
					if(num_words_of_k[k] < early_stopping_threshold){
						stop = false;
						break;
					}
				}
				if(stop){
					break;
				}
			}
		}
		for(int k = 1;k <= max_word_length;k++){
			pk_vpylm[k] = num_words_of_k[k] / sum_words;
			assert(pk_vpylm[k] > 0);
		}
		delete[] num_words_of_k;
		delete[] wrapped_character_ids;
	}
	// 単語分割のギブスサンプリング
	void perform_gibbs_sampling(){
		assert(_dataset_train.size() > 0);
		compile_if_needed();
		int num_sentences = _dataset_train.size();
		std::vector<int> segments;		// 分割の一時保存用
		shuffle(_rand_indices_train.begin(), _rand_indices_train.end(), sampler::mt);		// データをシャッフル
		int* old_segments = new int[_max_sentence_length + 3];
		int num_old_segments;
		// モデルパラメータを更新
		for(int step = 1;step <= num_sentences;step++){
			if (PyErr_CheckSignals() != 0) {	// ctrl+cが押されたかチェック
				return;		
			}
			if(step % 100 == 0 || step == num_sentences){
				show_progress(step, num_sentences);
			}
			// 訓練データを一つ取り出す
			int data_index = _rand_indices_train[step - 1];
			assert(data_index < _dataset_train.size());
			Sentence* sentence = _dataset_train[data_index];
			// モデルに追加されているかチェック
			if(_added_npylm_train[data_index] == true){
				double old_log_ps, new_log_ps;
				// 古い分割をモデルから削除
				for(int t = 2;t < sentence->get_num_segments();t++){
					_npylm->remove_customer_at_time_t(sentence, t);
				}
				// 新しい分割の棄却判定をするかどうか
				if(_always_accept_new_segmentation == false){
					// 古い分割を一時保存
					// <bos>と<eos>は無視
					for(int i = 0;i < sentence->get_num_segments_without_special_tokens();i++){
						old_segments[i] = sentence->_segments[i + 2];	// <bos>は2つ
					}
					num_old_segments = sentence->get_num_segments_without_special_tokens();
					// 古い分割での文の確率を計算
					old_log_ps = _npylm->compute_log_Pw(sentence);
				}
				
				#ifdef __DEBUG__
				// 正規化しない場合の結果と比較するためシードを合わせる
				int seed = (unsigned int)time(NULL);
				sampler::mt.seed(seed);
				#endif

				// 新しい分割を取得
				_lattice->perform_blocked_gibbs_sampling(sentence, segments, true);
				sentence->split(segments);
				
				#ifdef __DEBUG__
				// 正規化しない場合の結果と比較
				std::vector<int> a = segments;
				sampler::mt.seed(seed);
				_lattice->perform_blocked_gibbs_sampling(sentence, segments, false);
				std::vector<int> b = segments;
				assert(a.size() == b.size());
				for(int i = 0;i < a.size();i++){
					// cout << a[i] << "," << b[i] << endl;
					assert(a[i] == b[i]);
				}
				#endif

				// 以前の分割結果と現在の分割結果の確率を求める
				// 本来は分割を一定数サンプリングして平均をとるべき
				if(_always_accept_new_segmentation == false){
					new_log_ps = _npylm->compute_log_Pw(sentence);
					// 新しい分割の方が確率が低い場合、比率のベルヌーイ試行でどちらを採用するか決める.
					double bernoulli = std::min(1.0, exp(new_log_ps - old_log_ps));
					double r = sampler::uniform(0, 1);
					if(bernoulli < r){
						// 新しい分割を捨てて古いものに差し替える
						sentence->split(old_segments, num_old_segments);
						_num_ws_rejection++;
					}else{
						_num_ws_acceptance++;
					}
				}
			}
			// 新しい分割結果をモデルに追加
			for(int t = 2;t < sentence->get_num_segments();t++){
				_npylm->add_customer_at_time_t(sentence, t);
			}
			_added_npylm_train[data_index] = true;
		}
		// 客数チェック
		assert(_npylm->_hpylm->_root->_num_tables <= _npylm->_vpylm->get_num_customers());
		delete[] old_segments;
	}
	// デバッグ用
	void remove_all_data(){
		wchar_t* wrapped_character_ids = new wchar_t[_max_sentence_length + 2];	// <bow>と<eow>を追加
		for(int data_index = 0;data_index < _dataset_train.size();data_index++){
			if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
				return;
			}
			Sentence* sentence = _dataset_train[data_index];
			// 古い分割をモデルから削除
			if(_added_npylm_train[data_index] == true){
				for(int t = 2;t < sentence->get_num_segments();t++){
					_npylm->remove_customer_at_time_t(sentence, t);
				}
			}
		}
		delete[] wrapped_character_ids;
	}
	double compute_perplexity_train(){
		return _compute_perplexity(_dataset_train);
	}
	double compute_perplexity_test(){
		return _compute_perplexity(_dataset_test);
	}
	double _compute_perplexity(std::vector<Sentence*> &dataset){
		if(dataset.size() == 0){
			return -1;
		}
		double ppl = 0;
		int num_sentences = dataset.size();
		std::vector<int> segments;		// 分割の一時保存用
		for(int data_index = 0;data_index < num_sentences;data_index++){
			if (PyErr_CheckSignals() != 0) {	// ctrl+cが押されたかチェック
				return 0;		
			}
			Sentence* sentence = dataset[data_index]->copy();
			_lattice->viterbi_decode(sentence, segments);
			sentence->split(segments);
			ppl += _npylm->compute_log_Pw(sentence) / ((double)sentence->get_num_segments() - 2);
			delete sentence;
		}
		ppl = exp(-ppl / num_sentences);
		return ppl;
	}
	void show_sampled_segmentation_train(int num_to_show){
		_show_sampled_segmentation(num_to_show, _dataset_train, _rand_indices_train);
	}
	void show_sampled_segmentation_test(int num_to_show){
		shuffle(_rand_indices_test.begin(), _rand_indices_test.end(), sampler::mt);
		_show_sampled_segmentation(num_to_show, _dataset_test, _rand_indices_test);
	}
	void _show_sampled_segmentation(int num_to_show, std::vector<Sentence*> &dataset, std::vector<int> &rand_indices){
		num_to_show = std::min((int)dataset.size(), num_to_show);
		std::vector<int> segments;		// 分割の一時保存用
		for(int n = 0;n < num_to_show;n++){
			if (PyErr_CheckSignals() != 0) {	// ctrl+cが押されたかチェック
				return;		
			}
			int data_index = rand_indices[n];
			Sentence* sentence = dataset[data_index]->copy();
			_lattice->perform_blocked_gibbs_sampling(sentence, segments, true);
			sentence->split(segments);
			sentence->dump_words();
			delete sentence;
		}
	}
	void show_viterbi_segmentation_train(int num_to_show){
		_show_viterbi_segmentation(num_to_show, _dataset_train, _rand_indices_train);
	}
	void show_viterbi_segmentation_test(int num_to_show){
		shuffle(_rand_indices_test.begin(), _rand_indices_test.end(), sampler::mt);
		_show_viterbi_segmentation(num_to_show, _dataset_test, _rand_indices_test);
	}
	void _show_viterbi_segmentation(int num_to_show, std::vector<Sentence*> &dataset, std::vector<int> &rand_indices){
		num_to_show = std::min((int)dataset.size(), num_to_show);
		std::vector<int> segments;		// 分割の一時保存用
		for(int n = 0;n < num_to_show;n++){
			if (PyErr_CheckSignals() != 0) {	// ctrl+cが押されたかチェック
				return;		
			}
			int data_index = rand_indices[n];
			Sentence* sentence = dataset[data_index]->copy();
			_lattice->viterbi_decode(sentence, segments);
			sentence->split(segments);
			sentence->dump_words();
			delete sentence;
		}
	}
	void dump_hpylm(){
		cout << "HPYLM:" << endl;
		cout << "	深さ:		" << _npylm->_hpylm->_depth << endl;
		cout << "	ノード数:	" << _npylm->_hpylm->get_num_nodes() << endl;
		cout << "	客数:		" << _npylm->_hpylm->get_num_customers() << endl;
		cout << "	テーブル数:	" << _npylm->_hpylm->get_num_tables() << endl;
		cout << "	停止回数:	" << _npylm->_hpylm->get_sum_stop_counts() << endl;
		cout << "	通過回数:	" << _npylm->_hpylm->get_sum_pass_counts() << endl;
	}
	void dump_vpylm(){
		cout << "VPYLM:" << endl;
		cout << "	最大深さ:	" << _npylm->_vpylm->_depth << endl;
		cout << "	0-gram確率:	" << _npylm->_vpylm->_g0 << endl;
		cout << "	ノード数:	" << _npylm->_vpylm->get_num_nodes() << endl;
		cout << "	客数:		" << _npylm->_vpylm->get_num_customers() << endl;
		cout << "	テーブル数:	" << _npylm->_vpylm->get_num_tables() << endl;
		cout << "	停止回数:	" << _npylm->_vpylm->get_sum_stop_counts() << endl;
		cout << "	通過回数:	" << _npylm->_vpylm->get_sum_pass_counts() << endl;
	}
	void dump_Pk_vpylm(){
		double* pk_vpylm = _npylm->_pk_vpylm;
		cout << "P(k|VPYLM):" << endl;
		for(int k = 1;k <= _max_word_length;k++){
			cout << "	k = " << k << "	: " << pk_vpylm[k] << endl;
		}
	}
	void dump_lambda(){
		cout << "lambda:" << endl;
		wcout << L"	アルファベット:	" << _npylm->_lambda_for_type[1] << endl;
		wcout << L"	数字:		" << _npylm->_lambda_for_type[2] << endl;
		wcout << L"	記号:		" << _npylm->_lambda_for_type[3] << endl;
		wcout << L"	ひらがな:	" << _npylm->_lambda_for_type[4] << endl;
		wcout << L"	カタカナ:	" << _npylm->_lambda_for_type[5] << endl;
		wcout << L"	漢字:		" << _npylm->_lambda_for_type[6] << endl;
		wcout << L"	漢字+ひらがな:	" << _npylm->_lambda_for_type[7] << endl;
		wcout << L"	漢字+カタカナ:	" << _npylm->_lambda_for_type[8] << endl;
		wcout << L"	その他:		" << _npylm->_lambda_for_type[9] << endl;
	}
	void save(std::string filename){
		std::ofstream ofs(filename);
		boost::archive::binary_oarchive oarchive(ofs);
		oarchive << *_npylm;
		oarchive << _all_characters;
	}
};

class PyNPYLM{
public:
	NPYLM* _npylm;
	Lattice* _lattice;
	std::unordered_set<wchar_t> _all_characters;	// すべての文字
	int _max_word_length;
	int _max_sentence_length;
	PyNPYLM(std::string filename){
		_npylm = new NPYLM();
		_lattice = new Lattice(_npylm);
		load(filename);
	}
	~PyNPYLM(){
		delete _npylm;
		delete _lattice;
	}
	void load(std::string filename){
		std::ifstream ifs(filename);
		assert(ifs.good());
		boost::archive::binary_iarchive iarchive(ifs);
		iarchive >> *_npylm;
		iarchive >> _all_characters;
		_max_word_length = _npylm->_max_word_length;
		_max_sentence_length = _npylm->_max_sentence_length;
		_npylm->_init_cache(_max_word_length, _max_sentence_length);
		_lattice->_init_cache(_max_word_length, _max_sentence_length);
	}
	python::list parse(std::wstring str){
		init_cache_if_needed(str.size());
		std::vector<int> segments;		// 分割の一時保存用
		Sentence* sentence = new Sentence(str);
		_lattice->viterbi_decode(sentence, segments);
		sentence->split(segments);
		python::list words;
		for(int n = 0;n < sentence->get_num_segments_without_special_tokens();n++){
			std::wstring word = sentence->get_word_str_at(n + 2);
			words.append(word);
		}
		delete sentence;
		return words;
	}
	void init_cache_if_needed(int max_sentence_length){
		int max_word_length = _npylm->_max_word_length;
		if(max_sentence_length > _lattice->_max_sentence_length){
			_lattice->_init_cache(max_word_length, max_sentence_length);
		}
		if(max_sentence_length > _npylm->_max_sentence_length){
			_npylm->_init_cache(max_word_length, max_sentence_length);
		}
	}
};

BOOST_PYTHON_MODULE(model){
	python::class_<PyTrainer>("trainer")
	.def("add_textfile", &PyTrainer::add_textfile)
	.def("compile", &PyTrainer::compile)
	.def("detect_collision", &PyTrainer::detect_collision)
	.def("save", &PyTrainer::save)
	.def("get_num_sentences_train", &PyTrainer::get_num_sentences_train)
	.def("get_num_sentences_test", &PyTrainer::get_num_sentences_test)
	.def("set_always_accept_new_segmentation", &PyTrainer::set_always_accept_new_segmentation)
	.def("set_max_word_length", &PyTrainer::set_max_word_length)
	.def("set_lambda_prior", &PyTrainer::set_lambda_prior)
	.def("set_seed", &PyTrainer::set_seed)
	.def("show_sampled_segmentation_train", &PyTrainer::show_sampled_segmentation_train)
	.def("show_sampled_segmentation_test", &PyTrainer::show_sampled_segmentation_test)
	.def("show_viterbi_segmentation_train", &PyTrainer::show_viterbi_segmentation_train)
	.def("show_viterbi_segmentation_test", &PyTrainer::show_viterbi_segmentation_test)
	.def("dump_hpylm", &PyTrainer::dump_hpylm)
	.def("dump_vpylm", &PyTrainer::dump_vpylm)
	.def("dump_Pk_vpylm", &PyTrainer::dump_Pk_vpylm)
	.def("dump_lambda", &PyTrainer::dump_lambda)
	.def("perform_gibbs_sampling", &PyTrainer::perform_gibbs_sampling)
	.def("compute_perplexity_train", &PyTrainer::compute_perplexity_train)
	.def("compute_perplexity_test", &PyTrainer::compute_perplexity_test)
	.def("sample_lambda", &PyTrainer::sample_lambda)
	.def("sample_pitman_yor_hyperparameters", &PyTrainer::sample_pitman_yor_hyperparameters)
	.def("update_Pk_vpylm", &PyTrainer::update_Pk_vpylm);

	python::class_<PyNPYLM>("npylm", python::init<std::string>())
	.def("parse", &PyNPYLM::parse);
}