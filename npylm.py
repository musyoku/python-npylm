# -*- coding: utf-8 -*-
import argparse, sys, time
import model

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--text", type=str, help="訓練用のテキストファイル.")
args = parser.parse_args()

def main():
	npylm = model.npylm()
	npylm.load_textfile(args.text)
	npylm.set_max_word_length(15)	# 可能な単語の最大長
	npylm.init_lambda(6, 1)			# lambdaの事前分布（ガンマ分布）のハイパーパラメータ
	npylm.prepare_for_training()
	max_epoch = 1000
	num_lines = npylm.get_num_lines()
	progress = Progress()
	for epoch in xrange(1, max_epoch):
		start_time = time.time()

		# パラメータの更新
		npylm.perform_gibbs_sampling(False)
		npylm.dump_lambda()

		# ハイパーパラメータの推定
		npylm.sample_pitman_yor_hyperparameters()
		if epoch > 1:
			npylm.sample_lambda()
			npylm.update_pk_vpylm()

		elapsed_time = time.time() - start_time
		print "Epoch {} - {} lps".format(num_lines / elapsed_time)
		
		if epoch > 1:
			npylm.show_random_segmentation_result(30)

if __name__ == "__main__":
	main()