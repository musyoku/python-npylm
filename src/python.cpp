#include "python/model.h"
#include "python/dataset.h"
#include "python/dictionary.h"
#include "python/trainer.h"

using namespace ithmm;

BOOST_PYTHON_MODULE(ithmm){
	boost::python::class_<Dictionary>("dictionary")
	.def("string_to_word_id", &Dictionary::string_to_word_id)
	.def("is_id_unk", &Dictionary::is_id_unk)
	.def("is_string_unk", &Dictionary::is_string_unk)
	.def("save", &Dictionary::save)
	.def("load", &Dictionary::load);

	boost::python::class_<Corpus>("corpus")
	.def("add_words", &Corpus::python_add_words);

	boost::python::class_<Dataset>("dataset", boost::python::init<Corpus*, double, int, int>())
	.def("get_num_words", &Dataset::get_num_words)
	.def("get_dict", &Dataset::get_dict_obj, boost::python::return_internal_reference<>());

	boost::python::class_<Trainer>("trainer", boost::python::init<Dataset*, Model*>())
	.def("compute_log_p_dataset_train", &Trainer::compute_log_p_dataset_train)
	.def("compute_log_p_dataset_dev", &Trainer::compute_log_p_dataset_dev)
	.def("update_hyperparameters", &Trainer::update_hyperparameters)
	.def("gibbs", &Trainer::gibbs);

	boost::python::class_<Model>("model", boost::python::init<Dataset*, int>())
	.def(boost::python::init<std::string>())
	.def("get_tags", &Model::python_get_tags)
	.def("viterbi_decode", &Model::python_viterbi_decode)
	.def("update_hyperparameters", &Model::update_hyperparameters)
	.def("save", &Model::save)
	.def("load", &Model::load)
	.def("get_alpha", &Model::get_alpha)
	.def("get_gamma", &Model::get_gamma)
	.def("get_lambda_alpha", &Model::get_lambda_alpha)
	.def("get_lambda_gamma", &Model::get_lambda_gamma)
	.def("get_concentration_v", &Model::get_concentration_v)
	.def("get_concentration_h", &Model::get_concentration_h)
	.def("get_tau0", &Model::get_tau0)
	.def("get_tau1", &Model::get_tau1)
	.def("set_alpha", &Model::set_alpha)
	.def("set_gamma", &Model::set_gamma)
	.def("set_lambda_alpha", &Model::set_lambda_alpha)
	.def("set_lambda_gamma", &Model::set_lambda_gamma)
	.def("set_concentration_v", &Model::set_concentration_v)
	.def("set_concentration_h", &Model::set_concentration_h)
	.def("set_tau0", &Model::set_tau0)
	.def("set_tau1", &Model::set_tau1)
	.def("show_hpylm_for_each_tag", &Model::show_hpylm_for_each_tag)
	.def("show_sticks", &Model::show_sticks)
	.def("show_assigned_words_for_each_tag", &Model::show_assigned_words_for_each_tag)
	.def("show_assigned_words_and_probability_for_each_tag", &Model::show_assigned_words_and_probability_for_each_tag);
}