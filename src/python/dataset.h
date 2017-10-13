#pragma once
#include <unordered_set>
#include <vector>
#include "../npylm/sentence.h"
#include "corpus.h"

namespace npylm {
	class Dataset{
	private:
		Corpus* _corpus;
		void _add_words_to_dataset(std::wstring &sentence_str, std::vector<Sentence*> &dataset);
	public:
		Dictionary* _dict;
		std::vector<Sentence*> _sentence_sequences_train;
		std::vector<Sentence*> _sentence_sequences_dev;
		Dataset(Corpus* corpus, double train_split, int seed);
		~Dataset();
		int get_max_sentence_length();
		Dictionary &get_dict_obj();
	};
}