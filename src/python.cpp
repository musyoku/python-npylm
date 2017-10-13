#include "python/model.h"
#include "python/dataset.h"
#include "python/dictionary.h"
#include "python/trainer.h"

using namespace npylm;
using boost::python::arg;

BOOST_PYTHON_MODULE(npylm){
	boost::python::class_<Dictionary>("dictionary")
	.def("save", &Dictionary::save)
	.def("load", &Dictionary::load);

	boost::python::class_<Corpus>("corpus")
	.def("add_textfile", &Corpus::add_textfile)
	.def("add_sentence", &Corpus::add_sentence);

	boost::python::class_<Dataset>("dataset", boost::python::init<Corpus*, double, int>())
	.def("get_max_sentence_length", &Dataset::get_max_sentence_length)
	.def("detect_hash_collision", &Dataset::detect_hash_collision)
	.def("get_dict", &Dataset::get_dict_obj, boost::python::return_internal_reference<>());

	boost::python::class_<Trainer>("trainer", boost::python::init<Dataset*, Model*, bool>((arg("dataset"), arg("model"), arg("always_accept_new_segmentation")=true)))
	.def("sample_hpylm_vpylm_hyperparameters", &Trainer::sample_hpylm_vpylm_hyperparameters)
	.def("sample_lambda", &Trainer::sample_lambda)
	.def("gibbs", &Trainer::gibbs);

	boost::python::class_<Model>("model", boost::python::init<Dataset*, int>())
	.def(boost::python::init<std::string>())
	.def("set_initial_lambda_a", &Model::set_initial_lambda_a)
	.def("set_initial_lambda_b", &Model::set_initial_lambda_b)
	.def("set_vpylm_beta_stop", &Model::set_vpylm_beta_stop)
	.def("set_vpylm_beta_pass", &Model::set_vpylm_beta_pass)
	.def("save", &Model::save)
	.def("load", &Model::load);
}