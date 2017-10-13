import argparse, time, os, codecs, sys
import npylm

class stdout:
	BOLD = "\033[1m"
	END = "\033[0m"
	CLEAR = "\033[2K"

def printb(string):
	print(stdout.BOLD + string + stdout.END)

def printr(string):
	sys.stdout.write("\r" + stdout.CLEAR)
	sys.stdout.write(string)
	sys.stdout.flush()

# 訓練データを形態素解析して各品詞ごとにその品詞になりうる単語の総数を求めておく
def build_corpus(filepath, directory):
	assert filepath is not None or directory is not None
	corpus = npylm.corpus()

	if filepath is not None:
		with codecs.open(filepath, "r", "utf-8") as f:
			for sentence_str in f:
				sentence_str = sentence_str.strip()
				corpus.add_sentence(sentence_str)

	if directory is not None:
		for filename in os.listdir(directory):
			with codecs.open(os.path.join(directory, filename), "r", "utf-8") as f:
				for sentence_str in f:
					sentence_str = sentence_str.strip()
					corpus.add_sentence(sentence_str)

	return corpus

def main():
	parser = argparse.ArgumentParser()
	# 以下のどちらかを必ず指定
	parser.add_argument("--train-filename", "-file", type=str, default=None, help="訓練用のテキストファイルのパス")
	parser.add_argument("--train-directory", "-dir", type=str, default=None, help="訓練用のテキストファイルが入っているディレクトリ")

	parser.add_argument("--seed", type=int, default=1)
	parser.add_argument("--epochs", "-e", type=int, default=100000, help="総epoch")
	parser.add_argument("--working-directory", "-cwd", type=str, default="out", help="ワーキングディレクトリ")
	parser.add_argument("--train-split", "-split", type=float, default=0.9, help="テキストデータの何割を訓練データにするか")

	parser.add_argument("--lambda-a", "-lam-a", type=float, default=4)
	parser.add_argument("--lambda-b", "-lam-b", type=float, default=1)
	parser.add_argument("--vpylm-beta-stop", "-beta-stop", type=float, default=4)
	parser.add_argument("--vpylm-beta-pass", "-beta-pass", type=float, default=1)
	parser.add_argument("--max-word-length", "-l", type=int, default=16, help="可能な単語の最大長.")
	args = parser.parse_args()

	assert args.working_directory is not None
	try:
		os.mkdir(args.working_directory)
	except:
		pass

	# 訓練データを追加
	corpus = build_corpus(args.train_filename, args.train_directory)
	dataset = npylm.dataset(corpus, args.train_split, args.seed)

	# 単語辞書を保存
	dictionary = dataset.get_dict()
	dictionary.save(os.path.join(args.working_directory, "npylm.dict"))

	# モデル
	model = npylm.model(
		dataset, 
		args.max_word_length)	# 可能な単語の最大長を指定

	# ハイパーパラメータの設定
	model.set_initial_lambda_a(args.lambda_a);
	model.set_initial_lambda_b(args.lambda_b);
	model.set_vpylm_beta_stop(args.vpylm_beta_stop);
	model.set_vpylm_beta_pass(args.vpylm_beta_pass);

	# 学習の準備
	trainer = npylm.trainer(
		dataset, model, 
		# NPYLMでは通常、新しい分割結果をもとに単語nグラムモデルを更新する
		# Falseを渡すと分割結果の単語列としての確率が以前の分割のそれよりも下回っている場合に確率的に棄却する
		always_accept_new_segmentation=True)

	# 文字列の単語IDが衝突しているかどうかをチェック
	# 時間の無駄なので一度したらしなくてよい
	# メモリを大量に消費します
	if True:
		print("ハッシュの衝突を確認中 ...")
		num_checked_words = dataset.detect_hash_collision(args.max_word_length)
		print("衝突はありません (総単語数 {})".format(num_checked_words))

	# 学習ループ
	for epoch in range(1, args.epochs + 1):
		start = time.time()
		trainer.gibbs()				# 新しい状態系列をギブスサンプリング
		trainer.sample_hpylm_vpylm_hyperparameters()	# HPYLMとVPYLMのハイパーパラメータの更新
		trainer.sample_lambda()		# λの更新

		if epoch > 3:
			trainer.update_p_k_given_vpylm()

		# ログ
		elapsed_time = time.time() - start
		printr("Iteration {} / {} - {:.3f} sec".format(epoch, args.epochs, elapsed_time))
		if epoch % 100 == 0:
			printr("")
			trainer.print_segmentation_train()
		if epoch % 100 == 0:
			printr("")
			model.save(os.path.join(args.working_directory, "npylm.model"))

if __name__ == "__main__":
	main()