import argparse
import codecs
import os
import sys

import npylm


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--working-directory",
        "-cwd",
        type=str,
        default="out",
        help="ワーキングディレクトリ")
    args = parser.parse_args()

    model = npylm.model(os.path.join(args.working_directory, "npylm.model"))
    lambda_list = model.get_lambda()
    word_types = [
        "アルファベット", "数字", "記号", "ひらがな", "カタカナ", "漢字", "漢字+ひらがな", "漢字+カタカナ",
        "その他"
    ]
    for wtype, lam in zip(word_types, lambda_list):
        print(wtype, lam)


if __name__ == "__main__":
    main()
