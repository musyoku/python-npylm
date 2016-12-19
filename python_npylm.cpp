#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/python.hpp>
#include <boost/format.hpp>
#include "npylm/core/npylm.h"
#include "npylm/core/vocab.h"
#include "npylm/core/chartype.h"
#include "npylm/lattice/trigram.h"
#include "npylm/util.h"

using namespace boost;

template<class T>
python::list list_from_vector(vector<T> &vec){  
	 python::list list;
	 typename vector<T>::const_iterator it;

	 for(it = vec.begin(); it != vec.end(); ++it)   {
		  list.append(*it);
	 }
	 return list;
}
// Pythonラッパー
class PyNPYLM{
private:
	HPYLM* _hpylm;
	VPYLM* _vpylm;
	NPYLM3* _npylm;
	TrigramLattice* _lattice;
	Vocab* _vocab;
	vector<wstring> _dataset;
	bool _is_ready;

public:
	PyNPYLM(){
		c_printf("[*]%s\n", "NPYLMを初期化しています ...");
		_hpylm = new HPYLM(3);
		_vpylm = new VPYLM();
		_vocab = new Vocab();
		_npylm = new NPYLM3(_hpylm, _vpylm, _vocab);
		_lattice = new TrigramLattice(_npylm, _vocab);
		_is_ready = false;
	}
	void load_textfile(string filename){
		load_characters_in_textfile(filename, _dataset);
		// 全文字種を特定
		int max_sentence_length = 0;
		int sum_sentence_length = 0;
		for(int i = 0;i < _dataset.size();i++){
			wstring &sentence = _dataset[i];
			sum_sentence_length += sentence.size();
			for(int l = 0;l < sentence.size();l++){
				vocab->add_character(sentence[l]);
			}
			if(sentence.size() > max_sentence_length){
				max_sentence_length = sentence.size();
			}
		}
		double average_sentence_length = sum_sentence_length / (double)_dataset.size();
		c_printf("[*]%s\n", (boost::format("を読み込みました. (%d行, 平均文長%lf, 最大文長%d)") % filename % _dataset.size() % average_sentence_length % max_sentence_length).str().c_str());
		double g0 = 1.0 / vocab->get_num_characters();
		set_g0(g0);
		_lattice->init(max_sentence_length);
	}
	void set_g0(double g0){
		_vpylm->_g0 = g0;
		c_printf("[*]%s\n", (boost::format("g0 <- %lf") % g0).str().c_str());
		_is_ready = true;
	}
	vector<wstring> split_sentence_into_words(wstring &sentence, vector<int> &segments){
		int num_pieces_in_segments = accumulate(segments.begin(), segments.end(), 0);
		if(num_pieces_in_segments != sentence.size()){
			c_printf("[r]%s [*]%s %d != %d\n", "エラー:", "不正な分割です.", num_pieces_in_segments, sentence.size());
			exit(1);
		}
		vector<wstring> words;
		int pointer = 0;
		for(int i = 0;i < segments.size();i++){
			int segment_length = segments[i];
			wstring word(sentence.begin() + pointer, sentence.begin() + pointer + segment_length);
			words.push_back(word);
			pointer += segment_length;
		}
		return words;
	}
	vector<id> convert_words_to_token_ids(vector<wstring> &words){
		vector<id> token_ids;
		token_ids.push_back(_vocab->get_bos_id());
		token_ids.push_back(_vocab->get_bos_id());		// 3-gramなので2回
		for(int i = 0;i < words.size();i++){
			token_ids.push_back(vocab->add_string(words[i]));
		}
		token_ids.push_back(vocab->get_eos_id());
		return token_ids;
	}
	void show_segmentation_result_for_sentence(wstring &sentence, vector<int> &segments){
		_lattice->perform_blocked_gibbs_sampling(sentence, segments, true);
		vector<wstring> words = split_sentence_into_words(sentence, segments);
		for(int i = 0;i < words.size();i++){
			wcout << words[i] << L" / ";
		}
		wcout << endl;
	}
	void show_segmentation_result_for_data(int data_index){
		vector<int> segments;
		int data_index = rand_indices[step];
		wstring &sentence = _dataset[data_index];
		show_segmentation_result_for_sentence(sentence, segments);
	}
	void show_random_segmentation_result(int num_generation = 30){
		vector<int> rand_indices;
		for(int data_index = 0;data_index < _dataset.size();data_index++){
			rand_indices.push_back(data_index);
		}
		shuffle(rand_indices.begin(), rand_indices.end(), Sampler::mt);
		vector<int> segments;
		for(int step = 0;step < min(_dataset.size(), num_generation);step++){
			int data_index = rand_indices[step];
			wstring &sentence = _dataset[data_index];
			show_segmentation_result_for_sentence(sentence, segments);
		}
	}
	void train(bool compute_ppl_at_each_epoch = false){
		vector<int> rand_indices;
		for(int data_index = 0;data_index < _dataset.size();data_index++){
			rand_indices.push_back(data_index);
		}
		int max_epoch = 1000;
		int num_data = _dataset.size();
		unordered_map<int, vector<int>> old_segments_for_data;	// 古い単語分割
		unordered_map<id, int> words_from_segmentation;			// 単語分割で得られた語彙集合
		vector<int> segments;									// 分割の一時保存用
		// ギブスイテレーション
		for(int epoch = 1;epoch <= max_epoch;epoch++){
			auto start_time = chrono::system_clock::now();
			shuffle(rand_indices.begin(), rand_indices.end(), Sampler::mt);		// データをシャッフル
			words_from_segmentation.clear();					// 語彙集合をリセット
			npylm->update_pk_vpylm(_lattice->_max_length + 1);	// VPYLMから単語を生成し単語長の分布を更新
			// モデルパラメータを更新
			for(int step = 0;step < num_data;step++){
				show_progress(step, num_data);
				// 訓練データを一つ取り出す
				int data_index = rand_indices[step];
				wstring &sentence = _dataset[data_index];
				// 古い分割を取得
				auto itr_segments = old_segments_for_data.find(data_index);
				if(itr_segments == old_segments_for_data.end()){
					// 最初は<bos>,<eos>を除く全ての文字列が1単語としてモデルに追加される
					segments.clear();
					segments.push_back(sentence.size());
					npylm->_is_initializing_phase = true;
				}else{
					npylm->_is_initializing_phase = false;
					// 古い分割をモデルから削除
					vector<wstring> old_words = split_sentence_into_words(sentence, itr_segments->second);
					vector<id> old_token_ids = convert_words_to_token_ids(old_words, vocab);
					for(int token_t_index = 2;token_t_index < old_token_ids.size();token_t_index++){
						npylm->remove_customer_at_timestep(old_token_ids, token_t_index);
					}
					// 新しい分割を取得
					_lattice->perform_blocked_gibbs_sampling(sentence, segments);
				}
				// 新しい分割結果をモデルに追加
				vector<wstring> words = split_sentence_into_words(sentence, segments);
				vector<id> token_ids = convert_words_to_token_ids(words, vocab);	// token_idsにはbosとeosが含まれる
				for(int token_t_index = 2;token_t_index < token_ids.size();token_t_index++){
					npylm->add_customer_at_timestep(token_ids, token_t_index);
					if(token_t_index != token_ids.size() - 1){
						words_from_segmentation[token_ids[token_t_index]] += 1;		// <eos>以外を語彙集合に追加
					}
				}
				old_segments_for_data[data_index] = segments;
			}

			// HPYLM,VPYLMのdとthetaをサンプリング
			npylm->sample_pitman_yor_hyperparameters();

			// lambdaをサンプリング
			npylm->sample_lambda(words_from_segmentation);

			// パープレキシティ
			double ppl = 0;
			if(compute_ppl_at_each_epoch){
				// for(int step = 0;step < num_data;step++){
				// 	wstring &sentence = _dataset[step];
				// 	_lattice->perform_blocked_gibbs_sampling(sentence, segments);
				// 	vector<wstring> words = split_sentence_into_words(sentence, segments);
				// 	vector<id> token_ids = convert_words_to_token_ids(words, vocab);
				// 	double log_p = npylm->log2_Pw(token_ids) / token_ids.size();
				// 	ppl += log_p;
				// }
				// ppl = exp(-ppl / num_data);
			}

			// 結果を表示
			auto end_time = chrono::system_clock::now();
			auto duration = end_time - start_time;
			auto msec = chrono::duration_cast<chrono::milliseconds>(duration).count();
			printf("Epoch %d / %d - %.1f lps %.3f ppl - %d nodes (vpylm) - %d depth (vpylm) - %d nodes (hpylm)\n", 
				epoch, 
				max_epoch, 
				(double)num_data / msec * 1000.0,
				ppl,
				npylm->_vpylm->get_num_nodes(),
				npylm->_vpylm->get_max_depth(),
				npylm->_hpylm->get_num_nodes()
			);

			if(npylm->_hpylm->_root->_num_tables > npylm->_vpylm->get_num_customers()){
				c_printf("[r]%s [*]%s\n", "エラー:", "VPYLMの客数が異常です. npylm->_hpylm->_root->get_num_tables() > npylm->_vpylm->get_num_customers()");
				exit(1);
			}

			// 不要な単語IDを除去
			map<id, bool> flags;
			_npylm->set_active_tokens(flags);
			_vocab->remove_unused_token_ids(flags);
		}
	}

};

BOOST_PYTHON_MODULE(npylm){
	python::class_<PyNPYLM>("npylm")
	.def("load_textfile", &PyNPYLM::load_textfile)
	.def("train", &PyNPYLM::train)
	.def("show_random_segmentation_result", &PyNPYLM::show_random_segmentation_result)
	.def("show_segmentation_result_for_data", &PyNPYLM::show_segmentation_result_for_data)
}