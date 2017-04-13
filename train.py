# coding: utf-8
import argparse, time, os
import npylm

def main(args):
	assert args.model_dir is not None
	try:
		os.mkdir(args.model_dir)
	except:
		pass

	trainer = npylm.trainer()
	# ハイパーパラメータ
	trainer.set_max_word_length(args.max_word_length)		# 可能な単語の最大長
	trainer.set_lambda_prior(4, 1)		# lambdaの事前分布（ガンマ分布）のハイパーパラメータ

	# テキストファイルの追加
	train_split_ratio = 0.9		# 何割の文を学習ようにするか
	if args.input_dir is not None:
		assert os.path.exists(args.input_dir)
		files = os.listdir(args.input_dir)
		for filename in files:
			if filename.endswith(".txt"):
				print "loading", filename
				trainer.add_textfile(args.input_dir + "/" + filename, train_split_ratio)
	elif args.input_filename is not None:
		assert os.path.exists(args.input_filename)
		print "loading", args.input_filename
		trainer.add_textfile(args.input_filename, train_split_ratio)
	else:
		raise Exception()

	# NPYLMでは通常、新しい分割結果をもとに単語nグラムモデルを更新する
	# Falseを渡すと分割結果の単語列としての確率が以前の分割のそれよりも下回っている場合に確率的に棄却する
	# Falseの方が切りすぎない分割結果になるが切りすぎなさすぎることもある
	trainer.set_always_accept_new_segmentation(True)

	# データを追加し終わったら呼ぶ
	trainer.compile()

	max_epoch = 500
	num_sentences_train = trainer.get_num_sentences_train()
	num_sentences_test = trainer.get_num_sentences_test()
	print "データ:"
	print "	{}	(学習)".format(num_sentences_train)
	print "	{}	(テスト)".format(num_sentences_test)
	total_time = 0
	for epoch in xrange(1, max_epoch):
		start_time = time.time()

		# ギブスサンプリング
		trainer.perform_gibbs_sampling()

		# ハイパーパラメータの推定
		trainer.sample_pitman_yor_hyperparameters()
		trainer.sample_lambda()
		trainer.update_Pk_vpylm()

		elapsed_time = time.time() - start_time
		total_time += elapsed_time
		print "Epoch {} / {} - {} min - {} sentences/sec - {} min total".format(
			epoch,
			max_epoch,
			int(elapsed_time / 60),
			int(num_sentences_train / elapsed_time),
			int(total_time / 60)
		)
		if epoch > 1:
			# 客数などの表示
			trainer.dump_hpylm()
			trainer.dump_vpylm()
			# VPYLMから長さkの単語が出現する確率を表示
			trainer.dump_Pk_vpylm()
			# 文字種ごとのλ
			trainer.dump_lambda()
			# サンプリングした分割を表示
			trainer.show_sampled_segmentation_train(5)
			trainer.show_sampled_segmentation_test(5)
		# 保存
		trainer.save(args.model_dir + "/npylm.model")

if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--input-dir", type=str, default=None, help="訓練用のテキストファイルが入っているディレクトリ.")
	parser.add_argument("-f", "--input-filename", type=str, default=None, help="訓練用のテキストファイル.")
	parser.add_argument("-m", "--model-dir", type=str, default="out", help="モデル保存用ディレクトリ.")
	parser.add_argument("-l", "--max-word-length", type=int, default=16, help="可能な単語の最大長.")
	main(parser.parse_args())