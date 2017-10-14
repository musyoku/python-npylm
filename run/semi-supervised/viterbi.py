import argparse, os, codecs, sys
import npylm

def main():
	parser = argparse.ArgumentParser()
	# 以下のどちらかを必ず指定
	parser.add_argument("--input-filename", "-file", type=str, default=None, help="訓練用のテキストファイルのパス")
	parser.add_argument("--input-directory", "-dir", type=str, default=None, help="訓練用のテキストファイルが入っているディレクトリ")

	parser.add_argument("--working-directory", "-cwd", type=str, default="out", help="ワーキングディレクトリ")
	parser.add_argument("--output-directory", "-out", type=str, default="out", help="分割結果の出力先")
	args = parser.parse_args()

	try:
		os.mkdir(args.output_dir)
	except:
		pass

	model = npylm.model(os.path.join(args.working_directory, "npylm.model"))

	if args.input_filename is not None:
		segmentation_list = []
		with codecs.open(args.input_filename, "r", "utf-8") as f:
			for sentence_str in f:
				sentence_str = sentence_str.strip()
				segmentation = model.parse(sentence_str)
				if len(segmentation) > 0:
					segmentation_list.append(segmentation)

		filename = args.input_filename.split("/")[-1]
		with codecs.open(os.path.join(args.output_directory, filename), "w", "utf-8") as f:
			for segmentation in segmentation_list:
				f.write(" ".join(segmentation))
				f.write("\n")

	if args.input_directory is not None:
		for filename in os.listdir(args.input_directory):
			print("processing {} ...".format(filename))
			segmentation_list = []
			with codecs.open(os.path.join(args.input_directory, filename), "r", "utf-8") as f:
				for sentence_str in f:
					sentence_str = sentence_str.strip()
					segmentation = model.parse(sentence_str)
					if len(segmentation) > 0:
						segmentation_list.append(segmentation)

			with codecs.open(os.path.join(args.output_directory, filename), "w", "utf-8") as f:
				for segmentation in segmentation_list:
					f.write(" ".join(segmentation))
					f.write("\n")

if __name__ == "__main__":
	main()