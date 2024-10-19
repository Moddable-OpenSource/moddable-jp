# Manifest
Copyright 2017-2024 Moddable Tech, Inc.<BR>
改訂： 2024年9月16日

マニフェストは、Moddableアプリを構築するために必要なモジュールとリソースを記述するJSONファイルです。このドキュメントでは、JSONオブジェクトのプロパティと、マニフェストがModdable SDKビルドツールによってどのように処理されるかについて説明します。

## 目次

* [サンプル](#example)
* [プロパティ](#properties)
	* [`build`](#build)
	* [`include`](#include)
		* [gitリポジトリの含め方](#include-git)
	* [`creation`](#creation)
	* [`defines`](#defines)
	* [`config`](#config)
	* [`strip`](#strip)
	* [`modules`](#modules)
	* [`preload`](#preload)
	* [`resources`](#resources)
	* [`data`](#data)
	* [`platforms`](#platforms)
		* [`サブプラットフォーム`](#subplatforms)
	* [`bundle`](#bundle)
* [マニフェストの処理方法](#process)

<a id="example"></a>
## 例

簡単な例として、[balls example app](../../examples/piu/balls)のマニフェストを考えてみましょう。これは非常に短いものです：

```json
{
	"include": [
		"$(MODDABLE)/examples/manifest_base.json",
		"$(MODDABLE)/examples/manifest_piu.json"
	],
	"modules": {
		"*": "./main"
	},
	"resources":{
		"*": [
			"./balls"
		]
	}
}
```

`include` 配列は、含めるべき他のマニフェストをリストアップします。これは、各アプリケーションのマニフェストで個々のプロパティやモジュールを指定する必要をなくすため、多くのアプリケーションで同じプロパティやモジュールを使用する便利な方法です。例えば、`manifest_base.json`はすべてのサンプルアプリケーションに含まれています。これにより、resource、instrumentation、time、timerモジュールが各アプリケーションのビルドに含まれるようになります。

`modules` と `resources` オブジェクトは、ビルドに含めるべきballsアプリ固有のモジュールとリソースをリストアップします。ここで `modules` オブジェクトは、ballsアプリディレクトリにある `main.js` ファイルを含めるべきであると指定し、`resources` オブジェクトは、ballsアプリディレクトリにある `balls.png` 画像を含めるべきであると指定します。

<a id="properties"></a>
## プロパティ

<a id="build"></a>
### `build`

`build` オブジェクトは、マニフェストの他のパスで使用される環境変数を定義します。マニフェストは `MODDABLE` のようなシェル環境変数にアクセスできることに注意してください。

```json
"build": {
	"BUILD": "$(MODDABLE)/build",
	"MODULES": "$(MODDABLE)/modules",
	"COMMODETTO": "$(MODULES)/commodetto",
	"PIU": "$(MODULES)/piu"
}
```

アプリケーションをビルドするとき、デフォルトの出力ディレクトリ名はマニフェストを含むディレクトリの名前から取られます。`build` オブジェクトで `NAME` 環境変数を指定することにより、異なる出力ディレクトリ名を指定することができます。

```json
"build": {
	"NAME": "balls"
}
```

#### ESP32固有の環境変数

`esp32` プラットフォームオブジェクトは、ESP32デバイスのModdable SDKビルドをカスタマイズするためにアプリケーションが使用できるいくつかのオプショナルな環境変数をサポートしています：

| 変数 | 説明 |
| --- | :--- |
| `ESP32_SUBCLASS` | `esp32`以外のデバイスの場合、`esp32s2`、`esp32s3`、`esp32c3`、`esp32c6`、または`esp32h2`を設定
| `SDKCONFIGPATH` | カスタム [sdkconfig defaults](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#custom-sdkconfig-defaults) エントリを含むディレクトリへのパス名。
| `PARTITIONS_FILE` | CSV形式の[パーティションテーブル](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)へのパス名。
| `BOOTLOADERPATH` | カスタム [ESP-IDF ブートローダーコンポーネント](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/bootloader.html#custom-bootloader) を含むディレクトリへのパス名。
| `C_FLAGS_SUBPLATFORM` | Moddable SDKソースをコンパイルする際に使用するCコンパイラフラグ。
| `USE_USB` | プログラミングおよびデバッグ用にUSBポートを使用するようにデバイスを設定。

> 注意: このドキュメントはネイティブコードESP32およびESP-IDFのビルドの詳細をカバーしていません。追加情報については、[ESP-IDFドキュメント](https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/get-started/index.html)を参照してください。

#### `ESP32_SUBCLASS`

ビルドの対象となるESP32サブクラスは、`ESP32_SUBCLASS` プロパティを使用して指定されます。

有効な `ESP32_SUBCLASS`:
|   |
| :--- |
| `esp32s2` |
| `esp32s3` |
| `esp32c3` |
| `esp32c6` |
| `esp32h2` |

[modClock](https://github.com/Moddable-OpenSource/moddable/tree/public/contributed/modClock) サンプルアプリは、`SDKCONFIGPATH` および `PARTITIONS_FILE` 環境変数を活用しています：

```json
"build": {
	"SDKCONFIGPATH": "$(MODDABLE)/contributed/modClock/sdkconfig",
	"PARTITIONS_FILE": "$(MODDABLE)/contributed/modClock/sdkconfig/partitions.csv"
}
```

この例では、modClockの [partitions.csv](https://github.com/Moddable-OpenSource/moddable/blob/public/contributed/modClock/sdkconfig/partitions.csv) ファイルがビルド時に基本のModdable SDK [partitions.csv](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32/partitions.csv) ファイルを完全に置き換え、OTAアップデート用の追加パーティションを提供します。`sdkconfig` ディレクトリには、基本のModdable SDK [sdkconfig.defaults](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32/sdkconfig.defaults) エントリを上書きおよび補完するsdkconfigファイルが含まれています。次のセクションでは、Moddable ESP32ビルドがsdkconfigファイルをどのように処理するかを説明します。

`C_FLAGS_SUBPLATFORM` 環境変数は、サブプラットフォームのマニフェストで使用するためのものです（他の場所では使用しないでください）。これにより、サブプラットフォーム固有のコンパイラ設定を可能にします。例えば、初代ESP32シリコンを使用し、外部PSRAMが搭載されているサブプラットフォームでは、以下の設定を有効にして、シリコンのバグを回避するコードをコンパイラが生成するようにします：

```json
"build": {
	"C_FLAGS_SUBPLATFORM": "-mfix-esp32-psram-cache-issue -mfix-esp32-psram-cache-strategy=memw"
}
```

#### partitions.csv の処理方法 {/*examples*/}
[ESP-IDF パーティションテーブル](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)には、特定の機能をサポートするために特定のパーティションが含まれている必要があります：

- [Mods](../xs/mods.md) はmodのアーカイブを保存するパーティションが必要です
- ファイルはファイルシステムを保存するパーティションが必要です
- オーバー・ザ・エア（OTA）アップデートは、2つのOTAアプリパーティションとOTAデータパーティションが必要です

`mcconfig` ツールは、これらの機能をサポートするためにパーティションマップを自動的に変更することができます。これにより、プロジェクト用にターゲットパーティションテーブルを手動で作成する必要がなくなり、開発が簡素化されます。プロジェクトで使用される機能のみに対してパーティションを作成することで、フラッシュスペースを最適に利用します。

`mcconfig` ツールは、プロジェクトで使用される機能に基づいて、ファクトリーアプリパーティションを分割して新しいパーティションを作成します。Moddable SDKデバイスのデフォルトのパーティションテーブルはすべて、単一のファクトリーアプリパーティションを持っているため、この `mcconfig` の機能をサポートしています。

使用する機能を決定するために、`mcconfig` は以下のマニフェスト [define](#defines) をチェックします。これらの定義はそれが必要なマニフェストで設定されるため、通常はプロジェクトマニフェストで設定する必要はありません。

- Mods – `XS_MODS` がゼロ以外の値に設定されている場合、modsが使用中と見なされます

```json
"defines": {
	"XS_MODS": 1
}
```
- Files - `file partition` がパーティションの名前に設定されている場合、ファイルが使用中と見なされます

```json
"defines": {
	"file": {
		"partition": "#storage"
	}
}
```
- OTA – `ota autospilt` が設定されている場合、OTAが使用中と見なされます。

```json
"defines": {
	"ota": {
		"autosplit": 1
	}
}
```
プロジェクトのpartitions.csvファイルにこれらの機能のパーティションが含まれている場合、`mcconfig` は対応するパーティションを自動的に作成しません。例えば、modsパーティションがpartitions.csvファイルで定義されている場合、`mcconfig` はファクトリーアプリパーティションからそれを作成しません。

作成されたパーティションは以下のサイズを持っています。将来的にこれらのサイズを設定するオプションが実装される可能性があります。

- Mods - 256 KB
- Storage - 64 KB
- OTA - 8 KBはESP-IDFに必要なOTAデータパーティション用に予約されています。ファクトリーアプリパーティションの他のパーティションに使用されていないスペースは、2つのOTAアプリパーティションに分割されます。

#### sdkconfigファイルの処理方法 {#how-sdkconfig-files-are-processed}

Moddable SDKのsdkconfigデフォルトファイルは、それぞれESP32とESP32-S2用に`$MODDABLE/build/devices/esp32/xsProj-esp32`および`$MODDABLE/build/devices/esp32/xsProj-esp32s2`ディレクトリに位置しています。`sdkconfig.defaults` ([ESP32](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32/sdkconfig.defaults)/[ESP32-S2](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32s2/sdkconfig.defaults)) ファイルは、すべてのESP32/ESP32-S2ビルドに使用される基本設定ファイルです。リリースおよびリリースinstrumentedビルドは、基本の`sdkconfig.defaults`ファイルの上に追加の設定オプションをマージします。これは`sdkconfig.defaults.release` ([ESP32](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32/sdkconfig.defaults.release)/[ESP32-S2](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32s2/sdkconfig.defaults.release)) および`sdkconfig.inst` ([ESP32](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32/sdkconfig.inst)/[ESP32-S2](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32s2/sdkconfig.inst)) ファイルから行われます。マージ時に、基本の`sdkconfig.defaults`ファイルに存在する設定オプションは置き換えられ、基本の`sdkconfig.defaults`ファイルに存在しないオプションは追加されます。マージ処理の順序は以下の通りです：

1. すべての基本的な `sdkconfig.defaults` オプションがビルドに適用されます。
2. リリースビルドでは、`sdkconfig.defaults.release` オプションがマージされます。
3. リリースinstrumentedビルドでは、`sdkconfig.inst` オプションがマージされます。

	アプリケーションが `SDKCONFIGPATH` マニフェスト環境変数を使用してオプショナルなsdkconfigファイルを指定する場合、マージ処理には以下が含まれます：

4. 提供されている場合、アプリケーションの `$(SDKCONFIGPATH)/sdkconfig.defaults` オプションがマージされます。
5. リリースビルドでは、提供されている場合、アプリケーションの `$(SDKCONFIGPATH)/sdkconfig.defaults.release` オプションがマージされます。
6. リリースinstrumentedビルドでは、提供されている場合、アプリケーションの `$(SDKCONFIGPATH)/sdkconfig.inst` オプションがマージされます。

### `USE_USB`

一部のESP32デバイスは、USB-to-serialチップを使用する代わりに直接USBをサポートしています。例えば、`esp32-s2`、`esp32-s3`、`esp32-c3` クラスのデバイスです。`esp32` はUSBをサポートしていません。

1. `esp32-s2` デバイスはTinyUSBをサポートします。`"USE_USB": "1"` を設定します。
2. `esp32-s3` デバイスはTinyUSBとJTAG-CDCの両方をサポートします。`"USE_USB"` を `1` に設定するとTinyUSB、`2` に設定するとJTAG-CDCになります。
3. `esp32c3` デバイスはJTAG-CDCをサポートします。`"USE_USB": "2"` を設定します。

***

<a id="include"></a>
### `include`

`include` 配列は、含めるべき他のマニフェストをリストアップします。共通のプロパティをすべてのマニフェストで繰り返さないように、他のマニフェストを含めることがよく便利です。例えば、[`bmp280` の例](../../examples/drivers/bmp280)では `manifest_base.json` とBMP280温度/気圧センサーのマニフェストが含まれています：

```json
"include": [
	"$(MODDABLE)/examples/manifest_base.json",
	"$(MODULES)/drivers/bmp280/manifest.json"
]
```

Moddable SDKの各サンプルアプリケーションは、[examples directory](../../examples/) のマニフェストの少なくとも1つを含んでいます。

- `manifest_base.json` は、私たちのサンプルアプリケーションのそれぞれによって含まれています。これにはresource、instrumentation、time、timerが含まれています。また、多くのアプリケーションで機能する `creation` オブジェクトも含まれています。[`creation` オブジェクト](#creation)については、このドキュメントの後ほど詳しく説明されています。

- `manifest_commodetto.json` は、[Commodettoグラフィックライブラリ](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/commodetto.md) を直接使用するアプリケーション用です。

- `manifest_net.json` はWi-Fiを使用するアプリケーション用です。Socket、Net、SNTP、Wi-Fiモジュールが含まれています。HTTPやMQTTなどの特定のネットワークプロトコルは含まれていません。

- `manifest_piu.json` は [Piu アプリケーションフレームワーク](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/piu/piu.md) を使用するアプリケーション用です。Piuを使用するために必要なすべてのモジュールが含まれています。スクリーンとタッチドライバは、Piuマニフェストをデバイスに依存しないようにするため、通常は対象デバイス自体のマニフェストによって提供されます。

いくつかのタッチ、ディスプレイ、センサー[ドライバー](../../modules/drivers)および一部の[ネットワークモジュール](../../modules/network)も、プロジェクトに簡単に組み込むためのマニフェストを持っています。

<a id="include-git"></a>
#### Git リポジトリの含め方
マニフェストは直接Gitリポジトリを含めることができます。リポジトリはビルドプロセスの一部としてクローンされ、プロジェクトの一時的なビルドファイルと共に保存されます。

> **注**: この機能は実験的です。Git リポジトリを含めるためにマニフェスト JSON をそのまま保持する意図ですが、この機能を使用する経験からのフィードバックに基づいて変更が行われる可能性があります。

各gitリポジトリの取得は、マニフェストの`include`配列に指定されたオブジェクトによって指定されます：

- オブジェクトには、リポのgit URLを示す`"git"`プロパティが必要です。
- オブジェクトには、含めるマニフェストのパスを示す`"include"`プロパティがあることができます。

`"include"`プロパティのデフォルト値は`"manifest.json"`です。`"include"`プロパティは、同じリポジトリから複数のマニフェストを含めるために、パスの配列も可能です。

```json
{
	"build": {
		"URL":"https://github.com/moddable"
	},
	"include": [
		{
			"git":"$(URL)/test0.git"
		},
		{
			"git":"$(URL)/test1.git",
			"include":"modules/test1/manifest.json"
		},
		{
			"git":"$(URL)/test23.git",
			"include": [
				"test2/module.json",
				"test3/module.json"
			]
		}
	]
}
```

マニフェストを処理する際、**mcconfig** と **mcrun** はリポジトリをプロジェクトの一時ビルドファイル内の`repos`ディレクトリにクローンまたはプルします。

特定のブランチやタグは、オプショナルな`branch`および`tag`プロパティを使用してアクセスされます：

```json
    {
		"git":"$(URL)/test0.git",
		"branch":"feature-test"
	},
    {
		"git":"$(URL)/test1.git",
		"tag":"3.5.0"
	},
    {
		"git":"$(URL)/test2.git",
		"tag":"3.5.0",
		"branch":"feature-test"
	}
```

ホスト名、パス名、ブランチ、およびタグは、クローンされたリポジトリが保存されるパスに含まれており、競合を避けます。

> **注記**: クローンされたリポジトリはプロジェクトがクリーンされたときに削除されます（`mcconfig -d -m -t clean`）。したがって、クローンされたリポジトリは編集すべきではありません。

***

<a id="creation"></a>
### `creation`

`creation` オブジェクトは、アプリケーションを実行するXSマシンの作成パラメータを定義します。`manifest_base.json` で使用される値は、Moddable SDKのほとんどの例のアプリケーションを含む多くのアプリケーションで機能します。

```json
"creation": {
	"static": 32768,
	"chunk": {
		"initial": 1536,
		"incremental": 512
	},
	"heap": {
		"initial": 512,
		"incremental": 64
	},
	"stack": 256,
	"keys": {
		"initial": 32,
		"incremental": 0,
		"name": 53,
		"symbol": 3
	},
	"main": "main"
},
```

これらの値は、XS in Cのドキュメントで[説明されている](../xs/XS%20in%20C.md#machine-allocation)マシン割り当てに対応しています（唯一の例外は、[設定フェーズ](../base/setup.md)に続いてロードするモジュールのモジュール指定子である `main` プロパティです）。これらの値を変更する際は注意が必要で、不適切に設定すると不安定または使用不能なシステムになる可能性があります。特にリソースが限られているデバイスでは、大きな値が常に良いとは限りません。

#### `static` メモリ割り当て
`static` プロパティはマイクロコントローラにとって最も重要です。これは、スタック、オブジェクト、バイトコード、文字列など、JavaScript言語ランタイムが使用できるバイト数の合計です。ブックキーピングのオーバーヘッドを最小限に抑え、ランタイムが固定サイズのスロットと可変サイズのチャンクの領域を動的に管理できるように、単一のメモリブロックとして割り当てられます。`static` プロパティは、言語ランタイムによって割り当てられるメモリに厳格な制限を課し、スクリプトがメモリ予算を超過しないことを保証します（もし超過できると、スクリプトがホストOSに必要なメモリを取ってしまい、障害や不安定性を引き起こす可能性があります）。

`static` プロパティはシミュレーターでは無視されます。シミュレーターはオンデマンドのメモリ割り当てにフォールバックします。コンピュータはマイクロコントローラに比べてほぼ無限のメモリを持っているため、これは問題ではありません。

いくつかの状況では、動的に単一のメモリブロックを管理する `static` 割り当て技術が最適な選択ではありません。最初の例は、RAMが2つ以上の非連続なアドレス範囲に分割されているマイクロコントローラ上です（ESP32が一般的な例です）。この場合、仮想マシンのために単一のメモリブロックを使用すると、利用可能なRAMの一部を使用できなくなります。2つ目は、アプリケーションが特定のサイズのスロットとチャンクプールで動作するように慎重に調整されている場合です。正しく行われたこの調整は、特に起動時にガベージコレクションの頻度を減らすことでパフォーマンスを向上させることができます。これらの状況では、以下の手順を実行してください：

- `static` プロパティを `0` に設定して、静的アロケータを無効にします。（プロパティを削除するだけでは不十分です。`static` プロパティはインクルードされたマニフェストで定義されている可能性があります）
- `chunk` の `initial` プロパティをチャンクヒープのサイズ（バイト単位）に設定します
- `heap` の `initial` プロパティをスロットの数に設定します（32ビットMCUでは、各スロットは16バイトです）
- `chunk` の `incremental` と `heap` の `incremental` プロパティを `0` に設定します。（これにより、スロットとチャンクヒープが初期割り当てを超えて拡大するのを防ぎます）

このアプローチを使用すると、マイクロコントローラのメモリアロケータは以下を割り当てます：

- スタック用のメモリブロック（スタックは連続している必要があります）
- チャンク用のメモリブロック（これによりチャンクの断片化によるメモリの損失がなくなり、JavaScriptから可能な限り大きなブロックを割り当てることができます）
- スロットヒープ用の1つ以上のメモリブロック

> **注記**: マイクロコントローラのランタイムは、チャンクヒープを複数のメモリブロックにわたって割り当てるように強化される可能性がありますが、これまでのところ必要はありませんでした。チャンクヒープはスタックとスロットヒープの前に割り当てられ、可能な限り大きな連続した空きブロックを割り当てることができます。

#### `keys`
VMは初期化時に `keys.initial` ランタイムキーのスペースを割り当てます。組み込みプロジェクトの場合、この数は少なくすべきです。ほとんどのキーはビルド時に割り当てられ、ランタイムでは割り当てられません。ランタイムでこれ以上のキーが割り当てられるのを防ぐために、`keys.incremental` を `0` に設定します。追加のキーを割り当てることを許可するには、`keys.incremental` に非ゼロの値を提供します。

`keys` プロパティには以前、ランタイムで割り当て可能なキーの総数を示す `available` プロパティが含まれていました。`keys.initial` が提供されていない場合、`keys.available` の値が `keys.incremental` の `0` と共に使用されます。

***

<a id="defines"></a>
### `defines`

`defines` オブジェクトは、C言語の #defineプリプロセッサステートメントのセットを作成します。これは、ハードウェアドライバのC言語実装を設定するために設計されています。

`defines` オブジェクトについては、[defines ドキュメント](../tools/defines.md)でより詳しく説明されています。

***

<a id="config"></a>
### `config`

`config` オブジェクトには、アプリケーションのスクリプトからアクセス可能な値が含まれています。

これは一般的に、ターゲットプラットフォームのマニフェストでスクリーンドライバ、タッチドライバ、デフォルトの回転を指定するために使用されます。これらのプロパティは [Commodetto](../commodetto/commodetto.md) および [Piu](../piu/piu.md) のセットアップモジュールによって使用されます。

```json
"config": {
	"screen": "ili9341",
	"touch": "ft6206",
	"rotation": 90
}
```

また、[files manifest](../../modules/files/file/manifest.json)で各プラットフォームのファイルシステムのルートを指定したり 、いくつかのネットワーキングの例でWi-Fi認証情報を指定したりするためにも使用されます。

アプリケーションのスクリプトで `config` オブジェクトにアクセスするには、それをインポートします。例えば：

```js
import config from "mc/config";

if (!config.ssid) {
	trace("No Wi-Fi SSID\n");
	return;
}
```

`mc/config` モジュールの `config` オブジェクトは凍結されており、実行時にその値を変更することはできません。

`config` オブジェクトの内容は、`mcconfig` に提供されたコマンドラインにキーバリューペアを追加することで上書きされる場合があります。詳細は [toolドキュメント](./tools.md#arguments) の引数セクションを参照してください。

***

<a id="strip"></a>
### `strip`

ほとんどのアプリケーションは、すべての組み込みJavaScript言語機能を使用しません。ROMを節約するために、XSリンカーは使用されていないネイティブコードをXSエンジン自体から削除することができます。ストリップ機能の詳細については、[XS Differences](../xs/XS%20Differences.md) ドキュメントの **未使用の機能を削除する** セクションを参照してください。

マニフェスト内の `strip` オブジェクトは、XSリンカーによって削除されるべきJavaScript言語の組み込みオブジェクトや関数を指定する文字列または配列です。

- `"*"` はアプリケーションによって使用されていないものは何でも削除できることを意味します。これは `manifest_base.json` に使用される値です。

	```json
	"strip": "*"
	```

- 特定のオブジェクトや関数のみを削除したい場合は、JavaScriptのクラス名と関数名の配列を渡します。配列内の項目が削除されます。配列に含まれていないものは削除されません。

	以下の例では、`RegExp` クラス、`eval` 関数、および2つの `Array` reduce関数が削除されます。

	```json
	"strip": [
		"RegExp",
		"eval",
		"Array.prototype.reduce",
		"Array.prototype.reduceRight"
	]
	```

- また、使用されていないものを全て削除する以外に、特定の項目を削除するよう指定することもできます。

	`"*"` は使用されていないものを全て削除することを意味します。2つの `Array` reduce関数は明示的にリストアップされているため、使用されているかどうかに関わらず削除されます。

	```json
	"strip": [
		"*",
		"Array.prototype.reduce",
		"Array.prototype.reduceRight"
	]
	```

アプリケーションが削除されたクラスや関数を使用しようとすると、`dead strip!` エラーが発生します。

***

<a id="modules"></a>
### `modules`

`modules` オブジェクトは、ビルドに含まれるモジュールを指定します。アプリケーションによって使用されるすべてのモジュールは、`modules` オブジェクトにリストされていなければなりません。

`modules` オブジェクトの `*` パラメータは、パスの配列です。

```json
"modules": {
	"*": [
		"./main",
		"./assets"
	]
}
```

これらのモジュールは、ファイル名を使用して他のスクリプトモジュールにインポートされます。

```js
import ASSETS from "assets"
```

`import` ステートメントで異なる名前を使用したい場合は、`modules` オブジェクトに追加のキー/値ペアを含めることができます。キーは新しい名前で、値はファイルへのパスです。例えば：

```json
"modules": {
	"*": [
		"./main"
	],
	"newNameForAssets": "./assets"
}
```

これらのモジュールは、マニフェストによって与えられた名前を使用して他のスクリプトモジュールにインポートされます。

```js
import ASSETS from "newNameForAssets"
```

***

<a id="preload"></a>
### `preload`

XS JavaScriptエンジンのユニークな機能であるモジュールのプリロードについて説明します。プリロードは、アプリケーションがターゲットデバイスにダウンロードされる前のビルドプロセス中に、JavaScriptアプリケーションの一部を実行します。プリロードについての詳細は、[preloadingドキュメント](../xs/preload.md)を参照してください。

マニフェストの `preload` 配列は、アプリを実行するためにクローンされる読み取り専用のXS仮想マシンでプリロードされるモジュールをリストアップします。これはモジュール名の配列であり、モジュールへのパスではありません。

```json
"preload": [
	"main",
	"assets"
]
```

***

<a id="resources"></a>
### `リソース`

`resources` オブジェクトは、ビルドに含まれるリソース（画像、フォント、オーディオファイル）を指定します。

#### 画像

画像アセットは、GIF、JPEG、PNG画像ファイル形式である可能性があります。

GIFおよびJPEGファイルは `*` 配列に含めるべきです。

```json
"resources":{
	"*": [
		"$(MODDABLE)/examples/assets/images/screen2"
	]
}
```

PNGファイルは `png2bmp` によって変換されます。これにより、Moddableアプリがフラッシュストレージから直接使用できるBMPファイルに変換されます。`png2bmp` はアルファビットマップとカラービットマップに変換することができます。

- `*` 配列にリストされたファイルは、8ビットアルファと16ビットカラービットマップに変換されます。

- `*-color` 配列にリストされたファイルは、16ビットカラービットマップにのみ変換されます。

- `*-alpha` 配列にリストされたファイルは、8ビットアルファビットマップにのみ変換されます。

- `*-mask` 配列にリストされたファイルは、4ビットアルファビットマップに変換されます。

#### フォント

Moddable SDKはビットマップフォントを使用します。メトリックはバイナリFNTファイルによって提供され、グリフはPNGファイルによって提供されます。グリフファイルは他のPNGと同様に（`png2bmp` を使用して）変換されるため、`*`, `*-alpha`, `*-color`, `*-mask` 配列にフォントを含めることができます。

```json
"resources":{
	"*-mask": [
		"$(MODDABLE)/examples/assets/fonts/OpenSans-Semibold-28"
	]
}
```

Moddableアプリケーション用のフォントを作成する方法についての詳細は、[fontドキュメント](../commodetto/Creating%20fonts%20for%20Moddable%20applications.md)を参照してください。

#### オーディオ

[`AudioOut` モジュール](../pins/audioout.md) では、オーディオはMAUDオーディオリソースまたは生のオーディオサンプルとして提供する必要があります。WAVファイルはModdable SDKの `wav2maud` ツールを使用して自動的に `maud` 形式に変換され、IMA ADCPMで圧縮されます。

Waveオーディオファイルは `*` 配列に含めるべきです。

```json
"resources":{
	"*": [
		"$(MODDABLE)/examples/assets/sounds/bflatmajor"
	]
}
```

***

<a id="data"></a>
### `data`

`data` オブジェクトはビルドに含めるリソースを指定します。`resources` オブジェクトのリソースとは異なり、`data` オブジェクトで指定されたリソースはModdable SDKのビルドツールによって何らかの変換は行われません。TLS証明書やJSONファイルなどのリソースを含めるために `data` オブジェクトを使用できます。

#### TLS 証明書

`SecureSocket` オブジェクトはDER（バイナリ）形式のTLS証明書を使用します。証明書ストアは `$MODDABLE/modules/crypt/data` ディレクトリに位置しているか、または自身の証明書を含めることができます。DER形式の有効な証明書であればどれでも機能します。

TLS証明書は `*` 配列に含めるべきです。

```json
"data":{
	"*": [
		"$(MODULES)/crypt/data/ca170"
	]
}
```

***

<a id="platforms"></a>
### `platforms`

`platforms` オブジェクトを使用すると、特定のプラットフォームおよびサブプラットフォームにのみ適用されるべきプロパティを指定できます。`platforms` オブジェクトには、各プラットフォームまたはサブプラットフォームごとに1つのプロパティがあります。各プロパティには、そのプラットフォームがビルドのターゲットである場合にのみ使用される `include` 配列、`modules` オブジェクト、`preload` 配列、`resources` オブジェクト、`defines` オブジェクト、および `recipes` オブジェクトを含めることができます。

これは、モジュールの実装がプラットフォームごとに異なる場合に便利です。たとえば、以下の `platforms` オブジェクトはdigitalのマニフェストから来ています。GPIOの実装にはネイティブコードの使用が必要であり、プラットフォームごとに異なります。これにより、各ターゲットデバイスに対して正しいモジュールが使用されることを保証します。

```json
"platforms": {
	"esp": {
		"modules": {
			"*": "$(MODULES)/pins/digital/esp/*"
		}
	},
	"esp32": {
		"modules": {
			"*": "$(MODULES)/pins/digital/esp32/*"
		}
	},
	"gecko": {
		"modules": {
			"*": "$(MODULES)/pins/digital/gecko/*"
		}
	},
	"qca4020": {
		"modules": {
			"*": "$(MODULES)/pins/digital/qca4020/*"
		}
	},
	"...": {
		"error": "pins/digital module unsupported"
	}
}
```

"`...`" プラットフォーム識別子は、一致するプラットフォームが見つからない場合のフォールバックとして使用されます。これはエラーや警告に便利です。

たとえば、マニフェストの `platforms` オブジェクトが以下のようである場合、`esp` プラットフォームでのビルドは警告/エラーを生成せず、`esp32` プラットフォームでのビルドは警告を生成し、他のプラットフォームでのビルドはエラーを生成します。

```jsonc
"platforms":{
	"esp": {
		/* modules and resources for ESP8266 go here */
	},
	"esp32": {
		"warning": "module XYZ not fully tested on esp32",
		/* modules and resources for ESP32 go here */
	},
	"..." {
		"error": "module XYZ unsupported"
	}
}
```

The `esp32` platform has an additional property `dependency` which can be used to [add ESP Registry components](../devices/esp32/manifest.md#idf-components) to your project.

```json
	"platforms": {
		"esp32": {
			"dependency": [
				{ "name": "onewire_bus", "version": "^1.0.2" },
				{ "namespace": "waveshare", "name": "esp_lcd_jd9365_8", "version": "^0.0.2" }
			]
		}
	}
```

`namespace` is optional and defaults to `espressif`.

The library and include files from the dependencies will be loaded from the ESP Registry and made available to you. You can then write your module with a native part to interface with the component.

The [onewire module](https://github.com/Moddable-OpenSource/moddable/tree/public/modules/drivers/onewire) demonstrates the use of `dependency`.

<a id="subplatforms"></a>
#### サブプラットフォーム

サブプラットフォームは、製品ファミリーのバリエーションに対してアプリケーションを設定するために使用されます。デバイスが似ていて多くの同じ設定を共有しているが、わずかな違いがある場合に便利です。

たとえば、Moddable SDKによってサポートされている各Geckoデバイスにはサブプラットフォームがあります。以下のセグメントでは、すべての `gecko` プラットフォームに対して `timer` モジュールが指定されています。`wakeup` ピンは `gecko/giant` および `gecko/mighty` サブプラットフォームで異なって定義されています。

```json
"platforms": {
    "gecko": {
        "modules": {
            "*": [
                "$(MODULES)/base/timer/*",
                "$(MODULES)/base/timer/mc/*"
            ]
        },
        "preload": [
            "timer"
        ]
    },
    "gecko/giant": {
        "defines": {
            "sleep": {
                "wakeup": { "pin": 2, "port": "gpioPortF", "level": 0, "register": "GPIO_EM4WUEN_EM4WUEN_F2" }
            }
	     }
	 },
    "gecko/mighty": {
        "defines": {
            "sleep": {
                "wakeup": { "pin": 7, "port": "gpioPortF", "level": 0, "register": "GPIO_EXTILEVEL_EM4WU1" }
            }
         }
    }
}
```

`SUBPLATFORM` 変数は `mcconfig` によって自動的に定義されます。サブプラットフォームに一致するためのワイルドカードが利用可能です。`SUBPLATFORM` 変数とワイルドカードのマッチングを一緒に使用することで、サブプラットフォームのマニフェストの含め方が簡単になります：

```json
	"platforms": {
		"esp32/*": {
			"include": "./targets/$(SUBPLATFORM)/manifest.json"
		}
	}
```


***

<a id="bundle"></a>
### `bundle`

`bundle` オブジェクトは、Moddable Storeのアプリアーカイブを構築およびパッケージするために [`mcbundle` コマンドラインツール](./tools.md#mcbundle) によって使用されます。以下のプロパティを持っています：

| プロパティ | 必須 | 説明 |
| :---: | :---: | :--- |
| `id` | ✓ | アプリの署名です。通常、アプリの名前に `tech.moddable.` を加えたものを使用します。例えば、`balls` というアプリの場合は `tech.moddable.balls` となります。
| `devices` | ✓ | アプリをサポートするプラットフォーム識別子またはデバイス署名の配列。<BR><BR>ワイルドカード（例：`esp/*` や `esp32/*`）も使用できます。
| `custom` | | アプリの設定可能な設定のための `custom` ディレクトリへのパス（ある場合）。
| `icon` | | カスタムアプリアイコンへのパス（ある場合）。アプリアイコンはModdable Storeでアプリ名の隣に表示される画像です。Moddable StoreはModdableロゴに基づいたデフォルトのアイコンを提供します。

```json
"bundle": {
    "id": "tech.moddable.countdown",
    "devices": [
        "esp/moddable_one",
        "com.moddable.two"
    ],
    "custom": "./store/custom",
    "icon": "./store/icon.png"
}
```

***
<a id="process"></a>
## マニフェストの処理方法

`mcconfig`は、マニフェストに基づいてmakefileを生成し、`make`を実行してマイクロコントローラーやシミュレーター上でModdableアプリをビルドして起動するコマンドラインツールです。`mcconfig`はマニフェストを3つのパスで処理します：結合、マッチ、生成。

### 結合

最初のパスは、処理されたマニフェストのプロパティと組み込まれたマニフェストのプロパティを結合します（深さ優先順）。各マニフェストについて、共通のプロパティはビルドの目標であるプラットフォームのプロパティと結合されます。サブプラットフォームのプロパティはプラットフォームのプロパティと結合されます。

`modules`および`resources`オブジェクト内のパスは、それを定義するマニフェストに対して相対的になります。

同じ名前のプロパティを結合する際には、結合された値は2つの値の連結になります。例えば、アプリケーションのマニフェストの以下のスニペットを考えてみましょう。`include`オブジェクトは`manifest_base.json`からのプロパティを含めることを指定しています。`modules`オブジェクトは`main`と呼ばれるモジュールをただ一つ指定しています。

```jsonc
{
	"include": [
		"$(MODDABLE)/examples/manifest_base.json"
	],
	"modules": {
		"*": "./main"
	},
	/* other manifest properties here */
}
```

`manifest_base.json`の`modules`オブジェクトには、`$MODDABLE/modules`ディレクトリからの追加モジュールが含まれています。

```json
	"modules": {
		"*": [
			"$(MODULES)/files/resource/*",
			"$(MODULES)/base/instrumentation/*"
		]
	}
```

2つの`modules`オブジェクトの連結には、アプリケーションの`main`モジュールと、`manifest_base.json`で指定されたresourceおよびinstrumentationモジュールが含まれます。

```json
"modules": {
	"*": [
		"./main",
		"$(MODULES)/files/resource/*",
		"$(MODULES)/base/instrumentation/*"
	]
}
```

### マッチ

2回目のパスでは、組み合わせた`modules`および`resources`オブジェクトのプロパティと一致するファイルをマッチングします：

- `~`プロパティにはビルドから除外するファイルが含まれます。
- 各プロパティの名前はターゲットファイルで、その値はソースファイルまたはソースファイルの配列です。
- ターゲットとソースは、マッチするすべてのファイルを表すために`*`ワイルドカードを使用できます。

これらはサポートされている拡張子です：

- `modules`オブジェクトでは、`mcconfig`は`.c`、`.cc`、`.cpp`、`.h`、`.js`、`.m`ファイルにマッチします。
- `resources`オブジェクトでは、`mcconfig`は`.act`、`.bmp`、`.cct`、`.dat`、`.der`、`.fnt`、`.jpg`、`.json`、`.nfnt`、`.pk8`、`.png`、`.rle`、`.ski`、`.ttf`ファイルにマッチします。

### 生成

3回目のパスでは、一致したファイルのためのmake変数とルールが生成されます：

- `.js` ファイルは `xsc` でコンパイルされ、`xsl` でリンクされます。
- `preload` プロパティにリストされているモジュールは、アプリを実行するためにクローンされる読み取り専用のXS仮想マシンで `xsl` によってプリロードされます。
- `.png` ファイルは `png2bmp` によって変換されます。色またはアルファビットマップのみを取得するには、`*-color` および `*-alpha` 疑似ターゲットを使用します。
- `.h` ファイルはヘッダ依存関係とインクルードディレクトリを定義します。
- `.c` ファイルは $(CC) でコンパイルされリンクされますが、組み合わせた `recipes` オブジェクトのプロパティの値と一致する場合は、そのプロパティの名前に対応するmakeレシピを使用します。

生成されたmakefileには生成された変数、ビルドの目標に対応するmakeフラグメント、そして生成されたルールが含まれます。

デフォルトのmakeフラグメントとmakeレシピは `$(MODDABLE)/tools/mcconfig` にあります。これらのmakeフラグメントがコンパイラオプション、リンカーオプション、ライブラリなどを定義します。

makeフラグメントは、プラットフォームの`build`セクションで指定できます。

```json
"platforms": {
	"gecko/*": {
		"build": {
			"MAKE_FRAGMENT": "$(BUILD)/devices/gecko/targets/$(SUBPLATFORM)/make.$(SUBPLATFORM).mk"
		}
		"include": "./targets/$(SUBPLATFORM)/manifest.json"
	}
}
```
