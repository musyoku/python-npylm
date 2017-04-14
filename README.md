# NPYLM

[ベイズ階層言語モデルによる教師なし形態素解析](http://chasen.org/~daiti-m/paper/nl190segment.pdf)のC++実装です。

単語n-gramモデルは3-gramで固定です。2-gramは非対応です。

## 動作環境

- Boost

## ビルド

```
make install
```

## 実行

```
python train.py -f textfile.txt
```

## 注意事項

研究以外の用途には使用できません。

https://twitter.com/daiti_m/status/851810748263157760