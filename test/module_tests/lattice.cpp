#include  <iostream>
#include <chrono>
#include "../../src/npylm/sampler.h"
#include "../../src/python/model.h"
#include "../../src/python/dataset.h"
#include "../../src/python/dictionary.h"
#include "../../src/python/trainer.h"
using namespace npylm;
using std::wcout;
using std::cout;
using std::flush;
using std::endl;

double compute_forward_probability(Lattice* lattice, Sentence* sentence, bool normalize){
	assert(sentence->size() <= lattice->_max_sentence_length);
	int size = sentence->size() + 1;
	lattice->_alpha[0][0][0] = 1;
	lattice->_log_z[0] = 0;
	for(int i = 0;i < size;i++){
		for(int j = 0;j < lattice->_max_word_length + 1;j++){
			lattice->_substring_word_id_cache[i][j] = 0;
		}
	}
	for(int t = 0;t < size;t++){
		lattice->_log_z[t] = 0;
		for(int k = 0;k < lattice->_max_word_length + 1;k++){
			for(int j = 0;j < lattice->_max_word_length + 1;j++){
				lattice->_alpha[t][k][j] = -1;
			}
		}
	}
	lattice->forward_filtering(sentence, normalize);
	double sum_probability = 0;
	int t = sentence->size();
	for(int k = 1;k <= std::min(t, lattice->_max_word_length);k++){
		for(int j = 1;j <= std::min(t - k, lattice->_max_word_length);j++){
			if(normalize){
				sum_probability += lattice->_alpha[t][k][j] * exp(lattice->_log_z[t]);
			}else{
				sum_probability += lattice->_alpha[t][k][j];
			}
		}
	}
	return sum_probability;
}
void test_compute_forward_probability(){
	std::string filename = "../../dataset/test.txt";
	Corpus* corpus = new Corpus();
	corpus->add_textfile(filename);
	int seed = 0;
	Dataset* dataset = new Dataset(corpus, 1, seed);
	int max_word_length = 8;
	Model* model = new Model(dataset, max_word_length);
	Trainer* trainer = new Trainer(dataset, model, false);
	Lattice* lattice = model->_lattice;

	for(int epoch = 0;epoch < 20;epoch++){
		trainer->gibbs();
		for(Sentence* sentence: dataset->_sentence_sequences_train){
			double prob_n = compute_forward_probability(lattice, sentence, true);
			double prob_u = compute_forward_probability(lattice, sentence, false);
			assert(std::abs(prob_n - prob_u) < 1e-16);
		}
	}
}

int main(int argc, char *argv[]){
	test_compute_forward_probability();
	cout << "OK" << endl;
}