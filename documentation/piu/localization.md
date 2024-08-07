# ローカライゼーション
Copyright 2017 Moddable Tech, Inc.<BR>
改訂： 2017年3月3日

## 辞書

JavaScriptを使用する場合、文字列をローカライズする最も明白な形式は辞書です。アプリケーションは共通のキーを使用してローカライズされた文字列にアクセスします。

	var en = {
		"I love you": "I love you",
		"Me neither": "Me neither",
	};

	var fr = {
		"I love you": "Je t'aime",
		"Me neither": "Moi non plus",
	};

	var language = fr;
	function localize(it) {
		return language[it];
	}

同音異義語や文脈などの理由で、英語の文字列をキーとして使用することが常に可能であるとは限りません。しかし、可能な場合はそれを推奨します。コードが読みやすくなり、明らかな冗長性が避けられます。

ここに示した簡単な例は、以下の解決策を比較するために使用されています。もちろん、実際のアプリケーションは何千ものエントリ、小さなキーと大きなキー、値を持つ辞書を使用するため、状況に応じて掛け合わせてください。

## JSONファイル

通常、アプリケーションは一度に1つの言語のみを必要とするため、辞書はJSONファイルにすることができ、ユーザーが言語を選択するときにロードおよびアンロードされます。

#### en.json

	{"I love you":"I love you","Me neither":"Me neither"}

#### fr.json

	{"I love you":"Je t'aime","Me neither":"Moi non plus"}

JSONファイルをROMに保存するのは無駄です。なぜなら、すべての辞書がすべてのキーを定義しなければならないからです。例えば、上記の例では、`I love you`と`Me neither`のキーが英語とフランス語の辞書で繰り返されています。

ROM|サイズ
:---|---:
en.json|53バイト
fr.json|54バイト

JSONファイルを読み込むと大量のRAMを使用します。まず、キーがXSシンボルテーブルに登録されます。次に、辞書は自身のために1つのスロット、エントリごとに1つのスロット、文字列ごとに1つのチャンクを割り当てます。

RAM|サイズ
:---|---:
シンボル|64バイト
en.json|80バイト
fr.json|84バイト


## JavaScriptモジュール

JSONファイルの代わりに、辞書をXSがコンパイル、リンク、ROMにプリロードできるJavaScriptモジュールにすることができます。

#### en.js

	export default {"I love you":"I love you","Me neither":"Me neither"}

#### fr.js

	export default {"I love you":"Je t'aime","Me neither":"Moi non plus"}

これにより、ROM内の冗長なキーを避け、RAMを使用しません。ただし、このプロセスでもキーでXSシンボルテーブルを埋め、モジュール、エクスポート、オブジェクトごとに辞書に6つのスロットを使用し、エントリごとに1つのスロットを使用します。

ROM|Size
:---|---:
symbols|64 バイト
en.js|160 バイト
fr.js|164 バイト

さらに、XSにはそのようなオブジェクトに対する特別な処理がないため、多くのエントリを持つ辞書においてROM内の文字列を検索する時間はかなりのものになります。

## 文字列テーブル

最初の最適化は、JSONファイルやJavaScriptモジュールの代わりに文字列テーブルを使用することです。各テーブルはテーブルの長さとテーブル内の文字列のオフセットから始まります。すべての数値はリトルエンディアンの32ビット整数です。

#### locals.en.mhr

	2 12 23 I love you Me neither

#### locals.fr.mhr

	2 12 22 Je t'aime Moi non plus

Moddableアプリケーションのほとんどのリソースと同様に、文字列テーブルはRAMにロードされることはありません。XSは読み取り専用の文字列を使用できるため、JavaScriptの文字列は文字列テーブル内の文字列を参照することができます。

ROM|Size
:---|---:
locals.en.mhr|34 バイト
locals.fr.mhr|35 バイト

ツールはJSONファイルから文字列テーブルを生成できます。アプリケーションはホスト関数を通じてテーブルから文字列を取得できます。

## 文字列インデックス

もちろん、キーをインデックスにマッピングするための何かが必要です。例えば、`I love you` は `0` に、`Me neither` は `1` にマッピングされます。

再び辞書を使用することができますが、少なくともすべての言語に対して1つの辞書だけが必要です。

	var locals = {
		"I love you": 0,
    	"Me neither": 1,
	};
	var en = new StringTable("locals.en.mhr");
	var fr = new StringTable("locals.fr.mhr");
	var language = fr;
	function localize(it) {
		return language.get(locals[it]);
	}

しかし、そのような辞書には前述の欠点があります。つまり、XSシンボルテーブルを埋めることと、インデックスを検索するのに時間がかかることです。

## 最小完全ハッシュ法

すべてのキーと結果が既知の場合、完全ハッシュ関数はキーを衝突なしに結果にマッピングできます。実用的な解決策は、2つのハッシュ関数と中間テーブルを使用することです。最初のハッシュ関数はキーをシードにマッピングします。2番目のハッシュ関数はシードを使用してキーを結果にマッピングします。シードテーブルは疎である可能性がありますが、シードと結果のテーブルにはそれぞれ1つのエントリしか含まれません。シードを見つけるのには時間がかかりますが、ビルド時にツールによって行われます。

ここにいくつかの参考資料があります：

- [CMPH](http://cmph.sourceforge.net/index.html): 多くのアルゴリズムと説明が含まれているCライブラリ。
- [Steve Hanov's blog](http://stevehanov.ca/blog/index.php?id=119): PHPでの実用的な解決策の詳細なプレゼンテーション。
- [mixu/perfect](https://github.com/mixu/perfect): node.jsのポート。

ここでの結果は文字列テーブルへのインデックスです。したがって、結果テーブルを保存する必要はなく、結果テーブルは文字列テーブルを並べ替えるためにのみ使用されます。

保存する必要があるのはシードテーブルだけです。負のシードは、2番目のハッシュ関数をスキップできることを示し、インデックスはシードから1を引いた絶対値です。

#### locals.mhi (release)
	2 -2 -1

アプリケーションが有効なキーのみを使用する場合、キー自体を保存する必要はありません。無効なキーのデバッグのために、デバッグテーブルも生成できます。デバッグテーブルには、結果テーブルによって並べ替えられたキーが含まれています。キーがインデックスにマッピングされると、デバッグテーブルを使用してキーが一致するかどうかを確認できます。デバッグテーブルはシードテーブルに追加されます。

#### locals.mhi (debug)

	2 -2 -1 2 24 35 I love you Me neither

この技術を使用すると：

- テーブルはROMにありますが、JavaScriptモジュールよりも小さくなります。
- ローカライゼーションはXSシンボルテーブルを埋めません。
- 辞書のサイズに関係なく、ルックアップは大幅に高速です。

ROM|サイズ
:---|---:
locals.mhi (release)|12バイト
locals.mhi (debug)|46バイト

## mclocal

**mclocal**は、JSONファイルから文字列テーブルとシードテーブルを生成するコマンドラインツールです。

例えば

	mclocal en.json fr.json

は、上記の`locals.en.mhr`、`locals.fr.mhr`および`locals.mhi`を生成します。

**mclocal**は、すべてのJSONファイルからキーを統合し、JSONファイル内の欠落キーを報告します。

慣例により、**mcconfig**は、`strings`ディレクトリ内のすべてのJSONファイルに対して**mclocal**を呼び出すルールを持つmakefileを生成します。

### 引数

	mclocal file+ [-d] [-o directory] [-r name]

- `file+`: 1つ以上のJSONファイル。
- `-d`: デバッグテーブルを生成する。
- `-o directory`: 出力ディレクトリ。デフォルトは現在のディレクトリです。
- `-r name`: 出力ファイルの名前。デフォルトは`locals`です。

## Locals

Piuは、ローカライズされた文字列を取得し、言語を切り替えるためのクラス `Locals` を定義します。

	var locals = new Locals;

コンストラクタは2つの引数 `name` と `language` を取ります。デフォルトは `locals` と `en` です。リソースは `name`、`language`、および `.mhi` または `.mhr` 拡張子を組み合わせてアクセスされます。

アプリケーションはアクセサを使用して言語を切り替えます。

	var what = locals.get("I love you"); // what == "I love you"
	locals.language = "fr";
	var quoi = locals.get("I love you");	 // quoi == "Je t'aime"

便利なように、アプリケーションは文字列をローカライズするためのグローバル関数を定義できます。

	global.localize = function(it) {
		return locals.get(it);
	}
