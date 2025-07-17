<!--
 | Copyright (c) 2016-2023  Moddable Tech, Inc.
 |
 |   This file is part of the Moddable SDK Runtime.
 |
 |   The Moddable SDK Runtime is free software: you can redistribute it and/or modify
 |   it under the terms of the GNU Lesser General Public License as published by
 |   the Free Software Foundation, either version 3 of the License, or
 |   (at your option) any later version.
 |
 |   The Moddable SDK Runtime is distributed in the hope that it will be useful,
 |   but WITHOUT ANY WARRANTY; without even the implied warranty of
 |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |   GNU Lesser General Public License for more details.
 |
 |   You should have received a copy of the GNU Lesser General Public License
 |   along with the Moddable SDK Runtime.  If not, see <http://www.gnu.org/licenses/>.
 |
 | This file incorporates work covered by the following copyright and
 | permission notice:
 |
 |       Copyright (C) 2010-2016 Marvell International Ltd.
 |       Copyright (C) 2002-2010 Kinoma, Inc.
 |
 |       Licensed under the Apache License, Version 2.0 (the "License");
 |       you may not use this file except in compliance with the License.
 |       You may obtain a copy of the License at
 |
 |        http://www.apache.org/licenses/LICENSE-2.0
 |
 |       Unless required by applicable law or agreed to in writing, software
 |       distributed under the License is distributed on an "AS IS" BASIS,
 |       WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 |       See the License for the specific language governing permissions and
 |       limitations under the License.
-->

# XS in C
更新日: 2023年11月17日

**著作権とライセンスについては[ドキュメント末尾](#license)を参照してください**

## このドキュメントについて

このドキュメントでは、XS JavaScriptエンジンのランタイムへのCインターフェースであるXS in Cについて説明します。対象ソフトウェア/ハードウェアプラットフォーム用のXSのビルドに関する情報は、付属のドキュメント[XS Platforms.md](./XS%20Platforms.md)で提供されています。

ECMAScript仕様に従い、XSランタイムはすべてのスクリプトが使用できる汎用機能のみを実装しています。アプリケーションは、Cコールバックを通じて独自のスクリプトが使用できる特定の機能を定義します。XSランタイムを使用するアプリケーションは、ECMAScript用語でホストと呼ばれます。

XS in Cは、オブジェクトのプロパティにアクセスするためのマクロを提供します。XSは、多くのマクロについて機能的に同等な2つのバリエーションを提供しています。`xs`のみが前置されたマクロは作業がやや便利ですが、より大きなバイナリコードを生成します。一方、`xsmc`が前置されたマクロは、使いにくくなる代償として、より小さなバイナリコードを生成します。`xsmc*`マクロを使用するには、"xsmc.h"をインクルードし、"xs.h"をインクルードしないでください。`xs`のみが前置されたマクロを使用するには、"xs.h"をインクルードし、"xsmc.h"をインクルードしないでください。`xsmc.h`ヘッダーファイルをインクルードすると、一部の操作の`xs`バージョンが利用可能になります。

## 目次

* [スロット](#slots): CコールバックでECMAScript構造を処理する方法を説明し、ECMAScriptとXS in Cの対応関係を示す例を提供します。
	* [スロット型](#slot-types)
	* [プリミティブ](#primitives)
	* [ArrayBuffer](#arraybuffer)
	* [インスタンスとプロトタイプ](#instances-and-prototypes)
	* [識別子](#identifiers)
	* [プロパティ](#properties)
	* [引数と変数](#xsvars)
	* [ガベージコレクタ](#garbage-collector)
	* [例外](#exceptions)
	* [エラー](#errors)
	* [デバッガー](#debugger)
* [マシン](#machine): XSランタイムの主要構造（仮想マシン）を紹介し、ランタイムを使用してホストを構築し、スクリプトでCコールバックを利用可能にする方法を説明します。このセクションは、XS in CでC関数を使用してJavaScriptクラスを実装する方法を示す例で締めくくられます。
	* [マシン割り当て](#machine-allocation)
	* [コンテキスト](#context)
	* [ホスト](#host)
	* [JavaScript `@` 言語構文拡張](#syntax-extension)
* [用語集](#glossary): このドキュメントで定義または参照されているすべての用語が含まれます。
* [ライセンス](#license)

<a id="slots"></a>
## スロット

XSランタイムでは、*スロット*は基本的なストレージ単位です。スロットは、XS in Cを通じてのみ操作される不透明な構造です。

```c
typedef struct xsSlotRecord xsSlot
struct xsSlotRecord {
	void* data[4];
};
```

<a id="slot-types"></a>
### スロット型

スロットには11種類あります：

```c
enum {
	xsUndefinedType,
	xsNullType,
	xsBooleanType,
	xsIntegerType,
	xsNumberType,
	xsStringType,
	xsStringXType,
	xsSymbolType,
	xsBigIntType,
	xsBigIntXType,
	xsReferenceType
}
typedef char xsType;
```

undefined、null、boolean、number、string、symbol、bigintスロットは、ECMAScriptプリミティブ型に対応します。referenceスロットは、ECMAScript `reference`型に対応します。integer、stringx、bigintxスロットは、スクリプトでは観察できない最適化です。integerスロットはnumberスロットと同等ですが、浮動小数点演算をバイパスできます。stringxスロットはstringスロットと同等ですが、コピーを作成せずに（例：ROM内の）文字列をその場で使用します。bigintxスロットはbigintスロットと同等ですが、コピーを作成せずに（例：ROM内の）bigintをその場で使用します。

##### In ECMAScript:

```javascript
undefined
null
false
true
0
0.0
"foo"
```

##### In C:

```c
xsUndefined;
xsNull;
xsFalse;
xsTrue;
xsInteger(0);
xsNumber(0.0);
xsString("foo");
```

`xsTypeOf`マクロは、スロットの型を返します。これはECMAScript `typeof`キーワードに似ています。

**`xsType xsTypeOf(xsSlot theSlot)`**<BR>
**`xsType xsmcTypeOf(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot`|  テストするスロット

スロットの型を返します

> **注意**: XS in C APIのマクロは、現在のスコープ内で`xsMachine *`型の`the`という名前の変数にターゲット仮想マシンへの参照が必要です。

##### In ECMAScript:

```javascript
switch(typeof arguments[0]) {
	case "undefined": break;
	/* Null is an object. */
	case "boolean": break;
	/* Integers are numbers */
	case "number": break;
	/* StringX is a string */
	case "string": break;
	case "symbol": break;
	/* BigIntX is a bigint */
	case "bigint": break;
	case "object": break;
	case "function": break;
}
```

##### In C:

```c
switch(xsTypeOf(xsArg(0))) {
	case xsUndefinedType: break;
	case xsNullType: break;
	case xsBooleanType: break;
	case xsIntegerType: break;
	case xsNumberType: break;
	case xsStringType: break;
	case xsSymbolType: break;
	case xsBigIntType: break;
	case xsReferenceType: break;  /* Objects and functions are references. */
}
```

<a id="primitives"></a>
### プリミティブ

undefined、null、boolean、integer、number、string、symbolスロット（総称して*ダイレクトスロット*と呼ばれる）は、ECMAScriptプリミティブ型に対応し、integerとstringxスロットは最適化として追加されています。

#### Undefinedとnull

undefinedとnullスロットは値を含みません。`xsUndefined`と`xsNull`マクロは、これらの型のスロットを返します。

**`xsSlot xsUndefined`**

undefinedスロットを返します

***

**`void xsmcSetUndefined(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | `undefined`に設定するスロット

指定されたスロットの値を`undefined`に設定します

***

**`xsSlot xsNull`**

nullスロットを返します

***

**`void xsmcSetNull(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | `null`に設定するスロット

指定されたスロットの値を`null`に設定します

***

#### ブール値、整数、数値

これらのスロットは対応する型の値を含みます。

	typedef char xsBooleanValue;
	typedef long xsIntegerValue;
	typedef double xsNumberValue;

以下のマクロは、これらの各型のスロット（特定の値に設定）を返すか、スロット内の値にアクセス/設定します。スロット内の値にアクセスするときは、希望する型を指定します。必要に応じてスロットは要求された型に変換され、値が返されます。

**`xsSlot xsTrue`**

`true`を含むブールスロットを返します

***

**`xsSlot xsFalse`**

`false`を含むブールスロットを返します

***

**`xsSlot xsBoolean(xsBooleanValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theValue` | スロットに含める値

ブールスロットを返します

***

**`xsBooleanValue xsToBoolean(xsSlot theSlot)`**<BR>
**`xsBooleanValue xsmcToBoolean(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | ブール値に変換するスロット

スロットに含まれる値を返します

***

**`void xsmcSetFalse(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | falseに設定するスロット

スロットの値を`false`に設定します

***

**`void xsmcSetTrue(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | trueに設定するスロット

スロットの値を`true`に設定します

***

**`void xsmcSetBoolean(xsSlot theSlot, xsBooleanValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 設定するスロット
| `theValue` | 設定するブール値`true`または`false`

スロットの値を`true`または`false`に設定します

***

**`xsSlot xsInteger(xsIntegerValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theValue` | スロットに含める値

整数スロットを返します

***

**`xsIntegerValue xsToInteger(xsSlot theSlot)`**<BR>
**`xsIntegerValue xsmcToInteger(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 整数に変換するスロット

スロットに含まれる値を返します

***

**`void xsmcSetInteger(xsSlot theSlot, xsIntegerValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 設定するスロット
| `theValue` | 設定する整数値

スロットの値を整数に設定します

***

**`xsSlot xsNumber(xsNumberValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theValue` | スロットに含める値

数値スロットを返します

***

**`xsNumberValue xsToNumber(xsSlot theSlot)`**<BR>
**`xsNumberValue xsmcToNumber(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 数値に変換するスロット

スロットに含まれる値を返します

***

**`void xsmcSetNumber(xsSlot theSlot, xsNumberValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 設定するスロット
| `theValue` | 設定する数値

スロットの値を数値に設定します

***

#### 文字列

これらのスロットは対応する型の値を含みます。

```c
typedef char* xsStringValue;
```

文字列値はUTF-8 C文字列へのポインタです。XSランタイム仮想マシンとガベージコレクタは、スクリプトが使用するUTF-8 C文字列を管理します。

**`xsSlot xsString(xsStringValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theValue` | スロットに含める値

文字列スロットを返します

`xsString`マクロはパラメータを複製するため、C定数、Cグローバル、またはCローカルを安全に渡すことができます。

***

**`xsStringValue xsToString(xsSlot theSlot)`**<BR>
**`xsStringValue xsmcToString(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 文字列に変換するスロット

スロットに含まれる文字列を返します

速度のため、`xsToString`マクロはスロット自体に含まれる値、つまりXSが管理するメモリ内の文字列へのポインタを返します。XSランタイムは文字列値を含むメモリを圧縮できるため、`xsToString`マクロの結果は、XS in Cの他のマクロをまたいで使用することはできません。ECMAScript言語仕様は、文字列のインプレース修正を禁止しています。

***

**`void xsmcSetString(xsSlot theSlot, xsStringValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 設定するスロット
| `theValue` | 設定する文字列値

スロットの値を文字列に設定します

***

**`xsStringValue xsToStringBuffer(xsSlot theSlot, xsStringValue theBuffer, xsIntegerValue theSize)`**<BR>
**`xsStringValue xsmcToStringBuffer(xsSlot theSlot, xsStringValue theBuffer, xsIntegerValue theSize)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 文字列に変換するスロット
| `theBuffer` | 文字列をコピーするバッファ
| `theSize` | バッファのサイズ

文字列値をコピーし、文字列のコピーを含むバッファを返します。提供されるバッファは、文字列値のコピーを保持するのに十分な大きさでなければなりません。

***

**`xsSlot xsStringBuffer(void *theData, xsIntegerValue theSize)`**

| 引数 | 説明 |
| --- | :-- |
| `theData` | 文字列バッファにコピーするデータへのポインタ、または文字列バッファデータを未初期化のままにする場合は`NULL`
| `theSize` | コピーするデータサイズ（バイト単位）

文字列を割り当てられたバッファにコピーし、スロット値を文字列バッファに設定し、新しい文字列バッファインスタンスへの参照を返します。

***

**`void xsmcSetStringBuffer(xsSlot theSlot, xsStringValue theValue, xsIntegerValue theSize)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 設定するスロット
| `theValue` | 設定する文字列値
| `theSize` | 文字列のサイズ（バイト単位）

文字列を割り当てられたバッファにコピーし、スロット値を文字列バッファに設定します。

***

<a id="arraybuffer"></a>
### ArrayBuffer

ECMAScriptでは、`ArrayBuffer`は固定長のバイナリデータを格納するために一般的に使用されます。

#### マクロ

**`xsSlot xsArrayBuffer(void *theData, xsIntegerValue theSize)`**
**`void xsmcSetArrayBuffer(xsSlot theSlot, void *theData, xsIntegerValue theSize)`**


| 引数 | 説明 |
| --- | :-- |
| `theData` | `ArrayBuffer`にコピーするデータへのポインタ、または`ArrayBuffer`データを未初期化のままにする場合は`NULL`
| `theSize` | データのサイズ（バイト単位）

`ArrayBuffer`インスタンスを作成し、新しい`ArrayBuffer`インスタンスへの参照を返します

***

**`void xsGetArrayBufferData(xsSlot theSlot, xsIntegerValue theOffset, void *theData, xsIntegerValue theSize)`**<BR>
**`void xsmcGetArrayBufferData(xsSlot theSlot, xsIntegerValue theOffset, void *theData, xsIntegerValue theSize)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | `ArrayBuffer`スロット
| `theOffset` | データを取得する開始バイトオフセット
| `theData` | `ArrayBuffer`データを取得するデータポインタ
| `theSize` | コピーするデータサイズ（バイト単位）

`ArrayBuffer`からバイトをコピーします

***

**`void xsSetArrayBufferData(xsSlot theSlot, xsIntegerValue theOffset, void *theData, xsIntegerValue theSize)`**<BR>
**`void xsmcSetArrayBufferData(xsSlot theSlot, xsIntegerValue theOffset, void *theData, xsIntegerValue theSize)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | `ArrayBuffer`スロット
| `theOffset` | データを設定する開始バイトオフセット
| `theData` | `ArrayBuffer`データを設定するデータポインタ
| `theSize` | コピーするデータサイズ（バイト単位）

`ArrayBuffer`にバイトをコピーします

***

**`void xsmcSetArrayBuffer(xsSlot theSlot, void *theData, xsIntegerValue theSize)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | `ArrayBuffer`スロット
| `theData` | `ArrayBuffer`データを設定するデータポインタ
| `theSize` | コピーするデータサイズ（バイト単位）

提供されたデータから初期化された`ArrayBuffer`インスタンスを作成します

***

**`xsIntegerValue xsGetArrayBufferLength(xsSlot theSlot)`**<BR>
**`xsIntegerValue xsmcGetArrayBufferLength(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | `ArrayBuffer`スロット

`ArrayBuffer`のサイズをバイト単位で返します

***

**`void xsSetArrayBufferLength(xsSlot theSlot, xsIntegerValue theSize)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | `ArrayBuffer`スロット
| `theSize` | `ArrayBuffer`データのサイズ（バイト単位）。バッファのサイズが増加される場合、新しいデータは0に初期化されます。

`ArrayBuffer`の長さを設定します

***

**`void *xsToArrayBuffer(xsSlot theSlot)`**<BR>
**`void *xsmcToArrayBuffer(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | `ArrayBuffer`スロット

`ArrayBuffer`データへのポインタを返します

速度のため、`xsToArrayBuffer`マクロはスロット自体に含まれる値、つまりXSが管理するメモリ内のバッファへのポインタを返します。XSランタイムは文字列値を含むメモリを圧縮できるため、`xsToArrayBuffer`マクロの結果は、XS in Cの他のマクロをまたいで使用することはできません。

***

<a id="instances-and-prototypes"></a>
### インスタンスとプロトタイプ

XS in Cでは、ECMAScriptと同様に、オブジェクトは他のオブジェクトからプロパティを継承でき、そのオブジェクトはさらに他のオブジェクトから継承できます。継承するオブジェクトは*インスタンス*であり、継承元のオブジェクトは*プロトタイプ*です。

参照スロット（型`xsReferenceType`）は*間接*スロットであり、オブジェクト、関数、配列などのインスタンスへの参照を含みます。インスタンス自体は、インスタンスのプロパティ（または配列の場合はインスタンスのアイテム）であるスロットで構成されます。

#### マクロ

**`xsSlot xsObjectPrototype`**<BR>
**`xsSlot xsFunctionPrototype`**<BR>
**`xsSlot xsArrayPrototype`**<BR>
**`xsSlot xsStringPrototype`**<BR>
**`xsSlot xsBooleanPrototype`**<BR>
**`xsSlot xsNumberPrototype`**<BR>
**`xsSlot xsDatePrototype`**<BR>
**`xsSlot xsRegExpPrototype`**<BR>
**`xsSlot xsHostPrototype`**<BR>
**`xsSlot xsErrorPrototype`**<BR>
**`xsSlot xsEvalErrorPrototype`**<BR>
**`xsSlot xsRangeErrorPrototype`**<BR>
**`xsSlot xsReferenceErrorPrototype`**<BR>
**`xsSlot xsSyntaxErrorPrototype`**<BR>
**`xsSlot xsTypeErrorPrototype`**<BR>
**`xsSlot xsURIErrorPrototype`**<BR>
**`xsSlot xsSymbolPrototype`**<BR>
**`xsSlot xsArrayBufferPrototype`**<BR>
**`xsSlot xsDataViewPrototype`**<BR>
**`xsSlot xsTypedArrayPrototype`**<BR>
**`xsSlot xsMapPrototype`**<BR>
**`xsSlot xsSetPrototype`**<BR>
**`xsSlot xsWeakMapPrototype`**<BR>
**`xsSlot xsWeakSetPrototype`**<BR>
**`xsSlot xsPromisePrototype`**<BR>
**`xsSlot xsProxyPrototype`**<BR>

XSランタイムによって作成されたプロトタイプインスタンスへの参照を返します。

***

**`xsSlot xsNewArray(xsIntegerValue theLength)`**<BR>
**`xsSlot xsmcNewArray(xsIntegerValue theLength)`**

| 引数 | 説明 |
| --- | :-- |
| `theLength` | 設定する配列のlengthプロパティ

配列インスタンスを作成し、新しい配列インスタンスへの参照を返します

##### ECMAScriptで:

```javascript
new Array(5);
```

##### Cで:

```c
xsNewArray(5);
```

***

**`xsSlot xsNewObject()`**<BR>
**`xsSlot xsmcNewObject()`**

オブジェクトインスタンスを作成し、新しいオブジェクトインスタンスへの参照を返します

##### ECMAScriptでは：

```javascript
new Object();
```

##### Cでは：

```c
xsNewObject();
```

***

**`void xsmcSetNewObject(theSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 結果スロット

`xsmcSetNewObject`マクロは`xsNewObject`マクロと機能的に同等です。プロパティは提供されたスロットに返されます。

##### ECMAScriptでは：

```javascript
new Object();
```

##### Cでは：

```c
xsmcVars(1);
xsmcSetNewObject(xsVar(0));
```

***

**`xsBooleanValue xsIsInstanceOf(xsSlot theInstance, xsSlot thePrototype)`**<BR>
**`xsBooleanValue xsmcIsInstanceOf(xsSlot theInstance, xsSlot thePrototype)`**

| 引数 | 説明 |
| --- | :-- |
| `theInstance` | テストするインスタンスへの参照
| `thePrototype` | テストするプロトタイプへの参照

インスタンスが特定のプロトタイプを持っているかどうかを、直接的または間接的に（つまり、プロトタイプ階層の1つまたは複数のレベル上で）テストします。インスタンスがプロトタイプを持っている場合は`true`、そうでなければ`false`を返します。

`xsIsInstanceOf`マクロにはECMAScriptでの同等のものがありません。スクリプトは、プロトタイプを直接使用するのではなく、*コンストラクタ*を通じてインスタンスをテストします。コンストラクタは、`isPrototypeOf`でインスタンスをテストするために使用される`prototype`プロパティを持つ関数です。

##### ECMAScriptでは：

```javascript
if (Object.prototype.isPrototypeOf(this))
	return new Object();
```

##### Cでは：

```c
if (xsIsInstanceOf(xsThis, xsObjectPrototype))
	xsResult = xsNewObject();
```

***

<a id="identifiers"></a>
### キー、識別子、インデックス

ECMAScriptでは、オブジェクトのプロパティは数値、文字列、またはシンボル値（プロパティ**キー**）によって識別されます。XS in Cでは、以下で説明する`xsGetAt`、`xsSetAt`などのマクロを通じて、プロパティキーでプロパティにアクセスできます。

プロパティキーの数値または文字列値が32ビット符号なし整数に変換できる場合、XSは変換の結果（プロパティ**インデックス**）を使用してプロパティを識別します。XS in Cでは、以下で説明する`xsGetIndex`、`xsSetIndex`などのマクロを通じて、プロパティインデックスで直接プロパティにアクセスできます。プロパティインデックスはすべてのインスタンスで使用できますが、通常は`Array`インスタンスの項目にアクセスするために使用されます。

```c
typedef uint32_t xsIndex;
```

それ以外の場合、プロパティキーの文字列またはシンボル値がテーブルに格納され、XSは結果のテーブルインデックス（プロパティ**識別子**）を使用してプロパティを識別します。XS in Cでは、以下で説明する`xsGet`、`xsSet`などのマクロを通じて、プロパティ識別子で直接プロパティにアクセスできます。

```c
typedef uint16_t xsIdentifier;
```

64ビットプラットフォームでは、利用可能な識別子の数を32ビットまで拡張でき、`xsIdentifier`は`uint32_t`になります。

#### マクロ

**`xsIdentifier xsID(xsStringValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theValue` | 変換する文字列

文字列値を識別子に変換し、識別子を返します。特定の仮想マシンについて、同じ文字列値は常に同じ識別子に変換されるため、頻繁に使用される識別子を仮想マシンごとにキャッシュできます。

パフォーマンスのため、XS in CはXSコンパイラによって生成された識別子（例：`xsID_property`）によるプロパティアクセスもサポートしています。`xsID_*`識別子の型は`xsIdentifier`です。これらの識別子は、以下で説明する`xsGet`、`xsSet`などのマクロで使用できる最適化を提供します。`xsID_*`プロパティは`xsID()`マクロの使用と機能的に同等であり、すべての例で使用されています。

以下のCの例では、`xsGet`マクロ（次のセクションで説明）は、取得するプロパティまたは項目の識別子を2番目の引数として受け取ります。

##### ECMAScriptでは：

```javascript
this.foo
```

##### Cでは：

```c
xsGet(xsThis, xsID("foo"));
xsGet(xsThis, xsID_foo);
```

***

**`xsBooleanValue xsIsID(xsStringValue theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theValue` | テストする文字列

指定された文字列が既存のプロパティ識別子に対応するかどうかをテストします。文字列がプロパティ識別子である場合は`true`、そうでなければ`false`を返します。

***

<a id="properties"></a>
### プロパティ

このセクションでは、表1にまとめられているように、オブジェクトのプロパティ（または配列の項目）にアクセスするためのマクロについて説明します。

**表1.** プロパティ関連マクロ

<table class="normalTable">
  <tbody>
    <tr>
      <th scope="col">マクロ</th>
      <th scope="col">説明</th>
    </tr>
    <tr>
      <td><code>xsGlobal</code></td>
      <td>スクリプトで利用可能なグローバルプロパティで構成される特別なインスタンスを返します</td>
    </tr>
    <tr>
      <td><code>xsDefine, xsmcDefine</code></td>
      <td>識別子と属性を使用してインスタンスの新しいプロパティを定義します</td>
    </tr>
      <td><code>xsDefineAt</code></td>
      <td>キーと属性を使用してインスタンスの新しいプロパティまたは項目を定義します</td>
    </tr>
    <tr>
      <td><code>xsHas, xsmcHas</code></td>
      <td>インスタンスが特定の識別子に対応するプロパティを持っているかどうかをテストします</td>
    </tr>
    <tr>
      <td><code>xsHasAt</code></td>
      <td>インスタンスが特定のキーに対応するプロパティを持っているかどうかをテストします</td>
    </tr>
     <tr>
      <td><code>xsHasIndex, xsmcHasIndex</code></td>
      <td>インスタンスが特定のインデックスに対応するプロパティを持っているかどうかをテストします</td>
    </tr>
   <tr>
      <td><code>xsGet, xsmcGet</code></td>
      <td>識別子によってインスタンスのプロパティを取得します</td>
    </tr>
    <tr>
      <td><code>xsGetAt, xsmcGetAt</code></td>
      <td>キーによってプロパティまたはインスタンスを取得します</td>
    </tr>
    <tr>
      <td><code>xsGetIndex, xsmcGetIndex</code></td>
      <td>インデックスによってプロパティまたはインスタンスを取得します</td>
    </tr>
	<tr>
      <td><code>xsSet, xsmcSet</code></td>
      <td>識別子を使用してインスタンスのプロパティを設定します</td>
    </tr>
	<tr>
      <td><code>xsSetAt, xsmcSetAt</code></td>
      <td>キーを使用してインスタンスのプロパティを設定します</td>
	</tr>
 	<tr>
      <td><code>xsSetIndex, xsmcSetIndex</code></td>
      <td>インデックスを使用してインスタンスのプロパティを設定します</td>
    </tr>
   <tr>
      <td><code>xsDelete, xsmcDelete</code></td>
      <td>特定の識別子に対応するプロパティを削除します</td>
    </tr>
    <tr>
      <td><code>xsDeleteAt, xsmcDeleteAt</code></td>
      <td>特定のキーに対応するプロパティを削除します</td>
    </tr>
    <tr>
      <td><code>xsCall0</code> ... <code>xsCall7, xsmcCall</code></td>
      <td>インスタンスのプロパティによって参照される関数を呼び出します</td>
    </tr>
    <tr>
      <td><code>xsNew0</code> ... <code>xsNew7, xsmcNew</code></td>
      <td>インスタンスのプロパティによって参照されるコンストラクタを呼び出します</td>
    </tr>
    <tr>
      <td><code>xsTest, xsmcTest</code></td>
      <td>任意の型の値を受け取り、それがtrueかfalseかを判定します</td>
    </tr>
    <tr>
      <td><code>xsEnumerate</code></td>
      <td>インスタンスのプロパティを列挙します</td>
    </tr>
  </tbody>
</table>


> 以下の例の一部では、`xsVars`マクロでスタックに確保された変数スロットを使用します。[`引数と変数`](#xsvars)を参照してください。

#### xsGlobal

スクリプトで利用可能なグローバルは、XS in Cで`xsGlobal`マクロを使用して参照される特別なインスタンスのプロパティです。

**`xsSlot xsGlobal`**

グローバルで構成される特別なインスタンスへの参照を返します

`xsGlobal`マクロを最初のパラメータとして、`xsGet`、`xsSet`、`xsDelete`、`xsCall*`、`xsNew*`マクロを使用できます。これらのマクロを説明するセクションで例を示します。

***

#### xsDefine

識別子と属性を使用してインスタンスの新しいプロパティを定義するには、`xsDefine`マクロを使用します。プロパティの属性は、以下の定数の1つまたは複数を使用して設定されます。

```c
enum {
	xsDefault = 0,
	xsDontDelete = 2,
	xsDontEnum = 4,
	xsDontSet = 8,
	xsStatic = 16,
	xsIsGetter = 32,
	xsIsSetter = 64,
	xsChangeAll = 30
}
typedef unsigned char xsAttribute;
```

**`void xsDefine(xsSlot theThis, xsIdentifier theID, xsSlot theParam, xsAttribute theAttributes)`**
**`void xsmcDefine(xsSlot theThis, xsIdentifier theID, xsSlot theParam, xsAttribute theAttributes)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theID` | 定義するプロパティの識別子
| `theParam` | 定義するプロパティの値
| `theAttributes` | 設定する属性の組み合わせ

`theAttributes`については、設定したい属性に対応する定数を指定します（その他はクリアされます）。

`xsDontDelete`、`xsDontEnum`、`xsDontSet`属性は、ECMAScriptの`configurable`、`enumerable`、`writable`属性に対応します。デフォルトでは、プロパティは削除、列挙、設定が可能です。

プロパティが作成されるとき、インスタンスのプロトタイプに同じ名前のプロパティがある場合、その属性が継承されます。そうでなければ、デフォルトで、プロパティは削除、列挙、設定が可能で、スクリプトで使用できます。

##### ECMAScriptでは：

```javascript
Object.defineProperty(this, "foo", 7, { writable: true, enumerable: true, configurable: true });
```

##### Cでは：

```c
xsDefine(xsThis, xsID{"foo"), xsInteger(7), xsDefault);
```

***

#### xsDefineAt

キーと属性を使用してインスタンスの新しいプロパティまたは項目を定義するには、`xsDefineAt`マクロを使用します。`xsDefineAt`マクロは`xsDefine`マクロと機能的に同等ですが、定義するプロパティまたは項目を識別するためにスロットが使用される点が異なります。

**`void xsDefineAt(xsSlot theThis, xsSlot theSlot, xsSlot theParam, xsAttribute theAttributes)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theKey` | 定義するプロパティのキー
| `theParam` | 定義するプロパティの値
| `theAttributes` | 設定する属性の組み合わせ

##### ECMAScriptでは：

```javascript
Object.defineProperty(this, "foo", 7, { writable: true, enumerable: true, configurable: true });
Object.defineProperty(this, 5, 7, { writable: true, enumerable: true, configurable: true });
```

##### Cでは：

```c
xsDefineAt(xsThis, xsString("foo"), xsInteger(7), xsDefault);
xsDefineAt(xsThis, xsInteger(5), xsInteger(7), xsDefault);
```
***

#### xsHas

インスタンスが特定の識別子に対応するプロパティを持っているかどうかをテストするには、`xsHas`マクロを使用します。このマクロはECMAScriptの`in`キーワードに似ています。

**`xsBooleanValue xsHas(xsSlot theThis, xsIdentifier theID)`**<BR>
**`xsBooleanValue xsmcHas(xsSlot theThis, xsIdentifier theID)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | テストするインスタンスへの参照
| `theID` | テストするプロパティの識別子

インスタンスがプロパティを持っている場合は`true`、そうでなければ`false`を返します

##### ECMAScriptでは：

```javascript
if ("foo" in this)
	;
```

##### Cでは：

```c
if (xsHas(xsThis, xsID("foo"))
	;
```

***

#### xsHasAt

インスタンスが特定のキーに対応するプロパティを持っているかどうかをテストするには、`xsHasAt`マクロを使用します。

**`xsBooleanValue xsmcHasAt(xsSlot theThis, xsSlot theKey)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | テストするインスタンスへの参照
| `theKey` | テストするプロパティのキー

インスタンスがプロパティを持っている場合は`true`、そうでなければ`false`を返します


##### ECMAScriptでは：

```javascript
if ("foo" in this)
	;
if (5 in this)
	;
```

##### Cでは：

```c
if (xsHasAt(xsThis, xsString("foo"))
	;
if (xsHasAt(xsThis, xsInteger(5))
	;
```

***

#### xsHasIndex

インスタンスが特定のインデックスに対応するプロパティを持っているかどうかをテストするには、`xsHasIndex`マクロを使用します。このマクロはECMAScriptの`in`キーワードに似ています。

**`xsBooleanValue xsHasIndex(xsSlot theThis, xsIndex theIndex)`**<BR>
**`xsBooleanValue xsmcHasIndex(xsSlot theThis, xsIndex theIndex)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | テストするインスタンスへの参照
| `theIndex` | テストするプロパティのインデックス

インスタンスがプロパティを持っている場合は`true`、そうでなければ`false`を返します

##### ECMAScriptでは：

```javascript
if (7 in this)
	;
```

##### Cでは：

```c
if (xsHasIndex(xsThis, 7));
	;
```

***

#### xsGet

識別子によってインスタンスのプロパティを取得するには、`xsGet`マクロを使用します。

**`xsSlot xsGet(xsSlot theThis, xsIdentifier theID)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theID` | 取得するプロパティの識別子

プロパティに含まれる内容を含むスロットを返します。インスタンスまたはそのプロトタイプによってプロパティが定義されていない場合は`xsUndefined`を返します

##### ECMAScriptでは：

```javascript
foo
this.foo
```

##### Cでは：

```c
xsVars(1);)
xsVar(0) = xsGet(xsGlobal, xsID_foo);
xsVar(0) = xsGet(xsThis, xsID("foo"));
```

***


#### xsmcGet

`xsmcGet`マクロは`xsGet`マクロと機能的に同等です。プロパティは提供されたスロットに返されます。

**`void xsmcGet(xsSlot theSlot, xsSlot theThis, xsIdentifier theID)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | プロパティまたは項目を含むスロット
| `theThis` | プロパティを持つインスタンスへの参照
| `theID` | 取得するプロパティの識別子

##### ECMAScriptでは：

```javascript
foo
this.foo
```

##### Cでは：

```c
xsmcVars(1);
xsmcGet(xsVar(0), xsGlobal, xsID_foo);
xsmcGet(xsVar(0), xsThis, xsID("foo"));
```

***

#### xsGetAt

キーによってインスタンスのプロパティを取得するには、`xsGetAt`マクロを使用します。

**`xsSlot xsGetAt(xsSlot theThis, xsSlot theKey)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theKey` | 取得するプロパティのキー

プロパティに含まれる内容を含むスロットを返します。インスタンスまたはそのプロトタイプによってプロパティが定義されていない場合は`xsUndefined`を返します

##### ECMAScriptでは：

```javascript
this["foo"]
this[5]
```

##### Cでは：

```c
xsGetAt(xsThis, xsString("foo"));
xsGetAt(xsVar(0), xsInteger(5));
```

***

#### xsmcGetAt

`xsmcGetAt`マクロは`xsGetAt`マクロと機能的に同等です。プロパティは提供されたスロットに返されます。

**`void xsmcGetAt(xsSlot theSlot, xsSlot theThis, xsSlot theKey)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | プロパティを含むスロット
| `theThis` | プロパティを持つインスタンスへの参照
| `theKey` | 取得するプロパティのキー

##### ECMAScriptでは：

```javascript
this["foo"]
this[5]
```

##### Cでは：

```c
xsmcVars(1);
xsmcGetAt(xsVar(0), xsThis, xsString("foo"));
xsmcGetAt(xsVar(0), xsThis, xsInteger(5));
```
***

#### xsGetIndex

インデックスによってインスタンスのプロパティを取得するには、`xsGetIndex`マクロを使用します。

**`xsSlot xsGetIndex(xsSlot theThis, xsIndex theIndex)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theIndex` | 取得するプロパティのインデックス

プロパティに含まれる内容を含むスロットを返します。インスタンスまたはそのプロトタイプによって項目が定義されていない場合は`xsUndefined`を返します

##### ECMAScriptでは：

```javascript
this[0]
```

##### Cでは：

```c
xsGetIndex(xsThis, 0);
```

***

#### xsmcGetIndex

`xsmcGetIndex`マクロは`xsGetIndex`マクロと機能的に同等です。プロパティは提供されたスロットに返されます。

**`void xsmcGetIndex(xsSlot theSlot, xsSlot theThis, xsIndex theIndex)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | プロパティを含むスロット
| `theThis` | 項目を持つインスタンスへの参照
| `theIndex` | 取得する項目のインデックス

##### ECMAScriptでは：

```javascript
this[0]
```

##### Cでは：

```c
xsmcVars(1);
xsmcGetIndex(xsVar(0), xsThis, 0);
```

***

#### xsSet

識別子を使用してインスタンスのプロパティを設定するには、`xsSet`マクロを使用します。インスタンスによってプロパティが定義されていない場合、このマクロはプロパティをインスタンスに挿入します。

**`void xsSet(xsSlot theThis, xsIdentifier theID, xsSlot theParam)`**<BR>
**`void xsmcSet(xsSlot theThis, xsIdentifier theID, xsSlot theParam)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theID` | 設定するプロパティの識別子
| `theParam` | 設定するプロパティの値

##### ECMAScriptでは：

```javascript
foo = 0
this.foo = 1
```

##### Cでは：

```c
xsSet(xsGlobal, xsID("foo"), xsInteger(0));
xsSet(xsThis, xsID_foo, xsInteger(1));
```

***


#### xsSetAt

キーを使用してインスタンスのプロパティを設定するには、`xsSetAt`マクロを使用します。インスタンスによってプロパティまたは項目が定義されていない場合、このマクロはそれをインスタンスに挿入します。

**`void xsSetAt(xsSlot theThis, xsSlot theKey, xsSlot theValue)`**<BR>
**`void xsmcSetAt(xsSlot theThis, xsSlot theKey, xsSlot theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theKey` | 設定するプロパティのキー
| `theValue` | 設定するプロパティの値

##### ECMAScriptでは：

```javascript
this["foo"] = 0
this[3] = 1
```

##### Cでは：

```c
xsSetAt(xsThis, xsString("foo"), xsInteger(0));
xsSetAt(xsThis, xsInteger(3), xsInteger(1));
```

***

#### xsSetIndex

インデックスを使用してインスタンスのプロパティを設定するには、`xsSetIndex`マクロを使用します。インスタンスによってプロパティが定義されていない場合、このマクロはプロパティをインスタンスに挿入します。

**`void xsSetIndex(xsSlot theThis, xsIndex theIndex, xsSlot theParam)`**<BR>
**`void xsmcSetIndex(xsSlot theThis, xsIndex theIndex, xsSlot theParam)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theIndex` | 設定するプロパティのインデックス
| `theParam` | 設定するプロパティの値

##### ECMAScriptでは：

```javascript
this[3] = 1
```

##### Cでは：

```c
xsSetIndex(xsThis, 3, xsInteger(1));
```

***

#### xsDelete

特定の識別子に対応するインスタンスのプロパティを削除するには、`xsDelete`マクロを使用します。インスタンスによってプロパティが定義されていない場合、このマクロは何も効果がありません。

**`void xsDelete(xsSlot theThis, xsIdentifier theID)`**<BR>
**`void xsmcDelete(xsSlot theThis, xsIdentifier theID)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティを持つインスタンスへの参照
| `theID` | 削除するプロパティの識別子

##### ECMAScriptでは：

```javascript
delete globalThis.foo
delete this.foo
```

##### Cでは：

```c
xsDelete(xsGlobal, xsID("foo"));
xsDelete(xsThis, xsID_foo);
```

***

#### xsDeleteAt, xsmcDeleteAt

特定のキーに対応するインスタンスのプロパティを削除するには、`xsDeleteAt`マクロを使用します。インスタンスによってプロパティが定義されていない場合、このマクロは何も効果がありません。

**`void xsDeleteAt(xsSlot theThis, xsSlot theKey)`**<BR>
**`void xsmcDeleteAt(xsSlot theThis, xsSlot theKey)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis ` | プロパティを持つインスタンスへの参照
| `theKey` | 削除するプロパティのキー

##### ECMAScriptでは：

```javascript
delete this["foo"]
delete this[3]
```

##### Cでは：

```c
xsDeleteAt(xsThis, xsString("foo"));
xsDeleteAt(xsThis, xsInteger(3));
```

***

#### xsCall*

インスタンスのプロパティまたは項目が関数への参照である場合、`xsCall*`マクロの1つで関数を呼び出すことができます（`*`は`0`から`7`で、渡されるパラメータスロットの数を表します）。プロパティまたは項目がインスタンスまたはそのプロトタイプによって定義されていない、または関数への参照でない場合、`xsCall*`マクロは例外をスローします。

**`xsSlot xsCall0(xsSlot theThis, xsIdentifier theID)`**<BR>
**`xsSlot xsCall1(xsSlot theThis, xsIdentifier theID, xsSlot theParam0)`**<BR>
**`xsSlot xsCall2(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1)`**<BR>
**`xsSlot xsCall3(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2)`**<BR>
**`xsSlot xsCall4(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2, xsSlot theParam3)`**<BR>
**`xsSlot xsCall5(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2, xsSlot theParam3, xsSlot theParam4)`**<BR>
**`xsSlot xsCall6(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2, xsSlot theParam3, xsSlot theParam4, xsSlot theParam5)`**<BR>
**`xsSlot xsCall7(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2, xsSlot theParam3, xsSlot theParam4, xsSlot theParam5, xsSlot theParam6)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティまたは項目を持つインスタンスへの参照
| `theID` | 呼び出すプロパティまたは項目の識別子
| `theParam0` ... `theParam6` | 関数に渡すパラメータスロット

関数の結果スロットを返します

##### ECMAScriptでは：

```javascript
foo()
this.foo(1)
this[0](2, 3)
```

##### Cでは：

```
xsCall0(xsGlobal, xsID_foo);
xsCall1(xsThis, xsID("foo"), xsInteger(1));
xsCall2(xsThis, 0, xsInteger(2), xsInteger(3));
```

#### xsmcCall

`xsmcCall`マクロは`xsCall*`マクロと機能的に同等です。結果とパラメータスロットは関数パラメータとして提供されます。

**`void xsmcCall(xsSlot xsSlot, xsSlot theThis, xsIdentifier theID, ...)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 結果スロット
| `theThis` | プロパティまたは項目を持つインスタンスへの参照
| `theID` | 呼び出すプロパティまたは項目の識別子
| ... | コンストラクタに引数として渡されるスロットへのポインタの後にnullポインタ

##### ECMAScriptでは：

```javascript
foo(1)
this.foo(1)
this[0](2, 3)
```

##### Cでは：

```c
xsmcVars(3);
xsmcSetInteger(xsVar(0), 1);
xsmcSetInteger(xsVar(1), 2);
xsmcSetInteger(xsVar(2), 3);
xsmcCall(xsResult, xsGlobal, xsID("foo"), &xsVar(0), NULL);
xsmcCall(xsResult, xsThis, xsID_foo, &xsVar(0), NULL);
xsmcCall(xsResult, xsThis, 0, &xsVar(1), &xsVar(2), NULL);
```

***

<a id="xsnew"></a>
#### xsNew*

インスタンスのプロパティまたは項目がコンストラクタへの参照である場合、`xsNew*`マクロの1つでコンストラクタを呼び出すことができます（`*`は`0`から`7`で、渡されるパラメータスロットの数を表します）。プロパティまたは項目がインスタンスまたはそのプロトタイプによって定義されていない、またはコンストラクタへの参照でない場合、`xsNew*`マクロは例外をスローします。

**`xsSlot xsNew0(xsSlot theThis, xsIdentifier theID)`**<BR>
**`xsSlot xsNew1(xsSlot theThis, xsIdentifier theID, xsSlot theParam0)`**<BR>
**`xsSlot xsNew2(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1)`**<BR>
**`xsSlot xsNew3(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2)`**<BR>
**`xsSlot xsNew4(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2, xsSlot theParam3)`**<BR>
**`xsSlot xsNew5(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2, xsSlot theParam3, xsSlot theParam4)`**<BR>
**`xsSlot xsNew6(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2, xsSlot theParam3, xsSlot theParam4, xsSlot theParam5)`**<BR>
**`xsSlot xsNew7(xsSlot theThis, xsIdentifier theID, xsSlot theParam0, xsSlot theParam1, xsSlot theParam2, xsSlot theParam3, xsSlot theParam4, xsSlot theParam5, xsSlot theParam6)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | プロパティまたは項目を持つインスタンスへの参照
| `theID` | 呼び出すプロパティまたは項目の識別子
| `theParam0` ... `theParam6` | コンストラクタに渡すパラメータスロット

コンストラクタの結果スロットを返します


##### ECMAScriptでは：

```javascript
new foo()
new this.foo(1)
new this[0](2, 3)
```

##### Cでは：

```c
xsNew0(xsGlobal, xsID("foo"));
xsNew1(xsThis, xsID_foo, xsInteger(1));
xsNew2(xsThis, 0, xsInteger(2), xsInteger(3));
```

***

#### xsmcNew

`xsmcNew`マクロは`xsNew*`マクロと機能的に同等です。結果とパラメータスロットは関数パラメータとして提供されます。

**`void xsmcNew(xsSlot theSlot, xsSlot theThis, xsIdentifier theID, ...)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | コンストラクタの結果スロット
| `theThis` | プロパティまたは項目を持つインスタンスへの参照
| `theID` | 呼び出すプロパティまたは項目の識別子
| ... | コンストラクタに引数として渡されるスロットへのポインタの後にnullポインタ


##### ECMAScriptでは：

```javascript
new foo(1)
new this.foo(1)
new this[0](2, 3)
```

##### Cでは：

```c
xsmcVars(3);
xsmcSetInteger(xsVar(0), 1);
xsmcSetInteger(xsVar(1), 2);
xsmcSetInteger(xsVar(2), 3);
xsmcNew(xsResult, xsGlobal, xsID_foo, &xsVar(0), NULL);
xsmcNew(xsResult, xsThis, xsID("foo"), &xsVar(0), NULL);
xsmcNew(xsResult, xsThis, 0, &xsVar(1), &xsVar(2), NULL);
```

***

#### xsTest

ECMAScriptの`if`節のように、`xsTest`マクロは任意の型の値を受け取り、それがtrueかfalseかを判定します。このマクロはECMAScriptと同じルール（ECMA-262仕様のセクション12.5）を適用します。

**`xsBooleanValue xsTest(xsSlot theValue)`**<BR>
**`xsBooleanValue xsmcTest(xsSlot theValue)`**

| 引数 | 説明 |
| --- | :-- |
| `theValue` | テストする値

値がtrueの場合は`true`、そうでなければ`false`を返します


##### ECMAScriptでは：

```javascript
if (foo) {}
```

##### Cでは：

```c
if (xsTest(xsGet(xsGlobal, xsID_foo)) {}
```

***

#### xsEnumerate

`xsEnumerate`マクロを使用して、列挙可能なインスタンスプロパティのイテレータを取得します。イテレータは、プロパティを反復処理するための`next`、`value`、`done`関数を提供します。

**`xsSlot xsEnumerate(xsSlot theObject)`**

| 引数 | 説明 |
| --- | :-- |
| `theObject` | 列挙可能なプロパティを持つオブジェクトへの参照

イテレータを含むスロットを返します

##### ECMAScriptでは：

```javascript
rectangle = { x:0, y:0, width:200, height:100 };
for (let prop in rectangle)
	trace(`${prop}: ${rectangle[prop]}\n`);
```

##### Cでは：

```c
xsVars(5);
xsVar(0) = xsGet(xsGlobal, xsID_rectangle);
xsVar(1) = xsEnumerate(xsVar(0));
for (;;) {
	xsVar(2) = xsCall0(xsVar(1), xsID("next"));
	if (xsTest(xsGet(xsVar(2), xsID("done"))))
		break;
	xsVar(3) = xsGet(xsVar(2), xsID("value"));
	xsVar(4) = xsGetAt(xsVar(0), xsVar(3));
	xsTrace(xsToString(xsVar(3)));xsTrace(": ");
	xsTrace(xsToString(xsVar(4)));xsTrace("\n");
}
```

***

<a id="xsvars"></a>
### 引数と変数

XSランタイム仮想マシンは、ヒープとスロットのスタックを使用します。XS in Cでは、スタックスロットに直接アクセスし、ヒープスロットには参照を通して間接的にアクセスできます。

Cコールバックが実行されるとき、スタックには引数スロット、`this`スロット、結果スロットが含まれますが、変数スロットは含まれません。変数スロットを使用するには、`xsVars`または`xsmcVars`マクロでスタックに予約する必要があります。`xsVars`マクロはコールバック実行の開始時に一度だけ使用できます。`xsmcVars`マクロはコールバック内で複数回使用できます。`xsmcVars`を使用することで、コールバックはコードの異なる分岐で異なる数の変数を使用でき、XSスタックサイズを削減できます。

**`void xsVars(xsIntegerValue theCount)`**<BR>
**`void xsmcVars(xsIntegerValue theCount)`**

| 引数 | 説明 |
| --- | :-- |
| `theCount` | 予約する変数スロットの数

***

引数スロットと変数スロットは、インデックスによってアクセスおよび割り当てされます。インデックスが無効な場合、例外がスローされます。

初期状態では：

- 引数スロットは、関数またはコンストラクタに渡されるパラメータスロットです。

- コールバックが関数の場合、`this`スロットは呼び出されるインスタンスを参照し、結果スロットはundefinedです。

- コールバックがコンストラクタの場合、`this`と結果スロットは作成されるインスタンスを参照します。

- 変数スロットはundefinedです。

スクリプトは、コンストラクタを関数として、または関数をコンストラクタとして呼び出すことができます。Cコールバックがコンストラクタとして実行されるか関数として実行されるかを確認するには、結果スロットが最初にundefinedかどうかをチェックできます。

***

**`xsSlot xsArgc`<BR>
`int xsmcArgc`**

引数の数を含む整数スロットを返します

***

**`xsSlot xsArg(xsIntegerValue theIndex)`**

| 引数 | 説明 |
| --- | :-- |
| `theIndex` | 引数のインデックス、0から`xsArgc-1`まで

引数スロットを返します

***

**`xsSlot xsThis`**

`this`スロットを返します

***

**`xsSlot xsResult`**

結果スロットを返します

***

**`xsSlot xsVarc`**

変数の数を含む整数スロットを返します

***

**`xsSlot xsVar(xsIntegerValue theIndex)`**

| 引数 | 説明 |
| --- | :-- |
| `theIndex` | 変数のインデックス、0から`xsVarc-1`まで

変数スロットを返します

***

#### 例

通常、引数、`this`、結果、変数スロットにアクセスしますが、結果と変数スロットのみを割り当てます。コールバック実行の終了時に結果スロットにあるものが、関数またはコンストラクタによってスクリプトに返されます。

このセクション（および次のセクション）のCの例では、`xsMachine`は仮想マシン構造体です。[マシン](#machine)セクションに示されています。

##### ECMAScriptでは：

```javascript
function foo() {
	var c, i, s;
	c = arguments.length;
	s = "";
	for (i = 0; i < c; i++)
		s = s.concat(arguments[i]);
	return s;
}
```

##### Cでは：

```c
void xs_foo(xsMachine* the) {
	xsIntegerValue c, i;
	xsVars(1);
	c = xsToInteger(xsArgc));
	xsVar(0) = xsString("");
	for (i = 0; i < c; i++)
		xsVar(0) = xsCall1(xsVar(0), xsID_concat, xsArg(i));
	xsResult = xsVar(0);
}
```

<a id="garbage-collector"></a>
### ガベージコレクタ

XSランタイムがスロットを割り当てる必要があり、十分なメモリがない場合、未使用のスロットが自動的に削除されます。ランタイムガベージコレクタは、マークアンドスイープアルゴリズムを使用します。ランタイムに未使用のスロットを強制的に削除させるには、`xsCollectGarbage`マクロを使用できます。

**`void xsCollectGarbage()`**

***

Cグローバルやc割り当て構造体など、ガベージコレクタによって管理されていないメモリにスロットを格納する場合は、`xsRemember`および`xsForget`マクロを使用してランタイムに通知します。

**`void xsRemember(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 記憶するスロット


**`void xsForget(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | 忘れるスロット

`xsRemember`はスロットをリンクし、`xsForget`はスロットのリンクを解除して、ガベージコレクタがCグローバルやC割り当て構造体が参照するスロットをマークするためにスキャンするスロットのチェーンに対して操作を行います。

***

以前にスロットのチェーンにリンクされたスロットの値を取得するには、`xsAccess`を使用します。

**`xsSlot xsAccess(xsSlot theSlot)`**

| 引数 | 説明 |
| --- | :-- |
| `theSlot` | アクセスするスロット

スロットの値を返します

##### Cでは：

```c
xsSlot gFooSlot;
void xsSetupFoo(xsMachine* the) {
	gFooSlot = xsThis;
	xsRemember(gFooSlot);
}
void xsInvokeFoo(xsMachine* the) {
	xsVars(2);
	xsVar(0) = xsAccess(gFooSlot);
	xsVar(1) = xsString("message");
	xsCall1(xsVar(0), xsID_invoke, xsVar(1));
}
void xsCleanupFoo(xsMachine* the) {
	xsForget(gFooSlot);
}
```

***

ガベージコレクタはデフォルトで有効になっています。ガベージコレクタを有効または無効にするには、`xsEnableGarbageCollection`を使用します。

**`xsSlot xsEnableGarbageCollection(xsBooleanValue enable)`**

| 引数 | 説明 |
| --- | :-- |
| `enable` | ガベージコレクションを有効にするには`true`、無効にするには`false`を設定

***

<a id="exceptions"></a>
### 例外

Cで例外を処理するために、XSランタイムは`setjmp`、`longjmp`、および`jmp_buf`バッファのチェーンを使用し、以下のように定義されています：

```c
typedef struct xsJumpRecord xsJump
struct xsJumpRecord {
	jmp_buf buffer;
	xsJump* nextJump;
	xsSlot* stack;
	xsSlot* frame;
};
```

ただし、XS in Cが例外をスローおよびキャッチするためのマクロを定義しているため、これを直接使用する必要はありません。

例外をスローするには、`xsThrow`マクロを使用します。

**`void xsThrow(xsSlot theException)`**

| 引数 | 説明 |
| --- | :-- |
| `theException` | 例外スロット

現在の例外を割り当てます

***

**`xsSlot xsException`**

現在の例外にアクセスし、例外スロットを返します

***

以下の例に示すように、`xsTry`および`xsCatch`マクロは一緒に使用して例外をキャッチします。Cコールバックで例外をキャッチし、その例外を関数またはコンストラクタを呼び出すスクリプトに伝播させたい場合は、例外を再度スローします。

##### ECMAScriptでは：

```javascript
{
	try {
		/* Exception thrown here ... */
	}
	catch(e) {
		/* ... is caught here. */
		throw e
 	}
}
```

##### Cでは：

```c
 {
	xsTry {
		/* Exception thrown here ... */
	}
	xsCatch {
		/* ... is caught here. */
		xsThrow(xsException)
	}
}
```

<a id="errors"></a>
### エラー

例外はCコールバックによってスローされることがあります。Cコールバックは、多くの場合、スクリプトとシステム間のインターフェースを提供します。多くのシステムコールは失敗する可能性があり、例外として伝播できるエラーをアプリケーションに返す方法があります。

特定のエラーについて、XSランタイムはエラータイプとプロトタイプを提供します。

```c
enum {
	XS_NO_ERROR = 0,
	XS_UNKNOWN_ERROR,
	XS_EVAL_ERROR,
	XS_RANGE_ERROR,
	XS_REFERENCE_ERROR,
	XS_SYNTAX_ERROR,
	XS_TYPE_ERROR,
	XS_URI_ERROR,
	XS_ERROR_COUNT
};
```

XS in Cは、特定の例外をスローするために以下のマクロを定義しています。

**`void xsUnknownError(...)`<BR>
`void xsEvalError(...)`<BR>
`void xsRangeError(...)`<BR>
`void xsReferenceError(...)`<BR>
`void xsSyntaxError(...)`<BR>
`void xsTypeError(...)`<BR>
`void xsURIError(...)`**

| 引数 | 説明 |
| --- | :-- |
| ... | 例外をスローするときに表示するメッセージとオプション引数

##### Cでは：

```
xpt2046 xpt = calloc(1, sizeof(xpt2046Record));
if (!xpt) xsUnknownError("out of memory");

if (strlen(string) > MAXNAMESIZE)
	xsRangeError("name too long: %s", string);

char *slash = strrchr(path, '/');
if (!slash)
	xsURIError("No path");
```

***

`xsErrorPrintf`マクロは、メッセージパラメータのみが必要な場合の`xsUnknownError`のショートカットです。

**`xsErrorPrintf(xsStringValue theMessage)`<BR>
`xsUnknownError("%s", theMessage)`**

| 引数 | 説明 |
| --- | :-- |
| theMessage | 例外をスローするときに表示するメッセージ

##### Cでは：

```c
if (rotation != requestedRotation)
	xsErrorPrintf("not configured for requested rotation");
```

***

<a id="debugger"></a>
### デバッガ

XS in Cは、Cコールバックのデバッグを支援する2つのマクロを提供します。

`xsDebugger`マクロは、ECMAScriptの`debugger`キーワードと同等です。

**`void xsDebugger()`**

***

`xsTrace`マクロは、グローバル`trace`関数と同等です。

**`void xsTrace(xsStringValue theMessage)`**

| 引数 | 説明 |
| --- | :-- |
| `theMessage` | デバッガでログに記録するメッセージ

##### ECMAScriptでは：

```javascript
debugger;
trace("Hello xsbug!\n");
```

##### Cでは：

```c
xsDebugger();
xsTrace("Hello xsbug!\n");
```

**`void xsLog(xsStringValue format, ...)`**

| 引数 | 説明 |
| --- | :-- |
| `format` | printf形式のフォーマット文字列
| `...` | フォーマット文字列によって参照される項目

サポートされているフォーマットオプションは%c、%hd、%d、%ld、%g、%sです。

##### Cでは：

```c
int err = -108;
char *msg = "out of memory";
xsLog("The error is %d (%s)\n", err, msg);
```

<a id="machine"></a>
## マシン

XSランタイムの主要な構造は仮想マシンで、これがスクリプトを解析、コンパイル、リンク、実行します。仮想マシンは不透明な構造ですが、構造の一部のメンバーはXS in Cのマクロを最適化するために利用でき、直接使用する必要はありません。

```c
typedef struct xsMachineRecord xsMachine
struct xsMachineRecord {
	xsSlot* stack;
	xsSlot* scope;
	xsSlot* frame;
	xsByte* code;
	xsSlot* stackBottom;
	xsSlot* stackTop;
	xsSlot* stackPrototypes;
	xsJump* firstJump;
};
```

単一のマシンは複数のスレッドをサポートしません。複数のスレッドで作業するには、スレッドごとに1つのXSランタイムマシンを作成し、ホストがオプションでマシン間の通信方法を提供します。

<a id="machine-allocation"></a>
### マシンの割り当て

XSランタイムを使用するには、`xsCreateMachine`マクロでマシンを作成し、必要に応じてメモリを割り当てる必要があります。そのパラメータは：

- マシンに割り当てるものを指定する本質的にパラメータであるメンバーを持つ構造体。デフォルトを使用する場合は`NULL`を渡します。

```c
typedef struct {
	xsIntegerValue initialChunkSize;
	xsIntegerValue incrementalChunkSize;
	xsIntegerValue initialHeapCount;
	xsIntegerValue incrementalHeapCount;
	xsIntegerValue stackCount;
	xsIntegerValue initialKeyCount;
	xsIntegerValue incrementalKeyCount;
	xsIntegerValue nameModulo;
	xsIntegerValue symbolModulo;
	xsIntegerValue parserBufferSize;
	xsIntegerValue parserTableModulo;
	xsIntegerValue staticSize;
} xsCreation;
```

- マシンの名前

- コールバックで設定および取得できるコンテキスト（次のセクションで説明）。初期コンテキストが不要な場合は`NULL`を渡します。

**`xsMachine* xsCreateMachine(xsCreation* theCreation, xsStringValue theName, void* theContext)`**

| 引数 | 説明 |
| --- | :-- |
| `theCreation` | マシンのパラメータ
| `theName` | 文字列としてのマシンの名前
| `theContext` | マシンの初期コンテキスト、または`NULL`

成功した場合はマシンを返し、そうでなければ`NULL`を返します




`xsCreation`構造体で指定されるマシンのパラメータについて：

- マシンは文字列、バイトコード、配列バッファ、big int値などを格納するためにチャンクを使用します。`initialChunkSize`はチャンクに割り当てられるメモリの初期サイズです。`incrementalChunkSize`は、チャンクに割り当てられるメモリをランタイムがどのように拡張するかを指示します。

- マシンはヒープとスロットのスタックを使用します。`initialHeapCount`はヒープに割り当てられるスロットの初期数です。`incrementalHeapCount`は、ヒープに割り当てられるスロット数をランタイムがどのように増加させるかを指示します。`stackCount`はスタックに割り当てられるスロット数です。これらの値はすべてスロットであり、バイトではないことに注意してください。

- シンボルは文字列値と識別子を結合します。[`xsID`](#xs-id)を参照してください。`initialKeyCount`は、マシンが初期化時に割り当てるシンボルの数です。キーが枯渇すると`incrementalKeyCount`個のキーが追加されます。`incrementalKeyCount`が0の場合、キーが枯渇するとVMは中止します。`symbolModulo`は、マシンがシンボルに使用するハッシュテーブルのサイズです。`nameModulo`は、マシンがシンボル名に使用するハッシュテーブルのサイズです。

- 一部のXSホストは、ホストされるスクリプトのメモリニーズに対応するためにランタイムでスロットとチャンクヒープを無制限に拡張しようとします。他のホストは、マシンに割り当てられる最大メモリを制限します。後者の場合、`staticSize`は、スタックを含むチャンクとスロットの組み合わせに割り当てられる総バイト数を定義します。一般的に、リソース制約のあるデバイスで実行されるホストのみが`staticSize`を実装します。

***

マシンが完了したら、`xsDeleteMachine`マクロでそれを解放します。すべてのホストオブジェクトのデストラクタが実行され、マシンによって割り当てられたすべてのメモリが解放されます。

**`void xsDeleteMachine(xsMachine* the)`**

| 引数 | 説明 |
| --- | :-- |
| `the` | マシン

`xsDeleteMachine`マクロは、このドキュメントで説明されている多くのマクロの1つで、`the`という名前の明示的なマシンパラメータを持ち、`xsCreateMachine`によって返される値が渡されます。（他のそのようなマクロは`xsGetContext`、`xsSetContext`、`xsBeginHost`、`xsEndHost`です。）これらのマクロのみが明示的な`the`パラメータを持つのは、コールバック外で使用でき、例外をスローできない唯一のマクロだからです。コールバックは、他のすべてのマクロが`the`という名前の暗黙のパラメータを持つため、マシンパラメータを`the`と名前付けする必要があります。この規則の主な理由は簡潔性ですが、これらの他のマクロがコールバック内でのみ使用でき、例外をスローできるという事実も強調しています。

#### 例

以下の例は、`xsCreateMachine`と`xsDeleteMachine`の使用を示しています。例で呼び出される`xsMainContext`関数は、次のセクションで定義されています。

```c
int main(int argc, char* argv[])
{
	xsCreation aCreation = {
		128 * 1024 * 1024,	/* initialChunkSize */
		16 * 1024 * 1024, 	/* incrementalChunkSize */
		4 * 1024 * 1024, 	/* initialHeapCount */
		1 * 1024 * 1024,	/* incrementalHeapCount */
		1024,			/* stack count */
		2048+1024,		/* key count */
		1993,			/* name modulo */
		127			/* symbol modulo */
	};
	xsMachine* aMachine;

	aMachine = xsCreateMachine(&aCreation, "machine", NULL);
	if (aMachine) {
		xsMainContext(aMachine, argc, argv);
		xsDeleteMachine(aMachine);
	}
	else
		fprintf(stderr, "### Cannot allocate machine\n");
	return 0;
}
```

<a id="context"></a>
### コンテキスト

マシンは主にコールバックを通してCコードを呼び出します。コールバックでは、*コンテキスト*を設定および取得できます。これは、マシンの情報を格納および取得できる領域へのポインタです。

**`void xsSetContext(xsMachine* the, void* theContext)`**

| 引数 | 説明 |
| --- | :-- |
| `the` | マシン
| `theContext` | コンテキスト

コンテキストを設定します

***

**`void* xsGetContext(xsMachine* the)`**

| 引数 | 説明 |
| --- | :-- |
| `the` | マシン

コンテキストを返します

***

#### 例
以下のコードは、前のセクションの例で呼び出された`xsMainContext`関数でコンテキストが設定される様子を示しています。

```c
typedef struct {
	int argc;
	char** argv;
} xsContext;

void xsMainContext(xsMachine* theMachine, int argc, char* argv[])
{
	xsContext* aContext;

	aContext = malloc(sizeof(xsContext));
	if (aContext) {
		aContext->argc = argc;
		aContext->argv = argv;
		xsSetContext(theMachine, aContext);
		xsSetContext(theMachine, NULL);
		free(aContext);
	}
	else
		fprintf(stderr, "### Cannot allocate context\n");
}
```

<a id="host"></a>
### ホスト

このセクションでは、XS in Cのホスト関連マクロについて説明します（表2を参照）。ホスト関連マクロを使用する注釈付きの例が続きます。

ホストオブジェクトは、Cでのみアクセスできるデータポインタと、ホストオブジェクトがガベージコレクションされるときに呼び出されるネイティブデストラクタを持つXSオブジェクトです。ホストオブジェクトは、Cで`xsNewHostObject`を使用して作成され、JavaScriptで[XS `@`構文](#syntax-extension)を使用してJavaScript `(class Foo @ "aDestructorFunction" {}`で作成されます。内部的に、ホストオブジェクトにはデストラクタとデータポインタを保持する専用スロットがあります。非ホストオブジェクトにはこのスロットがありません。したがって、ホストオブジェクトのみがネイティブデストラクタとCからのみアクセス可能なデータポインタを持ちます。このデータポインタは、ホストデータまたはホストチャンクのいずれかです。

ホストデータは、XSがホストオブジェクトに格納するポインタです。ポインタとそれが指すデータは、完全にホストオブジェクトのCコードによって管理されます。XSはポインタを格納しますが、いかなる方法でもアクセスしません。ホストデータは通常`malloc`/`calloc`で割り当てられますが、これは必須ではありません。ホストデータは、ホストオブジェクトのデストラクタによって破棄されます。

ホストチャンクは、ホストオブジェクトがネイティブCコードから使用するために、XSがチャンクヒープに割り当てるメモリです。XSは、ホストオブジェクトがガベージコレクションされるときにこのストレージをガベージコレクションします。メモリは再配置可能（すべてのXSチャンクと同様）なので、ホストデータとは異なり、断片化によるメモリ損失を回避します。ただし、ガベージコレクタがメモリを圧縮するときにポインタが無効になる可能性があるため、追加の注意が必要です。したがって、Cコードは、ガベージコレクションをトリガーする可能性のある操作の後にポインタを再取得する必要があります。チャンクポインタは移動する可能性があるため、XSコールバック内でのみ使用できます。たとえば、割り込みからアクセスすることは、移動している可能性があるため安全ではありません。[Rectangleの例](#rectangle-example)では、ホストチャンクの使用方法を示しています。

ホストデータを使用してホストオブジェクトを実装する方が、ホストチャンクよりも簡単ですが、メモリ効率が劣る可能性があります。

> オブジェクトはホストデータまたはホストチャンクのいずれかを持ちますが、両方を持つことはありません。

**表2.** ホスト関連マクロ

<table class="normalTable">
  <tbody>
    <tr>
      <th scope="col">マクロ</th>
      <th scope="col">説明</th>
    </tr>
    <tr>
      <td>
        <p><code>xsNewHostFunction</code></p>
        <p><code>xsNewHostConstructor</code></p>
      </td>
      <td>ホスト関数またはホストコンストラクタを作成します</td>
    </tr>
    <tr>
      <td><code>xsNewHostObject</code></td>
      <td>ホストオブジェクトを作成します</td>
    </tr>
    <tr>
      <td><code>xsNewHostInstance, xsmcNewHostInstance</code></td>
      <td>ホストオブジェクトインスタンスを作成します</td>
    </tr>
    <tr>
      <td>
        <p><code>xsGetHostData, xsmcGetHostData</code></p>
        <p><code>xsSetHostData, xsmcSetHostData</code></p>
      </td>
      <td>ホストオブジェクト内のデータを取得または設定します</td>
    </tr>
    <tr>
      <td>
        <p><code>xsGetHostChunk, xsmcGetHostChunk</code></p>
        <p><code>xsSetHostChunk, xsmcSetHostChunk</code></p>
      </td>
      <td>ホストオブジェクト内のデータをチャンクとして取得または設定します</td>
    </tr>
    <tr>
      <td><code>xsSetHostDestructor</code></td>
      <td>ホストオブジェクトのデストラクタを設定します</td>
    </tr>
    <tr>
      <td>
        <p><code>xsBeginHost</code></p>
        <p><code>xsEndHost</code></p>
      </td>
      <td>スタックフレームを設定およびクリーンアップするために一緒に使用され、その間にXS in Cのすべてのマクロを使用できます</td>
    </tr>
  </tbody>
</table>

#### xsNewHostFunctionとxsNewHostConstructor

*ホスト関数*は特別な種類の関数で、その実装がECMAScriptではなくCにあります。スクリプトにとって、ホスト関数は通常の関数と同じです。ただし、スクリプトがホスト関数を呼び出すと、Cコールバックが実行されます。Cで実装されたコンストラクタである*ホストコンストラクタ*についても同じことが言えます。

```c
typedef void (*xsCallback)(xsMachine* the);
```

ホスト関数を作成するには、`xsNewHostFunction`マクロを使用します。

**`xsSlot xsNewHostFunction(xsCallback theCallback, xsIntegerValue theLength)`**

| 引数 | 説明 |
| --- | :-- |
| `theCallback` | 実行するコールバック
| `theLength` | コールバックが期待するパラメータの数

ホスト関数を作成し、新しいホスト関数への参照を返します

***

**`xsSlot xsNewHostConstructor(xsCallback theCallback, xsIntegerValue theLength, xsSlot thePrototype)`**

| 引数 | 説明 |
| --- | :-- |
| `theCallback` | 実行するコールバック
| `theLength` | コールバックが期待するパラメータの数
| `thePrototype` | 作成するインスタンスのプロトタイプへの参照

ホストコンストラクタを作成し、新しいホストコンストラクタへの参照を返します

***

#### xsNewHostObject

*ホストオブジェクト*は、Cでのみ直接アクセスできるデータを持つ特別な種類のオブジェクトです。ホストオブジェクト内のデータは、スクリプトからは見えません。

ガベージコレクタがホストオブジェクトを削除しようとするとき、ホストオブジェクトのデストラクタがあれば実行します。ホストオブジェクトへの参照はデストラクタに渡されません。デストラクタはデータを破棄することのみができます。

```c
typedef void (xsDestructor)(void* theData);
```

ホストオブジェクトを作成するには、`xsNewHostObject`マクロを使用します。

**`xsSlot xsNewHostObject(xsDestructor theDestructor)`**

| 引数 | 説明 |
| --- | :-- |
| `theDestructor` | ガベージコレクタによって実行されるデストラクタ。ホストオブジェクトのデストラクタを渡すか、デストラクタが不要な場合は`NULL`を渡します。

ホストオブジェクトを作成し、新しいホストオブジェクトへの参照を返します

***

#### xsNewHostInstance

ホストオブジェクトのインスタンスを作成するには、`xsNewHostInstance`マクロを使用します。

**`xsSlot xsNewHostInstance(xsSlot thePrototype)`**
**`xsSlot xsmcNewHostInstance(xsSlot thePrototype)`**

| 引数 | 説明 |
| --- | :-- |
| `thePrototype` | 作成するインスタンスのプロトタイプへの参照。この引数はホストオブジェクトでなければなりません。

ホストオブジェクトインスタンスを作成し、新しいホストオブジェクトインスタンスへの参照を返します

***

#### xsGetHostDataとxsSetHostData

ホストオブジェクトのデータを取得および設定するには、`xsGetHostData`および`xsSetHostData`マクロを使用します。`theThis`パラメータがホストオブジェクトを参照していない場合、両方とも例外をスローします。

**`void* xsGetHostData(xsSlot theThis)`**<BR>
**`void* xsmcGetHostData(xsSlot theThis)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | ホストオブジェクトへの参照

ホストデータポインタを返します。

***

**`void xsSetHostData(xsSlot theThis, void* theData)`<BR>
`void xsmcSetHostData(xsSlot theThis, void* theData)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | ホストオブジェクトへの参照
| `theData` | 設定するデータ

ホストデータポインタを設定します。

***

#### xsGetHostChunkとxsSetHostChunk

ホストオブジェクトのデータをチャンクとして取得および設定するには、`xsGetHostChunk`および`xsSetHostChunk`マクロを使用します。`theThis`パラメータがホストオブジェクトを参照していない場合、両方とも例外をスローします。ArrayBufferとStringが使用するメモリと同様に、チャンクメモリはXSランタイムによって割り当てられ管理されます。詳細については、[handle](./handle.md)ドキュメントを参照してください。

**`void* xsGetHostChunk(xsSlot theThis)`<BR>
`void* xsmcGetHostChunk(xsSlot theThis)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | ホストオブジェクトへの参照

ホストチャンクデータへのポインタを返します

***

**`void xsSetHostChunk(xsSlot theThis, void* theData, xsIntegerValue theSize)`<BR>
`void xsmcSetHostChunk(xsSlot theThis, void* theData, xsIntegerValue theSize)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | ホストオブジェクトへの参照
| `theData` | 設定するデータ、またはチャンクデータを未初期化のままにする場合は`NULL`
| `theSize` | データのサイズ（バイト単位）

データを格納するチャンクを割り当て、オプションで初期化します。

***

#### xsSetHostDestructor

ホストオブジェクトのデストラクタを設定する（または`NULL`を渡してデストラクタをクリアする）には、`xsSetHostDestructor`マクロを使用します。`theThis`パラメータがホストオブジェクトを参照していない場合、このマクロは例外をスローします。

**`void xsSetHostDestructor(xsSlot theThis, xsDestructor theDestructor)`<BR>
`void xsmcSetHostDestructor(xsSlot theThis, xsDestructor theDestructor)`**

| 引数 | 説明 |
| --- | :-- |
| `theThis` | ホストオブジェクトへの参照
| `theDestructor` | ガベージコレクタによって実行されるデストラクタ、またはデストラクタをクリアする場合は`NULL`

***

#### xsBeginHostとxsEndHost

新しいスタックフレームを確立するには`xsBeginHost`マクロを使用し、それを削除するには`xsEndHost`マクロを使用します。

**`void xsBeginHost(xsMachine* the)`**<BR>
**`void xsEndHost(xsMachine* the)`**

| 引数 | 説明 |
| --- | :-- |
| `the` | マシン

`xsBeginHost`マクロはスタックを設定し、`xsEndHost`マクロはスタックをクリーンアップするため、`xsBeginHost`と`xsEndHost`の間のブロックでXS in Cのすべてのマクロを使用できます。

`xsBeginHost`と`xsEndHost`の呼び出し間で発生するキャッチされない例外は、`xsEndHost`を超えて伝播しません。

<a id="file-example"></a>
##### 例

この例では、XS in Cのホストマクロを使用して`File`クラスを作成します。これは最も柔軟性を提供する低レベルの技術です。ほとんどのプロジェクトは、XS in Cを使用して直接クラスを作成するのではなく、より簡単な[`@`構文拡張](#syntax-extension)を使用してクラスを宣言します。

このコードは、JavaScriptから`File`クラスを使用してファイルを開いたり閉じたりします：

```c
const f = new File("/Users/user/test.js", "rb");
f.close();
```

以下のコードは`File`クラスを構築します。XS in Cホストマクロ呼び出しは、`xsBeginHost`と`xsEndHost`の間のブロックに表示されます。2つの変数スロットが`File`ホストオブジェクトとコンストラクタを格納するために使用されます。`File`オブジェクトには、引数を持たない単一のホスト関数`close`が含まれます。

`prototype`は、オブジェクトがガベージコレクションされるときに呼び出されるネイティブデストラクタ`xs_file_destructor`を含むホストオブジェクトです。このプロトタイプは、ネイティブコンストラクタ`xs_file_constructor`とともに`xsNewHostConstructor`に提供されます。

この例では、コンストラクタを作成した後に`close`関数をプロトタイプに追加しています。代わりに前に追加することもできます。

この例では、プロパティ`isOpen`のゲッターアクセサ関数も追加しています。

```c
#define kPrototype (0)
#define kConstructor (1)

xsBeginHost(the);
	xsVars(2);
	xsVar(kPrototype) = xsNewHostObject(xs_file_destructor);
	xsVar(kConstructor) = xsNewHostConstructor(xs_file_constructor, 0, xsVar(kPrototype));
	xsSet(xsGlobal, xsID("File"), xsVar(kConstructor));
	xsDefine(xsVar(kPrototype), xsID("close"),	xsNewHostFunction(xs_file_close, xsDefault));
	xsDefine(xsVar(kPrototype), xsID("isOpen"),	xsNewHostFunction(xs_file_get_isOpen, xsIsGetter));
xsEndHost(the);
```

`xs_file_constructor`関数はホストコンストラクタを実装します。コンストラクタは`File`オブジェクトプロトタイプのインスタンスをインスタンス化し、要求されたファイルを開き、stdio `FILE`ポインタを含む関連する`xsFileRecord`をホストデータとして格納します。

`xsNewHostConstructor`を呼び出して作成されたコンストラクタの実装は、`@`構文を使用して作成されたものと若干異なることに注意してください。具体的には、`xsNewHostConstructor`によって作成されたコンストラクタはインスタンスを作成する必要がありますが、XSは`@`構文で宣言されたコンストラクタのインスタンスを作成します。ここでコンストラクタは`xsNewHostInstance`を使用してインスタンスを作成し、戻り値`xsResult`に割り当てます。`xsNewHostInstance`に渡されるプロトタイプは、`xsTarget`を通してアクセスされるコンストラクタのプロトタイプから取得されます。`xsTarget`値は、JavaScriptの[`new.target`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/new.target)疑似プロパティのXS in C同等物です。

```c
typedef struct {
	FILE *fd;
} xsFileRecord, *xsFile;

static void xs_file_constructor(xsMachine *the)
{
	xsFileRecord f;
	FILE *fd;
	xsResult = xsGet(xsTarget, xsID("prototype"));
	xsResult = xsNewHostInstance(xsResult);
	fd = fopen(xsToString(xsArg(0)), xsToString(xsArg(1)));
	if (!fd)
		xsUnknownError("can't open");
	f = malloc(sizeof(xsFileRecord));
	if (!f) {
		fclose(fd);
		fxAbort(the, XS_NOT_ENOUGH_MEMORY_EXIT);
	}
	f->fd = fd;
	xsSetHostData(xsResult, f);
}
```

`xs_file_destructor`関数はホストオブジェクトのデストラクタを実装します。デストラクタはファイルを閉じ、ホストデータを解放します：

```c
static void xs_file_destructor(void *data)
{
	xsFile f = data;
	if (f) {
		fclose(f->fd);
		free(f);
	}
}
```

> 注意：デストラクタ関数は、`File`インスタンスがガベージコレクションされるときにXSによって呼び出されます。

`xs_file_close`関数は、インスタンスがガベージコレクタによって解放されるのを待つのではなく、ファイルをすぐに閉じます。この関数は、オブジェクトインスタンスホストデータから関連する`xsFileRecord`レコードを取得し、ホストオブジェクトデストラクタを呼び出してファイルを閉じます。

```c
static void xs_file_close(xsMachine *the)
{
	xsFile f = xsGetHostData(xsThis);
	if (!f) return;
	xs_file_destructor(f);
	xsSetHostData(xsThis, NULL);
}
```

`xs_file_get_isOpen`ゲッターアクセサ関数は、ファイルが開いているかどうかに応じて結果を`true`または`false`に設定します。

```c
static void xs_file_get_isOpen(xsMachine *the)
{
	xsFile f = xsGetHostData(xsThis);
	xsResult = f ? xsTrue : xsFalse;
}
```

***

<a id="syntax-extension"></a>
### JavaScript `@`言語構文拡張

XSは、CでJavaScript関数を実装するための`@`言語構文拡張を提供します。言語拡張はXSコンパイラによってのみ認識されます。このセクションでは、C関数でメソッドを実装するJavaScriptクラスを使用して言語拡張を紹介します。

<a id="rectangle-example"></a>

```javascript
class Rectangle @ "xs_rectangle_destructor" {
	constructor(...params) @ "xs_rectangle";

	get x() @ "xs_rectangle_get_x";
	set x() @ "xs_rectangle_set_x";
	get y() @ "xs_rectangle_get_y";
	set y() @ "xs_rectangle_set_y";
	get w() @ "xs_rectangle_get_w";
	set w() @ "xs_rectangle_set_w";
	get h() @ "xs_rectangle_get_h";
	set h() @ "xs_rectangle_set_h";

	contains(x, y) @ "xs_rectangle_contains";

	union(r) @ "xs_rectangle_union";
};

export default Rectangle;
```
`Rectangle`クラスは、`@`関数で指定されたコールバックを使用してCで完全に実装されています。たとえば、`contains`メソッドは`xs_rectangle_contains` C関数によって実装されます。C関数は、プロパティへのアクセス、ホストデータ、結果の返却にXS in Cマクロを使用します。JavaScriptアプリケーションは`Rectangle`クラスをインポートし、メソッドにアクセスします。

```javascript
import Rectangle from "rectangle";

let r1 = new Rectangle(0, 0, 200, 100);
let r2 = new Rectangle(20, 40, 300, 50);
let r3 = new Rectangle();
r3.union(r1, r2);
```

`Rectangle`コンストラクタ`xs_rectangle`関数は、パラメータをホストチャンクに格納します。コンストラクタは、単一の`Rectangle`インスタンスまたは個別の`x`、`y`、`w`、`h`値のいずれかを受け入れます。この関数は、`xsmcArgc`マクロを使用して関数パラメータをカウントし、`xsmcIsInstanceOf`マクロを使用して最初のパラメータがオブジェクトかどうかを判定します。

```c
typedef struct {
	int x;
	int y;
	int w;
	int h;
} xsRectangleRecord, *xsRectangle;

void xs_rectangle(xsMachine *the)
{
	xsRectangleRecord r;
	if (xsmcArgc == 0) {
		r.x = r.y = r.w = r.h = 0;
	}
	else if (xsmcIsInstanceOf(xsArg(0), xsObjectPrototype)) {
		xsRectangle r1 = xsmcGetHostChunk(xsArg(0));
		r = *r1;
	}
	else {
		r.x = xsmcToInteger(xsArg(0));
		r.y = xsmcToInteger(xsArg(1));
		r.w = xsmcToInteger(xsArg(2));
		r.h = xsmcToInteger(xsArg(3));
	}
	xsmcSetHostChunk(xsThis, &r, sizeof(r));
}
```
デストラクタ関数`xs_rectangle_destructor`は、オブジェクトインスタンスが削除またはガベージコレクションされるときに呼び出されます。インスタンスによって割り当てられたメモリやリソースは、デストラクタで解放する必要があります。XSランタイムがホストチャンクメモリを管理するため、デストラクタはチャンクを破棄する必要がありません。

```c
void xs_rectangle_destructor(void *data)
{
}
```
`Rectangle`クラスは、クラスプロパティのゲッターとセッターを提供します。

```javascript
	get x() @ "xs_rectangle_get_x";
	set x() @ "xs_rectangle_set_x";
```

`get`関数は、ホストチャンクから対応するフィールドを読み取り、`xsResult`を設定して呼び出し元にプロパティを返します。`set`関数は、提供された値をホストチャンクに格納します。

```c
void xs_rectangle_get_x(xsMachine *the)
{
	xsRectangle r = xsmcGetHostChunk(xsThis);
	xsmcSetInteger(xsResult, r->x);
}

void xs_rectangle_set_x(xsMachine *the)
{
	xsRectangle r = xsmcGetHostChunk(xsThis);
	r->x = xsmcToInteger(xsArg(0));
}
```

`union`メソッドは、関数に渡されたすべての矩形の合併を返します。`xs_rectangle_union`関数は、`xsmcArgc`マクロを使用して`Rectangle`インスタンスの数をカウントします。合併結果は、呼び出しインスタンスのホストチャンクに格納し直されます。JavaScriptアプリケーションは、`get *()`メソッドを使用して結果の矩形プロパティを読み取ります。

```c
void xs_rectangle_union(xsMachine *the)
{
	xsIntegerValue i, argc;
	xsRectangle r, r0 = xsmcGetHostChunk(xsThis);
	xsRectangleRecord rUnion;
	r = r0;
	for (i = 0; i < argc; ++i) {
		Union(&rUnion, r, xsmcGetHostChunk(xsArg(i)));
		r = &rUnion;
	}
	*r0 = rUnion;
}
```

スタンドアロン関数 -- クラスの一部ではない関数 -- もCで実装できます。`@`構文拡張は、通常関数本体が表示される場所で使用されます。

```c
function restart() @ "xs_restart";
```

`xs_restart`の実装における`xsThis`の値は、以下の呼び出しでは`xsGlobal`であるレシーバと一致します。

```javascript
	restart();
```

<a id="glossary"></a>
## 用語集

| 用語 | 定義 |
| :--- | :-- |
| constructor | ECMAScriptにおいて、`prototype`プロパティを持ち、`new`演算子がインスタンスを構築するために呼び出す関数。`prototype`プロパティの値は、コンストラクタが構築するインスタンスのプロトタイプになります。
| context | コールバックでXSランタイム仮想マシンの情報を格納および取得できる領域へのポインタ。
| direct slot | ECMAScriptプリミティブ型（undefined、null、boolean、number、string、symbol）に対応するスロット型の1つ。最適化として提供される整数と文字列スロットもあります。
| ECMAScript | アプリケーションロジックと制御を実装するためのオブジェクト指向、プロトタイプベースの言語。
| host | ECMAScript用語において、XSランタイムを使用するアプリケーション。
| host constructor | XSにおいて、ECMAScriptではなくCで実装されたコンストラクタ。
| host function | XSにおいて、ECMAScriptではなくCで実装された関数。
| host object | XSにおいて、Cでのみ直接アクセスできるデータを持つオブジェクト。
| indirect slot | オブジェクト、関数、配列などのインスタンスへの参照を含むスロットの型。ECMAScript `reference`型に対応します。
| instance | 別のオブジェクト（*プロトタイプ*と呼ばれる）からプロパティを継承するオブジェクト。
| property | ECMAScriptにおいて、オブジェクト内で名前によってアクセスされる値（配列内でインデックスによってアクセスされる項目とは対照的）。XS in Cにおいて、オブジェクト内でインデックスによってアクセスされるスロット（配列内で項目がインデックスによってアクセスされるのと同様）。
| prototype | 別のオブジェクト（インスタンスと呼ばれる）がプロパティを継承する元となるオブジェクト。
| sandbox | 信頼できないコードがコードを実行しているデバイスに害を与えることを防ぐために制限された環境。XSアプリケーションスクリプトのサンドボックスには、ECMAScript仕様で定義された標準機能と、XSモジュールによって定義および許可された追加機能が含まれます。
| slot | XSランタイムですべてが格納される不透明な構造で、XS in Cを通してのみ操作されます。
| XS | 標準ベースのネットワーク対応インタラクティブマルチメディアアプリケーション（GUIベースランタイム）または様々なデバイス用のコマンドラインツールを開発するために設計された、ランタイムライブラリとコマンドラインツールで構成されるツールキット。xsruntimeとxscも参照。
| XS runtime | XSのランタイムライブラリ部分。
| XS in C | XSランタイムのCインターフェース。
| xsbug | アプリケーション、モジュール、スクリプトをデバッグするために使用されるXSデバッガ。
| xsc | XSのコマンドラインツール部分。JavaScriptファイルをシンボルとバイトコードを含むXSバイナリファイルにコンパイルし、XSランタイム内に含まれるXS仮想マシンによって実行されます。

<!-- TBD:
	- Document xsCall*_noResult, xsmcCall_noResult
	- Document xsNewHostConstructorObject, xsNewHostFunctionObject
	- Document: xsReference
-->

<a id="license"></a>
## ライセンス
    Copyright (c) 2016-2023  Moddable Tech, Inc.

    This file is part of the Moddable SDK Runtime.

    The Moddable SDK Runtime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Moddable SDK Runtime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Moddable SDK Runtime.  If not, see <http://www.gnu.org/licenses/>.

    This file incorporates work covered by the following copyright and
    permission notice:

        Copyright (C) 2010-2016 Marvell International Ltd.
        Copyright (C) 2002-2010 Kinoma, Inc.

        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at

         http://www.apache.org/licenses/LICENSE-2.0

        Unless required by applicable law or agreed to in writing, software
        distributed under the License is distributed on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        See the License for the specific language governing permissions and
        limitations under the License.
