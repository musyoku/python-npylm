# NPYLM for Python

Pythonで使えるNPYLMのラッパーです。

以下の論文をもとに実装しています。

- [A Bayesian Interpretation of Interpolated Kneser-Ney](https://www.stats.ox.ac.uk/~teh/research/compling/hpylm.pdf)
- [Pitman-Yor過程に基づく可変長n-gram 言語モデル](Pitman-Yor過程に基づく可変長n-gram 言語モデル)
- [ベイズ階層言語モデルによる教師なし形態素解析](http://chasen.org/~daiti-m/paper/nl190segment.pdf)

特許の関係でソースを公開することはできないため共有ライブラリのみになります。

また特許の関係でモデルの保存機能もありません。学習する際は毎回最初からやり直しとなります。

NPYLMの実装に興味がある方は[実装方法](http://musyoku.github.io/2016/12/14/%E3%83%99%E3%82%A4%E3%82%BA%E9%9A%8E%E5%B1%A4%E8%A8%80%E8%AA%9E%E3%83%A2%E3%83%87%E3%83%AB%E3%81%AB%E3%82%88%E3%82%8B%E6%95%99%E5%B8%AB%E3%81%AA%E3%81%97%E5%BD%A2%E6%85%8B%E7%B4%A0%E8%A7%A3%E6%9E%90/)をお読みください。

# インストール

## 依存関係

`libboost_python.so`に依存します。

ない場合はインストールしておきます。

```
sudo apt-get install libboost-python-dev
```

またPython 2.7を前提にビルドしています。

## macOSの場合

`model.mac.so`を`model.so`にリネームします。

macOSではEl Capitan以降から導入されたSIPの影響でそのままでは実行できません。

`makefile`の`install`の項目のboostのパスを自身の環境に合わせて変更し、ターミナルで

```
make install
```

すると`model.so`が修正され実行可能になります。

## Ubuntuの場合

`model.linux.so`を`model.so`にリネームします。

# 学習

## オプション

- -t
	- テキストファイルのパス
- -l
	- 可能な単語の最大長
	- 日本語なら8、英語なら15程度

## Alice's Adventures in Wonderland

`alice.txt`は[Alice's Adventures in Wonderland, by Lewis Carroll](https://www.gutenberg.org/files/11/11-h/11-h.htm)からダウンロードし、前処理を行ったものになります。

学習するには以下のように実行します。

```
python npylm.py -t alice.txt -l 15
```

## 源氏物語

`genji.txt`は[源氏物語の世界](http://www.sainet.or.jp/~eshibuya/)からダウンロードし前処理を行ったものになります。

学習するには以下のように実行します。

```
python npylm.py -t genji.txt -l 8
```

