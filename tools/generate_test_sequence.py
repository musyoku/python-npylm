# coding: utf-8
import argparse, codecs
import numpy as np

def main(args):
	words = [
		"a",
		"bb",
		"ccc",
		"dddd",
		"eeeee",
		"ffffff",
		"g",
		"hh",
		"iii",
		"jjjj",
		"kkkkk",
		"llllll",
	]
	prob = np.zeros((len(words), ), dtype=np.float32)
	prob += 10, 13, 15, 17, 19, 10, 13, 15, 17, 19, 20, 21
	prob /= np.sum(prob)

	with codecs.open("../dataset/test.txt", "w", "utf-8") as f:
		for n in xrange(args.num_seq):
			sequence = ""
			for l in xrange(args.seq_length):
				index = int(np.argwhere(np.random.multinomial(1, prob) == 1))
				sequence += str(words[index])
			print sequence
			f.write(sequence + "\n")

if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("-l", "--seq-length", type=int, default=20, help="1つの文の長さ.")
	parser.add_argument("-n", "--num-seq", type=int, default=20, help="生成する文の個数.")
	main(parser.parse_args())