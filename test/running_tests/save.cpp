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
void compare_node(Node<T>* a, Node<T>* b){
	assert(a->_num_tables == b->_num_tables);
	assert(a->_num_customers == b->_num_customers);
	assert(a->_stop_count == b->_stop_count);
	assert(a->_pass_count == b->_pass_count);
	assert(a->_depth == b->_depth);
	assert(a->_token_id == b->_token_id);
	assert(a->_arrangement.size() == b->_arrangement.size());
	for(int i = 0;i < a->_arrangement.size();i++){
		assert(a->_arrangement[i] == b->_arrangement[i]);
	}
	assert(a->_children.size() == b->_children.size());
	for(int i = 0;i < a->_children.size();i++){
		compare_node(a->_children[i], b->_children[i]);
	}
}

void compare_npylm(NPYLM* a, NPYLM* b){
	compare_node(a->_hpylm->_root, b->_hpylm->_root);
	compare_node(a->_vpylm->_root, b->_vpylm->_root)
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
		trainer->gibbs();
		trainer->sample_hpylm_vpylm_hyperparameters();
		trainer->sample_lambda();
		model->save("npylm.model");
		Model* _model = new Model("npylm.model");
		compare_npylm(model->_npylm, _model->_npylm);
		delete _model;
	}
}