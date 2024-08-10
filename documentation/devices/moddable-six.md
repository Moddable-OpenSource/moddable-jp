# Moddable Six 開発者ガイド

Copyright 2024 Moddable Tech, Inc.<BR>
改訂日： 2024年7月18日

このドキュメントは、Moddable Sixに関する情報を提供します。ピンや内蔵コンポーネントの詳細、アプリのビルドとデプロイ方法、追加の開発リソースへのリンクが含まれています。

## 目次

- [Moddable Sixについて](#about-moddable-six)
	- [コンポーネント](#components)
	- [ピン配置](#pinout)
	- [ピン情報](#pin-info)
	- [寸法](#dimensions)
- [Moddable SDKとホスト環境のセットアップ](#setup)
- [アプリのビルドとデプロイ](#building-and-deploying-apps)
	- [USBポートを使用する](#install-over-usb)
	- [シリアルプログラマを使用する](#install-over-serial)
- [Moddable Sixの使用](#moddable-features)
	- [8 MB PSRAMメモリ](#additional-memory)
	- [デュアルコアESP32-S3 CPU](#dual-core)
	- [バックライト](#backlight)
	- [動的フレームレート制御](#frame-rate-control)
	- [ティアリング効果](#tearing-effect)
	- [アンプ付きオーディオ](#amplified-audio)
	- [STEMMA QT / Qwiic / JST SH 1mmクイックコネクタ](#quick-connector)
	- [NeoPixel](#neopixel)
	- [USB](#usb)
	- [タッチパネル](#touch-panel)
	- [フラッシュボタン](#flash-button)
- [トラブルシューティング](#troubleshooting)
- [開発リソース](#development-resources)
	- [シミュレータ](#simulator)
	- [サンプル](#examples)
	- [デバッグ](#debugging)
		- [JavaScriptとTypeScriptコードのデバッグ](#advanced-xsbug)
		- [ネイティブコードのデバッグ](#debug-native)
	- [ドキュメント](#documentation)
	- [サポート](#support)
	- [アップデート](#updates)

<a id="about-moddable-six"></a>
## Moddable Sixについて

<a id="components"></a>
### コンポーネント

#### ESP32-S3 MCU
Moddable SixはデュアルコアのESP32-S3によって動作されます。JavaScriptコードは標準のWeb Workersを使用して、2つのCPUコアをフルに活用できます。

#### PSRAMとフラッシュストレージ
ESP32-S3モジュールには、広大な8 MBのフラッシュストレージメモリと8 MBのPSRAMがあり、合計で8.5 MBのRAMを持っています。PSRAMとフラッシュはどちらも8ビットバスで接続されており、最大のスループットを実現します（Moddable Twoの少なくとも2倍の速度）。

#### 美しく、速く、応答性の高い画面
画面は240 x 320 QVGA IPSディスプレイで、8ビット幅の並列データバスによって駆動されます。新しいハードウェア設計と新しい並列ディスプレイドライバのおかげで、ディスプレイスループットはこれまでの少なくとも4倍の速さです。

ディスプレイの明るさはプログラムで調整可能です。

#### 応答性の高いマルチタッチ
タッチパネルはGT911タッチコントローラを使用しており、マルチタッチ入力をサポートしています。新しいタッチドライバとハードウェア設計により、タッチ割り込みが可能になり、応答性が向上し、CPUの負荷が最小限に抑えられます。

#### アンプ付きスピーカー
内蔵のアンプ付きスピーカーは、オーディオユーザーインターフェースのフィードバック、音楽のストリーミング、および音声プロンプトを可能にします。低コストの設計で高品質のオーディオを提供し、商用製品にオーディオフィードバックを導入するコストを削減します。

#### STEMMA QT / Qwiic / JST SH 1mm クイック接続
Moddable Sixには、I²Cセンサーや周辺機器を簡単に接続するためのJST SHコネクタがあります。このコネクタはAdafruitではSTEMMA QT、SparkFunではQwiicと呼ばれています。

#### NeoPixel
オンボードにはNeoPixelがあり、これは多色LEDで、表現力豊かなステータスインジケータとして使用できます。拡張ヘッダ上のNeoPixelデータを使用して、多くのNeoPixelのストランドを制御できます。

#### USB
Moddable SixはUSBポートを介してプログラミングとJavaScriptデバッグを可能にします。GDBを使用したネイティブコードのデバッグもUSB経由でサポートされています。TinyUSBソフトウェアスタックも使用できます。

<a id="pinout"></a>
### ピン配置

<img src="../assets/devices/moddable-six-pinout.png">

<a id="pin-info"></a>
### ピン情報

#### 6ピンプログラミングヘッダの説明

| 名前 | 機能 | 説明 |
| :---: | :---: | :--- |
| RTS | I/O | 自動プログラミングおよびリセット回路に接続 |
| DTR | I/O | 自動プログラミングおよびリセット回路に接続 |
| TX | I/O | ESP32-S3 GPIO 43に接続 |
| RX | I/O | ESP32-S3 GPIO 44に接続 |
| 5V | PWR | 5V入力はLM1117-3.3電圧レギュレータおよび16ピン外部コネクタの5Vピンに接続 |
| GND | GND | GNDに接続 |

#### 16ピン外部ヘッダーの説明

| 名前| 機能| 説明 |
| :---: | :---: | :--- |
| SDA&nbsp;IO4 | I/O | ESP32-S3 GPIO 4に接続（標準SDA、外部プルアップ抵抗なし） |
| SCL&nbsp;IO5 | I/O | ESP32-S3 GPIO 5に接続（標準SCL、外部プルアップ抵抗なし） |
| GND     | GND | GNDに接続 |
| 3.3V    | I/O | 3.3V入力および出力。ESP32-S3の3.3V入力および他の3.3V ICに接続。ボードがマイクロUSB、VIN外部コネクタ、またはプログラミングコネクタ経由で5Vで電源供給されている場合、調整された出力電力。3.3VはLM1117-3.3の出力。LM1117の出力は100ufタンタルコンデンサで調整されています。 |
| IO0     | I/O | ESP32-S3 GPIO 0に接続。デバイスのフラッシュボタンを押すとこのピンが接地されます。また、自動プログラミング回路にも使用されます。 |
| IO16    | I/O | ESP32-S3 GPIO 16に接続、アナログ |
| IO1     | I/O | ESP32-S3 GPIO 1に接続、アナログ |
| IO2     | I/O | ESP32-S3 GPIO 2に接続 |
| IO3     | I/O | ESP32-S3 GPIO 3に接続、アナログ |
| IO19    | I/O | ESP32-S3 GPIO 19に接続、USB D- |
| IO20    | I/O | ESP32-S3 GPIO 20に接続、USB D+、アナログ |
| AUD&nbsp;IO45 | I/O | ESP32-S3 GPIO 45に接続、オーディオ信号 |
| IO17    | I/O | ESP32-S3 GPIO 17に接続、アナログ |
| VIN 5V  | I/O | VINはModdable Sixに電力を供給するために使用できる5Vピンです。他の5V入力のいずれかでボードが電源供給されている場合、このピンは外部5V電源に使用できます。これは未調整のピンです。VINは5V入力ソースへの直接接続です。他の5V入力およびLM1117-3.3電圧レギュレータに接続。 |
| RGB&nbsp;IO48    | I/O | ESP32-S3 GPIO 48に接続、NeoPixel |
| IO15    | I/O | ESP32-S3 GPIO 15に接続、アナログ |

#### 電源

Moddable Sixは3.3Vデバイスです。5V電源はLM1117-3.3電圧レギュレータによって3.3Vに調整されます（仕様についてはデータシートを参照してください）。Moddable Sixのテストは、典型的な5V 0.5アンペアのUSB電源で行われました。

Moddable Sixには以下の方法で電源を供給できます：

* 5V - Micro USBコネクタ
* 5V - Moddable Programmerコネクタ
* 5V - 16ピン外部ヘッダのVIN
* 3.3V - 3.3Vピン外部ヘッダ

<a id="dimensions"></a>
### 寸法

Moddable Sixの完全な寸法はこの[PDFドキュメント](../assets/devices/moddable-six-dimensions.pdf)に記載されています。これは、Moddable Sixを製品筐体に取り付けたり、Moddable Six用のケースを設計する際に役立ちます。

<a id="setup"></a>
## Moddable SDKとホスト環境のセットアップ

Moddable Sixでアプリをビルドして実行するには、以下の手順が必要です：

1. [Moddable SDK](./../Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [ESP32ツール](./esp32.md)をインストールする
3. 以下の**アプリのビルドとデプロイ**セクションの指示に従う

<a id="building-and-deploying-apps"></a>
## アプリのビルドとデプロイ

ホスト環境とESP32ツールのセットアップが完了したら、以下の手順に従ってModdable Sixにアプリケーションをインストールします。

Moddable SixにはUSBポートとシリアルプログラマーポートの両方があります。どちらのポートでもxsbugを使用してプログラムおよびデバッグできます。シリアルプログラマーポートを使用するには、Moddable Programmerまたは他のUSB-シリアルアダプタが必要です。

<a id="install-over-usb"></a>
USBポートを使用するには：

1. USBケーブルをModdable SixのUSBコネクタに接続します

2. プラットフォームとして `-p esp32/moddable_six_cdc` を使用します

3. `mcconfig` を使用してアプリをビルドおよびデプロイします

	`mcconfig` は、マイクロコントローラおよびシミュレータ上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig` の完全なドキュメントは[こちら](../tools/tools.md)にあります。

	USBポートを使用してModdable Six用にビルドするには、`mcconfig` でプラットフォーム `-p esp32/moddable_six_cdc` を使用します。例えば、[`piu/balls` のサンプル](../../examples/piu/balls)をビルドするには：

	```text
	cd $MODDABLE/examples/piu/balls
	mcconfig -d -m -p esp32/moddable_six_cdc
	```

<a id="install-over-serial"></a>
また、シリアルプログラマを使用して接続するには：

1. プログラマをModdable Sixのシリアルコネクタに接続します

	プログラマの向きが正しいことを確認してください。向きは以下の画像と一致する必要があります。

	![image](https://gist.github.com/assets/6822704/c605ef5b-0b80-46a2-a87c-1cab80b59deb)

2. プログラマをマイクロUSBケーブルでコンピュータに接続します

	データ同期が可能なケーブルを使用していることを確認してください。電源供給のみのケーブルではありません。

3. `mcconfig`を使用してアプリをビルドしてデプロイします。

	`mcconfig`は、マイクロコントローラおよびシミュレータ上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)にあります。

	Moddable Programmerを使用してModdable Six用にビルドするには、`mcconfig`でプラットフォーム`-p esp32/moddable_six`を使用します。例えば、[`piu/balls`のサンプル](../../examples/piu/balls)をビルドするには：

	```text
	cd $MODDABLE/examples/piu/balls
	mcconfig -d -m -p esp32/moddable_six
	```

[examples readme](../../examples)には、画面の回転、Wi-Fi設定など、よく使用される`mcconfig`引数に関する追加情報が含まれています。


プラットフォーム `-p sim/moddable_six` を使用して、Moddable Sixシミュレータ用に `mcconfig` をビルドします。

<a id="moddable-features"></a>
## Moddable Six の使用
Moddable Sixは、シンプルなパッケージに多くのハードウェア機能を詰め込んでいます。このセクションでは、ソフトウェアがそのすべてのハードウェアを使用するために知っておくべきことを紹介します。

<a id="additional-memory"></a>
#### 8 MB PSRAM メモリ
Moddable SixのESP32-S3には約512 KBの内部メモリと、さらに8 MBの外部PSRAMがあります。PSRAMは高速な8ビットバスを介して接続されていますが、それでも内部の512 KBのRAMよりは遅いです。

Moddable SixのデフォルトのJavaScript仮想マシンは、162 KBのメモリ、8 KBのスタック、77 KBのスロットヒープ、および77 MBのチャンクヒープを持っています。これは多くのプロジェクトに十分なメモリであり、すべてのJavaScript仮想マシンメモリが高速な内部メモリに割り当てられることを保証します。8 MBを超えるメモリを使用する仮想マシンを作成することもできます。そのためには、プロジェクトの `manifest.json` に `"creation"` セクションを追加します。以下は、32 KBのスタック、2032 KBのスロットヒープ、および2032 KBのチャンクヒープを持つ4 MBの仮想マシンを作成する例です：

```json
"creation": {
	"static": 0,
	"chunk": {
		"initial": 2080768,
		"incremental": 0
	},
	"heap": {
		"initial": 130048,
		"incremental": 0
	},
	"stack": 2048
}
```

`"chunk"`の値はバイト単位であり、`"stack"`と`"heap"`の値は「スロット」単位です（ESP32-S3では各スロットは16バイトです）。`"creation"`に関する詳細は[マニフェストドキュメント](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/tools/manifest.md#creation)を参照してください。

すべてのメモリを活用するもう1つの方法は、プロジェクトを複数の独立したWeb Workerに分割することです。それについては次のセクションで説明します。

<a id="dual-core"></a>
#### デュアルコアESP32-S3 CPU
Moddable SDKは、ESP32-S3の2つのCPUコアを活用するのを簡単にします。標準のWeb Worker APIを使用すると、2つのコアでスクリプトを並行して実行できます。例えば、1つのワーカーはユーザーインターフェース用、もう1つはネットワーク通信用、そして3つ目はセンサーの読み取り処理用に使用することができます。この分離により、他の作業が行われている間にブロックされることを避けて、ユーザーインターフェースが応答性を保つことができます。また、コードを整理し、プロジェクトの異なる部分の相互作用を最小限に抑えるのにも役立ちます。


各Web Workerは独立したJavaScript仮想マシンです。各Workerは別々のFreeRTOSタスクで実行され、デュアルコアESP32-S3上で並行して実行できます。Workersはメッセージや`SharedArrayBuffer`オブジェクトを送信することで通信します。

Moddable Sixの8 MB PSRAMは、デフォルトの作成構成である177 KBを使用して約40の仮想マシンを含むことができます。

Moddable SDKのWeb Workersについて詳しく知るには、[ドキュメント](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/base/worker.md)を読み、[サンプル](https://github.com/Moddable-OpenSource/moddable/tree/public/examples/base/worker)を実行してみてください。

#### 美しく、速く、応答性の高い画面

タッチスクリーンは、GT911静電容量式タッチコントローラーを備えた8ビット幅の並列データバスで駆動される240 x 320 QVGA IPSディスプレイです。

<a id="backlight"></a>
#### バックライト

サンプル： [backlight](https://github.com/Moddable-OpenSource/moddable/tree/public/examples/piu/backlight)

Moddable Sixには、ソフトウェアで調整可能なバックライトがあります。

バックライト制御はGPIO 14に接続されています。ホストの`config`にはバックライトGPIOの定数が定義されています。

```js
import "config" from "mc/config";

Digital.write(config.backlight, 1); // backlight ON
Digital.write(config.backlight, 0); // backlight OFF
```

バックライトの明るさは、プロジェクトマニフェストの`config`セクションでビルド時に設定できます。デフォルトは100%です。

```js
"config": {
    "brightness": 75
}
```

`mcconfig`を使用してビルドする際に、コマンドラインで明るさを設定することもできます。ここでは50%に設定されています。

```text
mcconfig -d -m -p esp32/moddable_six_cdc brightness=50
```

Moddable Sixの`setup/target`モジュールは、コード内でバックライトを調整するために使用できる`backlight`という名前のグローバル変数をインストールします。ここでは80%に設定されています。

```js
backlight.write(80);
```

`backlight`グローバルには`PWM`のサブクラスのインスタンスが含まれています。これが自動的に作成されるのを望まない場合は、プロジェクトのマニフェストの`config`セクションで明るさを`"none"`に設定します。

```js
"config": {
    "brightness": "none"
}
```

<a id="frame-rate-control"></a>
#### 動的フレームレート制御

Moddable Sixの画面は、1秒間に30から100フレームのリフレッシュレートで動作できます。デフォルトは50 FPSです。フレームレートはプロジェクトコードで調整できます。

```js
screen.frameRate = 75;
```

ディスプレイの最適なハードウェアリフレッシュレートは、アニメーションのデザインと、Moddable Six内のESP32-S3がフレームをどれだけ速くレンダリングできるかに依存します。

<a id="tearing-effect"></a>
#### テアリング効果

Moddable Sixのディスプレイコントローラーには「テアリング効果」出力があります。これは、画面のリフレッシュをハードウェアリフレッシュと同期させることで、画面上の目に見えるテアリングを排除するために使用されます。これにより、はるかに安定したディスプレイとスムーズなアニメーションが提供されます。しかし、フレームが十分に速くレンダリングできない場合、それでもテアリングが発生します。その場合は、フレームレート制御を使用してレンダリングフレームレートを下げるか、アニメーションを最適化してください。場合によっては、テアリング効果ピンの使用を無効にする方が望ましいこともあります。これはプロジェクトのスクリプトで行うことができます。

```js
screen.syncFrames = false;
```

ディスプレイコントローラのティアリング効果出力はESP32-S3のGPIO 47に接続されています。これはILI9341_P8ディスプレイドライバによって管理されているため、ディスプレイドライバを使用している場合はコードで使用しないでください。

<a id="amplified-audio"></a>
#### アンプ付きオーディオ

サンプル： [somafm](https://github.com/Moddable-OpenSource/moddable/tree/public/contributed/somafm)
[resource-stream](https://github.com/Moddable-OpenSource/moddable/tree/public/examples/pins/audioout/resource-stream)
[mp3-resource-stream](https://github.com/Moddable-OpenSource/moddable/tree/public/examples/pins/audioout/mp3-resource-stream)

Moddable Sixは[PDM](https://en.wikipedia.org/wiki/Pulse-density_modulation)を使用して[PAM8302A](https://www.diodes.com/part/view/PAM8302A?BackID=8156)アンプを通じてオーディオを再生します。

オーディオ信号はGPIO 45から出力されます。ピンIO45は拡張ヘッダにもあります。IO45の信号はESP32-S3から直接出力されるフィルタなし、増幅なしのPDM出力です。これを使用して独自の外部フィルタ、アンプ、およびスピーカーを駆動することができます。アンプの出力はオンボードスピーカーのみに行き、ピンIO45には行きません。プロジェクトがオーディオを再生しない場合、PIN IO45はGPIOとして使用可能です。

GPIO 46は、オンボードスピーカーに接続されたPAM8302Aアンプを有効にします。オーディオが再生されているときにアンプに電力が自動的に供給されます。オーディオが再生されていないときは、アンプはオフになります。

<a id="quick-connector"></a>
#### STEMMA QT / Qwiic / JST SH 1mm クイックコネクタ

Moddable Sixには、I²Cプロトコルを使用してセンサーや周辺機器を簡単に接続するために使用される、一般に[Qwiic](https://www.sparkfun.com/qwiic)または[STEMMA QT](https://learn.adafruit.com/introducing-adafruit-stemma-qt/what-is-stemma-qt)と呼ばれるJST SH 1mmコネクタがあります。SDAにはGPIO 4、SCLにはGPIO 5を使用します。

<img width="405" src="../assets/devices/moddable-six-qwiic.jpg">

<a id="neopixel"></a>
#### NeoPixel

サンプル： [neopixel](https://github.com/Moddable-OpenSource/moddable/tree/public/examples/drivers/neopixel)

メインボードには、GPIO 48を使用して制御されるNeoPixelがあります。

Moddable Sixは、LED設定で`rainbow`オプションを有効にすることで、実行中にNeoPixelの色を循環させることができます。アプリケーションでNeoPixelを使用するか、GPIOとしてピンIO48を使用するには、プロジェクトマニフェストで`rainbow`設定オプションを`false`に設定します。

```js
"config": {
    "led": {
        "rainbow": false
    }
}
```

NeoPixelストリップは、拡張ヘッダーのピンRGB IO48に接続できます。

<img width="405" src="../assets/devices/moddable-six-external-neopixel.jpg">

> 注: NeoPixelストリップには外部の5V電源を供給してください。3.3V電源では、NeoPixelストリップが正しく動作しない場合があります。短いNeoPixelストリップの場合、Moddable Sixが5V電源で動作しているときに`VIN 5V`ピンを使用できます。

<a id="usb"></a>
#### USB

USBポートはGPIO 19とGPIO 20に接続されています。このポートはModdable Sixのプログラムおよびデバッグに使用できます。ESP32ファミリーのUSBに関する詳細は、[ESP32ドキュメントのUSBセクション](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/devices/esp32.md#using_usb)を参照してください。

プロジェクトでUSBを使用せず、ターゲット`esp32/moddable_six_cdc`でビルドしていない場合、拡張ヘッダーのIO19およびIO20をGPIOとして使用できます。

<a id="touch-panel"></a>
#### タッチパネル
タッチパネルは、GPIO 4およびGPIO 5のI²Cバスに接続されたGT911タッチコントローラーを使用します。I²Cアドレスは`0x14`または`0x5D`です。

GT911タッチコントローラーの割り込みはGPIO 38に接続されています。GT911タッチドライバーはこの割り込みを使用してタッチイベントを検出し、タッチパネルの応答性を向上させ、タッチイベントを継続的にポーリングする必要をなくします。

プロジェクトでタッチパネルを使用する場合、IO4およびIO5をGPIOとして使用することはできません。ただし、これらのピンを他のI²Cデバイスと一緒に使用することは可能です。そのデバイスがI²Cアドレス `0x14` または `0x5D` を使用しない限り、タッチパネルと一緒に使用できます。

<a id="flash-button"></a>
#### フラッシュボタン

Moddable Sixの背面にあるフラッシュボタンは、プロジェクトの入力として使用できます。これはGPIO0に接続されており、デジタル入力APIを使用してアクセスできます。ただし、グローバルな `Host` オブジェクトを使用してアクセスすることをお勧めします。これは少し簡単で、他のデバイスに移植可能であり、Moddable Sixシミュレーターでも動作するためです。以下はその例です：

```js
new Host.Button.Default({
	onPush() {
		trace(`Button pressed ${this.pressed}\n`);
	}
});
```

<a id="troubleshooting"></a>
## トラブルシューティング

Moddable Sixは、USBまたはシリアルコネクタを使用して開発用コンピュータと通信できます。USBコネクタを使用する場合は、`esp32/moddable_six_cdc` プラットフォームを使用してビルドします。シリアルコネクタを使用する場合は、`esp32/moddable_six` プラットフォームを使用してビルドします。

<a id="development-resources"></a>
## 開発リソース

<a id="simulator"></a>
### シミュレーター

Moddable SDKのシミュレーター、mcsimにはModdable Sixのシミュレーターが含まれています。このシミュレーターは、プロジェクトの多くの部分を迅速に開発およびデバッグするための貴重なツールです。

Moddable Sixシミュレーターを使用するには、`mcconfig`でビルドする際に`sim/moddable_six`プラットフォームを指定します：

```
mcconfig -d -m -p sim/moddable_six
```

Moddable Sixシミュレーターは以下をシミュレートします：

- QVGAディスプレイ
- タッチ入力
- オーディオ再生
- NeoPixel LED
- Web Workers
- Wi-Fi
- フラッシュボタン

シミュレーターのコントロールパネルには、「Default」とラベル付けされたボタンがあり、これはModdable Sixの背面にあるフラッシュボタンをシミュレートします。コントロールパネルを表示するには、ビュー メニューの「Show Controls」を使用します。

<a id="examples"></a>
### サンプル

Moddable SDKには、その多くの機能を使用する方法を示す150以上の[サンプルアプリ](../../examples)があります。これらのサンプルのほとんどはModdable Sixと互換性があります。

Moddable Six用に設計された一連のサンプルアプリは、[contributed/moddable_six](../../contributed/moddable_six)ディレクトリにあります。これらのアプリは、高フレームレートのアニメーション、UIへのオーディオ統合、タッチインタラクション、ベクターグラフィックスなど、Moddable Sixの機能を示しています。これらのアプリは、`mcconfig`を使用して`sim/moddable_six`プラットフォームでmacOS、Windows、およびLinux上のModdable Sixシミュレーターでも実行できます。

これらのアプリは、あなた自身のプロジェクトの出発点として最適です。ハードウェアとの通信、UIのインタラクションの変更、新機能の追加などに簡単に適応できます。

#### battery

[battery](../../contributed/moddable_six/battery) アプリは、家庭用バッテリーシステムのコントロールパネルです。シミュレートされたバッテリーを使用します。

このアプリには、流体アニメーションと高度な画面間の遷移を伴うバッテリーレベルの高度なレンダリングが含まれています。

<img width="45%" src="../assets/devices/moddable-six-battery1.png"> <img width="45%" src="../assets/devices/moddable-six-battery2.png">

<img width="45%" src="../assets/devices/moddable-six-battery3.png"> <img width="45%" src="../assets/devices/moddable-six-battery4.png">


#### led-color

[led-color](../../contributed/moddable_six/led-color) アプリは、オンボードのNeopixelによって表示される色をユーザーが選択するためのカラーピッカーを提供します。

<img width="45%" src="../assets/devices/moddable-six-led-color1.png"> <img width="45%" src="../assets/devices/moddable-six-led-color2.png">

#### plug-schedule

[plug-schedule](../../contributed/moddable_six/plug-schedule) アプリは、IoTプラグのインターフェースを提供します。

これは、Moddable Sixの例の中で最大かつ最も包括的なアプリです。ほぼ完全な製品に近いです。2つのスマートプラグのための個別のスケジュールを維持し、Wi-Fi設定、時間設定、グラフィカルなタイムゾーンピッカーを含みます。モバイルスタイルのUIは、スムーズでちらつきのないアニメーションを特徴とし、音声フィードバックで強化されています。

<img width="45%" src="../assets/devices/moddable-six-smartplug1.png"> <img width="45%" src="../assets/devices/moddable-six-smartplug2.png">

<img width="45%" src="../assets/devices/moddable-six-smartplug3.png"> <img width="45%" src="../assets/devices/moddable-six-smartplug4.png">


#### speaking-clock

[speaking-clock](../../contributed/moddable_six/speaking-clock) アプリは、毎分時間を音声で知らせるワードクロックです。

speaking clockの声はChatGPTです！音声サンプルはChatGPTから取得され、アプリに組み込まれています。これらはWAVEファイルに保存されているため、好きな声、例えば自分の声に簡単に置き換えることができます。

<img width="45%" src="../assets/devices/speaking-clock.gif">

<a id="debugging"></a>
### デバッグ

Moddable Sixは、JavaScript、TypeScript、およびネイティブコードの高度なデバッグサポートを提供します。

<a id="advanced-xsbug"></a>
#### JavaScriptおよびTypeScriptコードのデバッグ

Moddable SDKに含まれるxsbugデバッガーは、JavaScriptおよびTypeScriptコードのデバッグに使用されます。[xsbugのドキュメント](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/xs/xsbug.md)には基本が説明されています。TypeScriptをデバッグするために特別なことをする必要はありません。xsbugはソースマップをサポートしているため、TypeScriptのデバッグは自動的に機能します。xsbugは複数のJavaScript仮想マシンを同時にデバッグすることをサポートしているため、Web Workersもデバッグできます。

xsbugの基本に慣れたら、以下のブログ投稿をチェックして、xsbugに組み込まれている高度なデバッグ機能について学んでください。

- [条件付きブレークポイント](https://moddable.com/blog/conditional-breakpoints/)
- [関数ブレークポイント](https://www.moddable.com/blog/function-breakpoints/)
- [トレースポイント](https://www.moddable.com/blog/tracepoints/)

<a id="debug-native"></a>
#### ネイティブコードのデバッグ

Moddable Sixでネイティブコードをデバッグするには、GDBデバッガを使用します。

[GDB](https://www.gnu.org/software/gdb/documentation/)は、Unix系のビルドホストで広く使用されているGNUデバッガです。GDBは、[SDKおよびホスト環境のセットアップ](#setup)のステップでダウンロードされたEspressifツールチェーンに含まれています。

GDBはUSBポートを介してModdable Sixと通信します。

> **注意**: GDBは非常に強力なツールで、習得には時間がかかります。ESP32-S3でのGDBサポートにはいくつかの制限があり、使用が難しい場合があります。例えば、ハードウェアブレークポイントが2つしかサポートされていないなどです。

##### GDBのセットアップ

1. OpenOCDはGDBがESP32-S3と通信するのを可能にします。Espressifの指示に従ってESP-IDFをセットアップした場合、すでにOpenOCDがセットアップされているはずです。OpenOCDがインストールされていることを確認する手順については、Espressifのドキュメントを参照してください。[OpenOCDのインストールを確認する](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/jtag-debugging/#jtag-debugging-setup-openocd)。

2. あなたの `$HOME` ディレクトリに以下の内容で `.gdbinit` というGDBスタートアップコマンドテキストファイルを作成します：

```
target extended-remote :3333
set remote hardware-watchpoint-limit 2
mon reset halt
file xs_esp32.elf
flushregs
thb app_main
c
```

> 注意: すでに `$HOME` ディレクトリに `.gdbinit` が存在する場合があります。その場合は、別の場所に移動するか、名前を変更することをお勧めします。

##### GDBを使ったデバッグ

1. デバッグする予定のネイティブコードを含むアプリをビルドします。この例では、[SomaFMの例](https://github.com/Moddable-OpenSource/moddable/tree/public/contributed/somafm)をビルドします：

```
cd $MODDABLE/contributed/somafm
mcconfig -d -m -p esp32/moddable_six ssid="YOU WI-FI ACCESS POINT" password="YOUR WI-FI PASSWORD"
```

> **注意**: GDBを使用するには、Moddable SixをModdable SixのUSBポートを使用してコンピュータに接続します。シリアルポートではなくUSBポートでのみGDBがサポートされています。GDBと同時にxsbugを使用したい場合は、Moddable Programmerを使用してシリアルポートをコンピュータに接続することもできます。

2. 新しいターミナルウィンドウでOpenOCDを開始します

```
openocd -f board/esp32s3-builtin.cfg
```

3. ターミナルウィンドウで、アプリケーションの `bin/` ディレクトリに移動します。

```
cd $MODDABLE/build/bin/esp32/moddable_six/debug/somafm
```

4. GDBを起動します

```
xtensa-esp32s3-elf-gdb
```

いくつかの起動情報がスクロールされ、アプリケーションは `app_main()` で停止します：

```
Thread 2 "main" hit Temporary breakpoint 1, app_main () at /Users/mkellner/moddable/build/tmp/esp32/moddable_six_cdc/debug/somafm/xsProj-esp32s3/main/main.c:477
477		void app_main() {
```

5. GDBを使用します

	この時点で、通常通りにGDBを使用して、ブレークポイントの設定、変数、メモリ、レジスタの検査などができます。

```
(gdb) break xs_audioout
(gdb) c
```

<a id="documentation"></a>
### ドキュメント

Moddable SDKのすべてのドキュメントは [documentation](../) ディレクトリにあります。**documentation**、**examples**、および **modules** ディレクトリは共通の構造を持っており、情報を簡単に見つけることができます。主なハイライトは以下の通りです：

- `commodetto` サブディレクトリには、2DグラフィックスAPIを提供するビットマップグラフィックスライブラリであるCommodettoと、軽量レンダリングエンジンであるPocoに関連するリソースが含まれています。
- `piu` サブディレクトリには、複雑でレスポンシブなレイアウトを簡単に作成できるユーザーインターフェースフレームワークであるPiuに関連するリソースが含まれています。
- `networking` サブディレクトリには、BLE、ネットワークソケット、およびHTTP/HTTPS、MQTT、WebSockets、DNS、SNTP、telnetを含むソケット上に構築されたさまざまな標準的で安全なネットワーキングプロトコルに関連するネットワークリソースが含まれています。
- `io` サブディレクトリには、ECMA-419を使用したサポートされているハードウェアプロトコル（デジタル、アナログ、PWM、I²Cなど）に関連するリソースが含まれています。一般的な市販センサー用のドライバと対応するサンプルアプリも多数利用可能です。

<a id="Support"></a>
### サポート

質問がある場合は、[ディスカッションを開く](https://github.com/Moddable-OpenSource/moddable/discussions)ことをお勧めします。できるだけ早く対応しますし、他の開発者も助けてくれたり、あなたの質問に対する回答から利益を得ることができます。多くの質問はすでに回答されているので、新しい質問をする前に過去のディスカッションや問題を検索してみてください。

<a id="Updates"></a>
### 更新情報

私たちの最新情報を把握する最良の方法は、Twitter ([@moddabletech](https://twitter.com/moddabletech)) をフォローすることです。新しいブログ投稿やその他のModdableニュースについての発表を行っています。最新のModdable SDKリリースについて知るには、[GitHubのリリース](https://github.com/moddable-OpenSource/moddable/releases)を購読し、[リポジトリ](https://github.com/Moddable-OpenSource/moddable)をウォッチすることをお勧めします。


