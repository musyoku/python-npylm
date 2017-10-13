#pragma once
#include <unordered_set>

namespace npylm {
	class Dictionary{
	public:
		std::unordered_set<wchar_t> _all_characters;	// すべての文字
		Dictionary(){}
		void add_character(wchar_t character);
		int get_num_characters();
		bool load(std::string filename);
		bool save(std::string filename);
	};
}