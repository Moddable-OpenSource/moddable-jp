# Moddable Two Developer Guide
Copyright 2019-2024 Moddable Tech, Inc.<BR>
改訂： 2024年4月29日

このドキュメントは、Moddable Twoに関する情報を提供します。ピンやその他のコンポーネントの詳細、アプリのビルドとデプロイ方法、その他の開発リソースへのリンクが含まれています。

## Table of Contents

- [Moddable Twoについて](#about-moddable-two)
	- [コンポーネント](#components)
	- [ピン配置](#pinout)
	- [ピン情報](#pin-info)
	- [寸法](#dimensions)
- [SDKとホスト環境のセットアップ](#setup)
- [アプリのビルドとデプロイ](#building-and-deploying-apps)
- [トラブルシューティング](#troubleshooting)
- [開発リソース](#development-resources)
	- [サンプル](#examples)
	- [ドキュメント](#documentation)
	- [バックライト](#backlight)
	- [サポート](#support)
	- [更新](#updates)

<a id="about-moddable-two"></a>
## Moddable Twoについて

<img src="../assets/devices/moddable-two.png">

Moddable Twoは、開発者が安価なハードウェアでModdable SDKを試すのを容易にするハードウェアモジュールです。[Moddableのウェブサイト](http://www.moddable.com/moddable-two)で購入できます。

<a id="components"></a>
### コンポーネント

Moddable Twoの主なコンポーネントは、ESP32モジュールと静電容量式タッチスクリーンの2つです。ESP32モジュールにはWi-Fi/BLEアンテナと4 MBのフラッシュストレージメモリが含まれています。タッチスクリーンは、FT6206静電容量式タッチコントローラを備えたMIPIディスプレイシリアルインターフェース互換のディスプレイコントローラによって駆動される240 x 320 QVGA IPSディスプレイです。

<a id="pinout"></a>
### ピン配置

<img src="../assets/devices/moddable-two-pinout.png">

#### タッチパネル

タッチパネルはピン21と22を使用してI<sup>2</sup>Cバスに接続します。アプリがタッチパネルを使用する場合、これらのピンをGPIOとして使用することはできません。

アプリがタッチパネルを使用していても、同じアドレスを使用しない限り、他のI<sup>2</sup>Cセンサーを使用することができます。タッチパネルはアドレス0x38（7ビットアドレス）を使用します。

#### ディスプレイ

ディスプレイはピン13と14を使用してSPIバスに接続します。アプリがディスプレイを使用する場合、これらのピンをGPIOとして使用することはできません。

アプリがディスプレイを使用していても、他のSPIセンサーを使用することができます。

<a id="pin-info"></a>
### ピン情報

#### 6ピンプログラミングヘッダの説明

| 名前 | 機能 | 説明 |
| :---: | :---: | :--- |
| RTS | I/O | 自動プログラミングおよびリセット回路に接続 |
| DTR | I/O | 自動プログラミングおよびリセット回路に接続 |
| TX | I/O | ESP32 GPIO1に接続 |
| RX | I/O | ESP32 GPIO3に接続 |
| 5V | PWR | 5V入力はLM1117-3.3電圧レギュレータおよび16ピン外部コネクタの5Vピンに接続 |
| GND | GND | GNDに接続 |

#### 16ピン外部ヘッダの説明

| 名前| 機能| 説明 |
| :---: | :---: | :--- |
| SDA GP21 | I/O | ESP32 GPIO21に接続（標準SDA、外部プルアップ抵抗なし） |
| SCL GP22 | I/O | ESP32 GPIO22に接続（標準SCL、外部プルアップ抵抗なし） |
| GND      | GND | GNDに接続 |
| 3.3V     | I/O | 3.3V入力および出力。ESP32の3.3V入力および他の3.3V ICに接続。ボードがマイクロUSB、VIN外部コネクタまたはプログラミングコネクタ経由で5V電源供給されている場合、調整された出力電力。3.3VはLM1117-3.3の出力。LM1117の出力は100ufタンタルコンデンサで調整されている。 |
| GP13     | I/O | ESP32 GPIO13に接続（ディスプレイのMOSIも含む） |
| GP12     | I/O | ESP32 GPIO12に接続 |
| GP14     | I/O | ESP32 GPIO14に接続 |
| GP32     | I/O | ESP32 GPIO32に接続 |
| GP33     | I/O | ESP32 GPIO33に接続 |
| GP35     | I/O | ESP32 GPIO35に接続 |
| GP27     | I/O | ESP32 GPIO27に接続 |
| GP26     | I/O | ESP32 GPIO26に接続 |
| VIN      | I/O | VINはModdable Twoに電力を供給するために使用できる5Vピン。他の5V入力のいずれかでボードが電力供給されている場合、このピンは外部5V電源に使用できる。これは調整されていないピンであり、VINは5V入力ソースへの直接接続。その他の5V入力およびLM1117-3.3電圧レギュレータに接続。 |
| GP17     | I/O | ESP32 GPIO17に接続 |
| GP16     | I/O | ESP32 GPIO16に接続 |
| GP0      | I/O | ESP32 GPIO0に接続 |

#### 電源

Moddable Twoは3.3Vデバイスです。5V電源はLM1117-3.3電圧レギュレータによって3.3Vに調整されます（仕様についてはデータシートを参照してください）。Moddable Twoのテストは、一般的な5V 0.5アンペアのUSB電源で行われています。

Moddable Twoには以下の方法で電源を供給できます：

* 5V - Micro USBコネクタ
* 5V - Moddable Programmerコネクタ
* 5V - 16ピン外部ヘッダのVIN
* 3.3V - 3.3Vピン外部ヘッダ

<a id="dimensions"></a>
### 寸法

Moddable Twoの完全な寸法はこの[PDFドキュメント](../assets/devices/moddable-two-dimensions.pdf)に記載されています。これはModdable Twoのケースを設計する際に役立ちます。

<a id="setup"></a>
## SDKとホスト環境のセットアップ

Moddable Twoでアプリをビルドして実行するには、以下の手順が必要です：

1. [Moddable SDK](./../Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [ESP32ツール](./esp32.md)をインストールする
3. 以下の**アプリのビルドとデプロイ**セクションの指示に従う

<a id="building-and-deploying-apps"></a>
## アプリのビルドとデプロイ

ホスト環境とESP32ツールのセットアップが完了したら、以下の手順に従ってModdable Twoにアプリケーションをインストールします。

1. プログラマーをModdable Twoに接続します。

	プログラマーの向きが正しいことを確認してください。向きは以下の画像と一致する必要があります。

	<img src="../assets/devices/moddable-two-programmer.jpg">

	**注意**: プログラマーを使用せずに動作させる場合、Moddable TwoのUSBポートを電源供給に使用できます。このUSBポートはModdable Twoに電力を供給するためのものであり、プログラムの書き込みには使用できません。

2. プログラマーをマイクロUSBケーブルでコンピューターに接続します。

	USBケーブルはプログラマーに接続する必要があり、ボード上の電源専用USBポートには接続しないでください。データ同期が可能なケーブルを使用し、電源専用のケーブルは使用しないでください。

3. `mcconfig`を使用してアプリをビルドおよびデプロイします。

	`mcconfig`は、マイクロコントローラーおよびシミュレーター上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)で確認できます。

	プラットフォーム `-p esp32/moddable_two` を `mcconfig` と共に使用してModdable Two用にビルドします。例えば、[`piu/balls` のサンプル](../../examples/piu/balls)をビルドするには以下のようにします：

	```text
	cd $MODDABLE/examples/piu/balls
	mcconfig -d -m -p esp32/moddable_two
	```

	[サンプルの readme](../../examples) には、画面の回転、Wi-Fi設定など、他の一般的に使用される `mcconfig` 引数に関する追加情報が含まれています。

	プラットフォーム `-p simulator/moddable_two` を `mcconfig` と共に使用してModdable Twoシミュレータ用にビルドします。


<a id="troubleshooting"></a>
## トラブルシューティング

一般的な問題とその解決方法については、[ESP32 ドキュメント](./esp32.md#troubleshooting)のトラブルシューティングセクションを参照してください。

<a id="development-resources"></a>
## 開発リソース

<a id="examples"></a>
### サンプル

Moddable SDKには、その多くの機能を使用する方法を示す150以上の[サンプルアプリ](../../examples)があります。これらのサンプルの大部分はModdable Twoで動作します。

とはいえ、すべてのサンプルがModdable Twoハードウェアと互換性があるわけではありません。例えば、一部のサンプルはModdable Twoのディスプレイと互換性のない特定のディスプレイおよびタッチドライバをテストするために設計されており、ビルドエラーが発生します。

<a id="documentation"></a>
### ドキュメント

Moddable SDKのすべてのドキュメントは [documentation](../) ディレクトリにあります。**documentation**、**examples**、および **modules** ディレクトリは共通の構造を持っており、情報を簡単に見つけることができます。以下はそのハイライトの一部です：

- `commodetto` サブディレクトリには、2DグラフィックスAPIを提供するビットマップグラフィックスライブラリであるCommodettoと、軽量レンダリングエンジンであるPocoに関連するリソースが含まれています。
- `piu` サブディレクトリには、複雑でレスポンシブなレイアウトを簡単に作成できるユーザーインターフェースフレームワークであるPiuに関連するリソースが含まれています。
- `networking` サブディレクトリには、BLE、ネットワークソケット、およびHTTP/HTTPS、WebSockets、DNS、SNTP、telnetなどのソケット上に構築されたさまざまな標準的で安全なネットワーキングプロトコルに関連するネットワークリソースが含まれています。
- `pins` サブディレクトリには、サポートされているハードウェアプロトコル（デジタル、アナログ、PWM、I2Cなど）に関連するリソースが含まれています。一般的な市販センサー用のドライバと対応するサンプルアプリも多数用意されています。

<a id="backlight"></a>
### Backlight
オリジナルのModdable Twoには常時点灯のバックライトがあります。リビジョン2では、ソフトウェアでバックライトの明るさを調整する機能があります。バックライトの明るさ調整機能を持つModdable Twoユニットは、ボードの背面のModdableロゴの右側に小さく印刷された`ESP32 r9`で識別できます。

バックライトの制御はGPIO 18に接続されています。ホストの設定にはバックライトGPIOの定数が定義されています。

```javascript
import "config" from "mc/config";

Digital.write(config.backlight, 1);	// backlight ON
Digital.write(config.backlight, 0);	// backlight OFF
```

バックライトの明るさは、プロジェクトマニフェストの`config`セクションでビルド時に設定できます。デフォルトは100%です。

```javascript
"config": {
	"brightness": 75
}
```

`mcconfig`を使用してビルドする際に、コマンドラインで明るさを設定することもできます。ここでは50%に設定されています。

```text
mcconfig -d -m -p esp32/moddable_two brightness=50
```

Moddable Twoの`setup/target`モジュールは、コード内でバックライトを調整するために使用できる`backlight`という名前のグローバル変数をインストールします。ここでは80%に設定されています。

```javascript
backlight.write(80);
```

`backlight` グローバルには `PWM` のサブクラスのインスタンスが含まれています。`setup/target` がこの `PWM` インスタンスを作成しないようにするには、プロジェクトのマニフェストの `config` セクションで明るさを `"none"` に設定します。

```javascript
"config": {
	"brightness": "none"
}
```

**注意**: バックライトのサポートは `esp32/moddable_two` ビルドターゲットを使用するすべてのビルドに存在しますが、リビジョン2でのみ動作します。

<a id="support"></a>
### サポート

質問がある場合は、[issue を開く](https://github.com/Moddable-OpenSource/moddable/issues) ことをお勧めします。できるだけ早く対応し、他の開発者も助けを提供し、あなたの質問への回答から利益を得ることができます。多くの質問はすでに回答されているので、新しいissueを開く前に以前のissueを検索してみてください。

<a id="updates"></a>
### 更新情報

私たちの活動をフォローする最良の方法は、Twitter ([@moddabletech](https://twitter.com/moddabletech)) でフォローすることです。新しい投稿に関する発表や、その他のModdableニュースを [ブログ](http://blog.moddable.com/) で投稿しています。
