# Moddable SDKを使用してアプリケーション用のフォントを作成する
Copyright 2017-2024 Moddable Tech, Inc.<BR>
改訂日： 2024年3月21日

Moddableはフォントに[BMFont](http://www.angelcode.com/products/bmfont/doc/file_format.html)形式を使用します

Moddable SDKは任意の入力文字（最大4バイトのUTF-8値）をサポートしており、Unicode表現を持つグリフで書かれた任意の言語のグリフをレンダリングすることが可能です。

各フォントアセットは（.png)グリフファイルと、対応するフォントメトリクスファイル（.fnt)で構成され、BMFontバイナリ形式で保存されます。

これにより、アンチエイリアスまたは非アンチエイリアスのフォントセットを必要に応じて生成することができます。Moddable SDKのPocoレンダラーはフォントスケーラーを実装していないため、必要なフォントサイズ/ウェイトごとに生成し、アプリケーションにアセットとして含める必要があります。

BMFontはフォント内の任意の数のグリフを許可します。
Moddableは非連続グリフ範囲をサポートしており、開発者やグラフィックデザイナーがアプリケーションに必要な文字だけを含む効率的なフォントアセットを作成することができます。これにより、アセットのストレージサイズを大幅に削減することができます。

フォントは黒で生成されます。これにより、Moddable SDKのビルドツールが文字のアルファマスクを作成できます。フォントをカラーでレンダリングするには、アプリケーションがスタイルを適用します。

BMFont形式でTrueTypeおよびOpenTypeフォントを変換するためのツールは多数あります。Moddableチームはよく[Glyph Designer](https://71squared.com/glyphdesigner)（macOS）を使用します。もう1つの良い選択肢は、Vladimir Gamalyanによるコマンドラインツール[fontbm](https://github.com/vladimirgamalyan/fontbm)です。両方の使用方法についての指示は以下に提供されています。

## Glyph Designerの使い方

フォントとウェイトを選択します。フォントの色を黒に設定し、アウトラインをなしにします。

![](../assets/create-fonts/screen01.png)

文字選択ツールを使用して必要な文字を選択します。

![](../assets/create-fonts/screen02.png)

BMFontバイナリ形式で.pngグリフと.fntフォントメトリクスファイルをエクスポートします。

![](../assets/create-fonts/screen03.png)

## fontbmの使い方

fontbmを使用するには、まずシステムにインストールする必要があります。Windowsユーザー向けには、[ダウンロード可能なバイナリ](https://github.com/vladimirgamalyan/fontbm/releases/latest)が用意されています。現在[macOS](https://github.com/vladimirgamalyan/fontbm#building-macos)および[Linux](https://github.com/vladimirgamalyan/fontbm#building-linux)ユーザーは、手順に従ってソースコードからビルドする必要があります。

fontbmを入手したら、コマンドラインから使用して、Moddable SDKにフォントを追加するために必要な.fntファイルと.pngファイルを生成できます。

Moddable SDKプロジェクトのmanifest.jsonファイルを使用して、プロジェクトのビルドの一部としてfontbmを自動的に呼び出すことができます。これにより、プロジェクトで異なるフォントやフォントサイズを使用するのが非常に簡単になります。`mcconfig`や`mcrun`でfontbmサポートを使用するには、まず環境変数`FONTBM`をfontbm実行ファイルへのパスに設定して、Moddable SDKビルドツールがfontbmを見つけられるようにする必要があります。

### manifest.jsonでのfontbmの使用

TrueTypeフォントをマニフェストに追加するのは、事前にビルドされたBMFontsを追加するのと似ています。以下は、マニフェスト内の圧縮BMFontの例です：

```json
"resources":{
	"*-mask": [
		"$(MODDABLE)/examples/assets/fonts/OpenSans-Regular-20"
	]
}
```
TrueTypeフォントを使用する場合、次のようになります：

```json
"resources": {
	"*-mask": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/OpenSans/OpenSans-Regular",
			"size": 20
		}
	]
}
```

これにより、`OpenSans-Regular-20.fnt`と`OpenSans-Regular-20-alpha.bmp`の2つのリソースが生成されます。`size`プロパティは必須です：デフォルトのサイズはありません。

同様に、事前にビルドされた非圧縮BMFontは次のようになります：

```json
"resources":{
	"*-alpha": [
		"$(MODDABLE)/examples/assets/fonts/OpenSans-Regular-24"
	]
}
```

TrueTypeフォントを使用するには、次のようになります：

```json
"resources": {
	"*-alpha": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/OpenSans/OpenSans-Regular",
			"size": 24
		}
	]
}
```

これにより、1つのリソース `OpenSans-Regular-20.bm4` が生成されます。

#### 名前付け
TrueTypeフォントファイルの名前がフォントリソースの名前として適切でないことがよくあります。`name` プロパティを使用して、代わりに使用する名前を指定できます：

```json
"resources": {
	"*-mask": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/FiraMono/FiraMono-Regular",
			"size": 22,
			"name": "FiraMono-Regular"
		}
	]
}
```
これにより、1つのリソース `FiraMono-Regular-22.bm4` が生成されます。

#### 含めるグリフの選択
デフォルトでは、変換プロセスによって出力されるフォントには、Unicode文字の32から127までが含まれます。この動作を上書きするプロパティが4つあります：`characters`、`characterFiles`、`blocks`、および `localization`。これらのプロパティが複数存在する場合、それらが選択するすべての文字が含まれます。

##### 文字リスト
`characters` プロパティは、含める文字を示す文字列です。この例では、10進数を表示するために使用される文字のみが含まれます：

```json
"resources": {
	"*-mask": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/OpenSans/OpenSans-Regular",
			"size": 72,
			"characters": "0123456789-+.e"
		}
	]
}
```

##### 文字リストファイル
`characterFiles` プロパティは、UTF-8エンコードされたテキストファイルへのパスの配列です。ファイルで使用されているすべての文字が含まれます。

```json
"resources": {
	"*-mask": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/SourceCodePro/SourceCodePro-Regular",
			"size": 32,
			"characterFiles": [
				"$(MODDABLE)/examples/helloworld/main.js"
			]
		}
	]
}
```

##### Unicode ブロック
`blocks` プロパティは、[Unicode ブロック](https://en.wikipedia.org/wiki/Unicode_block) 名の配列です。これらは文字のグループを含めるための便利な方法です。例えば、ここではBasic Latin 1とキリル文字を含めるためにブロックが使用されています：

```json
"resources": {
	"*-mask": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/OpenSans/OpenSans-Regular",
			"size": 24,
			"blocks": ["Basic Latin", "Cyrillic"],
		}
	]
}
```

##### ローカライゼーション文字サマリー
Piuユーザーインターフェースフレームワークには、ローカライゼーションのサポートが組み込まれています。Piuのローカライゼーションマップを構築する一環として、`mclocal` ツールはローカライゼーションテーブルで使用されるすべての文字のリストも生成します。このリストは、`localization` プロパティを `true` に設定することでフォントに含まれるグリフを選択するために使用できます：

```json
"resources": {
	"*-mask": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/OpenSans/OpenSans-Regular",
			"size": 24,
			"localization": true
		}
	]
}
```


#### カーニング
デフォルトでは、カーニングテーブルは出力されません。カーニングは `kern` プロパティを `true` に設定することで有効になります：

```json
"resources": {
	"*-mask": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/OpenSans/OpenSans-Regular",
			"size": 24,
			"kern": true
		}
	]
}
```
Pocoレンダラーは、パフォーマンスを最大化するためにデフォルトではカーニングを行いません。カーニングサポートは、以下の設定でマニフェストにて有効にできます。

```json
"defines": {
	"cfe": {
		"kern": true
	}
}
```

#### モノクローム
デフォルトでは、出力されるグリフはアンチエイリアスされています。モノクローム出力（Moddable ThreeやModdable Fourのような1ビットディスプレイに最適）は、`monochrome`プロパティを`true`に設定することで生成できます。

```json
"resources": {
	"*-mask": [
		{
			"source": "$(MODDABLE)/examples/assets/scalablefonts/OpenSans/OpenSans-Regular",
			"size": 24,
			"monochrome": true
		}
	]
}
```
