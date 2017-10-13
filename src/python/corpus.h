#pragma once
#include <vector>

namespace npylm {
	class Corpus{
	private:
		int _max_sentence_length;
	public:
		std::vector<std::wstring> _sentence_str_list;
		Corpus(){
			_max_sentence_length = 0
		}
		void add_textfile(std::string filename);
		void add_sentence(std::wstring sentence);
		int get_num_sentences();
		int get_max_sentence_length();
	};
}