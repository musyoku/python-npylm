# -*- coding: utf-8 -*-
import argparse, sys, time
import model

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--text", type=str, help="訓練用のテキストファイル.")
parser.add_argument("-l", "--max_word_length", type=int, default=15, help="可能な単語の最大長.")
args = parser.parse_args()

def main():
	npylm = model.npylm()
	npylm.load_textfile(args.text)
	npylm.set_max_word_length(args.max_word_length)		# 可能な単語の最大長
	npylm.init_lambda(1, 1)				# lambdaの事前分布（ガンマ分布）のハイパーパラメータ

	# VPYLMから長さkの単語が生成される確率p(k|vpylm)の推定結果の棄却期間.
	# ギブスイテレーションがこの回数以下の場合は単語0-gram確率のポアソン補正を行わない.
	# 1イテレーション目は文章が丸ごと1つの単語としてモデルに追加されるので単語確率を求めることがない
	# 2イテレーション目はp(k|VPYLM)の精度が悪いので棄却
	# それ以降はお好み
	npylm.set_burn_in_period_for_pk_vpylm(4)

	npylm.prepare_for_training()
	max_epoch = 1000
	num_lines = npylm.get_num_lines()
	for epoch in xrange(1, max_epoch):
		start_time = time.time()

		# パラメータの更新
		npylm.perform_gibbs_sampling(False)

		# ハイパーパラメータの推定
		npylm.sample_pitman_yor_hyperparameters()
		npylm.sample_lambda()
		npylm.update_pk_vpylm()

		elapsed_time = time.time() - start_time
		print "Epoch {} - {} lps - {} nodes (vpylm) - {} depth (vpylm) - {} nodes (hpylm)".format(
			epoch, 
			num_lines / elapsed_time,
			npylm.get_num_nodes_of_vpylm(),
			npylm.get_depth_of_vpylm(),
			npylm.get_num_nodes_of_hpylm()
		)
		if epoch > 1:
			npylm.dump_lambda()

		# 分割結果を表示
		# ビタビアルゴリズムではなくForward filtering-Backward samplingの結果
		# そのため分割が確率的に変わる
		npylm.show_random_segmentation_result(10)

if __name__ == "__main__":
	main()