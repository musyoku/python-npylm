#pragma once
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <vector>
#include <cassert>
#include <fstream>
#include "common.h"
#include "sampler.h"
#include "node.h"

namespace npylm{
	template<class T>
	class PYLM{
	public:
		Node<T>* _root;				// 文脈木のルートノード
		int _depth;					// 最大の深さ. HPYLMは固定
		double _g0;					// ゼログラム確率
		// 深さmのノードに関するパラメータ
		vector<double> _d_m;		// Pitman-Yor過程のディスカウント係数
		vector<double> _theta_m;	// Pitman-Yor過程の集中度
		// "A Bayesian Interpretation of Interpolated Kneser-Ney" Appendix C参照
		// http://www.gatsby.ucl.ac.uk/~ywteh/research/compling/hpylm.pdf
		vector<double> _a_m;		// ベータ分布のパラメータ	dの推定用
		vector<double> _b_m;		// ベータ分布のパラメータ	dの推定用
		vector<double> _alpha_m;	// ガンマ分布のパラメータ	θの推定用
		vector<double> _beta_m;		// ガンマ分布のパラメータ	θの推定用
		void _delete_node(Node<T>* node){
			for(auto &elem: node->_children){
				Node<T>* child = elem.second;
				_delete_node(child);
			}
			delete node;
		}
		int get_num_nodes(){
			return _root->get_num_nodes() + 1;
		}
		int get_num_customers(){
			return _root->get_num_customers();
		}
		int get_num_tables(){
			return _root->get_num_tables();
		}
		int get_sum_stop_counts(){
			return _root->sum_stop_counts();
		}
		int get_sum_pass_counts(){
			return _root->sum_pass_counts();
		}
		void set_g0(double g0){
			_g0 = g0;
		}
		void init_hyperparameters_at_depth_if_needed(int depth){
			if(depth >= _d_m.size()){
				while(_d_m.size() <= depth){
					_d_m.push_back(HPYLM_INITIAL_D);
				}
			}
			if(depth >= _theta_m.size()){
				while(_theta_m.size() <= depth){
					_theta_m.push_back(HPYLM_INITIAL_THETA);
				}
			}
			if(depth >= _a_m.size()){
				while(_a_m.size() <= depth){
					_a_m.push_back(HPYLM_INITIAL_A);
				}
			}
			if(depth >= _b_m.size()){
				while(_b_m.size() <= depth){
					_b_m.push_back(HPYLM_INITIAL_B);
				}
			}
			if(depth >= _alpha_m.size()){
				while(_alpha_m.size() <= depth){
					_alpha_m.push_back(HPYLM_INITIAL_ALPHA);
				}
			}
			if(depth >= _beta_m.size()){
				while(_beta_m.size() <= depth){
					_beta_m.push_back(HPYLM_INITIAL_BETA);
				}
			}
		}
		// "A Bayesian Interpretation of Interpolated Kneser-Ney" Appendix C参照
		// http://www.gatsby.ucl.ac.uk/~ywteh/research/compling/hpylm.pdf
		void sum_auxiliary_variables_recursively(Node<T>* node, vector<double> &sum_log_x_u_m, vector<double> &sum_y_ui_m, vector<double> &sum_1_y_ui_m, vector<double> &sum_1_z_uwkj_m, int &bottom){
			for(auto elem: node->_children){
				Node<T>* child = elem.second;
				int depth = child->_depth;

				if(depth > bottom){
					bottom = depth;
				}
				init_hyperparameters_at_depth_if_needed(depth);

				double d = _d_m[depth];
				double theta = _theta_m[depth];
				sum_log_x_u_m[depth] += child->auxiliary_log_x_u(theta);	// log(x_u)
				sum_y_ui_m[depth] += child->auxiliary_y_ui(d, theta);		// y_ui
				sum_1_y_ui_m[depth] += child->auxiliary_1_y_ui(d, theta);	// 1 - y_ui
				sum_1_z_uwkj_m[depth] += child->auxiliary_1_z_uwkj(d);		// 1 - z_uwkj

				sum_auxiliary_variables_recursively(child, sum_log_x_u_m, sum_y_ui_m, sum_1_y_ui_m, sum_1_z_uwkj_m, bottom);
			}
		}
		// dとθの推定
		void sample_hyperparams(){
			int max_depth = _d_m.size() - 1;

			// 親ノードの深さが0であることに注意
			vector<double> sum_log_x_u_m(max_depth + 1, 0.0);
			vector<double> sum_y_ui_m(max_depth + 1, 0.0);
			vector<double> sum_1_y_ui_m(max_depth + 1, 0.0);
			vector<double> sum_1_z_uwkj_m(max_depth + 1, 0.0);

			// _root
			sum_log_x_u_m[0] = _root->auxiliary_log_x_u(_theta_m[0]);			// log(x_u)
			sum_y_ui_m[0] = _root->auxiliary_y_ui(_d_m[0], _theta_m[0]);		// y_ui
			sum_1_y_ui_m[0] = _root->auxiliary_1_y_ui(_d_m[0], _theta_m[0]);	// 1 - y_ui
			sum_1_z_uwkj_m[0] = _root->auxiliary_1_z_uwkj(_d_m[0]);				// 1 - z_uwkj

			// それ以外
			_depth = 0;
			// __depthは以下を実行すると更新される
			// HPYLMでは無意味だがVPYLMで最大深さを求める時に使う
			sum_auxiliary_variables_recursively(_root, sum_log_x_u_m, sum_y_ui_m, sum_1_y_ui_m, sum_1_z_uwkj_m, _depth);
			init_hyperparameters_at_depth_if_needed(_depth);

			for(int u = 0;u <= _depth;u++){
				_d_m[u] = sampler::beta(_a_m[u] + sum_1_y_ui_m[u], _b_m[u] + sum_1_z_uwkj_m[u]);
				_theta_m[u] = sampler::gamma(_alpha_m[u] + sum_y_ui_m[u], _beta_m[u] - sum_log_x_u_m[u]);
			}
			// 不要な深さのハイパーパラメータを削除
			int num_remove = _d_m.size() - _depth - 1;
			for(int n = 0;n < num_remove;n++){
				_d_m.pop_back();
				_theta_m.pop_back();
				_a_m.pop_back();
				_b_m.pop_back();
				_alpha_m.pop_back();
				_beta_m.pop_back();
			}
		}
	};
	class HPYLM: public PYLM<id>{
	public:
		HPYLM(int ngram = 2){
			// 深さは0から始まることに注意
			// 2-gramなら最大深さは1. root(0) -> 2-gram(1)
			// 3-gramなら最大深さは2. root(0) -> 2-gram(1) -> 3-gram(2)
			_depth = ngram - 1;

			_root = new Node<id>(0);
			_root->_depth = 0;	// ルートは深さ0

			for(int n = 0;n < ngram;n++){
				_d_m.push_back(HPYLM_INITIAL_D);	
				_theta_m.push_back(HPYLM_INITIAL_THETA);
				_a_m.push_back(HPYLM_INITIAL_A);	
				_b_m.push_back(HPYLM_INITIAL_B);	
				_alpha_m.push_back(HPYLM_INITIAL_ALPHA);
				_beta_m.push_back(HPYLM_INITIAL_BETA);
			}
		}
		~HPYLM(){
			_delete_node(_root);
		}
		template <class Archive>
		void serialize(Archive& archive, unsigned int version)
		{
			archive & _root;
			archive & _depth;
			archive & _g0;
			archive & _d_m;
			archive & _theta_m;
			archive & _a_m;
			archive & _b_m;
			archive & _alpha_m;
			archive & _beta_m;
		}
	};
} // namespace npylm