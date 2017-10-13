#pragma once
#include <boost/python.hpp>
#include "../ithmm/ithmm.h"
#include "dataset.h"
#include "dictionary.h"

namespace ithmm {
	class Model{
	private:
		void _set_locale();
	public:
		NPYLM* _npylm;
		Model(Dataset* dataset, int max_word_length);
		Model(std::string filename);
		~Model();
	};
}