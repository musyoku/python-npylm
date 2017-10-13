#include  <iostream>
#include "../../src/npylm/sampler.h"
#include "../../src/python/model.h"
#include "../../src/python/dataset.h"
#include "../../src/python/dictionary.h"
#include "../../src/python/trainer.h"
using namespace npylm;
using std::cout;
using std::flush;
using std::endl;

int main(int argc, char *argv[]){
	std::string filename = "../../dataset/test.txt";
	Corpus* corpus = new Corpus();
	corpus->add_textfile(filename);
	int seed = 0;
	Dataset* dataset = new Dataset(corpus, 0.9, seed);
	int max_word_length = 16;
	Model* model = new Model(dataset, max_word_length);
	Dictionary* dictionary = dataset->_dict;
	dictionary->save("npylm.dict");
	Trainer* trainer = new Trainer(dataset, model, false);

	for(int epoch = 0;epoch < 1000;epoch++){
		trainer->gibbs();
		trainer->sample_hpylm_vpylm_hyperparameters();
		trainer->sample_lambda();
		if(epoch % 10 == 0){
			trainer->print_segmentation_train(10);
		}
	}
}