#include <boost/functional/hash.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <unordered_set>
#include <string>
#include "../../src/npylm/common.h"
#include "../../src/npylm/hash.h"
using std::cout;
using std::flush;
using std::endl;

void test_hash_substring(){
	std::wstring sentence_str = L"本論文 では, 教師 データ や 辞書 を 必要 とせず, あらゆる言語に適用できる教師なし形態素解析器および言語モデルを提案する. 観測された文字列を, 文字 nグラム-単語 nグラムをノンパラメトリックベイズ法の枠組で統合した確率モデルからの出力とみなし, MCMC 法と動的計画法を用いて, 繰り返し隠れた「単語」を推定する. 提案法は, あらゆる言語の生文字列から直接, 全く知識なしに Kneser-Ney と同等に高精度にスムージングされ, 未知語のない nグラム言語モデルを構築する方法とみなすこともできる.話し言葉や古文を含む日本語, および中国語単語分割の標準的なデータセットでの実験により, 提案法の有効性および効率性を確認した.";
	for(int t = 0;t < sentence_str.size();t++){
		for(int k = 0;k < std::min((size_t)t, sentence_str.size());k++){
			size_t hash = npylm::hash_substring(sentence_str, t - k, t);
			std::wstring substr(sentence_str.begin() + t - k, sentence_str.begin() + t + 1);
			size_t _hash = npylm::hash_wstring(substr);
			assert(hash == _hash);
		}
	}
}

int main(){
	test_hash_substring();
	cout << "OK" << endl;
	return 0;
}