#pragma once
#include <unordered_set>
#include <vector>
#include "../npylm/common.h"
#include "../npylm/sentence.h"
#include "corpus.h"
#include "dictionary.h"

namespace npylm {
	class Dataset{
	private:
		Corpus* _corpus;
		void _add_words_to_dataset(std::wstring &sentence_str, std::vector<Sentence*> &dataset);
		void _detect_collision_of_sentence(Sentence* sentence, hashmap<id, std::wstring> &pool, int max_word_length);
	public:
		int _max_sentence_length;
		int _avg_sentence_length;
		Dictionary* _dict;
		std::vector<Sentence*> _sentence_sequences_train;
		std::vector<Sentence*> _sentence_sequences_dev;
		Dataset(Corpus* corpus, double train_split, int seed);
		~Dataset();
		int get_max_sentence_length();
		int get_average_sentence_length();
		int detect_hash_collision(int max_word_length);
		Dictionary &get_dict_obj();
	};
}