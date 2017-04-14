# coding: utf-8
import argparse, os, codecs, sys
import model

def main(args):
	assert args.model_dir is not None
	try:
		os.mkdir(args.output_dir)
	except:
		pass

	npylm = model.npylm(args.model_dir)

	if args.input_dir is not None:
		assert os.path.exists(args.input_dir)
		files = os.listdir(args.input_dir)
		for filename in files:
			if filename.endswith(".txt"):
				print filename, "を処理中です ..."
				i = 1
				sentences = []
				with codecs.open(args.input_dir + "/" + filename, "r", "utf-8") as f:
					for sentence in f:
						if i % 100 == 0:
							sys.stdout.write("\r{}行目を分割しています ...".format(i))
							sys.stdout.flush()
						sentences.append(npylm.parse(sentence))
						i += 1
				with codecs.open(args.output_dir + "/" + filename, "w", "utf-8") as f:
					for words in sentences:
						f.write(" ".join(words))
						f.write("\n")
				sys.stdout.write("\r\033[2K")
				sys.stdout.flush()

	elif args.input_filename is not None:
		assert os.path.exists(args.input_filename)
		print args.input_filename, "を処理中です ..."
		sentences = []
		with codecs.open(args.input_filename, "r", "utf-8") as f:
			i = 1
			for sentence in f:
				if i % 100 == 0:
					sys.stdout.write("\r{}行目を分割しています ...".format(i))
					sys.stdout.flush()
				sentences.append(npylm.parse(sentence.replace("\n", "")))
				i += 1
		filename = args.input_filename.split("/")[-1]
		with codecs.open(args.output_dir + "/" + filename, "w", "utf-8") as f:
			for words in sentences:
				f.write(" ".join(words))
				f.write("\n")
	else:
		raise Exception()

if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--input-dir", type=str, default=None, help="分割の対象となるテキストファイルが入っているディレクトリ.")
	parser.add_argument("-f", "--input-filename", type=str, default=None, help="分割の対象となるテキストファイル.")
	parser.add_argument("-o", "--output-dir", type=str, default="out", help="分割結果を保存するディレクトリ.")
	parser.add_argument("-m", "--model-dir", type=str, default="out", help="モデルが保存されているディレクトリ.")
	main(parser.parse_args())