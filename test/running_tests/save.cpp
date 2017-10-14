#include  <iostream>
#include <chrono>
#include "../../src/npylm/sampler.h"
#include "../../src/python/model.h"
#include "../../src/python/dataset.h"
#include "../../src/python/dictionary.h"
#include "../../src/python/trainer.h"
using namespace npylm;
using std::cout;
using std::flush;
using std::endl;

template<typename T>
void compare_node(lm::Node<T>* a, lm::Node<T>* b){
	assert(a->_num_tables == b->_num_tables);
	assert(a->_num_customers == b->_num_customers);
	assert(a->_stop_count == b->_stop_count);
	assert(a->_pass_count == b->_pass_count);
	assert(a->_depth == b->_depth);
	assert(a->_token_id == b->_token_id);
	assert(a->_arrangement.size() == b->_arrangement.size());
	for(auto elem: a->_arrangement){
		T key = elem.first;
		std::vector<int> &table_a = elem.second;
		std::vector<int> &table_b = b->_arrangement[key];
		assert(table_a.size() == table_b.size());
	}
	for(auto elem: a->_children){
		T key = elem.first;
		lm::Node<T>* children_a = elem.second;
		lm::Node<T>* children_b = b->_children[key];
		compare_node(children_a, children_b);
	}
}

void compare_npylm(NPYLM* a, NPYLM* b){
	assert(a != NULL);
	assert(b != NULL);
	compare_node(a->_hpylm->_root, b->_hpylm->_root);
	compare_node(a->_vpylm->_root, b->_vpylm->_root);
}

int main(int argc, char *argv[]){
	std::string filename = "../../dataset/test.txt";
	Corpus* corpus = new Corpus();
	corpus->add_textfile(filename);
	int seed = 0;
	Dataset* dataset = new Dataset(corpus, 1, seed);
	int max_word_length = 8;
	Model* model = new Model(dataset, max_word_length);
	Dictionary* dictionary = dataset->_dict;
	dictionary->save("npylm.dict");
	Trainer* trainer = new Trainer(dataset, model, false);

	for(int epoch = 0;epoch < 1000;epoch++){
		cout << "\r" << epoch << flush;
		trainer->gibbs();
		trainer->sample_hpylm_vpylm_hyperparameters();
		trainer->sample_lambda();
		model->save("npylm.model");
		Model* _model = new Model("npylm.model");
		compare_npylm(model->_npylm, _model->_npylm);
		delete _model;
	}
}