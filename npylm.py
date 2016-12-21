# -*- coding: utf-8 -*-
import argparse, sys, time
import model

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--text", type=str, help="訓練用のテキストファイル.")
args = parser.parse_args()

def main():
	npylm = model.npylm()
	npylm.load_textfile(args.text)
	npylm.set_max_word_length(15)		# 可能な単語の最大長
	npylm.init_lambda(1, 1)				# lambdaの事前分布（ガンマ分布）のハイパーパラメータ

	# VPYLMから長さkの単語が生成される確率p(k|vpylm)の推定結果の棄却期間.
	# ギブスイテレーションがこの回数以下の場合はポアソン補正を行わない.
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
		if epoch % 5 == 0:
			npylm.show_random_segmentation_result(50)

if __name__ == "__main__":
	main()