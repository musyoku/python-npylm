# coding: utf-8
import argparse, codecs
import numpy as np

def main(args):
	words = [char + char + char for char in ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"]]
	prob = np.full((len(words), len(words)), 1.0 / len(words), dtype=np.float32)
	with codecs.open("../dataset/test.txt", "w", "utf-8") as f:
		for n in range(args.num_seq):
			sequence = ""
			word_index = 0
			for l in range(args.seq_length):
				word_index = int(np.argwhere(np.random.multinomial(1, prob[word_index]) == 1))
				sequence += str(words[word_index])
			print(sequence)
			f.write(sequence + "\n")

if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("-l", "--seq-length", type=int, default=20, help="1つの文の長さ.")
	parser.add_argument("-n", "--num-seq", type=int, default=20, help="生成する文の個数.")
	main(parser.parse_args())