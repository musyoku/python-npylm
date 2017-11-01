#include <boost/python.hpp>
#include <cassert>
#include <iostream>
#include "../npylm/sampler.h"
#include "../npylm/wordtype.h"
#include "trainer.h"

namespace npylm {
	Trainer::Trainer(Dataset* dataset, Model* model, bool always_accept_new_segmentation){
		_dataset = dataset;
		_model = model;
		_dict = dataset->_dict;
		_vpylm_sampling_probability_table = new double[_dict->get_num_characters() + 1];	// </s>を含む
		_vpylm_sampling_id_table = new wchar_t[_dict->get_num_characters() + 1];			// </s>を含む
		_added_to_npylm_train = new bool[dataset->_sentence_sequences_train.size()];
		for(int data_index = 0;data_index < dataset->_sentence_sequences_train.size();data_index++){
			_rand_indices_train.push_back(data_index);
			_added_to_npylm_train[data_index] = false;
		}
		for(int data_index = 0;data_index < dataset->_sentence_sequences_dev.size();data_index++){
			_rand_indices_dev.push_back(data_index);
		}
		_always_accept_new_segmentation = always_accept_new_segmentation;
		_num_segmentation_rejection = 0;
		_num_segmentation_acceptance = 0;
	}

	// HPYLM,VPYLMのdとthetaをサンプリング
	void Trainer::sample_hpylm_vpylm_hyperparameters(){
		_model->_npylm->sample_hpylm_vpylm_hyperparameters();
	}
	// 文字種ごとにλのサンプリング
	void Trainer::sample_lambda(){
		std::vector<double> a_for_type(WORDTYPE_NUM_TYPES + 1, 0.0);
		std::vector<double> b_for_type(WORDTYPE_NUM_TYPES + 1, 0.0);
		std::unordered_set<id> words;
		NPYLM* npylm = _model->_npylm;
		for(int type = 1;type <= WORDTYPE_NUM_TYPES;type++){
			a_for_type[type] = npylm->_lambda_a;
			b_for_type[type] = npylm->_lambda_b;
		}
		for(auto sentence: _dataset->_sentence_sequences_train){
			// <bos>と<eos>は除外
			for(int t = 2;t < sentence->get_num_segments() - 1;t++){
				std::wstring word = sentence->get_word_str_at(t);
				id word_id = sentence->get_word_id_at(t);
				int word_length = sentence->get_word_length_at(t);
				if(word_length > npylm->_max_word_length){
					continue;
				}
				if(words.find(word_id) == words.end()){
					std::vector<int> &tables = npylm->_hpylm->_root->_arrangement[word_id];
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
			npylm->_lambda_for_type[type] = lambda;
		}
	}
	// VPYLMに文脈を渡し次の文字を生成
	wchar_t Trainer::sample_word_from_vpylm_given_context(wchar_t* context_ids, int context_length, int sample_t, bool skip_eow){
		double sum_probs = 0;
		lm::VPYLM* vpylm = _model->_npylm->_vpylm;
		int table_index = 0;
		auto all_characters = _dict->_all_characters;
		int num_characters = _dict->get_num_characters();
		for(wchar_t character_id: all_characters){
			assert(table_index < num_characters);
			double pw = vpylm->compute_p_w_given_h(character_id, context_ids, 0, context_length - 1);
			sum_probs += pw;
			_vpylm_sampling_probability_table[table_index] = pw;
			_vpylm_sampling_id_table[table_index] = character_id;
			table_index++;
		}
		if(skip_eow == false){
			assert(table_index < num_characters + 1);
			double pw = vpylm->compute_p_w_given_h(ID_EOW, context_ids, 0, context_length - 1);
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
	void Trainer::update_p_k_given_vpylm(){
		int num_samples = 20000;
		int early_stopping_threshold = 10;
		int max_word_length = _model->get_max_word_length() + 1;
		double* pk_vpylm = _model->_npylm->_pk_vpylm;
		int* num_words_of_k = new int[max_word_length];
		for(int i = 0;i <= max_word_length;i++){
			pk_vpylm[i] = 0;
			num_words_of_k[i] = 0;
		}
		wchar_t* wrapped_character_ids = new wchar_t[max_word_length + 2];
		double sum_words = 0;
		for(int m = 1;m <= num_samples;m++){
			if (PyErr_CheckSignals() != 0) {	// ctrl+cが押されたかチェック
				return;		
			}
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

			// すべてのkが生成されていたら早期終了
			if(m % 100 == 0){
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
			pk_vpylm[k] = (num_words_of_k[k] + 1) / (sum_words + max_word_length);	// ラプラススムージングを入れておく
			assert(pk_vpylm[k] > 0);
		}
		delete[] num_words_of_k;
		delete[] wrapped_character_ids;
	}
	// 単語分割のギブスサンプリング
	void Trainer::gibbs(){
		int num_sentences = _dataset->_sentence_sequences_train.size();
		assert(num_sentences > 0);
		int max_sentence_length = _dataset->get_max_sentence_length();
		std::vector<int> segments;		// 分割の一時保存用
		shuffle(_rand_indices_train.begin(), _rand_indices_train.end(), sampler::mt);		// データをシャッフル
		int* old_segments = new int[max_sentence_length + 3];
		int num_old_segments;
		// モデルパラメータを更新
		for(int step = 1;step <= num_sentences;step++){
			if (PyErr_CheckSignals() != 0) {	// ctrl+cが押されたかチェック
				return;		
			}
			// 訓練データを一つ取り出す
			int data_index = _rand_indices_train[step - 1];
			assert(data_index < _dataset->_sentence_sequences_train.size());
			Sentence* sentence = _dataset->_sentence_sequences_train[data_index];

			// 教師あり
			if(sentence->is_supervised()){
				// モデルに追加されているかチェック
				if(_added_to_npylm_train[data_index] == true){
					// 古い分割をモデルから削除
					for(int t = 2;t < sentence->get_num_segments();t++){
						_model->_npylm->remove_customer_at_time_t(sentence, t);
					}
				}
				// 同じ分割結果を再度モデルに追加
				// ？？？「同じ分割を追加するなら最初から削除しなければ良いのでは？」
				// 追加と削除を繰り返すことでHPYLMとVPYLMのパラメータ（客の配置）がギブスサンプリングされるので必要
				for(int t = 2;t < sentence->get_num_segments();t++){
					_model->_npylm->add_customer_at_time_t(sentence, t);
				}
				_added_to_npylm_train[data_index] = true;
				continue;
			}
			// 教師なし
			// モデルに追加されているかチェック
			if(_added_to_npylm_train[data_index] == true){
				double old_log_ps, new_log_ps;
				// 古い分割をモデルから削除
				for(int t = 2;t < sentence->get_num_segments();t++){
					_model->_npylm->remove_customer_at_time_t(sentence, t);
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
					old_log_ps = _model->_npylm->compute_log_p_w(sentence);
				}
				
				#ifdef __DEBUG__
				// 正規化しない場合の結果と比較するためシードを合わせる
				int seed = (unsigned int)time(NULL);
				sampler::mt.seed(seed);
				#endif

				// 新しい分割を取得
				_model->_lattice->blocked_gibbs(sentence, segments, true);
				sentence->split(segments);
				
				#ifdef __DEBUG__
				// 正規化しない場合の結果と比較
				std::vector<int> a = segments;
				sampler::mt.seed(seed);
				_model->_lattice->blocked_gibbs(sentence, segments, false);
				std::vector<int> b = segments;
				assert(a.size() == b.size());
				for(int i = 0;i < a.size();i++){
					assert(a[i] == b[i]);
				}
				#endif

				// 以前の分割結果と現在の分割結果の確率を求める
				// 本来は分割を一定数サンプリングして平均をとるべき
				if(_always_accept_new_segmentation == false){
					new_log_ps = _model->_npylm->compute_log_p_w(sentence);
					// 新しい分割の方が確率が低い場合、比率のベルヌーイ試行でどちらを採用するか決める.
					double bernoulli = std::min(1.0, exp(new_log_ps - old_log_ps));
					double r = sampler::uniform(0, 1);
					if(bernoulli < r){
						// 新しい分割を捨てて古いものに差し替える
						sentence->split(old_segments, num_old_segments);
						_num_segmentation_rejection++;
					}else{
						_num_segmentation_acceptance++;
					}
				}
			}
			// 新しい分割結果をモデルに追加
			for(int t = 2;t < sentence->get_num_segments();t++){
				_model->_npylm->add_customer_at_time_t(sentence, t);
			}
			_added_to_npylm_train[data_index] = true;
		}
		// 客数チェック
		assert(_model->_npylm->_hpylm->_root->_num_tables <= _model->_npylm->_vpylm->get_num_customers());
		delete[] old_segments;
	}
	double Trainer::compute_perplexity_train(){
		return _compute_perplexity(_dataset->_sentence_sequences_train);
	}
	double Trainer::compute_perplexity_dev(){
		return _compute_perplexity(_dataset->_sentence_sequences_dev);
	}
	double Trainer::_compute_perplexity(std::vector<Sentence*> &dataset){
		if(dataset.size() == 0){
			return 0;
		}
		double ppl = 0;
		int num_sentences = dataset.size();
		std::vector<int> segments;		// 分割の一時保存用
		for(int data_index = 0;data_index < num_sentences;data_index++){
			if (PyErr_CheckSignals() != 0) {	// ctrl+cが押されたかチェック
				return 0;		
			}
			Sentence* sentence = dataset[data_index]->copy();	// 干渉を防ぐためコピー
			_model->_lattice->viterbi_decode(sentence, segments);
			sentence->split(segments);
			ppl += _model->_npylm->compute_log_p_w(sentence) / ((double)sentence->get_num_segments() - 2);
			delete sentence;
		}
		ppl = exp(-ppl / num_sentences);
		return ppl;
	}
	// デバッグ用
	void Trainer::remove_all_data(){
		int max_sentence_length = _dataset->get_max_sentence_length();
		wchar_t* wrapped_character_ids = new wchar_t[max_sentence_length + 2];	// <bow>と<eow>を追加
		for(int data_index = 0;data_index < _dataset->_sentence_sequences_train.size();data_index++){
			if (PyErr_CheckSignals() != 0) {		// ctrl+cが押されたかチェック
				return;
			}
			Sentence* sentence = _dataset->_sentence_sequences_train[data_index];
			// 古い分割をモデルから削除
			if(_added_to_npylm_train[data_index] == true){
				for(int t = 2;t < sentence->get_num_segments();t++){
					_model->_npylm->remove_customer_at_time_t(sentence, t);
				}
			}
		}
		delete[] wrapped_character_ids;
	}
	void Trainer::print_segmentation_train(int num_to_print){
		_print_segmentation(num_to_print, _dataset->_sentence_sequences_train, _rand_indices_train);
	}
	void Trainer::print_segmentation_dev(int num_to_print){
		shuffle(_rand_indices_dev.begin(), _rand_indices_dev.end(), sampler::mt);
		_print_segmentation(num_to_print, _dataset->_sentence_sequences_dev, _rand_indices_dev);
	}
	void Trainer::_print_segmentation(int num_to_print, std::vector<Sentence*> &dataset, std::vector<int> &rand_indices){
		num_to_print = std::min((int)dataset.size(), num_to_print);
		std::vector<int> segments;		// 分割の一時保存用
		for(int n = 0;n < num_to_print;n++){
			if (PyErr_CheckSignals() != 0) {	// ctrl+cが押されたかチェック
				return;		
			}
			int data_index = rand_indices[n];
			Sentence* sentence = dataset[data_index]->copy();
			_model->_lattice->blocked_gibbs(sentence, segments, true);
			sentence->split(segments);
			sentence->dump_words();
			delete sentence;
		}
	}
}