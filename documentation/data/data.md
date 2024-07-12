# Data
Copyright 2017-2024 Moddable Tech, Inc.<BR>
改訂： 2024年3月29日

## 目次

* [Base64](#base64)
* [Hex](#hex)
* [CRC](#crc)
* [QRCode](#qrcode)
* [TextDecoder & Text Encoder](#text)
* [Inflate & Deflate (zlib)](#zlib)
* [URL & URLSearchParams](#url)

<a id="base64"></a>
## Base64 クラス

> **注意**: Base64エンコーディングとデコーディングは、現在JavaScriptで直接サポートされています。詳細は[Base64 proposal](https://tc39.es/proposal-arraybuffer-base64/)を参照してください。Moddable SDKはもはやBase64モジュールを使用していません。Base64モジュールは互換性のためにまだサポートされていますが、新しいコードでの使用は推奨されていません。

`Base64` クラスは、[RFC 4648](https://tools.ietf.org/html/rfc4648) で定義されたBase64アルゴリズムを使用してエンコードおよびデコードするための静的関数を提供します。

```js
import Base64 from "base64";
```

プロジェクトで使用するには、モジュールのマニフェストを含めます：

```json
	"include": [
		"$(MODULES)/data/base64/manifest.json"
	]
```

### `static decode(str)`

`decode` 関数は、Base64でエンコードされた `String` を受け取り、デコードされたバイトを含む `ArrayBuffer` を返します。

```js
trace(Base64.decode("aGVsbG8sIHdvcmxk") + "\n");
// output: "hello, world"
```

### `static encode(data)`

`encode` 関数は `String` またはバッファを受け取り、Base64エンコードが適用されたデータを含む `ArrayBuffer` を返します。

```js
trace(Base64.encode("hello, world") + "\n");
// output: "aGVsbG8sIHdvcmxk"
```

> **注意**: 文字列が提供された場合、その内容は Base64 エンコードを行う際に UTF-8 エンコードされた文字として扱われます。

<a id="hex"></a>
## Hex クラス

> **注意**: Hex エンコードおよびデコードは、現在 JavaScript で直接サポートされています。詳細は [提案](https://tc39.es/proposal-arraybuffer-base64/) を参照してください。Moddable SDK はもはや Hex モジュールを使用していません。Hex モジュールは互換性のためにまだサポートされていますが、新しいコードでの使用は推奨されません。

`Hex` クラスは、`ArrayBuffer` と16進エンコードされた `String` 値の間で変換するための静的関数を提供します。

```js
import Hex from "hex";
```

プロジェクトで使用するには、モジュールのマニフェストを含めます：

```json
	"include": [
		"$(MODULES)/data/hex/manifest.json"
	]
```

### `static toBuffer(string [, separator])`

`toBuffer` 関数は、オプションのセパレータを使用して、[16進数](https://en.wikipedia.org/wiki/Hexadecimal)でエンコードされた `String` を `ArrayBuffer` に変換します。

```js
let b0 = Hex.toBuffer("0123456789abcdef");
let b1 = Hex.toBuffer("01:23:45:67:89:AB:CD:EF", ":");
```

16進数の数字は大文字でも小文字でもかまいません。オプションのセパレータ引数が提供されている場合、それは各16進数の数字のペアの間に現れる必要があります。

オプションのセパレータは、7ビットASCII範囲内の1文字でなければなりません。

### `static toString(buffer [[, separator], hexChars]);`

`toString` 関数は、バッファを16進数でエンコードされた `String` に変換します。

```js
let buffer = Hex.toBuffer("0123456789abcdef");
let s0 = Hex.toString(buffer, ".");
// s0 is 01.23.45.67.89.AB.CD.EF
let s1 = Hex.toString(buffer);
// s1 is 0123456789ABCDEF
```

オプションのセパレータは、7ビットASCII範囲内の1文字でなければなりません。

オプションの `hexChars` 引数には、16進数エンコードに使用する16文字を含めることができます。文字は7ビットASCIIでなければなりません：

```js
let buffer = Hex.toBuffer("0123456789abcdef");
let s0 = Hex.toString(buffer, "-", "0123456789abwxyz");
// s0 は 01-23-45-67-89-ab-wx-yz です
```

<a id="crc"></a>
## CRC8, CRC16クラス

`CRC8` と `CRC16` クラスはデータのCRCチェックサムを計算します。

```
import {CRC8} from "crc";
import {CRC16} from "crc";
```

モジュールのマニフェストをプロジェクトに含めて使用します：

```json
	"include": [
		"$(MODULES)/data/crc/manifest.json"
	]
```

#### `CRC8(polynomial [, initial [, reflectInput [, reflectOutput [, xorOutput]]]])`
#### `CRC16(polynomial [, initial [, reflectInput [, reflectOutput [, xorOutput]]]])`

`CRC8` と `CRC16` 関数は、計算するCRCチェックサムを指定するためのいくつかのオプションを取ります。

| パラメータ | デフォルト | 説明 |
| :---: | :---: | :--- |
| `polynomial` | (なし) | 使用する多項式 (必須) |
| `initial` | `0` | 初期CRCアキュムレータ値 (オプション) |
| `reflectInput` | `false` | `true` の場合、各入力バイトは使用される前に反転されます (オプション) |
| `reflectOutput` | `false` | `true` の場合、各出力バイトは返される前に反転されます。出力の反転はCRC値全体に対して行われます (オプション) |
| `xorOutput` | `0` | 最終CRC値にXORする値 (オプション) |

`polynomial`、`initial`、および `xorOutput` の値は、CRC8の場合は8ビット整数、CRC16の場合は16ビット整数です。

[crc example](../../examples/data/crc/main.js) では、一般的なCRCチェックサムのパラメータの定義を示しています：

- `CRC-8`
- `CRC-8/CDMA2000`
- `CRC-8/DARC`
- `CRC-8/DVB-S2`
- `CRC-8/EBU`
- `CRC-8/I-CODE`
- `CRC-8/ITU`
- `CRC-8/MAXIM`
- `CRC-8/ROHC`
- `CRC-8/WCDM`
- `CRC-16/CCITT-FALSE`
- `CRC-16/ARC`
- `CRC-16/ARG-CCITT`
- `CRC-16/BUYPASS`
- `CRC-16/CDMA2000`
- `CRC-16/DDS-110`
- `CRC-16/DECT-R`
- `CRC-16/DECT-X`
- `CRC-16/DNP`
- `CRC-16/EN-13757`
- `CRC-16/GENIBUS`
- `CRC-16/MAXIM`
- `CRC-16/MCRF4XX`
- `CRC-16/RIELLO`
- `CRC-16/T10-DIF`
- `CRC-16/TELEDISK`
- `CRC-16/TMS37157`
- `CRC-16/USB`
- `CRC-A`
- `CRC-16/KERMIT`
- `CRC-16/MODBUS`
- `CRC-16/X-25`
- `CRC-16/XMODE`


### `close()`

`close` 関数は、CRCチェックサム計算に関連するリソースを解放します。

### `checksum(buffer)`

`checksum` 関数は、`buffer` に提供されたデータにCRC計算を適用します。CRCチェックサムが返されます。

`buffer` パラメータは `String` またはバッファである可能性があります。

`checksum` 関数は複数回呼び出すことができます。呼び出されるたびにCRCが更新されて返されます。新しい計算を開始するには `reset` 関数を呼び出します。

### `reset()`

`reset` 関数はCRCアキュムレータを `initial` 値にクリアします。

<a id="qrcode"></a>
## QRCodeクラス
`QRCode` クラスは、文字列やバッファからQRコードデータを生成します。そのデータはさまざまな方法でレンダリングできます。[Poco](https://github.com/Moddable-OpenSource/moddable/tree/public/modules/commodetto/qrcode) および [Piu](https://github.com/Moddable-OpenSource/moddable/tree/public/modules/piu/MC/qrcode) への拡張が提供されており、効率的にQRコードをレンダリングできます。コア実装は [Project Nayuki](https://www.nayuki.io/page/qr-code-generator-library) のQRコードジェネレータライブラリです。

```js
import qrCode from "qrcode";
```

プロジェクトで使用するには、モジュールのマニフェストを含めます：

```json
	"include": [
		"$(MODULES)/data/qrcode/manifest.json"
	]
```

詳細については、記事 [Moddable SDKのQRコードモジュール](https://blog.moddable.com/blog/qrcode/) を参照してください。

### `qrCode(options)`

`qrCode` 関数は、生成するQRコードを記述するオプションオブジェクトを受け取ります。この関数は、各バイトが白または黒のピクセルを示す0または1である `ArrayBuffer` を返します。返されるバッファには、生成されたQRコードの一辺のセル数を示す `size` プロパティがあります。

オプションオブジェクトでサポートされるプロパティは以下の通りです：

| プロパティ | 説明 |
| :---: | :--- |
| `maxVersion` |  生成されるQRコードの最大バージョンを示す1から40までの数値。バージョン番号はQRコードが含むことができるデータ量を決定します。実装は提供されたデータのサイズに対して可能な最小のバージョン番号を使用します。このプロパティはオプションで、デフォルトは40です。 |
| `input` |  QRコードにエンコードするデータを含む `String` またはバッファ。このプロパティは必須です。 |

`qrCode` 関数は、無効なパラメータが検出された場合やQRコードを生成するのに十分なメモリがない場合に例外をスローします。

```js
const code = qrCode({input: "Welcome to Moddable", maxVersion: 4});

// trace QR Code to console
code = new Uint8Array(code);
for (let y = 0; y < = code.size; y++) {
    for (let x = 0; x < = code.size; x++)
        trace(code[(y * code.size) + x] ? "X" : ".", "\n");
    trace("\n");
}
```

<a id="text"></a>
## TextDecoderクラス と TextEncoderクラス

`TextDecoder` と `TextEncoder` クラスは、JavaScriptの文字列とUTF-8データを含むメモリバッファ間の変換を実装します。

```js
import TextDecoder from "text/decoder";
import TextEncoder from "text/encoder";
```

これらをプロジェクトで使用するには、モジュールのマニフェストを含めます：

```json
	"include": [
		"$(MODULES)/data/text/decoder/manifest.json",
		"$(MODULES)/data/text/encoder/manifest.json"
	]
```

`TextDecoder` は [WHATWG によって指定された](https://encoding.spec.whatwg.org/#interface-textdecoder) [TextDecoder クラス](https://developer.mozilla.org/en-US/docs/Web/API/TextDecoder) を実装しています。UTF-8入力データのみを受け付けます。

`TextEncoder` は [WHATWG によって指定された](https://encoding.spec.whatwg.org/#interface-textencoder) [TextEncoder クラス](https://developer.mozilla.org/en-US/docs/Web/API/TextEncoder) を実装しています。この実装には `encodeInto()` が含まれています。

<a id="zlib"></a>
## zlib: Inflateクラス と Deflateクラス

`Inflate` と `Deflate` クラスはzlibの解凍と圧縮を実装します。JavaScript APIは [pako](https://github.com/nodeca/pako) ライブラリによって定義された [API](http://nodeca.github.io/pako/) に従います。
```

```js
import Inflate from "inflate";
import Deflate from "deflate";
```

プロジェクトでこれらのモジュールを使用するには、モジュールのマニフェストを含めます：

```json
	"include": [
		"$(MODULES)/data/zlib/manifest_deflate.json",
		"$(MODULES)/data/zlib/manifest_inflate.json"
	]
```

[inflateの例](../../examples/data/inflate/main.js)は、データをワンショット操作として解凍する方法と、ストリーミングのための`onData`コールバックを使用する方法を示しています。

> **注意**: zlibの解凍および特に圧縮には大量のメモリが必要です。これらのライブラリは、メモリ制約のためにすべてのマイクロコントローラで動作するわけではありません。

<a id="url"></a>
## URLクラス と URLSearchParamsクラス
`URL`と`URLSearchParams`クラスは、URLおよびその検索パラメータを操作するためのユーティリティを提供します。

```js
import URL from "url";
import {URL, URLSearchParams} from "url";
```

プロジェクトでこのモジュールを使用するには、モジュールのマニフェストを含めます：

```json
	"include": [
		"$(MODULES)/data/url/manifest.json"
	]
```

`URL`は[WHATWGによって仕様化された](https://url.spec.whatwg.org/#url-class) [URLクラス](https://developer.mozilla.org/en-US/docs/Web/API/URL)を実装しています。この実装は、2つの例外を除いて標準に完全に準拠しています： [Punycode](https://en.wikipedia.org/wiki/Punycode)および[IDNA](https://www.unicode.org/reports/tr46/)のサポートは未実装です。これらは主にブラウザでのユーザー入力URLの表示と安全な処理のために使用されますが、組み込みシステムでは一般的に問題にはなりません。多少の努力（およびコードサイズの増加）で、実装は両方をサポートすることができます。

`URLSearchParams`は、[WHATWGによって指定された](https://url.spec.whatwg.org/#urlsearchparams) [URLSearchParamsクラス](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams)を実装しています。

[両方のテスト](https://github.com/Moddable-OpenSource/moddable/tree/public/tests/modules/data/url)はModdable SDKに含まれています。これらのAPIをウェブブラウザで検証するために使用されるテストに基づいています。
