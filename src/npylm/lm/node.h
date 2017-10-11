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

namespace npylm {
	namespace lm {
		template<typename T>
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
		T _token_id;								// このノードに割り当てられた単語ID（または文字ID）

		Node(T token_id = 0);
		bool parent_exists();
		bool child_exists(T token_id);
		bool need_to_remove_from_parent();
		int get_num_tables_serving_word(T token_id);
		int get_num_customers_eating_word(T token_id);
		Node* find_child_node(T token_id, bool generate_if_not_exist = false);
		bool add_customer_to_table(T token_id, int table_k, double g0, vector<double> &d_m, vector<double> &theta_m, int &added_to_table_k_of_root);
		bool add_customer_to_table(T token_id, int table_k, double* parent_pw_at_depth, vector<double> &d_m, vector<double> &theta_m, int &added_to_table_k_of_root);
		bool add_customer_to_new_table(T token_id, double g0, vector<double> &d_m, vector<double> &theta_m, int &added_to_table_k_of_root);
		bool add_customer_to_new_table(T token_id, double* parent_pw_at_depth, vector<double> &d_m, vector<double> &theta_m, int &added_to_table_k_of_root);
		void _add_customer_to_new_table(T token_id);
		bool remove_customer_from_table(T token_id, int table_k, int &removed_from_table_k_of_root);
		bool add_customer(T token_id, double g0, vector<double> &d_m, vector<double> &theta_m, bool update_beta_count, int &added_to_table_k_of_root);
		bool add_customer(T token_id, double* parent_pw_at_depth, vector<double> &d_m, vector<double> &theta_m, bool update_beta_count, int &added_to_table_k_of_root);
		bool remove_customer(T token_id, bool update_beta_count, int &removed_from_table_k_of_root);
		double compute_Pw(T token_id, double g0, vector<double> &d_m, vector<double> &theta_m);
		double compute_Pw_with_parent_Pw(T token_id, double parent_pw, vector<double> &d_m, vector<double> &theta_m);
		double stop_probability(double beta_stop, double beta_pass, bool recursive = true);
		double pass_probability(double beta_stop, double beta_pass, bool recursive = true);
		void increment_stop_count();
		void decrement_stop_count();
		void increment_pass_count();
		void decrement_pass_count();
		bool remove_from_parent();
		void delete_child_node(T token_id);
		int get_max_depth(int base);
		int get_num_nodes();
		int get_num_tables();
		int get_num_customers();
		int sum_pass_counts();
		int sum_stop_counts();
		void enumerate_nodes_at_depth(int depth, vector<Node*> &nodes);
		double auxiliary_log_x_u(double theta_u);
		double auxiliary_y_ui(double d_u, double theta_u);
		double auxiliary_1_y_ui(double d_u, double theta_u);
		double auxiliary_1_z_uwkj(double d_u);
		void init_hyperparameters_at_depth_if_needed(int depth, vector<double> &d_m, vector<double> &theta_m);
		template <class Archive>
		void serialize(Archive& archive, unsigned int version);
	} // namespace lm
} // namespace npylm