#pragma once
#include <boost/python.hpp>
#include <vector>

namespace npylm {
	class Corpus{
	private:
		void _before_add_true_segmentation(boost::python::list &py_word_str_list, std::vector<std::wstring> &word_str_vec);
	public:
		std::vector<std::wstring> _sentence_str_list;
		std::vector<std::vector<std::wstring>> _word_sequence_list;
		Corpus(){}
		void add_textfile(std::string filename);
		void add_sentence(std::wstring sentence_str);
		void add_true_segmentation(std::vector<std::wstring> &word_str_vec);		// 正解の分割を追加する
		void python_add_true_segmentation(boost::python::list py_word_str_list);
		int get_num_sentences();
		int get_num_true_segmentations();
	};
}