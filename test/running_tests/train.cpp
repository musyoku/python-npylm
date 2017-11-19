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

void run_training_loop(){
	std::string filename = "../../dataset/test.txt";
	Corpus* corpus = new Corpus();
	corpus->add_textfile(filename);
	int seed = 0;
	Dataset* dataset = new Dataset(corpus, 0.95, seed);
	int max_word_length = 12;
	Model* model = new Model(dataset, max_word_length);
	Dictionary* dictionary = dataset->_dict;
	dictionary->save("npylm.dict");
	Trainer* trainer = new Trainer(dataset, model, true);

	for(int epoch = 1;epoch <= 200;epoch++){
	    auto start_time = std::chrono::system_clock::now();
		trainer->gibbs();
	    auto diff = std::chrono::system_clock::now() - start_time;
	    cout << (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() / 1000.0) << endl;
		trainer->sample_hpylm_vpylm_hyperparameters();
		trainer->sample_lambda();
		if(epoch > 3){
			trainer->update_p_k_given_vpylm();
		}
		if(epoch % 10 == 0){
			trainer->print_segmentation_train(10);
			cout << "ppl: " << trainer->compute_perplexity_train() << endl;
			trainer->print_segmentation_dev(10);
			cout << "ppl: " << trainer->compute_perplexity_dev() << endl;
		}
	}
}

int main(int argc, char *argv[]){
	for(int i = 0;i < 10;i++){
		run_training_loop();
	}
}