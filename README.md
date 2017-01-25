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

Boost 1.62の環境でビルドしているため、`libboost_python.so.1.62.0`に依存します。

ない場合はインストールしておきます。

b2の場合
```
sudo ./b2 install -j4 --with-python
sudo ldconfig
```

apt-getの場合

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

Late 2012、Mid 2014、Late 2016、2016（Touch Barモデル）で動作確認しています。

おそらくi7搭載機ならどのMacbookでも動くと思います。

## Ubuntuの場合

`model.linux.so`を`model.so`にリネームします。

ビルド環境はUbuntu 16.04 64bit Core-i7です。

Macの場合と違って実行できない場合があります。

# 学習

## オプション

- -t
	- テキストファイルのパス
- -l
	- 可能な単語の最大長
	- 日本語なら8、英語なら15程度
	- 実行速度に大きな影響を与える
- -o
	- 分割結果を保存するテキストファイルのパス
	- 未指定も可

## Alice's Adventures in Wonderland

`alice.txt`は[Alice's Adventures in Wonderland, by Lewis Carroll](https://www.gutenberg.org/files/11/11-h/11-h.htm)からダウンロードし、前処理を行ったものになります。

学習するには以下のように実行します。

```
python npylm.py -t alice.txt -l 15
```

結果

```
if / that's / all / youknow / about / it / , / you / may / stand / down / , / c / ontinued / theking / . / 
i / wonder / if / i / shall / fall / right / through / the / earth / ! / how / funn / yit'lls / eem / to / come / out / among / the / pe / ople / that / walk / with / the / irheadsdown / ward! / the / antipa / thies / , / i / think / 
living / at / the / bottomofthesea / . / 
curious / er / and / curious / er / ! / cried / alice / she / was / so / much / s / urpr / is / e / d / , / that / for / the / m / oment / she / quite / forgot / how / to / speak / good / english / 
before / her / was / an / other / long / pass / age / , / and / the / wh / itera / bbit / was / still / in / sight / , / hurrying / down / it / . / there / was / not / amoment / to / be / lost / away / went / alice / like / the / wind / , / and / was / just / in / time / to / hear / it / say / , / as / it / turned / a / corner / , / oh / my / ears / and / whiskers / , / how / late / it / 's / getting / ! / she / was / close / behind / it / when / she / turned / the / corner / , / but / the / rabb / it / was / no / long / er / to / be / seen / she / found / her / self / in / a / long / , / low / hall / , / which / was / lit / up / by / arowofl / amps / hanging / from / the / roof / . / 
assoona / s / she / had / made / out / the / proper / way / of / nursing / it / , / which / was / to / twist / it / up / in / to / a / sortof / knot / , / and / then / keep / tight / hold / of / it / s / right / ear / and / left / foot / , / so / as / to / pr / event / its / un / do / ing / it / self / , / she / carried / it / out / in / to / the / open / air / . / if / i / do / n't / take / this / chi / ld / away / with / me / , / thoughtalice / , / they’re / sure / to / kill / it / in / a / day / or / two / would / n't / it / be / murder / to / leave / it / behind / ? / she / said / the / last / word / s / out / loud / , / and / the / little / thing / grunted / in / reply / it / had / leftoff / sneezing / by / this / time / . / do / n't / grunt / , / saidalice / 
letme / see / four / times / five / is / twelve / , / and / four / times / six / is / thir / teen / , / and / four / timess / even / is / oh / dear / ! / i / shall / never / get / to / twenty / at / that / r / ate / ! / 
by / the / useof / this / ointment / one / shilling / the / box / 
they / 'd / have / been / ill / . / 
but / about / his / to / es / ? / ' / the / mockturtle / persisted / . / how / could / he / turn / themout / with / hisnose / , / youknow / ? / 
i / did / n't / know / i / twas / your / t / able / , / saidalice / 
and / them / oment / s / heappeared / on / the / other / side / , / the / puppy / made / an / other / rush / at / the / st / ick / , / and / tumbled / head / over / he / els / in / its / hurry / to / get / hold / of / it / 
it / 's / laid / for / a / gre / at / many / more / than / three / . / 
is / that / the / way / you / manage / ? / alice / asked / . / 
well / , / i / ca / n't / show / it / you / my / self / , / the / mockturtle / said / i / 'm / too / st / iff / . / and / thegrypho / n / never / learnt / it / . / 
alice / thought / thewhole / thing / very / absurd / , / but / they / all / looked / so / grave / that / she / did / not / dare / to / laugh / 
with / g / ently / smiling / jaws / ! / 
the / knave / did / so / , / very / care / fully / , / with / one / foot / . / 
secondly, / because / they / ’re / making / such / a / noise / in / side / , / no / one / could / possibly / hear / you / . / and / ce / rt / ainly / there / was / a / most / ext / raordinary / noise / going / on / with / in / a / constant / howling / and / sneezing / , / and / every / now / and / then / a / gre / at / crash / , / as / if / a / dish / or / kettle / had / been / broken / to / piece / s / . / 
i / do / n't / think / they / play / at / all / fair / ly / , / alice / began / , / in / rather / a / complain / ing / to / ne / , / and / they / all / quarrel / so / dread / fully / one / ca / n't / hear / one / self / speak / and / they / do / n't / see / mto / have / any / rule / s / in / particular / 
no / body / asked / your / op / in / ion / , / saidalice / . / 
the / mouse / did / not / notice / this / quest / ion / , / but / hurried / ly / wenton / , / fou / nditadvisable / to / go / with / ed / garatheling / to / meet / will / iam / and / of / fer / h / im / the / crown / . / will / iam / 's / conduct / at / first / was / moder / ate / . / but / the / in / solence / of / his / normans / howareyougett / ingon / now / , / my / dear / ? / it / c / ontinued / , / turningto / alice / as / it / spoke / . / 
i / won't / ! / saidalice / . / 
and / thewhole / party / atonce / crowd / ed / round / her / , / calling / out / in / a / confused / way / , / pr / ize / s / ! / pr / ize / s / ! / 
has / lasted / therest / of / my / life / . / 
there / 's / no / sortof / usein / knocking / , / said / the / footman / , / and / that / fortwo / reason / s / . / first / , / because / i / 'm / on / the / s / ame / side / of / the / door / as / you / are / 
i / had / not / ! / cried / the / mouse / , / sharply / and / very / angrily / . / 
you / are / , / said / theking / . / 
holdyourtongue! / added / thegrypho / n / , / before / alice / could / speak / agai / n / . / the / mockturtle / wenton / . / 
exactly / as / we / were / . / 
down,down,down. / would / the / fall / never / come / to / an / end / ! / i / wonder / how / many / miles / i / ’ve / fallen / by / this / time / ? / she / said / a / loud / . / 
i / shall / sit / here / , / the / footman / remarked / , / till / to / morrow / 
[lateredit / ionscontinued / asfollows / 
and / that's / the / jury / -box / , / thoughtalice / , / and / those / twelve / crea / ture / s / , / she / was / ob / ligedto / saycrea / ture / s / , / you / see / , / because / some / of / the / m / were / animals / , / and / some / were / birds / , / i / suppose / they / are / the / juror / s / . / she / said / this / last / word / two / or / three / times / over / toherself / , / being / rather / proud / of / it / for / she / thought / , / and / right / ly / too / , / that / very / few / little / gir / l / s / of / her / age / knew / the / meaning / of / it / at / all / . / however / , / jury- / men / would / have / done / just / as / well / . / 
very / ill / . / 
beauootiful / soooop! / 
for / he / can / tho / roughly / en / joy / 
oh, / you / sing / , / said / thegrypho / n / . / i / ’ve / forgot / ten / the / words / . / 
very / much / indeed / , / saidalice / . / 
then / you / may / s / it / down / , / theking / replied / . / 
```

## 源氏物語

`genji.txt`は[源氏物語の世界](http://www.sainet.or.jp/~eshibuya/)からダウンロードし前処理を行ったものになります。

学習するには以下のように実行します。

```
python npylm.py -t genji.txt -l 8
```

結果

```
あはれ / のこと / や。 / 
なかなか / 心やす / く / は / 思 / ひ / たまへ / な / せ / ど、 / さ / て / 片隅に隠ろへ / て / も / あり / ぬべ / き / 人 / の / 心 / や / す / さを、 / おだしう / 思 / ひ / たまへ / つ / る / に / 、 / に / はか / に / かの宮 / ものし / たまふ / な / らむ / 。 / 
宮は / 、 / 仏 / の / 御 / 前 / に / て / 、 / 経 / を / ぞ / 読み / たまひ / ける / 。 / 
うたた / 寝 / はいさめ / きこゆ / る / ものを / 。 / 
宮 / 、 / いと / 苦し / き / 判者 / に / も / 当たりて / はべ / る / かな / 。 / 
さる / は / 、 / 今宵 / ばかり / こそ / 、 / か / く / も / 立ち / 寄り / たま / は / め / 、 / え / 聞こえ / ぬ / こと / と / 言は / せ / た / り / 。 / 
あさまし / く / おぼえ / て / 、 / ともかくも / 思ひ分かれず、 / やをら / 起き / 出で / て / 、 / 生絹 / な / る / 単衣 / を / 一 / つ / 着 / て / 、 / すべり出で / にけり。 / 
そ / の / 夜 / 、 / 源氏 / 中将 / 、 / 正三位 / し / たまふ / 。 / 
声 / など / も / 、 / わざと / 似 / たまへ / りと / も / おぼえ / ざ / り / し / かど / 、 / あやしき / まで / ただ / それと / のみ / おぼゆ / る / に / 、 / 人 / 目 / 見 / 苦し / か / る / まじ / く / は / 、 / 簾 / も / ひき / 上げ / て / さし / 向かひ / きこえ / まほ / し / く / 、 / うち / 悩み / たまへ / らむ / 容貌 / ゆかし / く / おぼえ / たまふ / も / 、 / なほ / 、 / 世 / の / 中 / に / もの / 思は / ぬ / 人 / は / 、 / え / ある / まじ / き / わざ / に / や / あらむ / と / ぞ / 思 / ひ / 知 / られ / たまふ / 。 / 
怨じ / や / し / たまは / む / と / す / る / と / の / たまへ / ば / 、 / 見 / 苦し / う / 。 / 
さる / は / 、 / おり立ちて / 人 / の / やう / に / も / わび / た / まは / ざ / り / し / かど / 、 / 人 / ざま / の / さすがに / 心苦し / う / 見 / ゆ / る / な / り / 。 / 
されば / 、 / 思 / ひ / わた / さ / るる / に / や / あらむ / 。 / 
明け / ぬ / れ / ば / 、 / 車 / など / 率て / 来 / て / 、 / 守 / の / 消息 / など / 、 / いと / 腹立たし / げに / 脅 / かし / た / れば / 、 / かたじけな / く / 、 / よろづに / 頼み / きこえ / させ / て / なむ / 。 / 
年ごろ / よろづに / 嘆き / 沈み / 、 / さまざま / 憂 / き / 身 / と / 思 / ひ / 屈し / つる / 命 / も / 延べ / まほ / し / う / 、 / はればれし / き / に / つけて / 、 / まこと / に / 住吉の / 神 / も / おろかな / ら / ず / 思 / ひ / 知 / らる / 。 / 
いま / 、 / 聞こえ / む / 。 / 
御 / 後見 / ども / も / 、 / 何か / は / 、 / 若き / 御心 / どち / な / れば / 、 / 年ごろ / 見 / ならひ / たまへ / る / 御 / あはひ / を / 、 / に / はか / に / も / 、 / いかが / は / もて / 離れ / はしたな / め / は / きこえ / む / と / 見 / る / に / 、 / 女 / 君 / こそ / 何心なく / おはす / れ / ど、 / 男 / は / 、 / さ / こそ / も / の / げなき / ほど / と / 見 / きこゆ / れ / 、 / おほけな / く / 、 / いか / な / る / 御 / 仲らひ / に / か / ありけむ、 / よそ / よそに / な / り / て / は / 、 / これ / を / ぞ / 静心な / く / 思 / ふ / べ / き / 。 / 
鴬の / 若 / やかに / 、 / 近 / き / 紅梅の / 末 / に / うち / 鳴き / た / る / を / 、 / 袖 / こそ / 匂へ / と / 花 / を / ひき / 隠 / し / て / 、 / 御 / 簾 / 押し / 上げ / て / 眺め / たまへ / る / さま、 / 夢 / に / も / 、 / かか / る / 人 / の / 親 / に / て / 、 / 重 / き / 位 / と / 見え / たまは / ず / 、 / 若 / う / なまめかし / き / 御 / さま / な / り。 / 
八重 / 立つ / 雲 / を / 君 / や / 隔つる / 阿闍梨 / 、 / こ / の / 御 / 使 / を / 先に / 立て / て / 、 / かの / 宮 / に / 参 / り / ぬ / 。 / 
御 / 幸ひ / の / 、 / かく / 引き / かへ / すぐれ / たまへ / り / ける / を / 、 / 世 / の / 人 / おどろき / きこゆ / 。 / 
兄弟 / の / 童 / 殿上 / する / 、 / 常に / こ / の / 君 / に / 参り / 仕うまつ / る / を / 、 / 例 / よりも / なつかし / う / 語らひ / たまひ / て / 、 / 五節 / は / いつ / か / 内裏 / へ / 参 / る / と / 問ひ / たまふ / 。 / 
すこし / も / ゆゑ / あ / らむ / 女 / の / 、 / かの / 親王 / より / ほか / に / 、 / また / 言の葉 / を / 交は / す / べ / き / 人 / こそ / 世 / に / おぼえ / ね / 。 / 
かやうの / こと / を / 、 / 大将 / の / 君 / も / 、 / げに / こそ / 、 / あり / がた / き / 世 / な / り / けれ / 。 / 
麗景殿 / と / 聞こゆ。 / 
いらへ / たま / は / む / 言 / もな / く / て / 、 / た / だ / うち / 嘆き / たまへ / る / ほど / 、 / 忍び / やか / に / 、 / うつく / し / く / いと / なつかし / き / に / 、 / なほ / え / 忍ぶ / まじ / く / 、 / 御 / 服 / も / 、 / こ / の / 月 / に / は / 脱がせ / たまふ / べ / き / を / 、 / 日 / ついで / なむ吉ろし / か / ら / ざ / り / ける / 。 / 
かやうに / 、 / あ / わたたし / き / ほど / に / 、 / さらに / 思 / ひ / 出で / たまふ / けしき / 見 / え / で / 月日 / 経 / ぬ。 / 
光 / 見え / つ / る / 方 / の / 障子 / を / 押し開け / たまひ / て / 、 / 空 / の / あはれ / な / る / を / もろともに / 見 / たまふ。 / 
いと / いた / く / 眺め / て / 、 / 端近く / 寄り / 臥し / たまへ / る / に / 、 / 来 / て / 、 / ね / う / 、 / ね / うと / 、 / いと / らうた / げ / に / 鳴 / けば、 / かき / 撫で / て / 、 / うたて / も / 、 / すすむ / かな / と / 、 / ほほ笑ま / る / 。 / 
心ばへ / さへ / なつかし / う / あはれ / に / て / 、 / 睦れ / 遊び / たまへ / ば / 、 / いと / らうた / く / おぼゆ / 。 / 
かく / 久しう / わづらふ / 人 / は / 、 / むつかし / き / こと / 、 / おのづから / あ / る / べ / き / を / 、 / いささか / 衰へ / ず / 、 / いと / きよげに / 、 / ねぢけ / た / る / ところ / な / く / の / み / ものし / たまひ / て / 、 / 限り / と / 見え / ながら / も / 、 / か / く / て / 生き / た / る / わざ / なり / けり / など / 、 / お / ほなおほな / 泣く泣く / のたまへ / ば / 、 / 見 / つけ / し / より / 、 / 珍か / なる / 人 / のみ / ありさま / かな / 。 / 
今 / は / 、 / ひとつ / 口 / に / 言葉 / な / 交ぜられ / そ / 。 / 
```
