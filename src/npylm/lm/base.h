#pragma once
#include <boost/serialization/serialization.hpp>
#include <vector>
#include "common.h"
#include "node.h"

namespace npylm {
	namespace lm {
		template<typename T>
		class Base{
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
			void _delete_node(Node<T>* node);
			int get_num_nodes();
			int get_num_customers();
			int get_num_tables();
			int get_sum_stop_counts();
			int get_sum_pass_counts();
			void set_g0(double g0);
			void init_hyperparameters_at_depth_if_needed(int depth);
			void sum_auxiliary_variables_recursively(Node<T>* node, vector<double> &sum_log_x_u_m, vector<double> &sum_y_ui_m, vector<double> &sum_1_y_ui_m, vector<double> &sum_1_z_uwkj_m, int &bottom);
			void sample_hyperparams();
		};
	} // namespace lm
} // namespace npylm