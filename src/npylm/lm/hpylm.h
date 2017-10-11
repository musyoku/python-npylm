#pragma once
#include <boost/serialization/serialization.hpp>
#include <vector>
#include "common.h"
#include "node.h"

namespace npylm {
	namespace lm {
		class HPYLM: public PYLM<id>{
		private:
			friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive& archive, unsigned int version);
		public:
			HPYLM(int ngram = 2);
			~HPYLM();
		};
	}
}