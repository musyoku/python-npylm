#pragma once
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include "common.h"
#include "sampler.h"
using namespace std;

namespace npylm{
	template<class T>
	class Node{
	public:
	hashmap<T, Node*> _children;				// 子の文脈木
	hashmap<T, vector<int>> _arrangement;		// 客の配置 vector<int>のk番目の要素がテーブルkの客数を表す
	Node* _parent;								// 親ノード
	int _num_tables;							// 総テーブル数
	int _num_customers;							// 客の総数
	int _stop_count;							// 停止回数. VPYLM用
	int _pass_count;							// 通過回数. VPYLM用
	int _depth;									// ノードの深さ. rootが0であることに注意
	T _token_id;								// このノードに割り当てられた単語ID（文字ID）

	Node(T token_id = 0){
		_num_tables = 0;
		_num_customers = 0;
		_stop_count = 0;
		_pass_count = 0;
		_token_id = token_id;
		_parent = NULL;
	}
	bool parent_exists(){
		return !(_parent == NULL);
	}
	bool child_exists(T token_id){
		return !(_children.find(token_id) == _children.end());
	}
	bool need_to_remove_from_parent(){
		if(_parent == NULL){
			return false;
		}
		if(_children.size() == 0 && _arrangement.size() == 0){
			return true;
		}
		return false;
	}
	int get_num_tables_serving_word(T token_id){
		if(_arrangement.find(token_id) == _arrangement.end()){
			return 0;
		}
		return _arrangement[token_id].size();
	}
	int get_num_customers_eating_word(T token_id){
		if(_arrangement.find(token_id) == _arrangement.end()){
			return 0;
		}
		vector<int> &num_customers_at_table = _arrangement[token_id];
		int sum = 0;
		for(int i = 0;i < num_customers_at_table.size();i++){
			sum += num_customers_at_table[i];
		}
		return sum;
	}
	Node* find_child_node(T token_id, bool generate_if_not_exist = false){
		auto itr = _children.find(token_id);
		if (itr != _children.end()) {
			return itr->second;
		}
		if(generate_if_not_exist == false){
			return NULL;
		}
		Node* child = new Node(token_id);
		child->_parent = this;
		child->_depth = _depth + 1;
		_children[token_id] = child;
		return child;
	}
	// 客をテーブルに追加
	bool add_customer_to_table(T token_id, int table_k, double g0, vector<double> &d_m, vector<double> &theta_m, int &added_to_table_k_of_root){
		auto itr = _arrangement.find(token_id);
		if(itr == _arrangement.end()){
			return add_customer_to_new_table(token_id, g0, d_m, theta_m, added_to_table_k_of_root);
		}
		vector<int> &num_customers_at_table = itr->second;
		assert(table_k < num_customers_at_table.size());
		num_customers_at_table[table_k]++;
		_num_customers++;
		return true;
	}
	bool add_customer_to_table(T token_id, int table_k, double* parent_pw_at_depth, vector<double> &d_m, vector<double> &theta_m, int &added_to_table_k_of_root){
		auto itr = _arrangement.find(token_id);
		if(itr == _arrangement.end()){
			return add_customer_to_new_table(token_id, parent_pw_at_depth, d_m, theta_m, added_to_table_k_of_root);
		}
		vector<int> &num_customers_at_table = itr->second;
		assert(table_k < num_customers_at_table.size());
		num_customers_at_table[table_k]++;
		_num_customers++;
		return true;
	}
	bool add_customer_to_new_table(T token_id, double g0, vector<double> &d_m, vector<double> &theta_m, int &added_to_table_k_of_root){
		_add_customer_to_new_table(token_id);
		if(_parent != NULL){
			bool success = _parent->add_customer(token_id, g0, d_m, theta_m, false, added_to_table_k_of_root);
			assert(success == true);
		}
		return true;
	}
	bool add_customer_to_new_table(T token_id, double* parent_pw_at_depth, vector<double> &d_m, vector<double> &theta_m, int &added_to_table_k_of_root){
		_add_customer_to_new_table(token_id);
		if(_parent != NULL){
			bool success = _parent->add_customer(token_id, parent_pw_at_depth, d_m, theta_m, false, added_to_table_k_of_root);
			assert(success == true);
		}
		return true;
	}
	void _add_customer_to_new_table(T token_id){
		auto itr = _arrangement.find(token_id);
		if(itr == _arrangement.end()){
			vector<int> tables = {1};
			_arrangement[token_id] = tables;
		}else{
			vector<int> &num_customers_at_table = itr->second;
			num_customers_at_table.push_back(1);
		}
		_num_tables++;
		_num_customers++;
	}
	bool remove_customer_from_table(T token_id, int table_k, int &removed_from_table_k_of_root){
		auto itr = _arrangement.find(token_id);
		assert(itr != _arrangement.end());
		vector<int> &num_customers_at_table = itr->second;
		assert(table_k < num_customers_at_table.size());
		num_customers_at_table[table_k]--;
		_num_customers--;
		assert(num_customers_at_table[table_k] >= 0);
		if(num_customers_at_table[table_k] == 0){
			if(_parent != NULL){
				bool success = _parent->remove_customer(token_id, false, removed_from_table_k_of_root);
				assert(success == true);
			}
			num_customers_at_table.erase(num_customers_at_table.begin() + table_k);
			_num_tables--;
			if(num_customers_at_table.size() == 0){
				_arrangement.erase(token_id);
			}
		}
		return true;
	}
	// NPYLMではルートノードのどのテーブルに代理客が追加されたかを知る必要がある
	// 再帰計算があるので遅い
	bool add_customer(T token_id, double g0, vector<double> &d_m, vector<double> &theta_m, bool update_beta_count, int &added_to_table_k_of_root){
		init_hyperparameters_at_depth_if_needed(_depth, d_m, theta_m);
		double d_u = d_m[_depth];
		double theta_u = theta_m[_depth];
		double parent_pw = g0;
		if(_parent){
			parent_pw = _parent->compute_Pw(token_id, g0, d_m, theta_m);
		}
		auto itr = _arrangement.find(token_id);
		if(itr == _arrangement.end()){
			add_customer_to_new_table(token_id, g0, d_m, theta_m, added_to_table_k_of_root);
			if(update_beta_count == true){
				increment_stop_count();
			}
			if(_depth == 0){	// if root node
				added_to_table_k_of_root = 0;
			}
			return true;
		}

		vector<int> &num_customers_at_table = itr->second;
		double sum = 0;
		for(int k = 0;k < num_customers_at_table.size();k++){
			sum += std::max(0.0, num_customers_at_table[k] - d_u);
		}
		double t_u = _num_tables;
		sum += (theta_u + d_u * t_u) * parent_pw;

		double normalizer = 1.0 / sum;
		double bernoulli = sampler::uniform(0, 1);
		double stack = 0;
		for(int k = 0;k < num_customers_at_table.size();k++){
			stack += std::max(0.0, num_customers_at_table[k] - d_u) * normalizer;
			if(bernoulli <= stack){
				add_customer_to_table(token_id, k, g0, d_m, theta_m, added_to_table_k_of_root);
				if(update_beta_count){
					increment_stop_count();
				}
				if(_depth == 0){
					added_to_table_k_of_root = k;
				}
				return true;
			}
		}
		add_customer_to_new_table(token_id, g0, d_m, theta_m, added_to_table_k_of_root);
		if(update_beta_count){
			increment_stop_count();
		}
		if(_depth == 0){
			added_to_table_k_of_root = num_customers_at_table.size() - 1;
		}
		return true;
	}
	// NPYLMではルートノードのどのテーブルに代理客が追加されたかを知る必要がある
	// 再帰計算を防ぐため親の確率のキャッシュを使う
	bool add_customer(T token_id, double* parent_pw_at_depth, vector<double> &d_m, vector<double> &theta_m, bool update_beta_count, int &added_to_table_k_of_root){
		init_hyperparameters_at_depth_if_needed(_depth, d_m, theta_m);
		double d_u = d_m[_depth];
		double theta_u = theta_m[_depth];
		double parent_pw = parent_pw_at_depth[_depth];
		auto itr = _arrangement.find(token_id);
		if(itr == _arrangement.end()){
			add_customer_to_new_table(token_id, parent_pw_at_depth, d_m, theta_m, added_to_table_k_of_root);
			if(update_beta_count == true){
				increment_stop_count();
			}
			if(_depth == 0){	// ルートノードの場合
				added_to_table_k_of_root = 0;
			}
			return true;
		}

		vector<int> &num_customers_at_table = itr->second;
		double sum = 0;
		for(int k = 0;k < num_customers_at_table.size();k++){
			// 分母は定数なので無視
			sum += std::max(0.0, num_customers_at_table[k] - d_u);
		}
		double t_u = _num_tables;
		sum += (theta_u + d_u * t_u) * parent_pw;

		double normalizer = 1.0 / sum;
		double bernoulli = sampler::uniform(0, 1);
		double stack = 0;
		// 既存のテーブルのどこかに追加
		for(int k = 0;k < num_customers_at_table.size();k++){
			stack += std::max(0.0, num_customers_at_table[k] - d_u) * normalizer;
			if(bernoulli <= stack){
				add_customer_to_table(token_id, k, parent_pw_at_depth, d_m, theta_m, added_to_table_k_of_root);
				if(update_beta_count){
					increment_stop_count();
				}
				if(_depth == 0){	// ルートノードの場合
					added_to_table_k_of_root = k;
				}
				return true;
			}
		}
		// 新しいテーブルに追加
		add_customer_to_new_table(token_id, parent_pw_at_depth, d_m, theta_m, added_to_table_k_of_root);
		if(update_beta_count){
			increment_stop_count();
		}
		if(_depth == 0){
			added_to_table_k_of_root = num_customers_at_table.size() - 1;
		}
		return true;
	}
	// NPYLMではルートノードのどのテーブルから代理客が削除されたかを知る必要がある
	bool remove_customer(T token_id, bool update_beta_count, int &removed_from_table_k_of_root){
		auto itr = _arrangement.find(token_id);
		assert(itr != _arrangement.end());
		vector<int> &num_customers_at_table = itr->second;
		double sum = std::accumulate(num_customers_at_table.begin(), num_customers_at_table.end(), 0);		
		double normalizer = 1.0 / sum;
		double bernoulli = sampler::uniform(0, 1);
		double stack = 0;
		for(int k = 0;k < num_customers_at_table.size();k++){
			stack += num_customers_at_table[k] * normalizer;
			if(bernoulli <= stack){
				remove_customer_from_table(token_id, k, removed_from_table_k_of_root);
				if(update_beta_count == true){
					decrement_stop_count();
				}
				if(_depth == 0){
					removed_from_table_k_of_root = k;
				}
				return true;
			}
		}
		remove_customer_from_table(token_id, num_customers_at_table.size() - 1, removed_from_table_k_of_root);
		if(update_beta_count == true){
			decrement_stop_count();
		}
		if(_depth == 0){
			removed_from_table_k_of_root = num_customers_at_table.size() - 1;
		}
		return true;
	}
	// 再帰計算が含まれるので遅い
	double compute_Pw(T token_id, double g0, vector<double> &d_m, vector<double> &theta_m){
		init_hyperparameters_at_depth_if_needed(_depth, d_m, theta_m);
		double d_u = d_m[_depth];
		double theta_u = theta_m[_depth];
		double t_u = _num_tables;
		double c_u = _num_customers;
		auto itr = _arrangement.find(token_id);
		if(itr == _arrangement.end()){
			double coeff = (theta_u + d_u * t_u) / (theta_u + c_u);
			if(_parent != NULL){
				return _parent->compute_Pw(token_id, g0, d_m, theta_m) * coeff;
			}
			return g0 * coeff;
		}
		double parent_pw = g0;
		if(_parent != NULL){
			parent_pw = _parent->compute_Pw(token_id, g0, d_m, theta_m);
		}
		vector<int> &num_customers_at_table = itr->second;
		double c_uw = std::accumulate(num_customers_at_table.begin(), num_customers_at_table.end(), 0);
		double t_uw = num_customers_at_table.size();
		double first_term = std::max(0.0, c_uw - d_u * t_uw) / (theta_u + c_u);
		double second_coeff = (theta_u + d_u * t_u) / (theta_u + c_u);
		return first_term + second_coeff * parent_pw;
	}
	// 再帰計算を防ぐ
	double compute_Pw_with_parent_Pw(T token_id, double parent_pw, vector<double> &d_m, vector<double> &theta_m){
		init_hyperparameters_at_depth_if_needed(_depth, d_m, theta_m);
		double d_u = d_m[_depth];
		double theta_u = theta_m[_depth];
		double t_u = _num_tables;
		double c_u = _num_customers;
		auto itr = _arrangement.find(token_id);
		if(itr == _arrangement.end()){
			double coeff = (theta_u + d_u * t_u) / (theta_u + c_u);
			return parent_pw * coeff;
		}
		vector<int> &num_customers_at_table = itr->second;
		double c_uw = std::accumulate(num_customers_at_table.begin(), num_customers_at_table.end(), 0);
		double t_uw = num_customers_at_table.size();
		double first_term = std::max(0.0, c_uw - d_u * t_uw) / (theta_u + c_u);
		double second_coeff = (theta_u + d_u * t_u) / (theta_u + c_u);
		return first_term + second_coeff * parent_pw;
	}
	// VPYLM
	double stop_probability(double beta_stop, double beta_pass, bool recursive = true){
		double p = (_stop_count + beta_stop) / (_stop_count + _pass_count + beta_stop + beta_pass);
		if(recursive == false){
			return p;
		}
		if(_parent != NULL){
			p *= _parent->pass_probability(beta_stop, beta_pass);
		}
		return p;
	}
	// VPYLM
	double pass_probability(double beta_stop, double beta_pass, bool recursive = true){
		double p = (_pass_count + beta_pass) / (_stop_count + _pass_count + beta_stop + beta_pass);
		if(recursive == false){
			return p;
		}
		if(_parent != NULL){
			p *= _parent->pass_probability(beta_stop, beta_pass);
		}
		return p;
	}
	// VPYLM
	void increment_stop_count(){
		_stop_count++;
		if(_parent != NULL){
			_parent->increment_pass_count();
		}
	}
	// VPYLM
	void decrement_stop_count(){
		_stop_count--;
		assert(_stop_count >= 0);
		if(_parent != NULL){
			_parent->decrement_pass_count();
		}
	}
	// VPYLM
	void increment_pass_count(){
		_pass_count++;
		if(_parent != NULL){
			_parent->increment_pass_count();
		}
	}
	// VPYLM
	void decrement_pass_count(){
		_pass_count--;
		assert(_pass_count >= 0);
		if(_parent != NULL){
			_parent->decrement_pass_count();
		}
	}
	bool remove_from_parent(){
		if(_parent == NULL){
			return false;
		}
		_parent->delete_child_node(_token_id);
		return true;
	}
	void delete_child_node(T token_id){
		Node* child = find_child_node(token_id);
		if(child){
			_children.erase(token_id);
			delete child;
		}
		if(_children.size() == 0 && _arrangement.size() == 0){
			remove_from_parent();
		}
	}
	int get_max_depth(int base){
		int max_depth = base;
		for(auto &elem: _children){
			int depth = elem.second->get_max_depth(base + 1);
			if(depth > max_depth){
				max_depth = depth;
			}
		}
		return max_depth;
	}
	int get_num_nodes(){
		int num = _children.size();
		for(auto &elem: _children){
			num += elem.second->get_num_nodes();
		}
		return num;
	}
	int get_num_tables(){
		int num = 0;
		for(auto &elem: _arrangement){
			num += elem.second.size();
		}
		assert(num == _num_tables);
		for(auto &elem: _children){
			num += elem.second->get_num_tables();
		}
		return num;
	}
	int get_num_customers(){
		int num = 0;
		for(auto &elem: _arrangement){
			num += std::accumulate(elem.second.begin(), elem.second.end(), 0);
		}
		assert(num == _num_customers);
		for(auto &elem: _children){
			num += elem.second->get_num_customers();
		}
		return num;
	}
	int sum_pass_counts(){
		int sum = _pass_count;
		for(auto &elem: _children){
			sum += elem.second->sum_pass_counts();
		}
		return sum;
	}
	int sum_stop_counts(){
		int sum = _stop_count;
		for(auto &elem: _children){
			sum += elem.second->sum_stop_counts();
		}
		return sum;
	}
	void enumerate_nodes_at_depth(int depth, vector<Node*> &nodes){
		if(_depth == depth){
			nodes.push_back(this);
		}
		for(auto &elem: _children){
			elem.second->enumerate_nodes_at_depth(depth, nodes);
		}
	}
	// dとθの推定用
	// "A Bayesian Interpretation of Interpolated Kneser-Ney" Appendix C参照
	// http://www.gatsby.ucl.ac.uk/~ywteh/research/compling/hpylm.pdf
	double auxiliary_log_x_u(double theta_u){
		if(_num_customers >= 2){
			double x_u = sampler::beta(theta_u + 1, _num_customers - 1);
			return log(x_u + 1e-8);
		}
		return 0;
	}
	double auxiliary_y_ui(double d_u, double theta_u){
		if(_num_tables >= 2){
			double sum_y_ui = 0;
			for(int i = 1;i <= _num_tables - 1;i++){
				double denominator = theta_u + d_u * i;
				assert(denominator > 0);
				sum_y_ui += sampler::bernoulli(theta_u / denominator);;
			}
			return sum_y_ui;
		}
		return 0;
	}
	double auxiliary_1_y_ui(double d_u, double theta_u){
		if(_num_tables >= 2){
			double sum_1_y_ui = 0;
			for(int i = 1;i <= _num_tables - 1;i++){
				double denominator = theta_u + d_u * i;
				assert(denominator > 0);
				sum_1_y_ui += 1.0 - sampler::bernoulli(theta_u / denominator);
			}
			return sum_1_y_ui;
		}
		return 0;
	}
	double auxiliary_1_z_uwkj(double d_u){
		double sum_z_uwkj = 0;
		// c_u..
		for(auto elem: _arrangement){
			// c_uw.
			vector<int> &num_customers_at_table = elem.second;
			for(int k = 0;k < num_customers_at_table.size();k++){
				// c_uwk
				int c_uwk = num_customers_at_table[k];
				if(c_uwk >= 2){
					for(int j = 1;j <= c_uwk - 1;j++){
						assert(j - d_u > 0);
						sum_z_uwkj += 1 - sampler::bernoulli((j - 1) / (j - d_u));
					}
				}
			}
		}
		return sum_z_uwkj;
	}
	void init_hyperparameters_at_depth_if_needed(int depth, vector<double> &d_m, vector<double> &theta_m){
		if(depth >= d_m.size()){
			while(d_m.size() <= depth){
				d_m.push_back(HPYLM_INITIAL_D);
			}
			while(theta_m.size() <= depth){
				theta_m.push_back(HPYLM_INITIAL_THETA);
			}
		}
	}
	template <class Archive>
	void serialize(Archive& archive, unsigned int version)
	{
		archive & _children;
		archive & _arrangement;
		archive & _num_tables;
		archive & _num_customers;
		archive & _parent;
		archive & _stop_count;
		archive & _pass_count;
		archive & _token_id;
		archive & _depth;
	}
};
} // namespace npylm