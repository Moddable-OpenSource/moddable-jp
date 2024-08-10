# Poco
Copyright 2016-2020 Moddable Tech, Inc.<BR>
改訂： 2020年11月4日

## このドキュメントについて

このドキュメントは、Pocoレンダラーについて、Pocoを使う上での主要な概念を紹介する一連のサンプルから説明します。サンプルに続いて、Pocoのリファレンスがあり、各関数の呼び出しが完全に説明されています。

## Table of Contents

* [サンプル](#examples)
	* [長方形](#rectangle)
	* [原点](#origin)
	* [クリップ](#clip)
	* [モノクロビットマップ](#monochrome-bitmap)
	* [カラービットマップ](#color-bitmap)
	* [パターン](#pattern)
	* [グレービットマップ](#gray-bitmap)
	* [オフスクリーンビットマップ](#offscreen-bitmap)
	* [アルファ](#alpha)
	* [JPEG](#jpeg)
	* [テキスト](#text)
* [ピクセルフォーマット](#pixel-formats)
	* [出力ピクセルフォーマット](#destination-pixel-formats)
	* [ディスプレイピクセルフォーマット](#display-pixel-format)
	* [入力ビットマップピクセルフォーマット](#source-bitmap-pixel-formats)
	* [圧縮ピクセルフォーマット](#compressed-pixel-formats)
* [即時モードレンダリング](#immediate-mode-rendering)
* [回転](#rotation)
* [JavaScript APIリファレンス](#javascript-api-reference)
	* [関数](#js-functions)
	* [プロパティ](#js-properties)
* [C APIリファレンス](#c-api-reference)
	* [データ構造](#c-data-structures)
	* [関数](#c-functions)
* [その他](#odds-and-ends)

<a id="examples"></a>
## サンプル

これらのサンプルはPocoレンダラーを使用する方法を解説しています。すべてのサンプルはJavaScriptAPIを使用し、またCommodettoのアセットローダーやその他の機能も利用しています。

以下のサンプルを簡潔かつ集中して説明するために、コードではいくつかの前提条件を設定しています。

- サンプルでは、グローバル変数`screen`に`PixelsOut`オブジェクトがあることを前提としています

- 以下のカラー変数が定義されていることを前提としています：

	```javascript
	let white = poco.makeColor(255, 255, 255);
	let black = poco.makeColor(0, 0, 0);
	let gray = poco.makeColor(128, 128, 128);
	let red = poco.makeColor(255, 0, 0);
	let green = poco.makeColor(0, 255, 0);
	let blue = poco.makeColor(0, 0, 255);
	```

- 描画コマンドが`begin`と`end`の呼び出しの間に実行されることを前提としています。

	```javascript
	let poco = new Poco(screen);
	poco.begin();
	...	// example code here
	poco.end();
	```

各サンプルには、コードによってレンダリングされた画像が含まれています。画像は見やすくするために150％に拡大されています。この拡大によって一部のぼやけやジャギーが生じることがありますが、実際の画像にはこれらは含まれません。

***

<a id="rectangle"></a>
### 長方形

このサンプルでは、`screen`を灰色のピクセルで埋め、左半分を赤色のピクセルで覆い、その後、画面の中央半分に50%のブレンドレベル（128）で青色のピクセルを描画します。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);
poco.fillRectangle(red, 0, 0, poco.width / 2, poco.height);
poco.blendRectangle(blue, 128, poco.width / 4, 0, poco.width / 2, poco.height);
```

<img src="../assets/poco/fillrectangle.png" width="180" height="135"/>

***

<a id="origin"></a>
### 原点

このサンプルでは、描画の原点を移動する方法を示します。Pocoは、原点が変更されるときにプッシュされ、引数なしで `origin`が呼び出されるとポップされる原点スタックを保持します。各原点の変更では、前の原点からの相対的な移動を行います。原点スタックは、コンテナベースのユーザーインターフェースを構築するのに便利です。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);

poco.origin(10, 10);
poco.fillRectangle(red, 0, 0, 40, 20);

poco.origin(25, 25);
poco.fillRectangle(green, 0, 0, 40, 20);

poco.origin(25, 25);
poco.fillRectangle(blue, 0, 0, 40, 20);

poco.blendRectangle(black, 128, -4, -4, 20, 10);
poco.origin();

poco.blendRectangle(black, 128, -4, -4, 20, 10);
poco.origin();

poco.blendRectangle(black, 128, -4, -4, 20, 10);
poco.origin();
```

<img src="../assets/poco/origin.png" width="180" height="135"/>

***

<a id="clip"></a>
### クリップ

このサンプルでは、描画のクリップを使用する方法を示します。Pocoは、クリップが変更されるときにプッシュされ、引数なしで`clip`が呼び出されるとポップされるクリップスタックを保持します。各クリップの変更では、前のクリップと交差する領域に描画を制限します。クリップスタックは、コンテナベースのユーザーインターフェースを構築するのに便利です。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);

poco.clip(20, 20, poco.width - 40, poco.height - 40);
poco.fillRectangle(green, 0, 0, poco.width, poco.height);

poco.clip(0, 0, 40, 40);
poco.fillRectangle(blue, 0, 0, poco.width, poco.height);

poco.fillRectangle(white, 26, 0, 2, poco.height);

poco.clip();
poco.fillRectangle(red, 30, 0, 2, poco.height);

poco.clip();
poco.fillRectangle(black, 34, 0, 2, poco.height);
```

<img src="../assets/poco/clip.png" width="180" height="135"/>

***

<a id="monochrome-bitmap"></a>
### モノクロビットマップ

このサンプルでは、ピクセルがすべて黒または白である封筒のモノクロビットマップを描画します。前景および背景ピクセルの色を制御する方法や、それぞれのピクセルが描画されるかどうかを示しています。ビットマップは32 x 23のサイズの1ビットBMPファイルに格納されています。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);

let envelope = parseBMP(new Resource("envelope.bmp"));
poco.drawMonochrome(envelope, black, white, 14, 10);
poco.drawMonochrome(envelope, red, white, 14, 55);
poco.drawMonochrome(envelope, green, undefined, 74, 10);
poco.drawMonochrome(envelope, undefined, blue, 74, 55);
```

<img src="../assets/poco/monochrome.png" width="180" height="135"/>

***

<a id="color-bitmap"></a>
### カラービットマップ

このサンプルでは、顔のカラービットマップ画像を2つの方法で`drawBitmap`を使用して描画します。画面の左側には全体の画像を描画します。右側には`drawBitmap`のオプション引数のソース矩形パラメーターを使って、目と口だけを描画しています。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);

let image = parseBMP(new Resource("lvb.bmp"));

let x = 0;
let y = Math.round((poco.height - image.height) / 2);
poco.drawBitmap(image, x, y);

x = image.width;
poco.drawBitmap(image, x + 25, y + 38, 25, 38, 11, 7);   // left eye
poco.drawBitmap(image, x +  7, y + 40,  7, 40, 10, 6);   // right eye
poco.drawBitmap(image, x + 15, y + 56, 15, 56, 16, 6);   // mouth
```

<img src="../assets/poco/bitmap.png" width="180" height="135"/>

***

<a id="pattern"></a>
### パターン

このサンプルでは、`fillPattern`を使用して、30ピクセル四方のパターンを2つの方法で描画します。最初に画面全体をそのパターンで埋め、その後パターンの7ピクセル四方の領域を使用して画面中央部分を埋めます。

前のサンプルとは異なり、最初に画面をクリアするために`fillRectangle`を呼び出しません。これは、最初の`fillPattern`の呼び出しで画面のすべてのピクセルが覆われるためです。

```javascript
let pattern = parseBMP(new Resource("pattern1.bmp"));
poco.fillPattern(pattern, 0, 0, poco.width, poco.height);
poco.fillPattern(pattern, 28, 28, 63, 35, 21, 14, 7, 7);
```

<img src="../assets/poco/pattern.png" width="180" height="135"/>

***

<a id="gray-bitmap"></a>
### グレービットマップ

このサンプルでは、`drawGray`を使用して16階調のグレー画像を複数の色で描画します。`drawGray`はピクセル値をアルファブレンドレベルとして扱い、指定された色を背景とブレンドします。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);

let image = parseBMP(new Resource("envelope-gray.bmp"));

poco.drawGray(image, black, 10, 2);
poco.drawGray(image, white, 10, 47);

poco.drawGray(image, black, 70, 2);
poco.drawGray(image, green, 70 + 2, 2 + 2);

poco.drawGray(image, white, 70, 47);
poco.drawGray(image, red, 70 + 2, 47 + 2);
```

<img src="../assets/poco/gray.png" width="180" height="135"/>

***

<a id="offscreen-bitmap"></a>
### オフスクリーンビットマップ

このサンプルでは、`BufferOut`を使用してオフスクリーンビットマップを作成し、一連の内側に向かう四角形でビットマップを埋めます。次に、そのオフスクリーンビットマップをパターンとして使用して画面を埋めます。このサンプルでは、2つの`Poco`インスタンスを使用します。最初のインスタンスはオフスクリーンビットマップに描画し、2番目のインスタンスは画面に描画します。

```javascript
import BufferOut from "commodetto/BufferOut";

let offscreen = new BufferOut({width: 30, height: 30, pixelFormat: poco.pixelsOut.pixelFormat});
let pocoOff = new Poco(offscreen);
pocoOff.begin();
	pocoOff.fillRectangle(gray, 0, 0, 30, 30);
	pocoOff.fillRectangle(red, 2, 2, 26, 26);
	pocoOff.fillRectangle(black, 4, 4, 22, 22);
	pocoOff.fillRectangle(blue, 6, 6, 18, 18);
	pocoOff.fillRectangle(white, 8, 8, 14, 14);
	pocoOff.fillRectangle(green, 10, 10, 10, 10);
	pocoOff.fillRectangle(gray, 13, 13, 4, 4);
pocoOff.end();

poco.fillPattern(offscreen.bitmap, 0, 0, poco.width, poco.height);
```

<img src="../assets/poco/offscreen.png" width="180" height="135"/>

***

<a id="alpha"></a>
### アルファ

このサンプルでは、アルファマスクを使用してビットマップを描画する方法を示します。描画するビットマップとマスクは別々のビットマップとして存在し、1つの画像に対して複数のアルファマスクを使用して描画することができます。このサンプルでは、1つのビットマップをぞれぞれ円形マスクと四角形マスクを通して描画し、さらに元の画像とマスクも描画します。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);

let girl = parseBMP(new Resource("girl.bmp"));
let circle = parseBMP(new Resource("mask_circle.bmp"));
let square = parseBMP(new Resource("mask_square.bmp"));

poco.drawBitmap(girl, 0, 2);
poco.drawGray(circle, black, 40, 2);
poco.drawMasked(girl, 80, 2, 0, 0,
				circle.width, circle.height, circle, 0, 0);

poco.drawBitmap(girl, 0, 47);
poco.drawGray(square, black, 40, 47);
poco.drawMasked(girl, 80, 47, 0, 0,
				square.width, square.height, square, 0, 0);
```

<img src="../assets/poco/alpha.png" width="180" height="135"/>

***

<a id="jpeg"></a>
### JPEG

これらの2つのサンプルでは、JPEG画像を扱う異なる方法を示します。最初のサンプルは、JPEG全体をメモリ内のビットマップに解凍し、次にそのビットマップを画面に描画します。`trace`の呼び出しは、ビットマップの幅と高さにアクセスする方法を示しています。

```javascript
import JPEG from "commodetto/readJPEG";

let piano = JPEG.decompress(new Resource("piano.jpg"));
trace(`width ${piano.width}, height ${piano.height}\n`);
poco.drawBitmap(piano, 0, 0);
```

2つ目のサンプルでは、JPEG画像を1ブロックずつデコードし、次のブロックをデコードする前にそのブロックを画面に描画します。このアプローチの利点は、メモリに必要なのが通常8 x 8または16 x 16ピクセルの単一のJPEGブロックだけで済むことです。欠点は、JPEG画像が一度にすべて表示されるのではなく、ユーザーにはブロックごとに表示されることです。

```javascript
let jpeg = new JPEG(new Resource("piano.jpg"));
let block;
while (block = jpeg.read()) {
	poco.begin(block.x, block.y, block.width, block.height);
		poco.drawBitmap(block, block.x, block.y);
	poco.end();
}
```

どちらの方法でも、最終的には同じ結果が得られます。

<img src="../assets/poco/jpeg.png" width="180" height="135"/>

***

<a id="text"></a>
### テキスト

Pocoはテキスト描画に使用するフォントとしてBMPFont形式をサポートしています。BMFontは、アンチエイリアス処理が施された灰色またはカラーフォントで、主にゲームで使用されます。BMFontはフォントメトリクスとフォント画像の2つのファイルから構成されます。

以下のサンプルでは、36ポイントのPalatino BMFontを読み込み、描画しています。

```javascript
import parseBMF from "commodetto/parseBMF";

poco.fillRectangle(gray, 0, 0, poco.width, poco.height);
poco.fillRectangle(white, 2, 2, poco.width - 4, poco.height - 4);

let palatino36 = parseBMF(new Resource("palatino_36.fnt"));
palatino36.bitmap = parseBMP(new Resource("palatino_36.bmp"));

poco.drawText("Hello.", palatino36, black, 4, 20);
poco.drawText("Hello.", palatino36, green, 4, 55);
```

<img src="../assets/poco/text1.png" width="180" height="135"/>

テキストを描画する際に切り詰めるためには、`drawText`のオプション引数`width`を指定して、テキストに使用できる横幅を設定します。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);
poco.fillRectangle(white, 2, 2, poco.width - 4, poco.height - 4);

let palatino36 = parseBMF(new Resource("palatino_36.fnt"));
palatino36.bitmap = parseBMP(new Resource("palatino_36.bmp"));

poco.drawText("Hello, world. This is long.", palatino36, red, 2, 10);
poco.drawText("Hello, world. This is long.", palatino36, green, 2, 45, poco.width - 2);
```

<img src="../assets/poco/text2.png" width="180" height="135"/>

フォントの`height`プロパティを使用してテキストを水平および垂直に配置し、`getTextWidth`を使用して文字列の幅を測定します。

```javascript
poco.fillRectangle(gray, 0, 0, poco.width, poco.height);
poco.fillRectangle(white, 2, 2, poco.width - 4, poco.height - 4);

let palatino12 = parseBMF(new Resource("OpenSans-SemiboldItalic-18.fnt"));
palatino12.bitmap = parseBMP(new Resource("OpenSans-SemiboldItalic-18.bmp"));

poco.drawText("T Left", palatino12, red,
			  2, 2);
poco.drawText("T Right", palatino12, green,
			  poco.width - 2 - poco.getTextWidth("T Right", palatino12), 2);

poco.drawText("B Left", palatino12, blue,
			  2, poco.height - 2 - palatino12.height);
poco.drawText("B Right", palatino12, gray,
			  poco.width - 2 - poco.getTextWidth("B Right", palatino12),
			  poco.height - 2 - palatino12.height);

poco.drawText("Centered", palatino12, black,
			  (poco.width - poco.getTextWidth("Centered", palatino12)) / 2,
			  (poco.height - palatino12.height) / 2);
```

<img src="../assets/poco/text3.png" width="180" height="135"/>

`drawText`関数は、16階調のグレーレベルアルファビットマップを`color`引数に受け取ることもできます。このビットマップを使用して、複数の色で構成されたフォントを描画できます。以下のサンプルでは、Open Sans Bold Italicを黒と白のピクセルで描画しています。

```javascript
poco.fillRectangle(green, 0, 0, screen.width, screen.height);

let openSans52 = parseBMF(new Resource("OpenSans-BoldItalic-52.fnt"));
openSans52.bitmap = parseBMP(new Resource("OpenSans-BoldItalic-52-color.bmp"));
openSans52.mask = parseBMP(new Resource("OpenSans-BoldItalic-52-alpha.bmp"));

poco.drawText("Poco", openSans52, openSans52.mask, 0, 5);
```

<img src="../assets/poco/text4.png" width="180" height="135"/>

以下の画像は、`OpenSans-BoldItalic-52.bmp`ファイルの一部で、グリフイメージが含まれています。

<img src="../assets/poco/text_opensans_glyphs.png"/>

こちらは、グリフイメージのアルファチャンネルが含まれている`OpenSans-BoldItalic-52-alpha.bmp`ファイルの一部です。

<img src="../assets/poco/text_opensans_mask.png"/>

***

<a id="pixel-formats"></a>
## ピクセルフォーマット

<a id="destination-pixel-formats"></a>
### 出力ピクセルフォーマット
Pocoは以下のピクセルフォーマットでレンダリングします：

- 16ビットRGB565リトルエンディアン
- 8ビットRGB332
- 8ビットグレー
- 4ビットインデックスカラー
- 4ビットグレー

ターゲットデバイスにデプロイするコードのサイズを小さく保つために、Pocoはビルド時にこれらのピクセルフォーマットのうち1つだけをレンダリングするように設定されます。

<a id="display-pixel-format"></a>
### ディスプレイピクセルフォーマット
異なる出力形式を必要とするディスプレイの場合、ディスプレイドライバーはサポートされているレンダリングフォーマットとハードウェアが要求するフォーマットの間で変換を行います。例えば、多くの一般的なLCDコントローラーは16ビットのRGB565ビッグエンディアンピクセルを必要とします。これらのコントローラー用のディスプレイドライバーは、Pocoがレンダリングした16ビットRGB565リトルエンディアンピクセルを受け取り、LCDコントローラーに送信する際にビッグエンディアンに変換します。同様に、多くのディスプレイはモノクロ（1ビット）です。これらのディスプレイドライバーは4ビットまたは8ビットのグレーでレンダリングされたピクセルを受け取り、モノクロディスプレイ用に1ビットに変換します。

<a id="source-bitmap-pixel-formats"></a>
### 入力ビットマップピクセルフォーマット
Pocoは、すべてのピクセルフォーマットへのレンダリング用ソースとして、1ビットモノクロームおよび4ビットグレービットマップをサポートしています。さらに、設定された出力ピクセルフォーマットも常にソースフォーマットとしてサポートされます。例えば、レンダリングピクセルフォーマットが16ビットRGB565リトルエンディアンの場合、サポートされるソースピクセルフォーマットは1ビットモノクローム、4ビットグレー、および16ビットRGB565リトルエンディアンです。

<a id="compressed-pixel-formats"></a>
### 圧縮ピクセルフォーマット
Pocoは、2つの圧縮ピクセルフォーマットをサポートしています。

1つ目は、4ビットグレービットマップの重み付きランレングス圧縮です。これは、アンチエイリアスフォントやイメージマスクによく使用されます。これらは`CommodettoBitmap`および`PocoBitmap`のデータ構造を使用し、ピクセルフォーマットが`(kCommodettoBitmapGray16 | kCommodettoBitmapPacked)`に設定されています。

2つ目は、フルカラー画像を圧縮するために使用されるColorCellアルゴリズムの一種です。これらは`CommodettoBitmap`や`PocoBitmap`のデータ構造で参照されることはなく、BMP、PNG、JPEGと同様に画像ファイルとして扱われます。ColorCell画像は16ビットRGB565リトルエンディアンピクセルを使用するため、16ビットRGB565リトルエンディアンの出力先でのみレンダリングされます。

***

<a id="immediate-mode-rendering"></a>
## 即時モードレンダリング
デフォルトでは、Pocoはスキャンライン表示リストレンダラーです。つまり、すべての描画コマンドを保存し、指定されたフレームのすべての描画コマンドがキューに入ったときに、それらを一度にレンダリングします。Pocoがアクセス可能なメモリに保存されたフルフレームバッファを持ち、ダブルバッファ（例：2つのフレームバッファを交互に使用する）を持つディスプレイで使用される場合、スキャンライン表示リストレンダリングは、各描画コマンドが受信されるとすぐに実行するイミディエイトモードレンダリングよりも効率が低くなります。

Pocoはオプションで即時モードレンダリングをサポートしています。このサポートを有効にするには、Pocoをビルドする際に`kPocoFrameBuffer`を1に定義し、Cコード内で`PocoDrawingBegin`/`PocoDrawingEnd`の代わりに`PocoDrawingBeginFrameBuffer`/`PocoDrawingEndFrameBuffer`を使用します。即時モードレンダリングを使用するためにJavaScriptコードの変更は必要ありません。

***

<a id="rotation"></a>
## 回転
Pocoは、`PixelsOut`への描画を0度、90度、180度、または270度の回転でサポートしています。このサポートにより、ハードウェアのスキャン順序に関係なく、任意の向きでディスプレイを使用できます。

回転は実行時ではなくビルド時に、`kPocoRotation`をターゲットの回転角度（例：90）に定義することで選択されます。デフォルトの回転値は0です。回転はすべての描画操作の座標、出力先とソースの両方に適用されます。任意のアセット（例：保存されたビットマップやフォント）は、Pocoに渡される前に回転させる必要があります。これは手動（例：Photoshopによって）または自動（例：Moddable SDKのpng2bmpツールによって）で行います。

回転に対するこのアプローチは、回転していない画像と同じパフォーマンスレベルで、中間ビットマップバッファを必要とせずに、回転した出力をレンダリングすることができます。

***

<a id="javascript-api-reference"></a>
## JavaScript APIリファレンス

Pocoは、Commodettoの`Render`クラスのサブクラスであるレンダラーです。

```javascript
class Poco extends Render
```

<a id="js-functions"></a>
### 関数

#### `constructor(pixelsOut, dictionary)`

Pocoは`Render`のディクショナリを拡張し、ディスプレイリストバッファのサイズをバイト単位で指定する`displayListLength`プロパティを追加します。アプリケーションは通常、デフォルトのディスプレイリスト長を使用します。Pocoは描画操作がディスプレイリストをオーバーフローしそうになったときに検出し、描画操作を無視し、`end`が呼ばれたときに例外をスローします。

```javascript
import Poco from "commodetto/Poco";

let screen = ... // SPIOut instance
let poco = new Poco(screen, {displayListLength: 4096});
```

***

#### `close()`

Pocoによって割り当てられたすべてのメモリを解放します。`close`を呼び出した後、そのインスタンスの他の関数を呼び出すことはできません。

***

#### `clip(x, y, width, height)`

Pocoは、すべての描画操作に適用されるクリップ矩形を保持します。

`begin`が呼び出されると、クリップ矩形は`begin`に渡された更新領域に設定されます。Pocoはクリップスタックを保持しているため、アプリケーションが現在のクリップを保存および復元する必要はありません。4つの引数で`clip`を呼び出すと、現在のクリップと引数で指定された領域の交差部分が設定されます。引数なしで呼び出すと、クリップスタックの最後のクリップが削除され、前のクリップが復元されます。

```javascript
poco.clip(10, 10, 10, 10);
poco.clip();
```

クリップスタックは次のようにいくつかのクリップを保持します：

```javascript
poco.begin();			// Clip is entire PixelsOut area
poco.clip(10, 10, 10, 10);	// Clip is {x: 10, y: 10, w: 10, h: 10}
poco.clip(0, 0, 15, 15);	// Clip is {x: 10, y: 10, w: 5, h: 5}
poco.clip();			// Clip is {x: 10, y: 10, w: 10, h: 10}
poco.clip()			// Clip is entire PixelsOut area
poco.end();
```

クリップスタックがオーバーフローまたはアンダーフローすると、`end`から例外がスローされます。`end`が呼び出されたときにクリップスタックが空でないと、例外がスローされます。

4つの引数でclipを呼び出すと、結果の領域に1ピクセル以上含まれている場合は`true`が返され、クリップ領域が空の場合は`undefined`が返されます。

> **注意:** `clip`と`origin`は同じスタックを共有しているため、プッシュされた順序でポップする必要があります。

***

#### `origin(x, y)`

Pocoは、すべての描画操作に適用される原点を保持します。

`begin`が呼び出されると、原点は`{x: 0, y: 0}`に設定されます。Pocoは原点スタックを保持しているため、アプリケーションが現在の原点を保存および復元する必要はありません。2つの引数で`origin`を呼び出すと、現在の原点がその引数分だけオフセットされます。引数なしで呼び出すと、原点スタックの最後の原点が削除され、前の原点が復元されます。

```javascript
poco.begin();			// Origin is {x: 0, y: 0}
poco.origin(10, 10);		// Origin is {x: 10, y: 10}
poco.origin(5, 5);		// Origin is {x: 15, y: 15}
poco.origin();			// Origin is {x: 10, y: 10}
poco.origin();			// Origin is {x: 0, y: 0}
poco.end();
```

原点スタックがオーバーフローまたはアンダーフローすると、`end`から例外がスローされます。`end`が呼び出されたときに原点スタックが空でないと、例外がスローされます。

> **注意:** 原点を変更してもクリップ矩形は変更されません。また、`clip`と`origin`は同じスタックを共有しているため、プッシュされた順序でポップする必要があります。

***

#### `makeColor(r, g, b)`

`makeColor`関数は、0から255までの赤、緑、青の値を受け取り、対応するピクセル値を返します。返されるピクセルは、`Poco`インスタンスにバインドされた`PixelsOut`インスタンスのフォーマットに従います。

```javascript
let red = poco.makeColor(255, 0, 0);
let green = poco.makeColor(0, 255, 0);
let blue = poco.makeColor(0, 0, 255);
let black = poco.makeColor(0, 0, 0);
let white = poco.makeColor(255, 255, 255);
let gray = poco.makeColor(127, 127, 127);
```

多くの描画関数は、引数として色を受け取ります。ピクセルフォーマットに依存しないようにするために、色を計算するには`makeColor`を使用します。

***

#### `fillRectangle(color, x, y, width, height)`

`fillRectangle`関数は、`x`、`y`、`width`、および`height`の引数で指定された領域を指定された色で塗りつぶします。

```javascript
poco.fillRectangle(green, 10, 20, 40, 40);
```

***

#### `blendRectangle(color, blend, x, y, width, height)`

`blendRectangle`関数は、指定された色を`x`、`y`、`width`、および`height`の引数で指定された領域のピクセルとブレンドします。`blend`引数はブレンドのレベルを決定し、0は透明、255は不透明を意味します。

以下のコードは、不透明度が増加する16本の水平な緑の線を描画します。

```javascript
let green = poco.makeColor(0, 255, 0);
for (let blend = 15, y = 0; blend < 256; blend += 16, y += 1)
	poco.blendRectangle(green, blend, 0, y, pixelsOut.width, 1);
```

***

#### `drawPixel(color, x, y)`

`drawPixel`関数は、指定された色のピクセルを`x`および`y`の引数で指定された位置に描画します。

```javascript
poco.drawPixel(poco.makeColor(0, 0, 127), 5, 5);
```

> **注意:** 1フレームで`drawPixel`を多用すると、ディスプレイリストがすぐに満たされる可能性があります。

***

#### `drawBitmap(bits, x, y, sx, sy, sw, sh)`

`drawBitmap`関数は、ピクセルタイプが`Bitmap.Default`であるビットマップの全部または一部を描画します。ビットマップは`bits`引数で指定され、ビットマップを描画する位置は`x`および`y`引数で指定されます。以下のコードは、画像全体を位置`{x: 10, y: 5}`に描画します。

```javascript
let image = parseBMP(new Resource("image.bmp"));
poco.drawBitmap(image, 10, 5);
```

オプションの`sx`、`sy`、`sw`および`sh`引数は、描画するビットマップの領域を指定します。これらを省略すると、ビットマップ全体が描画されます。

以下のコードは、ビットマップの下半分を位置`{x: 0, y: 0}`に描画します。

```javascript
poco.drawBitmap(image, 0, 0, 0, image.height / 2, image.width, image.height / 2);
```

***

#### `drawMonochrome(monochrome, fore, back, x, y, sx, sy, sw, sh)`

`drawMonochrome`関数は、ピクセルタイプが`Bitmap.Monochrome`のビットマップの全部または一部を描画します。ビットマップは`bits`引数で指定され、ビットマップを描画する位置は`x`および`y`引数で指定されます。

`fore`および`back`引数は、ビットマップの白黒ピクセルに適用される前景色および背景色を指定します。`fore`が`undefined`の場合、前景ピクセルは描画されません。`back`が`undefined`の場合、背景ピクセルは描画されません。

```javascript
let red = poco.makeColor(255, 0, 0);
let gray = poco.makeColor(128, 128, 128);
let white = poco.makeColor(255, 255, 255);
let icon = parseBMP(new Resource("icon.bmp"));

poco.drawMonochrome(icon, red, white, 0, 5);  // red foreground and white background
poco.drawMonochrome(icon, gray, undefined, 0, 5);  // only foreground pixels in gray
poco.drawMonochrome(icon, undefined, red, 0, 5);  // only background pixels in red
```

オプションの`sx`、`sy`、`sw`および`sh`引数は、描画するビットマップの領域を指定します。これらを省略すると、ビットマップ全体が描画されます。

***

#### `drawGray(bits, color, x, y, sx, sy, sw, sh[, blend])`

`drawGray`関数は、ピクセルタイプが`Bitmap.Gray16`のビットマップの全部または一部を描画します。ビットマップは`bits`引数で指定され、ビットマップを描画する位置は`x`および`y`引数で指定されます。ビットマップのピクセルはアルファ値として扱われ、背景とブレンドされます。`color`引数はブレンド時に適用する色を指定します。

オプションの`sx`、`sy`、`sw`および`sh`引数は、描画するビットマップの領域を指定します。これらを省略すると、ビットマップ全体が描画されます。

オプションの`blend`引数は、背景とブレンドする前に、ビットマップ内のすべてのピクセルに追加のブレンドレベルを適用します。`blend`値は、0が透明で255が不透明です。

***

#### `drawMasked(bits, x, y, sx, sy, sw, sh, mask, mask_sx, mask_sy[, blend])`

`drawMasked`関数は、画像とアルファチャネルの2つのビットマップを使用して、マスクを通じて画像をアルファブレンドし、目的の位置に合成します。`bits`引数で指定された画像は`Bitmap.Default`フォーマットです。`mask`引数で指定されたアルファチャネルは`Bitmap.Gray16`フォーマットです。

`x`と`y`引数は出力の中で合成画像を配置する位置を指定します。`sx`、`sy`、`sw`、および`sh`引数は、使用する画像の領域を指定します。`mask_sx`と`mask_sy`引数は使用するマスクビットマップの左上隅を指定します。マスクビットマップ領域の寸法は`sw`と`sh`引数から取得されます。

以下のサンプルは、アルファチャネルを使用してボタン画像を描画する方法を示しています。画像とアルファチャネルは、それぞれアルファチャネルを持つPNGファイルから抽出され、別々のファイルに保存されています。

```javascript
let buttonImage = parseBMP(new Resource("button_image.bmp"));
let buttonAlpha = parseBMP(new Resource("button_alpha.bmp"));
poco.drawMasked(buttonImage, 0, 0, 0, 0,
	buttonImage.width, buttonImage.height, buttonAlpha, 0, 0);
```

アルファチャネルを画像とは別に保存することは珍しいですが、リソースに制約のあるデバイスでは次のような利点があります：

* アルファチャネル画像はピクセルあたり4ビットにすることができ、半分のサイズで良好な結果を得ることができます。
* 画像はアルファチャネル付きとなしの両方でレンダリングできます。
* 単一のマスクを任意の画像に適用でき、エフェクトやアニメーションを実現できます。

オプションの`blend`引数は、背景とブレンドする前にアルファチャネルビットマップのすべてのピクセルに追加のブレンドレベルを適用します。`blend`値は透明の場合は0、不透明の場合は255です。

***

#### `fillPattern(bits, x, y, w, h [, sx, sy, sw, sh])`

`fillPattern`関数は、`Bitmap.Default`タイプのピクセルで構成されるビットマップの全部または一部を繰り返し描画することで、指定された領域を埋めます。ビットマップは `bits`引数で指定されます。塗りつぶす領域の位置は`x`と`y`引数で指定され、その領域の寸法は`w`と`h`引数で指定されます。

```javascript
let pattern = parseBMP(new Resource("pattern.bmp"));
poco.fillPattern(pattern, 10, 10, 90, 90);
```

オプションの`sx`、`sy`、`sw`、および`sh`引数は、ビットマップのどの部分を使用するかを指定します。これらの引数が省略された場合、ビットマップ全体が使用されます。

```javascript
poco.fillPattern(pattern, 10, 10, 90, 90, 0, 0, 8, 8);
```

***

#### `drawText(text, font, color, x, y[, width])`

`drawText`関数は、`font`引数に指定されたBMFontを使用して`text`文字列を描画します。テキストは、`color`引数で指定された色で、`x`および`y`引数の位置に描画されます。テキストは左上揃えで描画されます。

次のコードは、`"Hello, world"`という文字列を2回描画します。最初は画面の左上隅にChicagoフォントを使用して黒で描画し、その下にPalatino 36フォントを使用して赤で描画します。

```javascript
poco.drawText("Hello, world", chicagoFont, black, 0, 0);
poco.drawText("Hello, world", palatino36, red, 0, chicagoFont.height);
```

オプションの引数`width`を指定すると、テキストが長すぎて利用可能な幅に収まらない場合、右端で切り詰められます。切り詰めが発生すると、3つのピリオド（`...`）が文字列の末尾に描画されます。

フォントに含まれていない文字列内の文字は無視されます。

エッジにアンチエイリアスをかけたフルカラーのテキストを描画するには、`Bitmap.Default`フォーマットのビットマップを持つBMFontを使用します。`color`引数の代わりに、`Bitmap.Gray16`フォーマットのマスクビットマップを渡します。マスクは少なくともBMFontのグリフアトラスと同じ大きさでなければなりません。各グリフが描画されるとき、フォント画像内のグリフに対応するマスク画像内のピクセルが、各グリフを描画先にアルファブレンドするために使用されます。

***

#### `getTextWidth(text, font)`

`getTextWidth`関数は、`font`を使用してレンダリングされたときの`text`文字列の幅をピクセル単位で計算します。

次のコードは、`"Hello, world"`という文字列をディスプレイの上部に水平中央に配置して描画します。

```javascript
let text = "Hello, world";
let width = poco.getTextWidth(text, palatino36);
poco.drawText(text, palatino36, green, (pixelsOut.width - width) / 2, 0);
```

フォントの高さは`font.height`プロパティで利用できます。

テキスト文字列内のフォントに含まれていない文字はレンダリングされません。

***

#### `drawFrame(frame, dictionary, x, y)`

`drawFrame`関数は、`frame`引数で参照されるColorCell圧縮画像を、`x`および`y`引数で指定された位置にレンダリングします。`dictionary`引数は、画像のサイズを示す幅と高さのプロパティを含む`Object`です。

***

<a id="js-properties"></a>
### プロパティ

#### `height`

回転を適用した後のPocoインスタンスのピクセル単位の論理的な高さ。回転が0または180の場合、これは`PixelsOut`インスタンスの高さに等しくなります。回転が90または270の場合、これは`PixelsOut`インスタンスの幅に等しくなります。

***

#### `width`

回転を適用した後のPocoインスタンスのピクセル単位の論理的な幅。回転が0または180の場合、これは`PixelsOut`インスタンスの幅と等しくなります。回転が90または270の場合、これは`PixelsOut`インスタンスの高さと等しくなります。

***

<a id="c-api-reference"></a>
## C APIリファレンス

Poco C APIは低レベルのレンダリングエンジンです。これはディスプレイリストに基づいており、すべての描画呼び出しはレンダリング前にリストにキューイングされることを意味します。ディスプレイリストを使用することで、レンダラーは一度に1スキャンライン分の完全な出力を生成すればよいので、アプリケーションプロセッサのメモリ内にフレームバッファを必要せず、メモリ使用量を最小限に抑えられます。

Poco C APIは、CommodettoやそのJavaScript APIとは独立して使用することができます。メモリ割り当ては行わず、ほとんど外部呼び出しも行わず（`memcpy`への呼び出しのみ）、呼び出し元のメモリ提供に依存します。

<a id="c-data-structures"></a>
### データ構造

<a id="poco-record"></a>
##### `PocoRecord`

`PocoRecord`はPocoの状態を保持します。多くのフィールドはプライベート実装であり、ライブラリのユーザーが直接アクセスしてはいけません。`PocoRecord`データ構造は0に初期化する必要があり、すべてのPoco関数呼び出しに対して同じ`PocoRecord`構造体を渡す必要があります。

次の`PocoRecord`のフィールドはパブリックであり、ライブラリのユーザーがアクセスできます。Pocoは最初の呼び出しが行われる前に、ライブラリのユーザによってこれらのフィールドが初期化されることを期待しています。

| フィールド | 説明 |
| :---: | :--- |
| `width` | ピクセル単位の出力の幅
| `height` | ピクセル単位の出力の高さ
| `displayList` | ディスプレイリストのメモリ開始位置へのポインタ
| `displayListEnd` | ディスプレイリストのメモリ終了位置へのポインタ
| `pixelsLength` | バイト単位のピクセル配列のサイズ

次のフィールドは`PocoDrawingBegin`と`PocoDrawingEnd`の呼び出しの間に `PocoRecord`構造体から読み取ることができます：

| フィールド | 説明 |
| :---: | :--- |
| `xOrigin` | 描画の原点の*x*座標
| `yOrigin` | 描画の原点の*y*座標
| `x` | 描画クリップの*x*座標
| `y` | 描画クリップの*y*座標
| `w` | 描画クリップの幅
| `h` | 描画クリップの高さ
| `xMax` | 描画クリップの右側の座標、`x + w`
| `yMax` | 描画クリップの下側の座標、`y + h`

***

##### `PocoCoordinate`

`PocoCoordinate`は符号付き整数値です。Commodettoで使用される場合、`CommodettoCoordinate`としてエイリアスされています。詳細については、[Commodettoドキュメント](./commodetto.md)の`CommodettoCoordinate`の説明を参照してください。

***

##### `PocoDimension`

`PocoDimension`は符号なし整数値です。Commodettoで使用される場合、`CommodettoDimension`としてエイリアスされています。詳細については、[Commodettoドキュメント](./commodetto.md)の`CommodettoDimension`の説明を参照してください。

***

##### `PocoPixel`

`PocoPixel`は整数値です。Commodettoで使用される場合、`CommodettoPixel`としてエイリアスされています。詳細については、[Commodettoドキュメント](./commodetto.md)の`CommodettoPixel`の説明を参照してください。

***

##### `PocoBitmapFormat`

`PocoBitmapFormat`は整数値です。Commodettoで使用される場合、`CommodettoBitmapFormat`としてエイリアスされています。詳細については、[Commodettoドキュメント](./commodetto.md)の`CommodettoBitmapFormat`の説明を参照してください。

***

##### `PocoRectangle`

`PocoRectangle`は矩形で囲まれた領域を定義し、左上の座標と寸法を指定します。

```c
typedef struct {
	PocoCoordinate	x;
	PocoCoordinate	y;
	PocoDimension	w;
	PocoDimension	h;
} PocoRectangleRecord, *PocoRectangle;
```

***

##### `PocoBitmap`

`PocoBitmap`構造体には、ビットマップのピクセル単位の幅と高さ、ビットマップ内のピクセルフォーマット、およびピクセルデータへのポインタが含まれています。

```c
typedef struct PocoBitmapRecord {
	PocoDimension			width;
	PocoDimension			height;
	PocoBitmapFormat		format;
	PocoPixel				*pixels;
} PocoBitmapRecord, *PocoBitmap;
```

ピクセルは左から右、上から下へと並んでおり、行間にパディングはありません。`PocoBitmap`には`rowBytes`や`stride`フィールドはありません。

> **注意:** `CommodettoBitmap`と異なり、`PocoBitmap`にはピクセルポインタの代わりにオフセットを格納するオプションはありません。

***

<a id="c-functions"></a>
### 関数

Pocoの呼び出しを行う前に、`PocoRecord`構造体を確保して初期化する必要があります。詳細は[`PocoRecord`](#poco-record)を参照してください。

##### `PocoMakeColor`

```c
PocoPixel PocoMakeColor(Poco poco, uint8_t r, uint8_t g, uint8_t b);
```

`PocoMakeColor`は0から255までの赤、緑、および青の値を受け取り、`PocoPixel`値を返します。返されるピクセル値は、いくつかのレンダリング呼び出しで色引数として使用できます。

> **注意:** 現在の実装では、Pocoは常に単一の出力ピクセルフォーマットをサポートしてビルドされるため、pocoパラメータは使用されません。

***

##### `PocoDrawingBegin`

```c
void PocoDrawingBegin(Poco poco, PocoCoordinate x, PocoCoordinate y,
	PocoDimension w, PocoDimension h);
```

`PocoDrawingBegin`は、`x`、`y`、`w`、`h`パラメータで囲まれたピクセルの更新エリアに対するレンダリングプロセスを開始します。描画の呼び出しは`PocoDrawingBegin`と `PocoDrawingEnd`の間でのみ行うことができます。

> **重要:** `PocoDrawingBegin`の呼び出し元は、描画呼び出しが更新エリア内のすべてのピクセルをカバーすることを確認する責任があります。Pocoは前のフレームを保持しません。描画されないピクセルには予測不可能な値を含むことになります。

***

##### `PocoDrawingEnd`

```c
int PocoDrawingEnd(Poco poco, PocoPixel *pixels, int byteLength,
	PocoRenderedPixelsReceiver pixelReceiver, void *refCon);

typedef void (*PocoRenderedPixelsReceiver)(PocoPixel *pixels,
	int byteCount, void *refCon);
```

`PocoDrawingEnd`は、`PocoDrawingBegin`の呼び出し以降にディスプレイリストに追加された描画コマンドをレンダリングします。

`PocoDrawingEnd`の呼び出し元は、レンダリングされたピクセルのバッファを`pixels`と`byteLength`引数で提供します。Pocoはバッファにできるだけ多くのピクセル行をレンダリングし、その後`pixelReceiver`関数を呼び出してピクセルを出力します。バッファは単一のピクセル行を保持するのに十分なサイズであればよく、複数行のピクセルを保持できるバッファを使用するとレンダリングのオーバーヘッドが減少します。

描画呼び出しの結果や`PocoDrawingEnd`の実行中にコマンドを表示リストに追加してエラーが発生した場合、`PocoDrawingEnd`は0以外の結果を返します：

* 1 -- ディスプレイリストのオーバーフロー
* 2 -- クリップおよび原点スタックのオーバーフロー
* 3 -- クリップおよび原点スタックのアンダーフロー、または順序外のポップ

***

##### `PocoRectangleFill`

```c
void PocoRectangleFill(Poco poco, PocoPixel color, uint8_t blend,
	PocoCoordinate x, PocoCoordinate y, PocoDimension w, PocoDimension h);
```

`PocoRectangleFill`は、`x`、`y`、`w`、`h`引数で定義された領域を指定された `color`で塗りつぶします。`blend`で指定されたレベルが`kPocoOpaque`（255）の場合、色はブレンドされずに背景に上書きされます。他のブレンドレベルの場合、色は背景と比率に応じてブレンドされます。

***

##### `PocoPixelDraw`

```c
void PocoPixelDraw(Poco poco, PocoPixel color,
		PocoCoordinate x, PocoCoordinate y);
```

`PocoPixelDraw`は、指定された`x`と`y`の位置に指定された色で単一のピクセルを描画します。

***

##### `PocoBitmapDraw`

```c
PocoCommand PocoBitmapDraw(Poco poco, PocoBitmap bits,
			PocoCoordinate x, PocoCoordinate y,
			PocoDimension sx, PocoDimension sy,
			PocoDimension sw, PocoDimension sh);
```

`PocoBitmapDraw`は、`x`と`y`で指定された位置に、`kCommodettoBitmapDefault`型のビットマップである`bits`の全体または一部を描画します。`sx`、`sy`、`sw`、`sh`の引数は、描画するビットマップの領域を定義します。

***

##### `PocoMonochromeBitmapDraw`

```c
void PocoMonochromeBitmapDraw(Poco poco, PocoBitmap bits,
		PocoMonochromeMode mode, PocoPixel fgColor, PocoPixel bgColor,
		PocoCoordinate x, PocoCoordinate y,
		PocoDimension sx, PocoDimension sy,
		PocoDimension sw, PocoDimension sh);
```

`PocoMonochromeBitmapDraw`は、`x`と`y`で指定された位置に、`kCommodettoBitmapMonochrome`型のビットマップである`bits`の全体または一部を描画します。`sx`、`sy`、`sw`、`sh`の引数は、描画するビットマップの領域を定義します。`mode`引数は、どのピクセルを描画するかを決定します：

* `kPocoMonochromeForeground` -- 前景ピクセル（ビットマップ内の値が1のピクセル）のみを描画します。
* `kPocoMonochromeBackground` -- 背景ピクセル（ビットマップ内の値が0のピクセル）のみを描画します。
* `kPocoMonochromeForeAndBackground` -- 前景および背景ピクセルの両方を描画します。

`fgColor`と`bgColor`引数は、前景と背景ピクセルの描画に使用する色を指定します。

##### `PocoGrayBitmapDraw`

```c
void PocoGrayBitmapDraw(Poco poco, PocoBitmap bits,
		PocoPixel color, uint8_t blend,
		PocoCoordinate x, PocoCoordinate y,
		PocoDimension sx, PocoDimension sy,
		PocoDimension sw, PocoDimension sh);
```

`PocoGrayBitmapDraw`は、`x`と`y`で指定された位置に、`kCommodettoBitmapGray16`型のビットマップである`bits`の全体または一部を描画します。`sx`、`sy`、`sw`、`sh`の引数は、描画するビットマップの領域を定義します。ビットマップのピクセルはアルファブレンドレベルとして扱われ、`color`引数の色と背景ピクセルとがブレンドされます。`blend`引数は各ピクセルのブレンドレベルに適用され、値は0が透明で255が不透明です。

***

##### `PocoBitmapDrawMasked`

```c
void PocoBitmapDrawMasked(Poco poco, PocoBitmap bits, uint8_t blend,
		PocoCoordinate x, PocoCoordinate y,
		PocoDimension sx, PocoDimension sy,
		PocoDimension sw, PocoDimension sh,
		PocoBitmap mask, PocoDimension mask_sx, PocoDimension mask_sy);
```

`PocoBitmapDrawMasked`は、`x`と`y`で指定された位置に、`kCommodettoBitmapDefault`型のビットマップである`bits`の、`sx`、`sy`、`sw`、`sh`で囲まれたピクセルを描画します。ピクセルは`kCommodettoBitmapGray16`型のビットマップである`mask`の、`mask_sx`、`mask_sy`,`sw`、`sh`で囲まれた対応するピクセルを、アルファブレンディングレベルとして描画します。`blend`引数は各ピクセルのブレンドレベルに適用され、値は0が透明で255が不透明です。

***

##### `PocoBitmapPattern`

```c
void PocoBitmapPattern(Poco poco, PocoBitmap bits,
		PocoCoordinate x, PocoCoordinate y,
		PocoDimension w, PocoDimension h,
		PocoDimension sx, PocoDimension sy,
		PocoDimension sw, PocoDimension sh);
```

`PocoBitmapPattern`は、`x`、`y`、`w`、`h`引数で囲まれた領域を、`sx`、`sy`、`sw`、`sh`引数で囲まれたビットマップ`bits`の領域の繰り返しコピーで塗りつぶします。ビットマップは`kCommodettoBitmapDefault`型でなければなりません。

***

##### `PocoDrawFrame`

```c
void PocoDrawFrame(Poco poco,
	uint8_t *data, uint32_t dataSize,
	PocoCoordinate x, PocoCoordinate y,
	PocoDimension w, PocoDimension h);
```

`PocoDrawFrame`は、ModdableのColorCellアルゴリズムの一種で保存された圧縮画像をレンダリングします。レンダリングする画像は`data`引数で示され、バイト数は`dataSize`引数で指定されます。画像は、`x`および`y`引数で指定された位置にレンダリングされます。圧縮画像のソース寸法（クリップされていないサイズ）は、`w`および`h`引数によって与えられます。

***

##### `PocoClipPush`

```c
void PocoClipPush(Poco poco, PocoCoordinate x, PocoCoordinate y,
		 PocoDimension w, PocoDimension h);
```

`PocoClipPush`は現在のクリップ領域をスタックにプッシュし、その後、現在のクリップと`x`、`y`、`w`、`h`引数で囲まれた領域が交差する領域で現在のクリップを置き換えます。

***

##### `PocoClipPop`

```c
void PocoClipPop(Poco poco);
```

`PocoClipPop`はスタックからクリップをポップし、現在のクリップをポップした値で置き換えます。

***

##### `PocoOriginPush`

```c
void PocoOriginPush(Poco poco, PocoCoordinate x, PocoCoordinate y);
```

`PocoOriginPush`は現在の原点をスタックにプッシュし、その後、`x`および`y`引数で現在の原点をオフセットします。

***

##### `PocoOriginPop`

```c
void PocoOriginPop(Poco poco);
```

`PocoOriginPop`は原点をスタックからポップし、現在の原点をポップした値で置き換えます。

***

##### `PocoDrawingBeginFrameBuffer`

```c
void PocoDrawingBeginFrameBuffer(Poco poco, PocoCoordinate x, PocoCoordinate y,
	PocoDimension w, PocoDimension h,
	PocoPixel *pixels, int16_t rowBytes);
```

`PocoDrawingBeginFrameBuffer`は、`x`、`y`、`w`、`h`パラメータによって囲まれたピクセルの更新領域に対して、即時モードレンダリング処理を開始します。`pixels`パラメータは出力ピクセルの最初のスキャンラインを指します。`rowBytes`パラメータは、各スキャンライン間のバイト単位のストライドです。

***

##### `PocoDrawingEndFrameBuffer`

```c
int PocoDrawingEndFrameBuffer(Poco poco;)
```

`PocoDrawingEndFrameBuffer`は、現在のフレームに対するすべての描画が完了したことを示します。

***

##### `PocoDrawExternal`


```c
void PocoDrawExternal(Poco poco, PocoRenderExternal doDrawExternal,
	uint8_t *data, uint8_t dataSize,
	PocoCoordinate x, PocoCoordinate y,
	PocoDimension w, PocoDimension h);

typedef void (*PocoRenderExternal)(Poco poco, uint8_t *data,
	PocoPixel *dst, PocoDimension w, PocoDimension h, uint8_t xphase);
```

`PocoDrawExternal`はカスタムレンダリング要素を現在のPocoディスプレイリストにインストールします。`data`引数は、描画操作を定義する`dataSize`バイト長のデータブロックを指します。このデータはPocoディスプレイリストにコピーされるので、できるだけコンパクトである必要があります。描画操作の境界は`x`、`y`、`w`、`h`引数で定義されます。`doDrawExternal`コールバック関数は呼び出されると、一度に1つまたは複数のスキャンラインでカスタム要素を描画します。

Pocoは、レンダリング操作でクリッピングや回転を行いません。これらは、レンダリングデータを作成するコードおよび/またはレンダリングコールバック関数で適用する必要があります。

複数のピクセルが1バイトに格納されているピクセルフォーマットで描画する場合、`xphase`は描画が開始されるピクセルを示します。例えば、1ピクセルあたり4ビットを使用するピクセルフォーマットでは、xphaseはバイト内の最初のピクセルに対しては0、2番目のピクセルに対しては1になります。

> **注意**: カスタムレンダリング要素の実装は高度なテクニックであり、Pocoレンダリングエンジンの実装に精通している必要があります。

***

<a id="odds-and-ends"></a>
## その他

### Commodettoとの関連性

PocoのC APIはCommodettoから独立して使用することができます。PocoはCommodettoに統合された最初のレンダラーです。PocoはCommodettoをサポートするすべてのハードウェア、特にXS JavaScriptエンジンで動作します。PocoのC APIはCommodettoと比較して、かなり性能の低いハードウェアでも動作します。

### 「Poco」という名前について

*poco*は音楽用語で「少し」を意味します。
