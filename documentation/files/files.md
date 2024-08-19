# ファイル
Copyright 2017-2024 Moddable Tech, Inc.<BR>
改訂： 2024年8月9日

## 目次

* [ファイルシステム](#filesystems)
	* [ファイル](#file)
	* [ディレクトリ](#directory)
	* [ファイルイテレータ](#file-iterator)
	* [ファイルシステム](#file-system)
	* [ホストファイルシステムの設定](#platforms)
		* [SPIFFS](#spiffs)
		* [FAT32](#fat32)
		* [littlefs](#littlefs)
* [Zip](#zip)
* [Resource](#resource)
* [Preference](#preference)
* [Flash](#flash)

<a id="filesystems"></a>
## ファイルシステム

ファイルモジュールには、ファイルおよび（サポートされている場合は）ディレクトリにアクセスするために使用されるいくつかのクラスが含まれています。

### ファイルパス

デフォルトのファイルシステムのルートパスはホストによって異なります。さまざまなデバイスで動作するスクリプトを簡単に作成できるように、Moddable SDKにはデフォルトのファイルシステムのルートパスが `config/mc` モジュールに含まれています。

```js
import config from "mc/config";

File.delete(config.file.root + "test.txt");
```

原則として、スクリプトは常にこのルートでフルパスをプレフィックスする必要があります。

スラッシュ文字 (`/`) は、ネイティブで異なるパスの区切り文字を使用するホストでも、常にパスの区切り文字として使用されます。

以下に説明する `System.config()` 関数は、`maxPathLength` プロパティを通じてサポートされる最長パスの長さを提供します。

<a id="file"></a>
### class File

- **ソースコード:** [file](../../modules/files/file)
- **関連する例:** [files](../../examples/files/files/)

`File` クラスはファイルへのアクセスを提供します。
エラーが発生した場合、クラスのメソッドはエラーメッセージを引数として渡す `UnknownError` 例外を発生させます。

```js
import {File} from "file";
```

#### `constructor(path [, write])`

`File` コンストラクタは、読み取りまたは書き込み用にファイルを開きます。オプションのwrite引数はモードを選択します。writeのデフォルト値は `false` です。開かれると、ファイル位置は0になります。

ファイルが存在しない場合、読み取りモードで開くと例外がスローされます。書き込みモードで開くと、ファイルが存在しない場合は新しいファイルが作成されます。

```js
let file = new File(config.file.root + "preferences.json");
```

***

#### `read(type [, count])`

`read` 関数は現在の位置からデータを読み取ります。データは `type` 引数の値に基づいて `String` または `ArrayBuffer` に読み込まれます。`count` 引数は読み取るバイト数です。`count` のデフォルト値は、現在の `position` からファイルの `length` までのバイト数です。

```js
let file = new File(config.file.root + "preferences.json");
let preferences = JSON.parse(file.read(String));
file.close();
```
***

#### `write(value [, ...values])`

`write` 関数は、現在の `position` から始めて、1つ以上の値をファイルに書き込みます。値は `String` または `ArrayBuffer` のいずれかです。

```js
File.delete(config.file.root + "preferences.json");
let file = new File(config.file.root  + "preferences.json", true);
file.write(JSON.stringify(preferences));
file.close();
```

***

#### `length` プロパティ

`length` プロパティは、ファイル内のバイト数を示す数値です。これは読み取り専用です。

***

#### `position` プロパティ

`position` プロパティは、次の読み取りまたは書き込み操作のためのファイル内のバイトオフセットを示す数値です。

***

#### `static delete(path)`

指定されたパスのファイルを削除する静的な `delete` 関数です。

```js
File.delete(config.file.root + "test.txt");
```

***

#### `static exists(path)`

指定されたパスにファイルが存在するかどうかを示すブール値を返す静的な `exists` 関数です。

```js
let exists = File.exists(config.file.root + "test.txt");
```

***

#### `static rename(from, to)`

`from` 引数で指定されたファイルを `to` 引数で指定された名前に変更する静的な `rename` 関数です。

```js
File.rename(config.file.root + "test.txt", "betterName.txt");
```

`to` 引数は、上記の例のようにファイル名でも、以下の例のように完全なファイルパスでもかまいません。ホストファイルシステムがディレクトリ間でファイルを移動するために `rename` をサポートしている場合、完全なファイルパス形式が便利です。

```js
File.rename(config.file.root + "test.txt", config.file.root + "better/name.txt");
```

***

#### 例: ファイルサイズを取得する

この例では、ファイルを読み取り専用モードで開き、ファイルの長さを取得します。ファイルが存在しない場合、ファイルは作成されず、例外がスローされます。

```js
let file = new File(config.file.root + "test.txt");
trace(`File length ${file.length}\n`);
file.close();
```

***

#### 例: ファイルを文字列として読み込む

この例では、ファイルの全内容を `String` に取得します。文字列を保存するためのメモリが不足している場合や、ファイルが存在しない場合は、例外がスローされます。

```js
let file = new File(config.file.root + "test.txt");
trace(file.read(String));
file.close();
```

***

#### 例: ファイルをArrayBuffersに読み込む

この例では、ファイルを1つ以上の `ArrayBuffer` オブジェクトに読み込みます。ファイルサイズが1024の整数倍でない場合、最後の `ArrayBuffer` は1024より小さくなります。

```js
let file = new File(config.file.root + "test.txt");
while (file.position < file.length) {
	let buffer = file.read(ArrayBuffer, 1024);
}
file.close();
```

***

#### 例: 文字列をファイルに書き込む

この例では、ファイルを削除し、書き込み用に開き（これにより新しい空のファイルが作成されます）、次に2つの `String` 値をファイルに書き込みます。スクリプトは次に読み書き位置をファイルの先頭に移動し、ファイルの全内容を1つの `String` に読み込み、それをコンソールにトレースします。

```js
File.delete(config.file.root + "test.txt");

let file = new File(config.file.root + "test.txt", true);
file.write("This is a test.\n");
file.write("This is the end of the test.\n");

file.position = 0;
let content = file.read(String);
trace(content);

file.close();
```

***

<a id="directory"></a>
### class Directory

- **ソースコード:** [file](../../modules/files/file)

`Directory` クラスはディレクトリを作成および削除します。ディレクトリ内のファイルやディレクトリを一覧表示するには、`Iterator` クラスを使用します。

```js
import {Directory} from "file";
```

> **注意**: SPIFFSファイルシステムはフラットファイルシステムであるため、ディレクトリを作成または削除することはできません。

#### `static create(path)`

`create` 関数は指定されたパスにディレクトリを作成します。`path` 内のすべての親ディレクトリは既に存在している必要があります。`create` は親ディレクトリを自動的に作成しません。

```js
Directory.create(config.file.root + "tmp");
```

***

#### `static delete(path)`

`delete` 関数は指定されたパスのディレクトリを削除します。ほとんどのファイルシステムでは、ディレクトリを削除するためには空である必要があり、空でない場合 `delete` は例外をスローします。

```js
Directory.delete(config.file.root + "tmp");
```

***

<a id="file-iterator"></a>
### class File Iterator

- **ソースコード:** [file](../../modules/files/file)
- **関連する例:** [files](../../examples/files/files/)

File `Iterator` クラスはディレクトリ内のファイルとサブディレクトリを列挙します。

```js
import {Iterator} from "file";
```

> **注意**: SPIFFSファイルシステムはフラットなファイルシステムであるため、それを使用するデバイスではサブディレクトリは返されません。

#### `constructor(path)`

コンストラクタは、反復するディレクトリのパスを唯一の引数として受け取ります。

```js
let iterator = new Iterator(config.file.root);
```

イテレータインスタンスはJavaScriptの[反復可能オブジェクト](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/for...of)であるため、`for...of`ループで使用できます。以下に例を示します。

***

#### `next()`

`next` 関数は繰り返し呼び出され、そのたびに1つのファイルに関する情報を取得します。すべてのファイルが返された後、`next` 関数は `undefined` を返します。各ファイルとサブディレクトリについて、nextはオブジェクトを返します。オブジェクトには常にファイル名を持つ `name` プロパティが含まれます。オブジェクトに `length` プロパティが含まれている場合、それはファイルを参照し、`length` プロパティはファイルのサイズをバイト単位で示します。`length` プロパティがない場合、それはディレクトリを参照します。

```js
let item = iterator.next();
```

***

#### 例: ディレクトリの内容をリストする

この例では、ディレクトリ内のすべてのファイルとサブディレクトリをリストします。

```js
let iterator = new Iterator(config.file.root);
let item;
while (item = iterator.next()) {
	if (undefined === item.length)
		trace(`Directory: ${item.name}\n`);
	else
		trace(`File: ${item.name}, ${item.length} bytes\n`);
}
```

イテレータの `next` 関数はオブジェクトを返します。オブジェクトに `length` プロパティがある場合、それはファイルです。`length` プロパティがない場合、それはディレクトリです。

これは `for...of` ループを使用した同じ例のバリエーションです。

```js
for (const item of (new Iterator(config.file.root))) {
	if (undefined === item.length)
		trace(`Directory: ${item.name}\n`);
	else
		trace(`File: ${item.name}, ${item.length} bytes\n`);
}
```

***

<a id="file-system"></a>
### クラス File System

- **ソースコード:** [file](../../modules/files/file)

File `System` クラスはファイルシステムに関する情報を提供します。

```js
import {System} from "file";
```

#### `static config()`

`config` 関数はファイルシステムに関する情報を持つ辞書を返します。現在、この辞書には `maxPathLength` という単一のプロパティがあり、これはバイト単位で最長のファイルパスの長さを示します。

```js
let maxPathLength = System.config().maxPathLength;
```

***

#### `static info()`

`info` 関数は、利用可能な場合、ファイルシステムの空き容量と使用容量に関する情報を持つ辞書を返します。辞書の `used` プロパティは使用中のバイト数を示し、`total` プロパティはファイルシステムの最大容量をバイト単位で示します。

```js
let info = System.info();
let percentFree = 1 - (info.used / info.total);
```

`info`によって返されるオブジェクトのプロパティは、ホストプラットフォームの機能に基づいて異なります。そのため、`total`および`used`プロパティが利用できない場合や、他のプロパティが存在する場合があります。

***

<a id="platforms"></a>
### ホストファイルシステムの構成

このセクションでは、いくつかの組み込みホストでファイルシステムがどのように実装されているかについて説明します。この情報は、デフォルトのファイルシステム構成が特定のプロジェクトのニーズを満たさない場合に役立ちます。

<a id="spiffs"></a>
#### SPIFFS -- ESP8266 & ESP32

ESP8266および（デフォルトで）ESP32では、ファイルモジュールは[SPIFFS](https://github.com/pellepl/spiffs)ファイルシステムを使用して実装されています。

SPIFFSはフラットファイルシステムであり、ディレクトリがなく、すべてのファイルがルートにあります。

SPIFFSファイルシステムは追加のメモリを必要とします。ビルドにSPIFFSを含めると、ESP8266で約500バイトのRAMが増加します。SPIFFSファイルシステムを使用するには、さらに約3 KBのRAMが必要です。メモリの影響を最小限に抑えるために、`File`クラスは必要な場合にのみSPIFFSファイルシステムをインスタンス化します。つまり、ファイルが開かれているときとファイルが削除されるときです。SPIFFSファイルシステムは使用されていないときに自動的に閉じられます。

SPIFFSファイルシステムが初期化されていない場合、初めて使用するときにフォーマットされます。初期化には最大で1分かかります。

ESP32では、SPIFFSパーティションサイズは`data`タイプおよび`spiffs`サブタイプのパーティションを持つパーティションファイルで指定されます。[デフォルトのpartitions.csv](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32/partitions.csv)は、この目的のために64 KBのパーティションを割り当てます。カスタムパーティションファイルは、プロジェクトマニフェストの`build`セクションで`PARTITIONS_FILE`変数を設定することで指定できます。

```JSON
"build": {
	"PARTITIONS_FILE": "./customPartitions.csv"
}
```

ESP32では、SPIFFSファイルシステムは指定されたパスにマウントされ、作成されたすべてのファイル/ディレクトリはそのルートパス内でアクセスする必要があります。デフォルトのルートパスは`/mod`ですが、マニフェストの`root`定義で変更できます。

```JSON
"defines": {
	"file":{
		"root": "/myroot/"
	}
}
```

<a id="fat32"></a>
#### FAT32 -- ESP32

`File`クラスは、ESP32用のオプションのFAT32ファイルシステムを実装しています。SPIFFSとは異なり、FAT32ファイルシステムはフラットではありません。ディレクトリ構造と長いファイル名（最大255文字）を持っています。

FAT32ファイルシステムが初期化されていない場合、最初に使用されたときにフォーマットされます。SPIFFSと同様に、`File`クラスは必要に応じてFAT32ファイルシステムをインスタンス化し、使用されていないときには自動的に閉じられます。

FAT32ファイルシステムを有効にするには、`fat32` [マニフェスト定義](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/tools/defines.md)を`1`に設定します：

```JSON
"defines": {
	"file":{
		"fat32": 1
	}
}
```

ESP32用のデフォルトのModdable SDKビルドで使用されるストレージパーティションは、FAT32用のパーティションを予約していません。したがって、FAT32を使用するプロジェクトでは異なるパーティションファイルを使用する必要があります。そのためには、プロジェクトマニフェストの`build`セクションで`PARTITIONS_FILE`変数を設定します：

```JSON
"build": {
	"PARTITIONS_FILE": "./customPartitions.csv"
}
```

FAT32パーティションのタイプは`data`で、サブタイプは`fat`です。FAT32の実装には約576 KBの最小パーティションサイズが必要です。パーティションのフォーマットはESP-IDFによって定義されます。以下の例は、最小サイズのFAT32パーティションを含むパーティションファイルを示しています：

```CSV
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x006000,
phy_init, data, phy,     0xf000,  0x001000,
factory,  app,  factory, 0x10000, 0x300000,
xs,       0x40, 1,       0x310000, 0x040000,
settings, data, 1,       0x350000, 0x010000,
storage,  data, fat,     0x360000, 0x090000,
```
FAT32パーティションのデフォルト名は`storage`です。別の名前を使用するには、マニフェストで`partition`定義を設定します。

```JSON
"defines": {
	"file":{
		"partition": "#userdata"
	}
}
```

デフォルトでは、FAT32ファイルシステムは`/mod`にマウントされます。デフォルトのルートを変更するには、マニフェストで`root`定義を設定します。

```JSON
"defines": {
	"file":{
		"root": "/myroot/"
	}
}
```

<a id="littlefs"></a>
#### littlefs
[littlefs](https://github.com/littlefs-project/littlefs)ファイルシステムは「マイクロコントローラ向けに設計された小さなフェイルセーフファイルシステム」です。これは、最小限のメモリ（1 KB未満）を使用して、小さなコードフットプリント（約60 KB）で高信頼性の階層型ファイルシステムを提供し、高度な構成可能性を備えています。littlefsは長いファイル名（最大255文字）もサポートしており、新しいパーティションを非常に迅速にフォーマットします。

Moddable SDKは、上記のAPIを使用してlittlefsをサポートしています。littlefsを使用するには、そのマニフェストを含めます。

```json
"include": {
	"$MODDABLE/modules/files/file/manifest_littlefs.json"
}
```

> **注**: プロジェクトは littlefs マニフェストまたはデフォルトのファイルマニフェスト (`$MODDABLE/modules/files/file/manifest.json`) を使用することができます。両方を同じプロジェクトに含めることは現在できません。

littlefsのバックストアはホストプラットフォームによって異なります：

- **ESP32** - littlefsはファイルシステムを保持するために "storage" パーティションを使用します。
- **ESP8266** - ファイルシステムはフラッシュの上位3 MBに保存されます (SPIFFSと同じ領域）。
- **nRF52** - littlefsはファームウェアイメージとインストールされたモジュールの後の空き領域を使用します。デフォルトのサイズは64 KBで、マニフェストの `defines` に `MODDEF_FILE_LFS_PARTITION_SIZE` を設定することで上書きできます。十分な空き領域がない場合、ファイルシステムにアクセスすると例外がスローされます。
- **その他** - littlefsはファイルシステムを保持するために静的メモリバッファを使用します。デフォルトのサイズは64 KBで、マニフェストの `MODDEF_FILE_LFS_PARTITION_SIZE` で上書きできます。このRAMディスクモードにより、シミュレータでlittlefsを使用することができます。

```json
	"defines": {
		"file": {
			"lfs": {
				"partition_size": 131072
			}
		}
	},
```

littlefsの実装は、FreeRTOS（ESP32およびnRF52）を実行しているデバイス上でスレッドセーフであり、littlefsをWorkersと一緒に使用することができます。ESP8266では単一プロセスとして実行されるため、スレッドセーフは関係ありません。スレッドセーフのサポートは、他のランタイム環境にも拡張される可能性があります。

littlefsの実装は、パフォーマンスとメモリ使用量のトレードオフを構成することができます。Moddable SDKのデフォルト構成は、可能な限り最小のメモリを使用します。ファイルシステムを軽量に使用するプロジェクトにとって、これは十分なパフォーマンスを提供します。パフォーマンスを向上させるために、プロジェクトのマニフェストで構成を変更することができます。`read_size`、`prog_size`、`lookahead_size`、および`block_cycles`の値は、[`lfs.h`](https://github.com/littlefs-project/littlefs/blob/40dba4a556e0d81dfbe64301a6aa4e18ceca896c/lfs.h#L194-L230)に記載されています。実験により、4つの`*_size`設定を16バイトから512バイトに増やすことで、2 KBのRAMを犠牲にして大幅なパフォーマンス向上が得られることが示されています。

```json
	"defines": {
		"file": {
			"lfs": {
				"read_size": 16,
				"prog_size": 16,
				"cache_size": 16,
				"lookahead_size": 16,
				"block_cycles": 500
			}
		}
	},
```

使用されていないとき（すべてのファイルとファイルイテレータが閉じられているとき）、littlefsの実装はファイルシステムをアンマウントします。これにより、すべてのメモリが解放されます。これは、SPIFFSやFAT32によって実装されているのと同じ動作です。

<a id="zip"></a>
## class ZIP

- **ソースコード:** [zip](../../modules/files/zip)
- **関連する例:** [zip](../../examples/files/zip/), [httpzip](../../examples/network/http/httpzip)

`ZIP` クラスは、メモリに格納されたZIPファイルの内容に対する読み取り専用のファイルシステムアクセスを実装します。通常、これらはフラッシュメモリに格納されます。ZIPファイルは、読み取り専用のファイルシステムをプロジェクトに埋め込む便利な方法です。

`ZIP` の実装は、ZIPファイルに含まれるファイルへのアクセスを提供します。通常、ほとんどのZIPファイルのファイルは圧縮されています。`ZIP` クラスは読み取り時にデータを解凍しません。ZIPはファイルを圧縮する必要がないため、非圧縮の内容でZIPファイルを作成するオプションもあります。十分なメモリを持つデバイスでは、読み取り後にzlibの `inflate` モジュールを使用してデータを解凍することで、圧縮された内容のZIPファイルを使用することができます。

ZIPファイルを圧縮なしで作成する方法の1つに、[`zip`](https://linux.die.net/man/1/zip) コマンドラインツールがあります。圧縮レベルをゼロに指定すると、圧縮なしのZIPファイルが作成されます。以下のコマンドラインは、ディレクトリ `test` の内容を圧縮なしで `test.zip` という名前のZIPファイルに作成します。

	zip -0r test.zip test

内容を圧縮するには、異なる圧縮レベルを使用します。最高の圧縮レベルは `9` です：

	zip -9r test.zip test

**注意**: `zip` コマンドラインツールは、ZIPファイルのルートに「test」という名前のディレクトリを作成します。例えば、「test/example.txt」にあるファイルは、ZIPファイル内では「test/example.txt」としてアクセスされ、「example.txt」としてはアクセスされません。

### `constructor(buffer)`

`ZIP` コンストラクタは、バッファの内容を読み取り専用ファイルシステムとしてアクセスするための `ZIP` オブジェクトをインスタンス化します。バッファは `ArrayBuffer` またはホストバッファのいずれかである必要があります。

コンストラクタは、バッファがZIPアーカイブを含んでいることを検証し、含んでいない場合は例外をスローします。

ZIPアーカイブはメモリに格納されます。ROMの場合、ホストバッファを使用してアクセスされます。ホストバッファは `ArrayBuffer` の一種です。ホストプラットフォームソフトウェアは、プラットフォーム固有のメカニズムを通じてホストバッファインスタンスを提供します。この例では、ホストバッファを作成するために `Resource` コンストラクタを使用します。

```js
let buffer = new Resource("test.zip");
let archive = new ZIP(buffer);
```

***

### `file(path)`

`file` 関数は、ZIPアーカイブ内の指定されたパスの内容にアクセスするためのオブジェクトをインスタンス化します。返されるインスタンスは `File` クラスと同じAPIを実装しています。

```js
let file = archive.file("small.txt");
```

***

### `iterate(path)`

`iterate` 関数は、ZIPアーカイブ内の指定されたディレクトリパスの内容にアクセスするためのオブジェクトをインスタンス化します。返されるインスタンスはIteratorクラスと同じAPIを実装しています。ディレクトリパスはスラッシュ（"`/`"）文字で終わり、ルートパスを除いてスラッシュで始まりません。

```js
let root = archive.iterate("/");
```

***

### `map(path)`

`map` 関数は、指定されたパスのファイルのバイトを参照するホストバッファを返します。

***

### `method`

読み取り専用の `method` プロパティは、ZIPファイル内でファイルがどのように圧縮されているかを示す整数を返します。値はZIP仕様から取られています。例えば、値8は `deflate` 圧縮が使用されたことを示します。

***

### `crc`

読み取り専用の `crc` プロパティは、ZIPファイルに格納されているファイルのCRC値を含む整数を返します。このプロパティはキャッシュに便利です。

***

### 例: ZIPアーカイブからファイルを読み取る

`ZIP` インスタンスの `file` 関数は、ファイルにアクセスするために使用されるインスタンスを提供します。異なる方法でインスタンス化されますが、ZIPファイルインスタンスは `File` クラスと同じAPIを共有します。

```js
let file = archive.file("small.txt");
trace(`File size: ${file.length} bytes\n`);
let string = file.read(String);
trace(string);
file.close();
```

***

### 例: ZIPアーカイブのディレクトリの内容をリストする

次の例では、アーカイブのルートにあるファイルとディレクトリを反復処理します。多くの場合、ルートには単一のディレクトリしか含まれていません。

```js
let root = archive.iterate("/");
let item;
while (item = root.next()) {
    if (undefined == item.length)
        trace(`Directory: ${item.name}\n`);
    else
        trace(`File: ${item.name}, ${item.length} bytes\n`);
}
```

ZIPイテレータはディレクトリパスがスラッシュ（"`/`"）で終わることを期待します。ルートにある「test」という名前のディレクトリの内容を反復処理するには、次のコードを使用します。

```js
let iterator = archive.iterate("test/");
```

***

<a id="resource"></a>
## class Resource

- **ソースコード:** [resource](../../modules/files/resource)
- **関連する例:** [zip](../../examples/files/zip/)、多くの [Commodetto の例](../../examples/commodetto) には [sprite](../../examples/commodetto/sprite) や [text](../../examples/commodetto/text) が含まれます

`Resource` クラスは、アプリケーションのリソースマップからアセットにアクセスするためのものです。

```js
import Resource from "Resource";
```

### `constructor(path)`

`Resource` コンストラクタは、リソースパスを引数として取り、リソースデータを含むホストバッファを返します。

```js
let resource = new Resource("logo.bmp");
trace(`resource size is ${resource.byteLength}\n`);
```

***

### `static exists(path)`
静的な `exists` 関数は、指定されたパスにリソースが存在するかどうかを示すブール値を返します。

```js
let path = "test.zip";
if (Resource.exists(path))
	trace(`File ${path} exists\n`);
```

***

### `slice(begin[[, end], copy])`

`slice` 関数は、リソースの一部を `ArrayBuffer` として返します。`end` のデフォルト値はリソースのサイズです。

```js
let resource = new Resource("table.dat");
let buffer1 = resource.slice(5);		// オフセット5から始まるバッファを取得
let buffer2 = resource.slice(0, 10);	// 最初の10バイトのバッファを取得
```

オプションの `copy` 引数のデフォルト値は `true` です。これを `false` に設定すると、返される値は元のリソースデータを参照する読み取り専用の `HostBuffer` になります。このオプションは、リソースデータの一部をRAMにコピーせずに参照を作成するのに便利です。

***

<a id="preference"></a>
## class Preference

- **ソースコード:** [preference](../../modules/files/preference/)
- **関連する例:** [preference](../../examples/files/preference/), [preferences](../../examples/piu/preferences/)

`Preference` クラスは永続的な設定の保存を提供します。設定は、アプリケーションの実行間で持続する必要がある少量のデータを保存するのに適しています。

```js
import Preference from "preference";
```

設定はドメインごとにグループ化されます。ドメインには1つ以上のキーが含まれます。各ドメイン/キーのペアは、`Boolean`、整数（例：小数部分のない `Number`）、`String` または `ArrayBuffer` のいずれかの単一の値を保持します。

```js
const domain = "wifi";
let ssid = Preference.get(domain, "ssid");
let password = Preference.get(domain, "psk");
```

キー/ドメイン名および設定値の長さの制限は、ターゲットプラットフォームによって異なります。

 - ESP8266では、キー/ドメイン名は32文字に制限され、値は63バイトに制限されます。
 - ESP32では、`Preference` クラスはESP-IDFの [NVSライブラリ](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html) によってサポートされており、キー/ドメイン名は15文字、値は4000バイトに制限されます。

組み込みデバイスでは、設定のためのストレージスペースは限られています。その容量はデバイスによりますが、4 KB程度しかない場合もあります。そのため、アプリケーションは設定をできるだけ小さく保つように注意する必要があります。

> **注意**: 組み込みデバイスでは、設定は消去サイクルが限られているSPIフラッシュに保存されます。アプリケーションは書き込み操作（設定と削除）の回数を最小限に抑えるべきです。実際には、これは大きな問題ではありません。しかし、例えば設定を1分ごとに更新するアプリケーションは、最終的にSPIフラッシュの設定保存領域の消去サイクルを超える可能性があります。

### `static set(domain, key, value)`

静的な `set` 関数は設定値を設定します。

```js
Preference.set("wifi", "ssid", "linksys");
Preference.set("wifi", "password", "admin");
Preference.set("wifi", "channel", 6);
```

***

### `static get(domain, key)`

静的な `get` 関数は設定値を読み取ります。設定が存在しない場合、`get` は `undefined` を返します。

```js
let value = Preference.get("settings", "timezone");
if (value !== undefined)
	trace(`timezone ${value}\n`);
```

***

### `static delete(domain, key)`

静的な `delete` 関数は、設定を削除します。設定が存在しない場合、エラーはスローされません。

```js
Preference.delete("wifi", "password");
```

***

### `static keys(domain)`

指定されたドメインのすべてのキーの配列を返します。

```js
let wifiKeys = Preference.keys("wifi");
for (let key of wifiKeys)
	trace(`${key}: ${Preference.get("wifi", key)}\n`);
```

***

<a id="flash"></a>
## class Flash

- **ソースコード:** [file](../../modules/files/flash)

`Flash` クラスはフラッシュメモリパーティションへのアクセスを提供します。

### `constructor(name)`
Flashコンストラクタは、`name` 引数で示されるパーティションにバインドされたインスタンスを作成します。利用可能なパーティションの名前は、ホストに依存します。

***
### `close()`

Flashインスタンスが保持するすべてのリソースを解放します。`close()` を呼び出した後にインスタンスのメソッドを呼び出すと、例外がスローされます。

***
### `erase(block)`

フラッシュパーティションの1ブロックを消去します。`block` 引数はパーティション内のブロックのインデックスで、ブロック0から始まります。ブロック番号からインデックスに変換するには、インスタンスの `blockSize` を掛けます。

***
### `read(offset, byteLength)`

パーティション内のバイト `offset` から `byteLength` バイトを `ArrayBuffer` に読み込みます。

***
### `write(offset, byteLength, buffer)`

パーティション内のバイト `offset` から `buffer` の最初の `byteLength` バイトを書き込みます。`buffer` 引数は任意のバイトバッファで構いません。

***
### `map()`
フラッシュパーティションから直接読み取るためにビューにラップできる読み取り専用のホストバッファを返します。ホストが `map()` をサポートしていない場合、この関数は例外をスローします。

***

### `byteLength`
読み取り専用の `byteLength` プロパティは、フラッシュパーティションのサイズを提供します。

***
### `blockSize`
読み取り専用の `blockSize` プロパティは、フラッシュパーティション内のブロック（セクターとも呼ばれる）のサイズを提供します。この値を使用することが、一般的なフラッシュブロックサイズ `4096` をハードコーディングする代わりに推奨されます。

***

**注意**。`readString()` APIは実験的なものであり、本番環境で使用すべきではありません。入力が有効なUTF-8文字列であると仮定しているため、潜在的に安全ではありません。
