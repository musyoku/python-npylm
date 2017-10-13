#pragma once
#include <vector>

namespace npylm {
	class Corpus{
	public:
		std::vector<std::wstring> _sentence_str_list;
		Corpus(){}
		void add_textfile(std::string filename);
		void add_sentence(std::wstring sentence_str);
		int get_num_sentences();
	};
}