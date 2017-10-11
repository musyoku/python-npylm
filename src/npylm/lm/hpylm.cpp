#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <cassert>
#include <fstream>
#include "hpylm.h"
#include "sampler.h"

namespace npylm {
	namespace lm {
		HPYLM::HPYLM(int ngram = 2){
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
		HPYLM::~HPYLM(){
			_delete_node(_root);
		}
		template <class Archive>
		void HPYLM::serialize(Archive& archive, unsigned int version)
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
	}
}