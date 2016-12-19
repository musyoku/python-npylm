#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/python.hpp>
#include <boost/format.hpp>
#include "npylm/core/npylm.h"
#include "npylm/core/vocab.h"
#include "npylm/core/chartype.h"
#include "npylm/lattice/trigram.h"
#include "npylm/util.h"

using namespace boost;

template<class T>
python::list list_from_vector(vector<T> &vec){  
	 python::list list;
	 typename vector<T>::const_iterator it;

	 for(it = vec.begin(); it != vec.end(); ++it)   {
		  list.append(*it);
	 }
	 return list;
}
// Pythonラッパー
class PyNPYLM{
private:
	HPYLM* _hpylm;
	VPYLM* _vpylm;
	NPYLM3* _npylm;
	TrigramLattice* _lattice;
	Vocab* _vocab;
	vector<wstring> _dataset;
	bool _is_ready;

public:
	PyNPYLM(){
		c_printf("[*]%s\n", "NPYLMを初期化しています ...");
		_hpylm = new HPYLM(3);
		_vpylm = new VPYLM();
		_vocab = new Vocab();
		_npylm = new NPYLM3(_hpylm, _vpylm, _vocab);
		_lattice = new TrigramLattice(_npylm, _vocab);
		_is_ready = false;
	}
	void load_textfile(string filename){
		load_characters_in_textfile(filename, _dataset);
		// 全文字種を特定
		int max_sentence_length = 0;
		int sum_sentence_length = 0;
		for(int i = 0;i < dataset.size();i++){
			wstring &sentence = dataset[i];
			sum_sentence_length += sentence.size();
			for(int l = 0;l < sentence.size();l++){
				vocab->add_character(sentence[l]);
			}
			if(sentence.size() > max_sentence_length){
				max_sentence_length = sentence.size();
			}
		}
		double average_sentence_length = sum_sentence_length / (double)dataset.size();
		c_printf("[*]%s\n", (boost::format("を読み込みました. (%d行, 平均文長%lf, 最大文長%d)") % filename % dataset.size() % average_sentence_length % max_sentence_length).str().c_str());
		double g0 = 1.0 / vocab->get_num_characters();
		set_g0(g0);
	}
	void set_g0(double g0){
		_vpylm->_g0 = g0;
		c_printf("[*]%s\n", (boost::format("g0 <- %lf") % g0).str().c_str());
		_is_ready = true;
	}
	python::list perform_gibbs_sampling(python::list &sentence, python::list &prev_depth){
		std::vector<id> token_ids;
		int len = python::len(sentence);
		for(int i = 0;i < len;i++) {
			token_ids.push_back(python::extract<id>(sentence[i]));
		}
		if(python::len(prev_depth) != token_ids.size()){
			c_printf("[r]%s [*]%s\n", "エラー:", "prev_ordersとword_idsの長さが違います.");
		}
		for(int token_t_index = 0;token_t_index < token_ids.size();token_t_index++){
			int depth_t = python::extract<int>(prev_depth[token_t_index]);
			if(depth_t != -1){
				vpylm->remove_customer_at_timestep(token_ids, token_t_index, depth_t);
			}
		}				
		vector<int> new_depth;
		for(int token_t_index = 0;token_t_index < token_ids.size();token_t_index++){
			int depth_t = vpylm->sample_depth_at_timestep(token_ids, token_t_index);
			vpylm->add_customer_at_timestep(token_ids, token_t_index, depth_t);
			new_depth.push_back(depth_t);
		}
		return list_from_vector(new_depth);
	}
};

BOOST_PYTHON_MODULE(vpylm){
	python::class_<PyNPYLM>("vpylm")
	.def("set_g0", &PyNPYLM::set_g0)
	.def("load_textfile", &PyNPYLM::load_textfile)
}