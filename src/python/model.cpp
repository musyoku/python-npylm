#include <numeric>
#include "../ithmm/sampler.h"
#include "model.h"

namespace ithmm {
	Model::Model(Dataset* dataset): Model(dataset, -1){
		
	}
	Model::Model(Dataset* dataset, int depth_limit){
		_set_locale();
		double alpha = sampler::uniform(iTHMM_ALPHA_MIN, iTHMM_ALPHA_MAX);
		double gamma = sampler::uniform(iTHMM_GAMMA_MIN, iTHMM_GAMMA_MAX);
		double lambda_alpha = sampler::uniform(iTHMM_LAMBDA_ALPHA_MIN, iTHMM_LAMBDA_ALPHA_MAX);
		double lambda_gamma = sampler::uniform(iTHMM_LAMBDA_GAMMA_MAX, iTHMM_LAMBDA_GAMMA_MAX);
		double conc_h = sampler::uniform(ITHMM_SBP_CONCENTRATION_HORIZONTAL_STRENGTH_MIN, ITHMM_SBP_CONCENTRATION_HORIZONTAL_STRENGTH_MAX);
		double conc_v = sampler::uniform(ITHMM_SBP_CONCENTRATION_VERTICAL_STRENGTH_MIN, ITHMM_SBP_CONCENTRATION_VERTICAL_STRENGTH_MAX);
		double tau0 = iTHMM_TAU_0;
		double tau1 = iTHMM_TAU_1;
		_ithmm = new iTHMM(alpha, gamma, lambda_alpha, lambda_gamma, conc_h, conc_v, tau0, tau1,
							1.0 / dataset->_word_count.size(),
							depth_limit);
		_ithmm->initialize_with_training_dataset(dataset->_word_sequences_train);
	}
	Model::Model(Dataset* dataset, double alpha, double gamma, double lambda_alpha, double lambda_gamma, double conc_h, double conc_v, double tau0, double tau1, int depth_limit){
		_ithmm = new iTHMM(alpha, gamma, lambda_alpha, lambda_gamma, conc_h, conc_v, tau0, tau1,
							1.0 / dataset->_word_count.size(),
							depth_limit);
		_ithmm->initialize_with_training_dataset(dataset->_word_sequences_train);
	}
	Model::Model(std::string filename){
		_set_locale();
		_ithmm = new iTHMM();
		if(load(filename) == false){
			std::cout << filename << " not found." << std::endl;
			exit(0);
		}
	}
	Model::~Model(){
		delete _ithmm;
	}
	// 日本語周り
	void Model::_set_locale(){
		setlocale(LC_CTYPE, "ja_JP.UTF-8");
		std::ios_base::sync_with_stdio(false);
		std::locale default_loc("ja_JP.UTF-8");
		std::locale::global(default_loc);
		std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype); //※
		std::wcout.imbue(ctype_default);
		std::wcin.imbue(ctype_default);
	}
	double Model::get_alpha(){
		return _ithmm->_alpha;
	}
	double Model::get_gamma(){
		return _ithmm->_gamma;
	}
	double Model::get_lambda_alpha(){
		return _ithmm->_lambda_alpha;
	}
	double Model::get_lambda_gamma(){
		return _ithmm->_lambda_gamma;
	}
	double Model::get_concentration_v(){
		return _ithmm->_conc_v;
	}
	double Model::get_concentration_h(){
		return _ithmm->_conc_h;
	}
	double Model::get_tau0(){
		return _ithmm->_tau0;
	}
	double Model::get_tau1(){
		return _ithmm->_tau1;
	}
	void Model::set_alpha(double alpha){
		_ithmm->_alpha = alpha;
	}
	void Model::set_gamma(double gamma){
		_ithmm->_gamma = gamma;
	}
	void Model::set_lambda_alpha(double lambda_alpha){
		_ithmm->_lambda_alpha = lambda_alpha;
	}
	void Model::set_lambda_gamma(double lambda_gamma){
		_ithmm->_lambda_gamma = lambda_gamma;
	}
	void Model::set_concentration_v(double concentration){
		_ithmm->_conc_v = concentration;
	}
	void Model::set_concentration_h(double concentration){
		_ithmm->_conc_h = concentration;
	}
	void Model::set_tau0(double tau0){
		_ithmm->_tau0 = tau0;
	}
	void Model::set_tau1(double tau1){
		_ithmm->_tau1 = tau1;
	}
	bool Model::load(std::string filename){
		return _ithmm->load(filename);
	}
	bool Model::save(std::string filename){
		return _ithmm->save(filename);
	}
	// 存在する全ての状態を集める
	void Model::enumerate_all_states(std::vector<Node*> &nodes){
		assert(nodes.size() == 0);
		_ithmm->_structure_tssb->enumerate_nodes_from_left_to_right(nodes);
	}
	// 全ての棒の長さを計算しておく
	void Model::precompute_all_stick_lengths(std::vector<Node*> &all_states){
		assert(all_states.size() > 0);
		for(auto node: all_states){
			double p_eos_given_s = node->compute_transition_probability_to_eos(_ithmm->_tau0, _ithmm->_tau1);
			double total_stick_length = 1.0 - p_eos_given_s;	// <eos>以外に遷移する確率をTSSBで分配する
			_ithmm->update_stick_length_of_tssb(node->get_transition_tssb(), total_stick_length);
		}
		_ithmm->update_stick_length_of_tssb(_ithmm->_bos_tssb, 1.0);
	}
	boost::python::list Model::python_get_tags(){
		boost::python::list tags;
		std::vector<Node*> nodes;
		enumerate_all_states(nodes);
		for(const auto &node: nodes){
			std::string indices = "[" + node->_dump_indices() + "]";
			tags.append(indices);
		}
		return tags;
	}
	boost::python::list Model::python_viterbi_decode(boost::python::list py_word_ids){
		// あらかじめ全HTSSBの棒の長さを計算しておく
		std::vector<Node*> nodes;
		enumerate_all_states(nodes);
		precompute_all_stick_lengths(nodes);
		// デコード用のテーブルを確保
		int num_words = boost::python::len(py_word_ids);
		double** forward_table = new double*[num_words];
		double** decode_table = new double*[num_words];
		for(int i = 0;i < num_words;i++){
			forward_table[i] = new double[nodes.size()];
			decode_table[i] = new double[nodes.size()];
		}
		// Python側から渡された単語IDリストを変換
		std::vector<Word*> words;
		for(int i = 0;i < num_words;i++){
			Word* word = new Word();
			word->_id = boost::python::extract<int>(py_word_ids[i]);
			word->_state = NULL;
			words.push_back(word);
		}
		// ビタビアルゴリズム
		std::vector<Node*> sampled_state_sequence;
		viterbi_decode(words, nodes, sampled_state_sequence, forward_table, decode_table);
		// 結果を返す
		boost::python::list result;
		for(int i = 0;i < words.size();i++){
			std::wstring tag = L"[" + sampled_state_sequence[i]->_wdump_indices() + L"]";
			result.append(tag);
		}
		for(int i = 0;i < num_words;i++){
			delete[] forward_table[i];
			delete[] decode_table[i];
		}
		delete[] forward_table;
		delete[] decode_table;
		for(int i = 0;i < words.size();i++){
			delete words[i];
		}
		return result;
	}
	// 状態系列の復号
	// ビタビアルゴリズム
	void Model::viterbi_decode(std::vector<Word*> &sentence, std::vector<Node*> &all_states, std::vector<Node*> &sampled_state_sequence, double** forward_table, double** decode_table){
		// 初期化
		Word* word = sentence[0];
		for(int i = 0;i < all_states.size();i++){
			Node* state = all_states[i];
			Node* state_in_bos = _ithmm->_bos_tssb->find_node_by_tracing_horizontal_indices(state);
			assert(state_in_bos != NULL);
			double p_s = state_in_bos->_probability;
			double p_w_given_s = _ithmm->compute_p_w_given_s(word->_id, state);
			assert(p_s > 0);
			assert(p_w_given_s > 0);
			forward_table[0][i] = p_w_given_s * p_s;
			decode_table[0][i] = 0;
		}
		for(int t = 1;t < sentence.size();t++){
			Word* word = sentence[t];
			for(int j = 0;j < all_states.size();j++){
				Node* state = all_states[j];
				forward_table[t][j] = 0;
				double max_value = 0;
				double p_w_given_s = _ithmm->compute_p_w_given_s(word->_id, state);
				for(int i = 0;i < all_states.size();i++){
					Node* prev_state = all_states[i];
					TSSB* transition_tssb = prev_state->get_transition_tssb();
					assert(transition_tssb != NULL);
					Node* state_in_prev_htssb = transition_tssb->find_node_by_tracing_horizontal_indices(state);
					assert(state_in_prev_htssb != NULL);
					double p_s_given_prev = state_in_prev_htssb->_probability;
					double value = p_s_given_prev * forward_table[t - 1][i];
					if(value > max_value){
						max_value = value;
						forward_table[t][j] = value * p_w_given_s;
						decode_table[t][j] = i;
					}
				}
			}
		}
		// 後ろ向きに系列を復元
		std::vector<int> series_indices;
		int n = sentence.size() - 1;
		int k = 0;
		double max_value = 0;
		for(int i = 0;i < all_states.size();i++){
			if(forward_table[n][i] > max_value){
				k = i;
				max_value = forward_table[n][i];
			}
		}
		series_indices.push_back(k);
		for(int t = n - 1;t >= 0;t--){
			k = decode_table[t + 1][series_indices[n - t - 1]];
			series_indices.push_back(k);
		}
		std::reverse(series_indices.begin(), series_indices.end());
		// ノードをセット
		sampled_state_sequence.clear();
		for(int t = 0;t <= n;t++){
			int k = series_indices[t];
			sampled_state_sequence.push_back(all_states[k]);
		}
	}
	// データの対数尤度を計算
	// 前向きアルゴリズム
	double Model::compute_p_sentence(std::vector<Word*> &sentence, std::vector<Node*> &states, double** forward_table){
		// 初期化
		Word* word = sentence[0];
		for(int i = 0;i < states.size();i++){
			Node* state = states[i];
			Node* state_in_bos = _ithmm->_bos_tssb->find_node_by_tracing_horizontal_indices(state);
			assert(state_in_bos != NULL);
			double p_s = state_in_bos->_probability;
			double p_w_given_s = _ithmm->compute_p_w_given_s(word->_id, state);
			assert(p_s > 0);
			assert(p_w_given_s > 0);
			forward_table[0][i] = p_w_given_s * p_s;
		}
		for(int t = 1;t < sentence.size();t++){
			Word* word = sentence[t];
			for(int j = 0;j < states.size();j++){
				Node* state = states[j];
				forward_table[t][j] = 0;
				double p_w_given_s = _ithmm->compute_p_w_given_s(word->_id, state);
				for(int i = 0;i < states.size();i++){
					Node* prev_state = states[i];
					TSSB* transition_tssb = prev_state->get_transition_tssb();
					assert(transition_tssb != NULL);
					Node* state_in_prev_htssb = transition_tssb->find_node_by_tracing_horizontal_indices(state);
					assert(state_in_prev_htssb != NULL);
					double p_s_given_prev = state_in_prev_htssb->_probability;
					forward_table[t][j] += p_w_given_s * p_s_given_prev * forward_table[t - 1][i];
				}
			}
		}
		int t = sentence.size() - 1;
		double p_x = 0;
		for(int j = 0;j < states.size();j++){
			p_x += forward_table[t][j];
		}
		return p_x;
	}
	void Model::show_assigned_words_for_each_tag(Dictionary* dict, int number_to_show_for_each_tag, bool show_probability){
		using std::cout;
		using std::wcout;
		using std::endl;
		std::vector<Node*> nodes;
		enumerate_all_states(nodes);
		for(const auto &node: nodes){
			std::multiset<std::pair<int, double>, multiset_value_comparator> ranking;
			_ithmm->geneerate_word_ranking_of_node(node, ranking);
			int n = 0;
			std::string indices = node->_dump_indices();
			// linuxでバグるのでstringとwstring両方作る
			std::string tab = "";
			for(int i = 0;i < node->_depth_v;i++){
				tab += "	";
			}
			cout << "\x1b[32;1m" << tab << "[" << indices << "]" << "\x1b[0m" << endl;
			std::wstring wtab = L"";
			for(int i = 0;i < node->_depth_v;i++){
				wtab += L"	";
			}
			wcout << wtab;
			for(const auto &elem: ranking){
				int word_id = elem.first;
				if(dict->is_id_unk(word_id)){
					continue;
				}
				std::wstring &word = dict->_id_to_str[word_id];
				double p = elem.second;
				int count = node->_num_word_assignment[word_id];
				wcout << "\x1b[1m" << word << "\x1b[0m" << L" (" << count;
				if(show_probability){
					wcout << L";p=" << p;
				} 
				wcout << L") ";
				n++;
				if(n > number_to_show_for_each_tag){
					break;
				}
			}
			wcout << endl;
			ranking.clear();
		}
	}
	void Model::show_assigned_words_and_probability_for_each_tag(Dictionary* dict, int number_to_show_for_each_tag){
		std::vector<Node*> nodes;
		enumerate_all_states(nodes);
		std::wcout << "word      	count	probability" << std::endl;
		for(const auto &node: nodes){
			std::multiset<std::pair<int, double>, multiset_value_comparator> ranking;
			_ithmm->geneerate_word_ranking_of_node(node, ranking);
			int n = 0;
			std::string indices = node->_dump_indices();
			std::cout << "\x1b[32;1m" << "[" << indices << "]" << "\x1b[0m" << std::endl;
			for(const auto &elem: ranking){
				int word_id = elem.first;
				std::wstring &word = dict->_id_to_str[word_id];
				for(int i = 0;i < std::max(0, 15 - (int)word.size());i++){
					word += L" ";
				}
				double p = elem.second;
				int count = node->_num_word_assignment[word_id];
				std::wcout << "\x1b[1m" << word << "\x1b[0m" << L"	" << count << L"	" << p << std::endl;
				n++;
				if(n > number_to_show_for_each_tag){
					break;
				}
			}
			std::wcout << std::endl;
			ranking.clear();
		}
	}
	void Model::show_hpylm_for_each_tag(Dictionary* dict){
		std::vector<Node*> nodes;
		enumerate_all_states(nodes);
		for(const auto &node: nodes){
			std::multiset<std::pair<int, double>, multiset_value_comparator> ranking;
			_ithmm->geneerate_word_ranking_of_node(node, ranking);
			std::string indices = node->_dump_indices();
			// linuxでバグるのでstringとwstring両方作る
			std::string tab = "";
			for(int i = 0;i < node->_depth_v;i++){
				tab += "	";
			}
			std::cout << "\x1b[32;1m" << tab << "[" << indices << "]" << "\x1b[0m" << std::endl;
			std::wstring wtab = L"";
			for(int i = 0;i < node->_depth_v;i++){
				wtab += L"	";
			}
			std::wcout << wtab;
			for(const auto &table: node->_hpylm->_arrangement){
				int word_id = table.first;
				std::wstring &word = dict->_id_to_str[word_id];
				int num_tables = table.second.size();
				int num_customers = std::accumulate(table.second.begin(), table.second.end(), 0);
				std::wcout << "\x1b[1m" << word << "\x1b[0m" << L" (#t=" << num_tables << ";#c=" << num_customers << L") ";
			}
			std::wcout << std::endl;
		}
	}
	void Model::show_sticks(){
		std::vector<Node*> nodes;
		enumerate_all_states(nodes);
		for(const auto &node: nodes){
			std::string indices = node->_dump_indices();
			std::cout << "\x1b[1m[" << indices << "]\x1b[0m";
			_show_stick(node);
		}
	}
	void Model::_show_stick(Node* node_in_structure){
		assert(node_in_structure != NULL);
		double p_eos = node_in_structure->compute_transition_probability_to_eos(_ithmm->_tau0, _ithmm->_tau1);
		TSSB* transition_tssb = node_in_structure->get_transition_tssb();
		assert(transition_tssb != NULL);
		_ithmm->update_stick_length_of_tssb(transition_tssb, 1.0 - p_eos);

		std::vector<Node*> nodes;
		transition_tssb->enumerate_nodes_from_left_to_right(nodes);
		for(const auto &node: nodes){
			std::string indices = node->_dump_indices();
			std::string tab = "";
			for(int i = 0;i < node->_depth_v;i++){
				tab += "	";
			}
			std::cout << "\x1b[32;1m" << tab << "[" << indices << "]" << "\x1b[0m " << node->_probability << std::endl;
		}
	}
	void Model::update_hyperparameters(){
		_ithmm->sample_hpylm_hyperparameters();
	}
}