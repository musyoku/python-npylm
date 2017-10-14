#include <boost/functional/hash.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <unordered_set>
#include <vector>
#include <string>
#include "../../src/npylm/common.h"
#include "../../src/npylm/hash.h"
#include "../../src/npylm/sentence.h"
using namespace npylm;
using std::cout;
using std::flush;
using std::endl;

void test_get_substr_word_id(){
	std::wstring sentence_str = L"本論文 では, 教師 データ や 辞書 を 必要 とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する. 観測された文字列を, 文字 nグラム-単語 nグラムをノンパラメトリックベイズ法の枠組で統合した確率モデルからの出力とみなし, MCMC 法と動的計画法を用いて, 繰り返し隠れた「単語」を推定する. 提案法は, あらゆる言語の生文字列から直接, 全く知識なしに Kneser-Ney と同等に高精度にスムージングされ, 未知語のない nグラム言語モデルを構築する方法とみなすこともできる.話し言葉や古文を含む日本語, および中国語単語分割の標準的なデータセットでの実験により, 提案法の有効性および効率性を確認した.";
	Sentence* sentence = new Sentence(sentence_str);
	for(int t = 0;t < sentence_str.size();t++){
		for(int k = 0;k < std::min((size_t)t, sentence_str.size());k++){
			size_t hash = hash_substring(sentence_str, t - k, t);
			std::wstring substr(sentence_str.begin() + t - k, sentence_str.begin() + t + 1);
			size_t _hash = hash_wstring(substr);
			size_t __hash = sentence->get_substr_word_id(t - k, t);
			assert(hash == _hash && _hash == __hash);
		}
	}
	delete sentence;
}

void test_get_substr_word_str(){
	std::wstring sentence_str = L"本論文 では, 教師 データ や 辞書 を 必要 とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する. 観測された文字列を, 文字 nグラム-単語 nグラムをノンパラメトリックベイズ法の枠組で統合した確率モデルからの出力とみなし, MCMC 法と動的計画法を用いて, 繰り返し隠れた「単語」を推定する. 提案法は, あらゆる言語の生文字列から直接, 全く知識なしに Kneser-Ney と同等に高精度にスムージングされ, 未知語のない nグラム言語モデルを構築する方法とみなすこともできる.話し言葉や古文を含む日本語, および中国語単語分割の標準的なデータセットでの実験により, 提案法の有効性および効率性を確認した.";
	Sentence* sentence = new Sentence(sentence_str);
	for(int t = 0;t < sentence_str.size();t++){
		for(int k = 0;k < std::min((size_t)t, sentence_str.size());k++){
			std::wstring substr(sentence_str.begin() + t - k, sentence_str.begin() + t + 1);
			std::wstring _substr = sentence->get_substr_word_str(t - k, t);
			assert(substr.compare(_substr) == 0);
		}
	}
	delete sentence;
}

void test_split_by_array(){
	std::wstring sentence_str = L"提案法は, あらゆる言語の生文字列から直接, 全く知識なしに Kneser-Ney と同等に高精度にスムージングされ, 未知語のない nグラム言語モデルを構築する方法とみなすこともできる.";
	Sentence* sentence = new Sentence(sentence_str);
	int* segments_without_special_tokens = new int[4];
	for(int i = 1;i <= sentence_str.size() - 3;i++){
		for(int m = 1;m <= sentence_str.size() - i - 1;m++){
			for(int k = 1;k <= sentence_str.size() - i - m - 1;k++){
				int n = sentence_str.size() - i - m - k;
				assert(i + m + k + n == sentence_str.size());

				std::wstring first(sentence_str.begin(), sentence_str.begin() + i);
				std::wstring second(sentence_str.begin() + i, sentence_str.begin() + i + m);
				std::wstring third(sentence_str.begin() + i + m, sentence_str.begin() + i + m + k);
				std::wstring forth(sentence_str.begin() + i + m + k, sentence_str.begin() + sentence_str.size());
				// std::wcout << first << " / " << second << " / " << third << " / " << forth<< endl;
				segments_without_special_tokens[0] = i;
				segments_without_special_tokens[1] = m;
				segments_without_special_tokens[2] = k;
				segments_without_special_tokens[3] = n;
				sentence->split(segments_without_special_tokens, 4);

				std::wstring _first = sentence->get_word_str_at(2);
				std::wstring _second = sentence->get_word_str_at(3);
				std::wstring _third = sentence->get_word_str_at(4);
				std::wstring _forth = sentence->get_word_str_at(5);
				assert(first.compare(_first) == 0);
				assert(second.compare(_second) == 0);
				assert(third.compare(_third) == 0);
				assert(forth.compare(_forth) == 0);

				size_t hash_first = hash_substring(sentence_str, 0, i - 1);
				size_t hash_second = hash_substring(sentence_str, i, i + m - 1);
				size_t hash_third = hash_substring(sentence_str, i + m, i + m + k - 1);
				size_t hash_forth = hash_substring(sentence_str, i + m + k, i + m + k + n - 1);
				assert(hash_first == sentence->get_word_id_at(2));
				assert(hash_second == sentence->get_word_id_at(3));
				assert(hash_third == sentence->get_word_id_at(4));
				assert(hash_forth == sentence->get_word_id_at(5));
			}
		}
	}
	delete[] segments_without_special_tokens;
	delete sentence;
}

void test_split_by_vector(){
	std::wstring sentence_str = L"提案法は, あらゆる言語の生文字列から直接, 全く知識なしに Kneser-Ney と同等に高精度にスムージングされ, 未知語のない nグラム言語モデルを構築する方法とみなすこともできる.";
	Sentence* sentence = new Sentence(sentence_str);
	std::vector<int> segments_without_special_tokens{0, 0, 0, 0};
	for(int i = 1;i <= sentence_str.size() - 3;i++){
		for(int m = 1;m <= sentence_str.size() - i - 1;m++){
			for(int k = 1;k <= sentence_str.size() - i - m - 1;k++){
				int n = sentence_str.size() - i - m - k;
				assert(i + m + k + n == sentence_str.size());

				std::wstring first(sentence_str.begin(), sentence_str.begin() + i);
				std::wstring second(sentence_str.begin() + i, sentence_str.begin() + i + m);
				std::wstring third(sentence_str.begin() + i + m, sentence_str.begin() + i + m + k);
				std::wstring forth(sentence_str.begin() + i + m + k, sentence_str.begin() + sentence_str.size());
				// std::wcout << first << " / " << second << " / " << third << " / " << forth<< endl;
				segments_without_special_tokens[0] = i;
				segments_without_special_tokens[1] = m;
				segments_without_special_tokens[2] = k;
				segments_without_special_tokens[3] = n;
				sentence->split(segments_without_special_tokens);

				std::wstring _first = sentence->get_word_str_at(2);
				std::wstring _second = sentence->get_word_str_at(3);
				std::wstring _third = sentence->get_word_str_at(4);
				std::wstring _forth = sentence->get_word_str_at(5);
				assert(first.compare(_first) == 0);
				assert(second.compare(_second) == 0);
				assert(third.compare(_third) == 0);
				assert(forth.compare(_forth) == 0);

				size_t hash_first = hash_substring(sentence_str, 0, i - 1);
				size_t hash_second = hash_substring(sentence_str, i, i + m - 1);
				size_t hash_third = hash_substring(sentence_str, i + m, i + m + k - 1);
				size_t hash_forth = hash_substring(sentence_str, i + m + k, i + m + k + n - 1);
				assert(hash_first == sentence->get_word_id_at(2));
				assert(hash_second == sentence->get_word_id_at(3));
				assert(hash_third == sentence->get_word_id_at(4));
				assert(hash_forth == sentence->get_word_id_at(5));
			}
		}
	}
	delete sentence;
}

int main(){
	setlocale(LC_CTYPE, "ja_JP.UTF-8");
	std::ios_base::sync_with_stdio(false);
	std::locale default_loc("ja_JP.UTF-8");
	std::locale::global(default_loc);
	std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype); //※
	std::wcout.imbue(ctype_default);
	std::wcin.imbue(ctype_default);

	test_get_substr_word_id();
	cout << "OK" << endl;
	test_get_substr_word_str();
	cout << "OK" << endl;
	test_split_by_array();
	cout << "OK" << endl;
	test_split_by_vector();
	cout << "OK" << endl;
	return 0;
}