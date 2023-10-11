#include "model.h"
#include "../npylm/common.h"
#include "../npylm/wordtype.h"
#include <iostream>

namespace npylm {
Model::Model(Dataset* dataset, int max_word_length)
{
    _set_locale();
    int max_sentence_length = dataset->get_max_sentence_length();
    double vpylm_g0 = 1.0 / (double)dataset->_dict->get_num_characters();
    _npylm = new NPYLM(max_word_length, max_sentence_length, vpylm_g0, 4, 1, VPYLM_BETA_STOP, VPYLM_BETA_PASS);
    _lattice = new Lattice(_npylm, max_word_length, dataset->get_max_sentence_length());
}
Model::Model(Dataset* dataset,
    int max_word_length, // 可能な単語長の最大値. 英語16, 日本語8程度
    double initial_lambda_a, // 単語長のポアソン分布のλの事前分布のハイパーパラメータ
    double initial_lambda_b, // 単語長のポアソン分布のλの事前分布のハイパーパラメータ
    double vpylm_beta_stop, // VPYLMのハイパーパラメータ
    double vpylm_beta_pass)
{ // VPYLMのハイパーパラメータ
    _set_locale();
    int max_sentence_length = dataset->get_max_sentence_length();
    double vpylm_g0 = 1.0 / (double)dataset->_dict->get_num_characters();
    _npylm = new NPYLM(max_word_length, max_sentence_length, vpylm_g0, initial_lambda_a, initial_lambda_b, vpylm_beta_stop, vpylm_beta_pass);
    _lattice = new Lattice(_npylm, max_word_length, dataset->get_max_sentence_length());
}
Model::Model(std::string filename)
{
    _set_locale();
    _npylm = new NPYLM();
    if (load(filename) == false) {
        std::cout << filename << " not found." << std::endl;
        exit(0);
    }
    _lattice = new Lattice(_npylm, _npylm->_max_word_length, _npylm->_max_sentence_length);
}
Model::~Model()
{
    delete _npylm;
}
// 日本語周り
void Model::_set_locale()
{
    setlocale(LC_CTYPE, "ja_JP.UTF-8");
    std::ios_base::sync_with_stdio(false);
    std::locale default_loc("ja_JP.UTF-8");
    std::locale::global(default_loc);
    std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype); //※
    std::wcout.imbue(ctype_default);
    std::wcin.imbue(ctype_default);
}
int Model::get_max_word_length()
{
    return _npylm->_max_word_length;
}
void Model::set_initial_lambda_a(double lambda)
{
    _npylm->_lambda_a = lambda;
    _npylm->sample_lambda_with_initial_params();
}
void Model::set_initial_lambda_b(double lambda)
{
    _npylm->_lambda_b = lambda;
    _npylm->sample_lambda_with_initial_params();
}
void Model::set_vpylm_beta_stop(double stop)
{
    _npylm->_vpylm->_beta_stop = stop;
}
void Model::set_vpylm_beta_pass(double pass)
{
    _npylm->_vpylm->_beta_pass = pass;
}

// Added for connection ///////////////////////
void Model::set_hpylm_beta_hypers(double beta_a, double beta_b){
    _npylm->_hpylm->_pylm_beta_a = beta_a;
    _npylm->_hpylm->_pylm_beta_b = beta_b;
    _npylm->_hpylm->initialize();
}
void Model::set_hpylm_gamma_hypers(double gamma_alpha, double gamma_beta){
    _npylm->_hpylm->_pylm_gamma_alpha = gamma_alpha;
    _npylm->_hpylm->_pylm_gamma_beta = gamma_beta;
    _npylm->_hpylm->initialize();
}
void Model::set_vpylm_beta_hypers(double beta_a, double beta_b){
    _npylm->_vpylm->_pylm_beta_a = beta_a;
    _npylm->_vpylm->_pylm_beta_b = beta_b;
}
void Model::set_vpylm_gamma_hypers(double gamma_alpha, double gamma_beta){
    _npylm->_vpylm->_pylm_gamma_alpha = gamma_alpha;
    _npylm->_vpylm->_pylm_gamma_beta = gamma_beta;
}
void Model::set_vpylm_orderbeta_hypers(double stop, double pass){
    _npylm->_vpylm->_beta_stop = stop;
    _npylm->_vpylm->_beta_pass = pass;
}
void Model::set_vpylm_poisson_hypers(double lambda_a, double lambda_b){
    _npylm->_lambda_a = lambda_a;
    _npylm->_lambda_b = lambda_b;
    _npylm->sample_lambda_with_initial_params();
}
/////////////////////////////////////////


bool Model::load(std::string filename)
{
    bool success = false;
    std::ifstream ifs(filename);
    if (ifs.good()) {
        boost::archive::binary_iarchive iarchive(ifs);
        iarchive >> *_npylm;
        success = true;
    }
    ifs.close();
    return success;
}
bool Model::save(std::string filename)
{
    bool success = false;
    std::ofstream ofs(filename);
    if (ofs.good()) {
        boost::archive::binary_oarchive oarchive(ofs);
        oarchive << *_npylm;
        success = true;
    }
    ofs.close();
    return success;
}
void Model::parse(std::wstring sentence_str, std::vector<std::wstring>& words)
{
    // 領域の再確保
    _lattice->reserve(_npylm->_max_word_length, sentence_str.size());
    _npylm->reserve(sentence_str.size());
    words.clear();
    std::vector<int> segments; // 分割の一時保存用
    Sentence* sentence = new Sentence(sentence_str);
    _lattice->viterbi_decode(sentence, segments);
    sentence->split(segments);
    for (int n = 0; n < sentence->get_num_segments_without_special_tokens(); n++) {
        std::wstring word = sentence->get_word_str_at(n + 2);
        words.push_back(word);
    }
    delete sentence;
}
boost::python::list Model::python_parse(std::wstring sentence_str)
{
    // 領域の再確保
    _lattice->reserve(_npylm->_max_word_length, sentence_str.size());
    _npylm->reserve(sentence_str.size());
    std::vector<int> segments; // 分割の一時保存用
    Sentence* sentence = new Sentence(sentence_str);
    _lattice->viterbi_decode(sentence, segments);
    sentence->split(segments);
    boost::python::list words;
    for (int n = 0; n < sentence->get_num_segments_without_special_tokens(); n++) {
        std::wstring word = sentence->get_word_str_at(n + 2);
        words.append(word);
    }
    delete sentence;
    return words;
}
// use_scaling=trueならアンダーフローを防ぐ
double Model::compute_log_forward_probability(std::wstring sentence_str, bool use_scaling)
{
    // キャッシュの再確保
    _lattice->reserve(_npylm->_max_word_length, sentence_str.size());
    _npylm->reserve(sentence_str.size());
    Sentence* sentence = new Sentence(sentence_str);
    double log_px = _lattice->compute_log_forward_probability(sentence, use_scaling);
    delete sentence;
    return log_px;
}

boost::python::list Model::python_get_lambda()
{
    boost::python::list ret;
    for (int type = 1; type <= WORDTYPE_NUM_TYPES; type++) {
        ret.append(_npylm->_lambda_for_type[type]);
    }
    return ret;
}
}