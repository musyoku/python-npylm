#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <cassert>
#include <fstream>
#include <vector>
#include "../sampler.h"
#include "hpylm.h"

namespace npylm {
	namespace lm {
		HPYLM::HPYLM(int ngram){
			// 深さは0から始まることに注意
			// 2-gramなら最大深さは1. root(0) -> 2-gram(1)
			// 3-gramなら最大深さは2. root(0) -> 2-gram(1) -> 3-gram(2)
			_depth = ngram - 1;

			_root = new Node<id>(0);
			_root->_depth = 0;	// ルートは深さ0

			for(int n = 0;n < ngram;n++){
				_d_m.push_back(HPYLM_INITIAL_D);	
				_theta_m.push_back(HPYLM_INITIAL_THETA);
				_a_m.push_back(_pylm_beta_a);	
				_b_m.push_back(_pylm_beta_b);	
				_alpha_m.push_back(_pylm_gamma_alpha);
				_beta_m.push_back(_pylm_gamma_beta);
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
		template void HPYLM::serialize(boost::archive::binary_iarchive &ar, unsigned int version);
		template void HPYLM::serialize(boost::archive::binary_oarchive &ar, unsigned int version);

        void HPYLM::initialize(){
            _d_m.clear();
            _theta_m.clear();
            _a_m.clear();
            _b_m.clear();
            _alpha_m.clear();
            _beta_m.clear();

            _d_m.shrink_to_fit();
            _theta_m.shrink_to_fit();
            _a_m.shrink_to_fit();
            _b_m.shrink_to_fit();
            _alpha_m.shrink_to_fit();
            _beta_m.shrink_to_fit();
            
            for(int n = 0;n < _depth + 1;n++){
                _d_m.push_back(HPYLM_INITIAL_D);	
                _theta_m.push_back(HPYLM_INITIAL_THETA);
                _a_m.push_back(_pylm_beta_a);	
                _b_m.push_back(_pylm_beta_b);	
                _alpha_m.push_back(_pylm_gamma_alpha);
                _beta_m.push_back(_pylm_gamma_beta);
            }
        }
	}
}