#pragma once
#include <boost/serialization/serialization.hpp>
#include <vector>
#include <unordered_map> 
#include "../sentence.h"
#include "../common.h"
#include "model.h"
#include "node.h"

namespace npylm {
	namespace lm {
		class VPYLM: public Model<wchar_t> {
		private:
			friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive& archive, unsigned int version);
		public:
			double _beta_stop;		// 停止確率q_iのベータ分布の初期パラメータ
			double _beta_pass;		// 停止確率q_iのベータ分布の初期パラメータ
			int _max_depth;
			// 計算高速化用
			double* _sampling_table;
			double* _parent_pw_cache;
			Node<wchar_t>** _path_nodes;
			VPYLM(){}
			VPYLM(double g0, int max_possible_depth, double beta_stop, double beta_pass);
			~VPYLM();
			bool add_customer_at_time_t(wchar_t const* character_ids, int t, int depth_t);
			bool add_customer_at_time_t(wchar_t const* character_ids, int t, int depth_t, double* parent_pw_cache, Node<wchar_t>** path_nodes);
			bool remove_customer_at_time_t(wchar_t const* character_ids, int t, int depth_t);
			Node<wchar_t>* find_node_by_tracing_back_context(wchar_t const* character_ids, int t, int depth_t, bool generate_node_if_needed = false, bool return_middle_node = false);
			Node<wchar_t>* find_node_by_tracing_back_context(wchar_t const* character_ids, int t, int depth_t, double* parent_pw_cache);
			Node<wchar_t>* find_node_by_tracing_back_context(wchar_t const* character_ids, int t, int depth_t, Node<wchar_t>** path_nodes_cache);
			double compute_p_w(wchar_t const* character_ids, int character_ids_length);
			double compute_log_p_w(wchar_t const* character_ids, int character_ids_length);
			double compute_p_w_given_h(wchar_t const* character_ids, int context_substr_start, int context_substr_end);
			double compute_p_w_given_h(wchar_t target_id, wchar_t const* character_ids, int context_substr_start, int context_substr_end);
			int sample_depth_at_time_t(wchar_t const* character_ids, int t, double* parent_pw_cache, Node<wchar_t>** path_nodes);
		};
	}
} // namespace npylm