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
	.def("get_dict", &Dataset::get_dict_obj, boost::python::return_internal_reference<>());

	boost::python::class_<Trainer>("trainer", boost::python::init<Dataset*, Model*, bool>((arg("dataset"), arg("model"), arg("always_accept_new_segmentation")=true)))
	.def("gibbs", &Trainer::gibbs);

	boost::python::class_<Model>("model", boost::python::init<Dataset*, int, double, double, double, double>())
	.def(boost::python::init<std::string>())
	.def("save", &Model::save)
	.def("load", &Model::load);
}