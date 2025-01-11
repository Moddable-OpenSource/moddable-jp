# Commodetto
Copyright 2016-2023 Moddable Tech, Inc.<BR>
改訂： 2023年8月31日

## このドキュメントについて

Commodettoは、リソースが制約されたマイクロコントローラで動作するデバイスに、現代的なユーザーインターフェースのレンダリングを提供するために設計されたグラフィックスライブラリです。多くのアプリケーションにおいて、Commodettoはユーザーインターフェースをレンダリングするためのアセットを含め、数キロバイトのRAMしか必要としません。

このドキュメントは、Commodettoの各部分の概要、Commodettoアプリケーションに含まれるアセットのフォーマット要件に関する情報、およびCommodetto JavaScript APIを定義するオブジェクトの詳細を提供します。

## 目次

* [Commodettoの概要](#overview)
	* [ネイティブデータ型](#native-data-types)
	* [ホストバッファ](#host-buffers)
* [ビットマップ操作](#bitmap-operations)
	* [ビットマップクラス](#bitmap-class)
	* [PixelsOutクラス](#pixelsout-class)
	* [SPIOutクラス](#"spiout-class)
	* [BufferOutクラス](#bufferout-class)
	* [BMPOutクラス](#bmpout-class)
	* [RLE4Outクラス](#rle4out-class)
	* [ColorCellOutクラス](#colorcellout-class)
* [アセット解析](#asset-parsing)
	* [BMP](#bmp)
	* [JPEG](#jpeg)
	* [PNG](#png)
	* [BMFont](#bmfont)
* [レンダリング](#rendering)
	* [Renderクラス](#render-class)
* [ピクセルフォーマット変換](#pixel-format-conversion)
	* [Convertクラス](#convert-class)
* [その他](#odds-and-ends)

<a id="overview"></a>
## Commodettoの概要

Commodettoは以下の部分で構成されています：

* 軽量な[Pocoレンダリングエンジン](./poco.md) - フレームバッファを必要とせず、1回に1つのスキャンラインを効率的にレンダリングできるディスプレイリストレンダラー
* 一般的なファイル形式を扱うためのアセットローダー
* レンダリングされたピクセルをディスプレイやファイルに送信するためのピクセル出力
* すべての機能に対応するJavaScript API

Commodettoの各部分はモジュールで構成されており、必要なモジュールのみを展開したり、新しいモジュールを追加したり、既存のモジュールを置き換えたりすることが容易です。レンダリング機能もモジュールであり、専門的なレンダリングモジュールの統合を可能にします。

**Pocoレンダリングエンジン**は、最も基本的なレンダリング操作のみを含んでいます：単色で矩形を塗りつぶすまたはブレンドする操作、コピー、パターン塗りつぶし、アルファブレンドを含む少数のビットマップ描画操作です。テキストは、Pocoビットマップレンダリング操作を使用してレンダリングエンジンの外部で実装されており、異なるエンジンを統合してグリフ生成、テキストレイアウト、テキスト測定を行うことができます。

**アセットローダー**は、写真、ユーザーインターフェース要素、フォントなどのグラフィカルアセットをレンダリングのために準備します。Commodettoには、BMP画像、JPEG写真、PNG画像、BMFontフォントのためのアセットローダーが含まれています。追加のアセットローダーモジュールを追加することも可能です。アセットローダーは、多くの種類のアセットをRAMにロードすることなく、フラッシュストレージ（例えば、ROM）から直接レンダリングすることを可能にします。

**ピクセル出力**は、レンダリングされたピクセルをその目的地に届けます。Commodettoには、ファイルやメモリ内ビットマップに書き込むためのモジュールが含まれています。ホストデバイスがサポートするトランスポート（SPI、I<sup>2</sup>C、シリアル、メモリマップドポートを含む）を介してディスプレイにピクセルを送信するためのモジュールを追加することができます。

<a id="native-data-types"></a>
### ネイティブデータ型

Commodettoのデータ型には、ピクセル、座標、寸法、ビットマップが含まれます。これらの型は、ピクセルフォーマットを定義する列挙とともに以下に説明されています。

#### ピクセル型

リソースが制約されたハードウェアでうまく動作するために、Commodettoは簡略化された仮定を行います。1つの仮定は、特定のデプロイメントでサポートされる出力ピクセルフォーマットが1つだけであるということです。汎用のグラフィックスライブラリは、多くの異なるディスプレイやファイルフォーマットに対応するために、多くの異なる出力ピクセルフォーマットをサポートする必要があります。Commodettoは、デプロイされるデバイスが1種類のスクリーンに接続されると仮定しており、この仮定によりコードサイズと一部のランタイムオーバーヘッドが削減されます。

Commodettoは、異なるピクセルフォーマットをサポートする機能を持っており、使用するピクセルフォーマットの選択はコンパイル時に設定されます。出力ピクセルフォーマットを変更することは、例えば異なる画面を使用するために、異なる出力ピクセルフォーマットを設定してCommodettoを再コンパイルするだけで済みます。

ピクセルフォーマットは、Cの#define `kCommodettoBitmapFormat`の値によってビルド時に決定されます。レンダリング先のピクセルフォーマットとしてサポートされているのは、`kCommodettoBitmapRGB565LE`、`kCommodettoBitmapRGB332`、`kCommodettoBitmapGray256`、`kCommodettoBitmapGray16`、および`kCommodettoBitmapCLUT16`です。

`CommodettoPixel`はピクセルのデータ型です。この型の定義は、`kCommodettoBitmapFormat`の値に依存します。16ビットピクセルの場合は`uint16_t`、8ビットおよび4ビットピクセルの場合は`uint8_t`です。

```c
typedef uint16_t CommodettoPixel;
```

***

#### ピクセルフォーマット
Commodettoは、同時に複数のソースピクセルフォーマットをサポートしています。これにより、異なる種類のアセットを効率的に保存およびレンダリングすることができます。

出力ピクセルフォーマットは常にサポートされているソースピクセルフォーマットの1つです。さらに、1ビットのモノクロームと4ビットのグレーのソースピクセルフォーマットは常にサポートされています。

`CommodettoBitmapFormat` 列挙型はピクセルフォーマットを定義します。各デプロイメントでサポートされるピクセルフォーマットのサブセットがあります。

```c
typedef enum {
	kCommodettoBitmapDefault = 1,
	kCommodettoBitmapMonochrome = 3,
	kCommodettoBitmapGray16,
	kCommodettoBitmapGray256,
	kCommodettoBitmapRGB332,
	kCommodettoBitmapRGB565LE,
	kCommodettoBitmapRGB565BE,
	kCommodettoBitmap24RGB,
	kCommodettoBitmap32RGBA,
	kCommodettoBitmapCLUT16,

	kCommodettoBitmapPacked = 0x80
} CommodettoBitmapFormat;
```

| フォーマット | 説明 |
| :---: | :--- |
| kCommodettoBitmapDefault | 出力ピクセルフォーマット。このフォーマットは、Commodetto がビルドされる際の `kCommodettoBitmapFormat` の値に応じて変わります。 |
| kCommodettoBitmapMonochrome | ピクセルは1ビットデータで、バイトに詰め込まれ、バイトの最上位ビットが最も左のピクセルです。 |
| kCommodettoBitmapGray16 | ピクセルは4ビットデータで、0は白、15は黒を表し、その間の値は比例的に補間されたグレーのレベルです。ピクセルはバイトに詰め込まれ、最上位のニブルが最も左のピクセルです。 |
| kCommodettoBitmapGray16 \| kCommodettoBitmapPacked | ピクセルは `kCommodettoBitmapGray16` と同じで、重み付きRLEアルゴリズムを使用して圧縮されています。 |
| kCommodettoBitmapGray256 | ピクセルは8ビットデータで、0は白、255は黒を表し、その間の値は比例的に補間されたグレーのレベルです。 |
| kCommodettoBitmapRGB332 | ピクセルは8ビットデータで、RGB値が詰め込まれています。最上位の3ビットが赤、その後に3ビットの緑、2ビットの青が続きます。 |
| kCommodettoBitmapRGB565LE | ピクセルは16ビットデータで、RGB値が詰め込まれています。最上位の5ビットが赤、その後に6ビットの緑、5ビットの青が続きます。16ビットの値はリトルエンディアンのバイトオーダーで保存されます。 |
| kCommodettoBitmapRGB565BE | ピクセルは16ビットデータで、RGB値が詰め込まれています。最上位の5ビットが赤、その後に6ビットの緑、5ビットの青が続きます。16ビットの値はビッグエンディアンのバイトオーダーで保存されます。 |
| kCommodettoBitmap24RGB | ピクセルは3つの8ビット値で、赤、緑、青の順です。 |
| kCommodettoBitmap24RGBA | ピクセルは4つの8ビット値で、赤、緑、青、アルファの順です。 |

***

#### 座標タイプ

`CommodettoCoordinate` タイプは座標を指定します。

```c
typedef int16_t CommodettoCoordinate;
```

座標タイプは符号付きで、アプリケーションが画面の任意の端に重なるオブジェクトをレンダリングできるようにします。

非常に小さなディスプレイ（各次元で128ピクセル以下）のデバイスでは、`CommodettoCoordinate` を8ビット値、例えば `int8_t` に再定義してメモリ要件を削減できます。この場合、アプリケーションコードは-128より小さいか127より大きい座標を使用しないように注意する必要があります。そうしないと、結果は予測不可能になります。

***

#### 次元タイプ

`CommodettoDimension` は次元を指定するために使用されるタイプです。

```c
typedef uint16_t CommodettoDimension;
```

次元タイプは符号なしで、Commodettoは負の幅や高さに意味を持たせません。

`CommodettoDimension` は、非常に小さなディスプレイ（各次元で256ピクセル）を持つデバイスで `uint8_t` に再定義してメモリ使用量を削減できます。この場合、アプリケーションコードは255ピクセルを超える次元を指定しないように注意する必要があります。

***

#### ビットマップタイプ

`CommodettoBitmap` 構造体はビットマップを定義します。

```c
typedef struct {
	CommodettoDimension	w;
	CommodettoDimension	h;
	CommodettoBitmapFormat	format;
	int8_t			havePointer;
	union {
		void		*data;
		int32_t		offset;
	} bits;
} CommodettoBitmapRecord, *CommodettoBitmap;
```

`w` と `h` フィールドはビットマップの幅と高さをピクセル単位で示します。ビットマップの左上隅の座標は常に `{x: 0, y: 0}` であるため、ビットマップには *x* および *y* 座標は含まれません。`format` フィールドはビットマップ内のピクセルのピクセルフォーマットを示します。

残りのフィールドはピクセルデータがどこにあるかを示します。`havePointer` フィールドがゼロでない場合、ユニオン内の `data` フィールドが有効で、ピクセルデータを指します。

```c
CommodettoBitmap bitmap = ...  /* ビットマップを取得 */;
CommodettoPixel *pixels = bitmap.bits.data;
```

`havePointer` フィールドが0の場合、`pixels` のアドレスはこのビットマップデータ構造の外部に保存されます。`offset` フィールドは、外部ピクセルポインタからのバイト数としてピクセルデータが始まる位置を示します。これは、`ArrayBuffer` に格納されたピクセルのアドレスを計算するためによく使用されます。

```c
CommodettoBitmap bitmap = ...  /* ビットマップを取得 */;
unsigned char *buffer = xsToArrayBuffer(xsArg(0));
CommodettoPixel *pixels = (CommodettoPixel *)(bitmap.bits.offset + buffer);
```

***

<a id="host-buffers"></a>
### ホストバッファ

組み込みの `ArrayBuffer` オブジェクトは、JavaScriptでバイナリデータを保存および操作するための標準的な方法です。Commodettoは、アセットのピクセルを `ArrayBuffer` に保存することをサポートしています。しかし、JavaScriptの標準では、`ArrayBuffer` がRAMに存在することが事実上要求されています。多くの組み込みデバイスは限られたRAMしか持たず、比較的豊富なROM（フラッシュメモリ）を持っているため、アセットをRAMに移動することなく直接ROMから使用することが望ましいです。

この状況をサポートするために、Commodettoが基づいているXS JavaScriptエンジンは、`HostBuffer` オブジェクトを追加します。`HostBuffer` にはコンストラクタがありません。なぜなら、ネイティブコードによってのみ作成できるからです。一度インスタンス化されると、`HostBuffer` のJavaScript APIは `ArrayBuffer` APIと同一です。`HostBuffer` が参照するメモリは読み取り専用メモリにあるため、書き込み操作は行うべきではありません。結果は予測不可能であり、ターゲットハードウェアによっては、書き込みがクラッシュしたり、静かに失敗したりする可能性があります。

Moddable SDKでは、`Resource` コンストラクタはROM内のデータブロックに対して `HostBuffer` を作成する方法です。

> **注:** Commodetto と Poco の JavaScript-to-C バインディングは、`ArrayBuffer` が許可されている場所で `HostBuffer` を使用することをサポートしています。ただし、一部の JavaScript-to-C バインディングは `ArrayBuffer` の代わりに `HostBuffer` を受け入れず、例外をスローします。

***

<a id="bitmap-operations"></a>
## ビットマップ操作

このセクションでは、ビットマップを操作するために使用されるクラス、`Bitmap`、`PixelsOut`、および `PixelsOut` の4つのサブクラスについて説明します。

<a id="bitmap-class"></a>
### ビットマップクラス

Commodettoの `Bitmap` オブジェクトは、ビットマップをレンダリングするために必要な3つの情報、ビットマップの寸法、ビットマップ内のピクセルのフォーマット、およびピクセルデータへの参照を含んでいます。次の例では、`ArrayBuffer` に格納された16ビットのピクセルを持つ `Bitmap` オブジェクトを作成します。

```js
import Bitmap from "commodetto/Bitmap";

let width = 40, height = 30;
let pixels = new ArrayBuffer(height * width * 2);
let bitmap = new Bitmap(width, height, Bitmap.RGB565LE, pixels, 0);
```

`Bitmap` オブジェクトのピクセルは、RAMの使用を最小限に抑えるために、しばしば `HostBuffer` に格納されます。例えば、`Resource` は次のリソースに対応する `HostBuffer` を作成します。

```js
let pixels = new Resource("pixels.dat");
let bitmap = new Bitmap(width, height, Bitmap.RGB565LE, pixels, 0);
```

#### 関数

##### `constructor(width, height, format, buffer, offset)`

`Bitmap` コンストラクタは以下の引数を取ります：

| 引数 | 説明 |
| :---: | :--- |
| `width` | ビットマップの幅をピクセル単位で示す数値 |
| `height` | ビットマップの高さをピクセル単位で示す数値 |
| `format` | ビットマップ内のピクセルの種類を示すもので、`Bitmap` の静的プロパティから取られます（例: `Bitmap.RGB565LE` または `Bitmap.Gray16`） |
| `buffer` | ビットマップのピクセルデータを含む `ArrayBuffer` または `HostBuffer` |
| `offset` | ピクセルデータまでの `buffer` の開始位置からのバイト単位のオフセットを示す数値 |

ビットマップのすべてのプロパティは、構築時に固定され、その後変更することはできません。

***

#### プロパティ

| 名前 | 型 | 読み取り専用 | 説明 |
| :---: | :---: | :---: | :--- |
| `width` | `Number` | ✓ | ビットマップの幅をピクセル単位で示します。 |
| `height` | `Number` | ✓ | ビットマップの高さをピクセル単位で示します。 |
| `format` | `Number` | ✓ | ビットマップのピクセルのピクセルフォーマットを返します。 |

***

#### 静的関数

#### `depth(pixelFormat)`

指定されたピクセルフォーマットのビット数を返します。

```js
Bitmap.depth(Bitmap.RGB565LE);		// 16を返します
Bitmap.depth(Bitmap.Monochrome);	// 1を返します
Bitmap.depth(Bitmap.Gray256);		// 8を返します
```

***

#### 静的プロパティ

| 名前 | 説明 |
| :---: | :--- |
| `Bitmap.Default` | `kCommodettoBitmapDefault` の値に対応する定数 |
| `Bitmap.RLE` | `kCommodettoBitmapPacked` の値に対応する定数 |
| `Bitmap.Monochrome` | `kCommodettoBitmapMonochrome` の値に対応する定数 |
| `Bitmap.Gray16` | `kCommodettoBitmapGray16` の値に対応する定数 |
| `Bitmap.Gray256` | `kCommodettoBitmapGray256` の値に対応する定数 |
| `Bitmap.RGB332` | `kCommodettoBitmapRGB332` の値に対応する定数 |
| `Bitmap.RGB565LE` | `kCommodettoBitmapRGB565LE` の値に対応する定数 |
| `Bitmap.RGB565BE` | `kCommodettoBitmapRGB565BE` の値に対応する定数 |
| `Bitmap.RGB24` | `kCommodettoBitmapRGB24` の値に対応する定数 |
| `Bitmap.RGBA32` | `kCommodettoBitmapRGBA32` の値に対応する定数 |
| `Bitmap.CLUT16` | `kCommodettoBitmapCLUT16` の値に対応する定数 |

***

<a id="pixelsout-class"></a>
### PixelsOut クラス

`PixelsOut` クラスは、ピクセルを出力するための抽象基底クラスです。これは、異なる宛先に出力するために他のクラスによってオーバーライドされます：

* `SPIOut` -- SPI経由で接続されたLCDディスプレイにピクセルを送信
* `BufferOut` -- オフスクリーンメモリバッファにピクセルを送信
* `BMPOut` -- BMPファイルにピクセルを書き込み
* `RLE4Out` -- Commodetto 4ビットグレー RLE形式にピクセルを圧縮
* `ColorCellOut` -- ModdableのColorCell変種にピクセルを圧縮

`PixelsOut` インスタンスは、値の辞書で初期化されます。辞書は、コンストラクタに任意の数の名前と値のペアを提供します。異なる `PixelsOut` サブクラスは、異なる初期化パラメータを必要とします。例えば：

```javascript
let display = new SPIOut({width: 320, height: 240,
		pixelFormat: Bitmap.RGB565BE, orientation: 90, dataPin: 30});

let offScreen = new BufferOut({width: 40, height: 40,
		pixelFormat: Bitmap.RGB565LE});

let bmpOut = new BMPOut({width: 240, height: 240,
		pixelFormat: Bitmap.RGB565LE, path: "/test.bmp"});
```

Commodettoを使用してアプリケーションを構築する開発者は、`PixelsOut` APIの大部分を直接使用する必要はありません。アプリケーションは `PixelsOut` のサブクラスのコンストラクタを使用し、それを `Renderer` インスタンスに直ちにバインドします。この `Renderer` インスタンスが必要に応じて `PixelsOut` APIを呼び出します。アプリケーションは `Renderer` インスタンスとだけやり取りします。この `PixelsOut` セクションの残りの情報は、自分自身の `PixelsOut` サブクラスを実装する開発者を対象としています。

構築されると、`PixelsOut` インスタンスはピクセルを受け取ることができます。ピクセルを出力に送信するために使用される関数呼び出しは、`begin`、`send`、および `end` の3つです。以下の例は、`PixelsOut` インスタンスの一部に黒いピクセルを出力する方法を示しています。

```javascript
let x = 10, y = 20;
let width = 40, height = 50;
display.begin(x, y, width, height);

let scanLine = new ArrayBuffer(display.pixelsToBytes(width));
for (let line = 0; line < height; line++)
	display.send(scanLine);

display.end();
```

`begin` 呼び出しは、更新する `PixelsOut` ビットマップの領域を示します。その後に、ピクセルを含む1つ以上の `send` 呼び出しが続きます。上記の例では、`send` は40個の黒（すべて0）のピクセルを持つバッファを50回呼び出しています。`send` への呼び出しによって渡されるデータのバイト数は、`begin` への呼び出しで指定された領域を埋めるために必要なバイト数と正確に一致している必要があります。すべてのデータが提供されたら、`end` を呼び出します。

`PixelsOut` APIは、データが出力に送信されるタイミングを定義していません。異なる `PixelsOut` サブクラスはそれを異なる方法で実装します：一部はデータを即座に同期的に送信し、一部は非同期転送を使用し、他はデータをバッファして `end` が呼び出されたときにのみ表示します。

以下の例は、`PixelsOut` クラスの定義を示しています（関数の本体は省略されています）。

```javascript
class PixelsOut {
	constructor(dictionary) {}
	begin(x, y, width, height) {}
	end() {}
	continue(x, y, width, height) {}

	send(pixels, offset = 0, count = pixels.byteLength - offset) {}

	get width() {}
	get height() {}

	get pixelFormat() {}
	pixelsToBytes(count) {}
}
```

#### 関数

##### `constructor(dictionary)`

`PixelsOut` のコンストラクタは、プロパティ名と値の辞書を1つの引数として受け取ります。`PixelsOut` 基底クラスは、すべての `PixelsOut` サブクラスに対して定義される3つのプロパティを定義します。

| 名前 | 説明 |
| :---: | :--- |
| `width` | 出力の幅をピクセルで指定する数値 |
| `height` | 出力の高さをピクセルで指定する数値 |
| `pixelFormat` | 出力ピクセルのフォーマットを指定する文字列（例: 16ビット 565 リトルエンディアンピクセルの場合は `Bitmap.RGB565BE`、16ビット 565 ビッグエンディアンピクセルの場合は `Bitmap.RGB565BE`）

サブクラスは必要に応じて辞書に追加のプロパティを定義します。

```javascript
import PixelsOut from "commodetto/PixelsOut";

let out = new PixelsOut({width: 120, height: 160, pixelFormat: Bitmap.RGB565BE});
```

> **注:** `PixelsOut` オブジェクトの `width`、`height`、および `pixelFormat` プロパティは、オブジェクトが作成された時点で固定され、変更することはできません。一般に、コンストラクタに提供される辞書に含まれるすべてのプロパティは、`PixelsOut` サブクラスの実装を簡単に保つために読み取り専用と見なすべきです。ただし、`PixelsOut` のサブクラスは、一部のプロパティを変更する方法を提供することを選択する場合があります。

***

##### `begin(x, y, width, height)`

`begin` 関数は、フレームの出力への送信を開始します。`x` と `y` の引数はフレームの左上隅を示し、`width` と `height` の引数はフレームのサイズを示します。`begin` 関数への引数で指定された領域は、コンストラクタにその辞書で提供された寸法内に完全に収まる必要があります。

***

##### `end()`

`end` 関数は、現在のフレームの出力への送信が完了したことを示します。フレームを正しく出力するには、`end` を呼び出す必要があります。

***

<a id="PixelsOut-continue"></a>
##### `continue(x, y, width, height)`

`continue` 関数は、単一のフレームの複数の領域に描画する方法です。`continue` を呼び出すことは、`end` の後に `begin` を呼び出すことに似ています。つまり、次のように

```javascript
out.continue(10, 20, 30, 40);
```

することは、ほぼ次のようにすることと同じです：

```javascript
out.end();
out.begin(10, 20, 30, 40);
```

違いは、`continue` が使用されると、すべての出力ピクセルが同じフレームの一部であるのに対し、`end`/`begin` では、`end` の前に送信されたピクセルは1つのフレームの一部であり、その後に送信されたピクセルは次のフレームの一部であるということです。

出力によっては、例えば `BufferOut` の場合、違いはありません。他の出力、例えばハードウェアアクセラレーションされたレンダラーの場合、結果は視覚的に異なります。

> **注:** すべての `PixelsOut` サブクラスが `continue` を実装しているわけではありません。サポートされていない場合、例外がスローされます。以下のサブクラスの説明では、`continue` がサポートされているかどうかが示されています。

***

##### `send(pixels, offset = 0, count = pixels.byteLength - offset)`

`send` 関数はピクセルを出力に送信します。`pixels` 引数はピクセルを含む `ArrayBuffer` または `HostBuffer` です。`offset` 引数はバッファ内でピクセルが始まるオフセット（バイト単位）であり、`count` はバッファから送信するバイト数です。

```javascript
let pixels = new ArrayBuffer(40);
out.send(pixels);		// Send all pixels in buffer
out.send(pixels, 30);	// Send last 5 pixels in buffer
out.send(pixels, 2, 2)	// Send 2nd pixel in buffer
```

>**注:** `offset` と `count` 引数はピクセル単位ではなくバイト単位です。

***

##### `adaptInvalid(r)`

`adaptInvalid` 関数は、`PixelsOut` 領域内のエリアを指定する矩形引数を取り、必要に応じて矩形を修正してPixelsOutの有効な更新領域にします。`adaptInvalid` 関数は、一部のディスプレイコントローラーが特定の量子でしかディスプレイを更新できないために必要です。例えば、あるディスプレイではx座標と幅が偶数である必要があり、別のディスプレイでは完全な水平走査線のみを更新でき、さらに別のディスプレイでは単一のピクセルが変更された場合でもディスプレイ全体を更新します。


	let r = renderer.rectangle(x, y, w, h);
	pixelsOut.adaptInvalid(r);
	pixelsOut.begin(r.x, r.y, r.w, r.h);
	...

***

##### `pixelsToBytes(count)`

`pixelsToBytes` 関数は、`count` 引数で指定されたピクセル数を格納するのに必要なバイト数を計算します。以下は、ピクセルの単一スキャンラインに対応する `ArrayBuffer` を割り当てます：

```javascript
let buffer = new ArrayBuffer(out.pixelsToBytes(width));
```

***

#### プロパティ

| 名前 | 型 | 読み取り専用 | 説明 |
| :---: | :---: | :---: | :--- |
| `width` | `Number` | ✓ | `PixelsOut` インスタンスの幅をピクセル単位で示します。 |
|`height` | `Number` | ✓ | `PixelsOut` インスタンスの高さをピクセル単位で示します。 |
| `pixelFormat` | `Number` | ✓ | `PixelsOut` インスタンスのピクセルのフォーマットを示します。 |
`async` | `Boolean` | ✓ | PixelsOut が非同期レンダリングをサポートしている場合に true を返します。これは、`send` に渡されたピクセルがコピーされず、次の `send` または `end` の呼び出しが完了するまで変更されないままである必要があることを意味します。 |
| `c_dispatch` |  | ✓ | ネイティブの `PixelsOutDispatchRecord` を含む `HostBuffer` へのポインタをオプションで返します。ネイティブディスパッチテーブルが利用できない場合は未定義を返します。ネイティブ `PixelsOutDispatchRecord` は、ネイティブコードが `PixelsOut` の `begin`、`send`、`continue`、`end`、および `adaptInvalid` 関数を XS 仮想マシンを介さずに直接呼び出すことを可能にします。ネイティブディスパッチテーブルは厳密には最適化であり、JavaScript API における機能のみを提供します。 |
| `clut` |  |  | (まだ文書化されていません) |

***

<a id="spiout-class"></a>
### SPIOut クラス

`SPIOut` は `PixelsOut` のサブクラスで、SPI接続のディスプレイにピクセルを送信する `PixelsOut` インスタンスの実装のためのプレースホルダーです。

```javascript
class SPIOut extends PixelsOut
```

***

<a id="bufferout-class"></a>
### BufferOut クラス

`BufferOut` は `PixelsOut` のサブクラスで、オフスクリーンのメモリ内ビットマップを実装します。

```javascript
class BufferOut extends PixelsOut;
```

ターゲットデバイスではメモリが限られたリソースであるため、アプリケーションは `BufferOut` を控えめに使用するべきです。

`BufferOut` インスタンスを作成するには：

```javascript
import BufferOut from "commodetto/BufferOut";

let offscreen = new BufferOut({width: 40, height: 30, pixelFormat: Bitmap.RGB565LE});
```

`BufferOut` コンストラクタはビットマップピクセルのためのバッファを割り当てます。オフスクリーンビットマップが作成されると、`PixelsOut` クラスで説明されているように `send` 関数を使用してピクセルが送信されます。`BufferOut` インスタンスのピクセルは、インスタンスのビットマップを通じてアクセスされます。

```javascript
let bitmap = offscreen.bitmap;
```

`BufferOut`は、`PixelsOut`のオプションの`continue`関数を実装しています。

#### プロパティ

| 名前 | 型 | 読み取り専用 | 説明 |
| :---: | :---: | :---: | :--- |
| `bitmap ` | `Bitmap` | ✓ | `BufferOut`インスタンスのピクセルにアクセスするための`Bitmap`インスタンスを返します。

***

<a id="bmpout-class"></a>
### BMPOut クラス

`BMPOut`は、`PixelsOut`のサブクラスで、ピクセルを受け取り、次のフォーマットでBMPファイルに書き込みます： `Bitmap.Gray16`、`Bitmap.Gray256`、`Bitmap.RGB565LE`、`Bitmap.RGB332`、および`Bitmap.CLUT16`。`BMPOut`の実装は、ファイルにピクセルを段階的に書き込むため、利用可能なメモリよりも大きなファイルを作成することができます。

`BMPOut`は、コンストラクタの辞書に`path`プロパティを追加します。`path`は、出力BMPファイルへのフルパスを含む文字列です。

> **注意:** BMPファイルの幅にビット深度を掛けた値は、32の倍数でなければなりません。

***

<a id="rle4out-class"></a>
### RLE4Out クラス

`RLE4Out`は、`PixelsOut`のサブクラスで、`Bitmap.Gray16`フォーマットのピクセルを受け取り、それをRLEエンコードされたビットマップにパックします。

```javascript
class RLE4Out extends PixelsOut;
```

RLEエンコードされたビットマップはサイズが小さく、アセットに必要なROMの量を減らします。さらに、多くの一般的なビットマップの使用において、RLEエンコードされたビットマップはより速く描画されます。

`RLE4Out`クラスはデバイス上で使用できます。しかし、より一般的には、ビットマップはRLEエンコードされ、ビルド時にストレージに書き込まれます。これにより、デバイス上で画像をエンコードするための時間とメモリを節約しながら、ストレージスペースを節約できます。

Moddable SDKの`rle4encode`ツールは、`RLE4Out`クラスを使用して4ビットグレースケールBMPファイルを圧縮します。`compressbmf`ツールは、BMFフォントの各個別のグリフに`RLE4Out`クラスを適用します。

`RLE4Out`クラスはオプションの`continue`関数を実装していません。

#### プロパティ

| 名前 | 型 | 読み取り専用 | 説明 |
| :---: | :---: | :---: | :--- |
| `bitmap ` | `Bitmap` | ✓ | `RLE4Out`インスタンスのピクセルにアクセスするための`Bitmap`インスタンスを返します。

***

<a id="colorcellout-class"></a>
### ColorCellOut クラス

`ColorCellOut`は`PixelsOut`のサブクラスで、`Bitmap.RGB565LE`形式のピクセルを受け取り、[ColorCell](https://en.wikipedia.org/wiki/Color_Cell_Compression)画像圧縮アルゴリズムの修正版を使用して圧縮します。ColorCellは90年代初頭に広く使用され、主に[Apple Video](https://en.wikipedia.org/wiki/Apple_Video)アルゴリズムで使用されました。

Moddable SDKの`image2cs`ツールは、`ColorCellOut`クラスを使用してJPEG、PNG、GIF画像および画像シーケンスを圧縮します。

`ColorCellOut`クラスの`send`関数は、各呼び出しで提供される走査線の数が4の倍数であることを要求します。これにより、`send`の各呼び出しで画像の完全な行をエンコードすることができます。

`begin`関数は、フレームの圧縮オプションの辞書を5番目のオプションパラメータとして受け取ります。この辞書には、0から1の値を持つオプションの`quality`プロパティが含まれており、0が最低品質、1が最高品質です。

#### プロパティ

| 名前 | 型 | 読み取り専用 | 説明 |
| :---: | :---: | :---: | :--- |
| `bitmap ` | `Bitmap` | ✓ | `ColorCellOut`インスタンスのピクセルにアクセスするための`Bitmap`インスタンスを返します。

***

<a id="asset-parsing"></a>
## アセット解析

ディスプレイ用のユーザーインターフェースを構築するには、アイコン、ビットマップ、写真、フォントなどの視覚的なアセットが必要です。これらのアセットを保存するための一般的に使用されるファイル形式は多数あり、その中には制約のあるデバイスでうまく機能するものもあります。Commodettoには、いくつかの一般的なアセットファイル形式を直接使用するための関数が含まれていますが、Commodettoはこれらのファイル形式のすべての機能をサポートしているわけではありません。Commodettoで使用するアセットを作成するグラフィックデザイナーは、アセット形式の要件を理解しておく必要があります。

> **注:** 最適なレンダリング性能と最小のストレージサイズを実現するために、ビルド時にアセットをターゲットデバイスの推奨フォーマットに変換することが有益です。

<a id="bmp"></a>
### BMP

[BMP](https://msdn.microsoft.com/en-us/library/dd183391.aspx) ファイルフォーマットは、Windowsでの使用のためにMicrosoftによって作成されました。これは、さまざまなフォーマットの非圧縮ピクセルの柔軟なコンテナです。このフォーマットは明確に文書化されており、グラフィックツールによってよくサポートされています。BMPはCommodettoでの非圧縮ビットマップの推奨フォーマットです。Commodetto BMPファイルパーサーは、以下のBMPのバリエーションをサポートしています：

* **16ビット 565 リトルエンディアンピクセル** -- 画像の幅は2の倍数でなければなりません。Photoshopを使用してBMPファイルを保存する際は、BMPダイアログで**Advanced Mode**を選択し、**R5 G6 B5**をチェックしてください。
* **16ビット 555 リトルエンディアンピクセル** -- 画像の幅は2の倍数でなければなりません。このフォーマットはBMPファイルの最も一般的な16ビットフォーマットですが、推奨されません。ピクセルを565に変換する必要があり、時間がかかり、変換されたピクセルを保存するためのメモリが必要です。
* **8ビット グレー** -- 画像の幅は4の倍数でなければならず、グレーパレットを持っている必要があります。
* **8ビット カラー** -- 画像の幅は4の倍数でなければならず、RGB 332パレットを持っている必要があります。
* **4ビット グレー** -- 画像の幅は8の倍数でなければならず、グレーパレットを持っている必要があります。Photoshopで作業する際は、BMPフォーマットで画像を保存する前に**Image Mode**を**Grayscale**に設定してください。
* **4ビット カラー** -- 画像の幅は8の倍数でなければなりません。
* **1ビット 白黒** -- 画像の幅は32の倍数でなければなりません。

デフォルトでは、BMPファイルはファイル内でビットマップの下の行から順に保存されます。例えば、下から上への順序です。Commodettoでは、ビットマップを上から下に保存する必要があります。BMPファイルを保存する際には、上から下の順序で保存するオプションを選択してください。Photoshopでは、BMPを上から下の順序で保存するために、**Flip row order** をチェックしてください。

`parseBMP` 関数は、BMPデータを含む `ArrayBuffer` または `HostBuffer` からビットマップを作成します。ファイル形式がサポートされていることを確認するための検証を行い、サポートされていないBMPのバリアントを検出した場合には例外をスローします。

```javascript
import parseBMP from "commodetto/parseBMP";

let bmpData = new Resource("image.bmp");
let bitmap = parseBMP(bmpData)
trace(`Bitmap width ${bitmap.width}, height ${bitmap.height}\n`);
```

***

<a id="jpeg"></a>
### JPEG

JPEGファイル形式は、写真を保存するための最も一般的な形式です。多くのリソース制約のあるデバイスは、JPEG画像を解凍する性能を持っていますが、結果を保存するメモリを持っているわけではありません。Commodettoは、解凍されたJPEG画像がメモリに収まらなくても、JPEG画像を出力にレンダリングする方法を提供します。

CommodettoのJPEGデコーダーは、JPEG仕様のサブセットをサポートしています。YUVエンコーディングではH1V1とH2V2がサポートされています。グレースケールのJPEG画像もサポートされており、JPEG画像の幅と高さに制限はありません。プログレッシブJPEG画像はサポートされていません。

#### 画像全体の解凍
オフスクリーンビットマップに完全なJPEGデータを解凍するには、`loadJPEG` 関数を使用します。

```javascript
import loadJPEG from "commodetto/loadJPEG";

let jpegData = new Resource("image.jpg");
let bitmap = loadJPEG(jpegData);
```

#### ブロックごとのデコード
JPEGデコーダーは、ブロック単位でデコードするモードも実装しており、解凍されたデータの単一ブロックを一度に返します。

```javascript
import JPEG from "commodetto/readJPEG";

let jpegData = new Resource("image.jpg");
let decoder = new JPEG(jpegData);

while (decoder.ready) {
	let block = decoder.read();
	trace(`block x: ${block.x}, y: ${block.y},
			width: ${block.width}, height: ${block.height}\n`);
}
```

返される各ブロックはビットマップです。ビットマップの `width` と `height` フィールドはブロックの寸法を示します。幅と高さはブロックごとに変わることがあります。`x` と `y` プロパティは、フルJPEG画像の左上隅に対するブロックの配置を示します。ブロックは左から右、上から下の順に返されます。

同じビットマップオブジェクトがすべてのブロックに使用されるため、`read` を呼び出すたびにブロックの内容が変わります。これは、アプリケーションが後でレンダリングするためにすべてのブロックを配列に収集できないことを意味します。そのためには、アプリケーションが各ブロックからデータをコピーする必要があります。

レンダラーを使用すると、JPEG画像をデコードしながらブロックごとにディスプレイに送信することが容易になり、各ブロックのデータをコピーする必要がなくなります。Pocoレンダラーのドキュメントには、この技術の例が含まれています。

#### ストリーミングデコード

前のセクションでは、完全な圧縮JPEG画像が利用可能な場合にJPEG画像をデコードする方法を説明しました。JPEGデコーダーは、ストリーミングJPEG画像、つまりネットワーク経由で到着するJPEGデータをデコードするためにも使用できます。このアプローチには、メモリに圧縮されたJPEG画像全体を保存する必要がないという利点があります。代わりに、ネットワークバッファが到着するとJPEGデコーダーにプッシュされ、`read`を使用して利用可能なバッファから可能な限り多くのデコードされたブロックを取得します。

JPEGデコーダーをストリーミングモードで使用するには、引数なしでコンストラクタを呼び出します：

```javascript
let decoder = new JPEG();
```

データが到着したら、データを含む`ArrayBuffer`をデコーダーにプッシュします：

```javascript
decoder.push(buffer);
```

最後のバッファが到着したら、データストリームの終了を示すパラメータを指定して`push`を呼び出します。これを行わないと、画像の下部からブロックが欠落する可能性があります：

```javascript
decoder.push();
```

JPEGデコーダーの`ready`プロパティは、読み取り可能なブロックがあることを示します。`ready`プロパティが初めてtrueを返すときは、JPEGヘッダーが正常に解析され、JPEGの`width`および`height`プロパティが初期化されたことも示します。

```javascript
decoder.push(buffer);
while (decoder.ready) {
	let block = decoder.read();
	// render block
}
```

> **注:** Commodettoは、メモリ使用量を最小限に抑えるよう最適化された、優れたパブリックドメインの[picojpeg](https://code.google.com/archive/p/picojpeg/)デコーダーを使用しています。品質とパフォーマンスの一部は犠牲になりますが、結果は一般的に非常に良好です。コンパイラの警告を排除するためにpicojpegに小さな変更が加えられており、それらの変更はModdable SDKのソースコード配布に含まれています。

***

<a id="png"></a>
### PNG

PNG画像フォーマットは、ボタンやスライダーなどのユーザーインターフェース要素のアセットに一般的に使用されます。PNGファイルフォーマットは大幅に圧縮されているため、PNG画像は使用するために`BufferOut`インスタンスに解凍される必要があります。また、PNGで使用される圧縮のため、画像を解凍するにはかなりのメモリが必要です。それにもかかわらず、ユーザーインターフェース作業でPNGが非常に一般的であるため、Commodettoは、デバイスや実用的なシナリオで使用するためのPNGモジュールを実装しています。

CommodettoのPNGデコーダーは、PNGファイルフォーマットのほとんどのバリエーションをサポートしていますが、2つの例外があります：


- インターレース画像はサポートされていません。インターレースはプログレッシブデコードと互換性がないためです。

- 16ビットチャンネルの画像はサポートされていません。高解像度がターゲットデバイスの画像品質能力を超えるためです。

オフスクリーンビットマップにPNGデータをデコードするには、静的な`decompress`関数を使用します。

```javascript
import PNG from "commodetto/readPNG";

let pngData = new Resource("image.png");
let bitmap = PNG.decompress(pngData);
```

PNGの`decompress`関数は、以下のPNGバリアントをサポートしています：

- 3チャンネル、各チャンネル8ビット（24ビットRGB）
- 4チャンネル、各チャンネル8ビット（32ビットRGBA）
- 1チャンネル、各チャンネル8ビット（8ビットグレー）
- 1チャンネル、各チャンネル8ビットのパレット付き（8ビットインデックスRGB）
- 1チャンネル、各チャンネル1ビット（1ビットモノクローム）

アルファチャンネルを含む画像の場合、`decompress`のオプションの第2引数に`true`を渡します。アルファチャンネルは、返されるビットマップ画像の`mask`プロパティとして4ビットグレービットマップで返されます。

```javascript
let pngData = new Resource("image_with_alpha.png");
let bitmap = PNG.decompress(pngData, true);
let alpha = bitmap.mask;
```

PNGデコーダーは、進行的デコードモードも実装しており、解凍されたデータの単一スキャンラインを一度に返します。スキャンラインはPNGデコーダーからの生データで、ピクセル変換は適用されていません。以下の例は、24ビット（RGB）または32ビット（RGBA）のPNG画像を16ビットBMPに変換します。

```js
let png = new PNG(new Resource("image.png"));
let width = png.width, height = png.height;
trace(`width ${width}, height ${height}, channels ${png.channels}, depth ${png.depth}, bpp ${png.channels * png.depth}\n`);

if ((8 != png.depth) || ((3 != png.channels) && (4 != png.channels)))
	throw new Error("unsupported PNG variant");

let bmp = new BMPOut({width: width, height: height, pixelFormat: Bitmap.RGB565LE, path: "image.bmp"});
bmp.begin(0, 0, width, height);

let scan16 = new Uint16Array(width);
for (let i = 0; i < height; i++) {
	let scanLine = png.read();
	for (let j = 0, offset = 0; j < width; j++, offset += png.channels) {
		let r = scanLine[offset    ];
		let g = scanLine[offset + 1];
		let b = scanLine[offset + 2];
		scan16[j] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
	}
	bmp.send(scan16.buffer);
}
bmp.end();
```

`width`と`height`プロパティに加えて、PNGインスタンスにはPNG画像の内容に基づいた`depth`と`channels`プロパティが含まれています。

PNGデコーダーは、画像をデコードする際に最大45 KBのメモリを使用します。このメモリ量は、すべてのターゲットデバイスで利用可能または実用的であるとは限りません。このメモリ要件は主に、PNG画像で使用されるzlib圧縮アルゴリズムによるものです。

> **注:** Commodettoは、PNG画像に含まれる[zlib](http://www.zlib.net/manual.html)データを解凍するために、パブリックドメインの[miniz](https://github.com/richgel999/miniz/)ライブラリを使用しています。PNGの解析は、KinomaJSのApacheライセンスの[`FskPNGDecodeExtension.c`](https://github.com/Kinoma/kinomajs/blob/master/extensions/FskPNGDecode/sources/FskPNGDecodeExtension.c)に部分的に基づいており、大幅に簡略化されています。

***

<a id="bmfont"></a>
### BMFont

[BMFont](http://www.angelcode.com/products/bmfont/)は、ビットマップフォントを保存するためのフォーマットです。これは、OpenGLを使用して効率的にレンダリングされる形式で、ゲームに独自のフォントを埋め込むために広く使用されています。BMFontはよく設計されており、サポートが簡単です。Commodettoは、アンチエイリアスフォントとマルチカラーフォントの両方を保存するためにBMFontを使用します。さらに、BMFontには優れたツールサポートがあります。特に、macOSフォントをCommodetto互換のBMFontに変換する優れた[Glyph Designer](https://71squared.com/glyphdesigner)があります。WindowsおよびLinuxユーザー向けには、コマンドラインの[bmfont](https://github.com/vladimirgamalyan/fontbm)がうまく使用されています。

BMFontは、フォントのメトリクスデータをフォントのグリフアトラス（ビットマップ）とは別に保存します。これは、BMFontをロードするには、メトリクスをロードし、グリフアトラスをロードするという2つのステップが必要であることを意味します。BMFontは、メトリクスデータをテキスト、XML、バイナリなどのさまざまな形式で保存することを可能にします。Commodettoは、メトリクスのバイナリ形式をサポートしています。

`parseBMF` 関数は、BMFontバイナリメトリクスファイルを `Render` オブジェクトで使用するために準備します。

```javascript
import parseBMF from "commodetto/parseBMF";
import parseBMP from "commodetto/parseBMP";

let palatino36 = parseBMF(new Resource("palatino36.fnt"));
palatino36.bitmap = parseBMP(new Resource("palatino36.bmp"));
```

メトリクスが `parseBMF` で準備された後、グリフアトラスは `parseBMP` を使用して準備され、メトリクスに `bitmap` プロパティとして添付されます。

文字の不連続な範囲を持つBMFontファイルがサポートされています。Commodettoは、BMFontに存在する可能性のあるカーニングデータを使用するように設定できますが、デフォルトでは無効になっています。

アンチエイリアスされたテキストの場合、グリフアトラスビットマップを含むBMPファイルは4ビットグレイ形式でなければなりません。マルチカラーテキストの場合、ビットマップは `Bitmap.default` 形式（例：Commodettoがレンダリングするように設定されているピクセル形式）でなければなりません。

Commodettoは、RLE圧縮されたグリフでBMFont形式を拡張します。グリフは `RLE4Out` クラスを使用して個別に圧縮されます。Moddable SDKツール `compressbmf` は圧縮を実行します。このツールはまた、圧縮されたグリフを `.fnt` メトリクスファイルに追加し、単一のフォントのメトリクスとグリフデータを単一のファイルに保存します。

***

<a id="rendering"></a>
## レンダリング

Commodettoは複数のレンダリングエンジンをサポートするように設計されています。最初のエンジンは、小さなビットマップベースのスキャンラインレンダラーである[Poco](./poco.md)です。レンダラーはピクセルを描画する方法を知っており、ディスプレイ、オフスクリーンバッファ、またはファイルにピクセルを出力するために`PixelsOut`インスタンスに依存します。

レンダラーが作成されると、出力にバインドされます。例えば、BMPファイルにレンダリングするには：

```javascript
import BMPOut from "commodetto/BMPOut";
import Poco from "commodetto/Poco";

let bmpOut = new BMPOut({width: decoded.width, height: decoded.height,
		pixelFormat: Bitmap.RGB565LE, path: "allegra64.bmp"});
let render = new Poco(bmpOut);
```

ディスプレイにレンダリングするには、`BMPOut`の代わりに`SPIOut`を使用します。以下のようにします：

```javascript
import SPIOut from "commodetto/SPIOut";
import Poco from "commodetto/Poco";

let display = new SPIOut({width: 320, height: 240,
		pixelFormat: Bitmap.RGB565LE, dataPin: 30});
let render = new Poco(display);
```

Pocoレンダラーのドキュメントには、一般的な使用例を含むレンダリング操作が記載されています。

***

<a id="render-class"></a>
### Renderクラス

`Render`クラスはピクセルを生成するための抽象基底クラスです。これは、Pocoのような特定のレンダリングエンジンによってオーバーライドされます。`Render`クラスには、レンダリングプロセスを管理するだけで、実際のレンダリングを行わない4つの関数しかありません。利用可能な具体的なレンダリング操作は、`Render`のサブクラスによって定義されます。

以下の例は、`SPIOut`を使用してPocoレンダラーを使い、白い背景に `{x: 5, y: 5}` の位置に10ピクセルの赤い四角を描画する方法を示しています。

```javascript
let display = new SPIOut({width: 320, height: 240,
		pixelFormat: Bitmap.RGB565LE, dataPin: 30});
let render = new Poco(display);

let white = poco.makeColor(255, 255, 255);
let red = poco.makeColor(255, 0, 0);

render.begin();
render.fillRectangle(white, 0, 0, display.width, display.height);
render.fillRectangle(red, 5, 5, 10, 10);
render.end();
```

#### 関数

##### `constructor(pixelsOut, dictionary)`

`Render` コンストラクタは2つの引数を取ります。1つは `Render` オブジェクトが出力用にピクセルを送信する `PixelsOut` インスタンス、もう1つはレンダリングオプションを設定するための辞書です。すべての辞書プロパティは `Render` のサブクラスによって定義されます。

***

##### `begin(x, y, width, height)`

`begin` 関数はフレームのレンダリングを開始します。更新される領域は、`x` と `y` の座標、および `width` と `height` の寸法で指定されます。この領域は、レンダラーにバインドされた `PixelsOut` インスタンスの境界内に完全に含まれている必要があります。

```javascript
render.begin(x, y, width, height);
```

すべての描画は、`begin` で定義された更新領域にクリップされます。

引数なしで `begin` を呼び出すことは、`x` と `y` を0に、`width` と `height` を `render` インスタンスの `width` と `height` の値に等しくして呼び出すことと同等です。つまり、次のように

```javascript
render.begin()
```

は次のコードと同等です：

```javascript
render.begin(0, 0, render.width, render.height);
```

`width` と `height` が省略された場合、更新領域は `begin` に渡された `x` と `y` 座標で定義される矩形になります。`render` の境界の右下隅は次のようになります。

```javascript
render.begin(x, y);
```

は次のコードと同等です：

```javascript
render.begin(x, y, render.width - x, render.height - y);
```

***

##### `end()`

`end` 関数はフレームのレンダリングを完了します。この関数によって、保留中のすべてのレンダリング操作が完了します。

> **注意:** Poco のようなディスプレイリストレンダラーの場合、すべてのレンダリングは `end` の実行中に行われます。そのため、描画呼び出しの直後にディスプレイが更新されるわけではありません。

***

##### `continue(x, y, width, height)`

`continue` 関数は、単一のフレーム内に複数の更新領域がある場合に使用されます。`continue` の引数は `begin` の引数と同様に動作します。`continue` 関数は次のシーケンスにほぼ等しいです：

```javascript
render.end();
render.begin(x, y, width, height);
```

違いはバッファを持つディスプレイの場合です。例えば、Commodettoがページフリッピングハードウェアを持つディスプレイやコンピュータシミュレータ上で動作している場合です。このような場合、`end`が呼び出されたときにのみ出力フレームが表示されます。`continue`を使用すると、完全なフレームがレンダリングされるまで中間結果はオフスクリーンに隠れたままになります。

次のフラグメントは、単一フレームのための3つの別々の更新領域を示しています。

```javascript
render.begin(10, 10, 20, 20);
// Draw
...
render.continue(200, 100, 40, 40);
// Draw more
...
render.continue(300, 0, 20, 20);
// Draw even more
...
render.end();
```

***

##### `adaptInvalid(r)`

レンダラーの`adaptInvalid`関数は、レンダリングに使用される`PixelsOut`の更新領域制約に合わせて、`r`引数で定義された更新領域を調整するために使用されます。レンダラーの`adaptInvalid`の実装は、特に回転を考慮して、レンダラーの機能を考慮しながら`PixelsOut`の`adaptInvalid`関数を呼び出します。

```javascript
let r = renderer.rectangle(x, y, w, h);
renderer.adaptInvalid(r);
renderer.begin(r.x, r.y, r.w, r.h);
...
```

単一のディスプレイコントローラーで動作するように書かれたアプリケーションは、その制約を自分で適用することで`adaptInvalid`を無視しても安全であるべきです。複数のディスプレイコントローラーで動作するように書かれたコードは、ディスプレイのサブセクションへの信頼性のある更新のために`adaptInvalid`を使用する必要があるでしょう。

Piuユーザーインターフェースフレームワークは必要に応じて`adaptInvalid`を呼び出すため、アプリケーションスクリプトが直接呼び出す必要はありません。

***

<a id="pixel-format-conversion"></a>
## ピクセルフォーマット変換

Commodettoは、主にコンピュータ上で画像を処理するツールで使用されることを目的としたピクセルフォーマット変換機能を提供します。これらは、例えばModdable SDKの`image2cs`や`png2bmp`ツールで使用されます。コンバータは小型で効率的であるため、マイクロコントローラへの展開にも使用できます。

<a id="convert-class"></a>
### Convertクラス

`Convert`クラスは、Commodettoがサポートする2つのピクセルフォーマット間で変換を行います。変換はネイティブコードで実装されているため、JavaScriptで書かれた場合よりも高速に実行されます。

```js
import Convert from "commodetto/Convert";
```

`Convert`クラスはピクセルの配列に対して動作します。Bitmapや行バイト（またはストライド）の概念はAPIの一部ではありません。代わりに、それらは`Convert`クラスを呼び出すコードによって処理される必要があります。

#### `constructor(src, dst)`

コンストラクタは2つのピクセルフォーマット、ソースフォーマットとデスティネーションフォーマットを受け取ります。

以下は、RGB565LEからGray256にピクセルを変換するためのコンバータをインスタンス化する例です。

```js
let converter = new Convert(Bitmap.RGB565LE, Bitmap.Gray256);
```

***

#### `process(src, dst)`
#### `process(src, srcOffset, srcLength, dst, src, dstOffset, dstLength)`

`process`関数はピクセル変換を行います。`src`引数はコンストラクタで指定されたフォーマットの入力ピクセルです。入力と出力のピクセルは、`ArrayBuffer`、`TypedArray`、`DataView`、または`HostBuffer`のバッファに格納されます。

`process`を呼び出す方法は2つあります。1つ目は入力と出力のバッファのみを渡す方法です。2つ目は、使用する入力と出力バッファ内のオフセットと長さを渡す方法です。`process`は常に入力と出力バッファのビューを尊重し、渡されたオフセットと長さをビューに適用します。

```js
converter.process(inputPixels, outputPixels);
```

`process`の呼び出し側は、十分な大きさの出力バッファを確保する責任があります。この計算には`Bitmap.depth`関数が便利です。

```js
let outputPixelFormat = Bitmap.Gray256;
let inputPixelCount = 240;
let outputBufferSize = ((Bitmap.depth(outputPixelFormat) * inputPixelCount) + 7) >> 3;
let outputPixels = new ArrayBuffer(outputBufferSize);
```

***

<a id="odds-and-ends"></a>
## 雑多な事項

### 対象ハードウェア

Commodettoの主な設計制約は、リソースが制限されたマイクロコントローラ上で最新のユーザーインターフェースをレンダリングすることです。対象デバイスは以下のような広範な特性を持っています：

- ARM Cortex M3/M4 CPU
- シングルコア
- 80から200 MHZのCPU速度
- 128から512 KBのRAM
- 1から4 MBのフラッシュストレージ
- グラフィックスレンダリングハードウェアなし

より高性能なプロセッサが利用可能であれば、Commodettoはさらに高いパフォーマンスを発揮します。より多くのメモリがあれば、オフスクリーンのビットマップやより複雑なシーンを処理できます。CPU速度が速いほど、計算負荷の高いグラフィックス操作をより多く活用できます。フラッシュストレージが増えれば、より多くのアセットを保存できます。ハードウェアによるグラフィックスアクセラレーションを利用することで、より複雑な画面をより高速にレンダリングできます。

Commodettoは、XS JavaScriptエンジンをサポートする任意のハードウェアを対象に動作します。CommodettoはANSI Cで書かれており、外部関数（`memcpy`、`malloc`、および`free`）への呼び出しはほんの一握りです。コアのPocoレンダラーはメモリを割り当てません。

***

### 「Commodetto」という名前について

*commodetto* という言葉は、音楽用語で「ゆったりと」という意味です。ここでのCommodettoという名前の使用は、ベートーヴェンのピアノ変奏曲集、特にWoO 66の第3変奏から取られています。変奏のサンプルは[こちらで試聴可能](http://www.prestoclassical.co.uk/r/Warner%2BClassics/5857612)です。この変奏の雰囲気は軽やかでゆったりとしていますが、作曲や演奏には何も単純なことや些細なことはありません。
