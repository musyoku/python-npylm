import argparse, os, codecs, sys
import npylm

def build_corpus(filepath, directory):
	assert filepath is not None or directory is not None
	corpus = npylm.corpus()
	sentence_list = []

	if filepath is not None:
		with codecs.open(filepath, "r", "utf-8") as f:
			for sentence_str in f:
				sentence_str = sentence_str.strip()
				sentence_list.append(sentence_str)

	if directory is not None:
		for filename in os.listdir(directory):
			with codecs.open(os.path.join(directory, filename), "r", "utf-8") as f:
				for sentence_str in f:
					sentence_str = sentence_str.strip()
					sentence_list.append(sentence_str)

	return sentence_list

def main():
	parser = argparse.ArgumentParser()
	# 以下のどちらかを必ず指定
	parser.add_argument("--input-filename", "-file", type=str, default=None, help="訓練用のテキストファイルのパス")
	parser.add_argument("--input-directory", "-dir", type=str, default=None, help="訓練用のテキストファイルが入っているディレクトリ")

	parser.add_argument("--working-directory", "-cwd", type=str, default="out", help="ワーキングディレクトリ")
	args = parser.parse_args()

	model = npylm.model(os.path.join(args.working_directory, "npylm.model"))
	sentence_list = build_corpus(args.input_filename, args.input_directory)

	for sentence_str in sentence_list:
		segmentation = model.parse(sentence_str)
		print(" / ".join(segmentation))

if __name__ == "__main__":
	main()