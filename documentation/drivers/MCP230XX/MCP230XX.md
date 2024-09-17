# MCP230XX
Copyright 2017 Moddable Tech, Inc.<BR>
改訂： 2017年12月21日

[MCP23008](http://www.microchip.com/wwwproducts/en/MCP23008) デバイスは、I2Cバスアプリケーション向けに8ビットの汎用並列I/O拡張を提供します。（MCP23008製品ページからの説明）

[MCP23017](http://www.microchip.com/wwwproducts/en/MCP23017) デバイスは、I2Cバスアプリケーション向けに16ビットの汎用並列I/O拡張を提供します。（MCP23017製品ページからの説明）

### モジュール "MCP230XX"

ドライバモジュール "MCP230XX" は以下をエクスポートします：

```js
export {
  MCP23008,
  MCP23017
};
```

### MCP23008 クラス

`MCP23008` クラスは、I2Cバス上の単一のMCP23008 ICを表すインスタンスを生成します。`MCP23008` クラスは内部の `Expander` クラスを拡張しており、`Expander` クラスは `SMBus` クラスを拡張しています。`Expander` はエクスポートされません。

`MCP23008` のインスタンスオブジェクトには、8つの `Pin` インスタンスオブジェクトエントリが含まれています。

```js
import Timer from "timer";
import {MCP23008} from "MCP230XX";


export default function() {
  let leds = new MCP23008(); // defaults to 0x20!
  let mask = 0x88;

  Timer.repeat(() => {
    mask = ((mask << 1) | (mask >> 7)) & 0xFF;

    for (let i = 0; i < 8; i++) {
      leds[i].write(mask & (1 << i) ? 1 : 0);
    }
  }, 50);
}
```

![](ESP8266-MCP23008-leds.png)

#### コンストラクタの説明

##### `MCP23008([dictionary])`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するためのプロパティを持つオブジェクト。サポートされているパラメータは以下の[Dictionary](#MCP23008-dictionary)セクションに記載されています。

```
let leds = new MCP23008({ sda: 4, scl: 5 });
```

<a id="MCP23008-dictionary"></a>
#### Dictionary

| パラメータ | 型 | デフォルト値 | 説明
| --- | --- | --- | :--- |
| `address`  | `number` | `0x20` | I2Cデバイスのアドレス |
| `hz`       | `number` | 100kHz | I2Cデバイスのクロック周波数。 |
| `sda`      | `number` | 4 | I2Cのsda（データ）ピン。 |
| `scl`      | `number` |  5 | I2Cのscl（クロック）ピン。     |
| `inputs`   | `number` (byte) | `0b11111111` | 8つのGPIOピンの入力/出力初期化状態を表すバイト。`1`は入力、`0`は出力 |
| `pullups`  | `number` (byte)  | `0b00000000` | 8つのGPIOピンのプルアップ初期化状態を表すバイト。`1`はプルアップ、`0`はデフォルト |

#### プロパティ

すべてのプロパティは読み取り専用です。

| 名前 | 型 | 値 | 説明 |
| --- | --- | --- | :--- |
| `length` | `number` | `8` | コレクション内のピンの数 |
| `offset` | `number` | `0` | レジスタオフセット |
| `IODIR` | `number` | `0x00` | `IODIR` レジスタ |
| `GPIO` | `number` | `0x06` | `GPIO` レジスタ |
| `GPPU` | `number` | `0x09` | `GPPU` レジスタ |
| 0-8 | `pin` | | `Pin` インスタンス（[Pin Class](#pin-class)のセクションを参照） |

#### メソッド

##### `write(byte)`

一時的にすべてのピンのモードを出力に設定し、一度にすべてのピンに書き込みます。

```js
let expander = new MCP23008(); // デフォルトは0x20です！
expander.write(0b11111111); // すべてのピンを1に設定
```

##### `read()`

一時的にすべてのピンのモードを入力に設定し、一度にすべてのピンを読み取り、その値を返します。

```js
let expander = new MCP23008(); // デフォルトは0x20です！
trace(`${expander.read()}\n`);
```

### MCP23017 クラス

`MCP23017` クラスは、I2Cバス上の単一のMCP23017 ICを表すインスタンスを生成します。`MCP23017` クラスは内部の `Expander` クラスを拡張しており、`Expander` クラスは `SMBus` クラスを拡張しています。

`MCP23017`のインスタンスオブジェクトには、16の`Pin`インスタンスオブジェクトエントリが含まれています。

```js
import Timer from "timer";
import {MCP23017} from "MCP230XX";


export default function() {
  let leds = new MCP23017(); // defaults to 0x20!
  let mask = 0x8888;

  Timer.repeat(() => {
    mask = ((mask << 1) | (mask >> 15)) & 0xFFFF;

    for (let i = 0; i < 16; i++) {
      leds[i].write(mask & (1 << i) ? 1 : 0);
    }
  }, 50);
}
```

![](ESP8266-MCP23017-leds.png)

#### コンストラクタの説明

##### `MCP23017([dictionary])`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するためのプロパティを持つオブジェクト。サポートされているパラメータは以下の[Dictionary](#MCP23017-dictionary)セクションで指定されています。


```
let leds = new MCP23017({ sda: 4, scl: 5 });
```

<a id="MCP23017-dictionary"></a>
#### Dictionary

| パラメータ | 型 | デフォルト値 | 説明
| --- | --- | --- | :--- |
| `address`  | `number` | `0x20` | I2Cデバイスのアドレス |
| `hz`       | `number` | 100kHz | I2Cデバイスのクロックスピード。 |
| `sda`      | `number` | 4 | I2Cのsda（データ）ピン。 |
| `scl`      | `number` | 5 | I2Cのscl（クロック）ピン。 |
| `inputs`   | `number` (word) |  `0b1111111111111111` | 16のGPIOピンの入力/出力初期化状態を表すワード。入力の場合は`1`、出力の場合は`0` |
| `pullups`  | `number` (word) | `0b0000000000000000` | 16のGPIOピンのプルアップ初期化状態を表すワード。プルアップの場合は`1`、デフォルトの場合は`0` |

#### プロパティ

すべてのプロパティは読み取り専用です。

| 名前 | 型 | 値 | 説明 |
| --- | --- | --- | :--- |
| `length` | `number` | `16` | コレクション内のピンの数 |
| `offset` | `number` | `1` | レジスタオフセット |
| `IODIR` | `number` | `0x00` | `IODIR` レジスタ |
| `GPIO` | `number` | `0x0C` | `GPIO` レジスタ |
| `GPPU` | `number` | `0x12` | `GPPU` レジスタ |
| 0-16 | `pin` | | `Pin` インスタンス (セクション [Pin Class](#pin-class) を参照) |

#### メソッド

##### `write(word)`

一時的にすべてのピンのモードを出力に設定し、一度にすべてのピンに書き込みます。

```js
let expander = new MCP23017(); // デフォルトは 0x20!
expander.write(0b1111111111111111); // すべてのピンを 1 に設定
```

##### `read()`

一時的にすべてのピンのモードを入力に設定し、一度にすべてのピンを読み取り、その値を返します。

```js
let expander = new MCP23017(); // デフォルトは 0x20!
trace(`${expander.read()}\n`);
```

### ピンクラス

`Pin` クラスは、`MCP23008` または `MCP23017` インスタンスオブジェクト内の単一のピンを表します。このクラスはエクスポートされません。`Pin` インスタンスは `MCP23008` および `MCP23017` インスタンスによって自動的に作成されます。

```js
import Timer from "timer";
import { MCP23008 } from "MCP230XX";

export default function() {
	const leds = new MCP23008({
		inputs: 0b00000000
	});

  	// leds[0], leds[1], etc. are Pin instances
	leds[0].write(1);
	leds[1].write(0);
	leds[2].write(1);
	leds[3].write(0);
	leds[4].write(1);
	leds[5].write(0);
	leds[6].write(1);
	leds[7].write(0);
}
```

![](ESP8266-MCP23008-leds.png)

#### コンストラクタの説明

##### `Pin(dictionary)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するためのプロパティを持つオブジェクト。必要なパラメータは以下の[Dictionary](#Pin-dictionary)セクションで指定されています。


<a id="Pin-dictionary"></a>
#### Dictionary

| パラメータ | 型 | 説明
| --- | --- | :--- |
| `pin`  | `number` | GPIOピン番号 |
| `expander` | `expander` | この`Pin`インスタンスを作成した`Expander`のインスタンス |

#### プロパティ

すべてのプロパティは読み取り専用です。

| 名前 | 型 | 説明
| --- | --- | :--- |
| `pin` | `number` | GPIOピン番号 |
| `expander` | `expander` | この`Pin`が属する`Expander`のインスタンス |

#### メソッド

##### `mode(mode)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `mode` | `number` | 希望するモードを表す数値。入力、入力プルアップ、または出力である可能性があります。

ピンのモードを指定されたモードに設定します。

##### `read()`

ピンのモードを入力に設定し、ピンオブジェクトの値を読み取り、その値を返します。

##### `write(value)`

ピンのモードを出力に設定し、値をピンオブジェクトに書き込みます。

### マニフェストの例

```
{
  "include": [
    "$(MODDABLE)/examples/manifest_base.json",
    "$(MODULES)/drivers/mcp230xx/manifest.json"
  ],
  "modules": {
    "*": [
      "./main",
    ]
  },
  "platforms": {
    "esp": {
      "modules": {
        "*": "$(MODULES)/pins/i2c/esp/*",
      },
    },
    "esp32": {
      "modules": {
        "*": "$(MODULES)/pins/i2c/esp32/*",
      },
    },
  }
}
```
