#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <fstream>
#include "dictionary.h"

namespace npylm {
	void Dictionary::add_character(wchar_t character){
		_all_characters.insert(character);
	}
	int Dictionary::get_num_characters(){
		return _all_characters.size();
	}
	bool Dictionary::load(std::string filename){
		std::string dictionary_filename = filename;
		std::ifstream ifs(dictionary_filename);
		if(ifs.good()){
			boost::archive::binary_iarchive iarchive(ifs);
			iarchive >> _all_characters;
			ifs.close();
			return true;
		}
		ifs.close();
		return false;
	}
	bool Dictionary::save(std::string filename){
		std::ofstream ofs(filename);
		boost::archive::binary_oarchive oarchive(ofs);
		oarchive << _all_characters;
		ofs.close();
		return true;
	}
}