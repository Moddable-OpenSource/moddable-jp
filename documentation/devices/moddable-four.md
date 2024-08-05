# Moddable Four 開発者ガイド
Copyright 2021-2024 Moddable Tech, Inc.<BR>
改訂： 2024年4月20日

このドキュメントは、Moddable Fourに関する情報を提供します。ピンやその他のコンポーネントの詳細、アプリのビルドとデプロイ方法、その他の開発リソースへのリンクが含まれています。

## Table of Contents

- [Moddable Fourについて](#about-moddable-four)
	- [コンポーネント](#components)
	- [ピン配置](#pinout)
	- [ピン情報](#pin-info)
- [SDKとホスト環境のセットアップ](#setup)
- [アプリのビルドとデプロイ](#building-and-deploying-apps)
- [Moddable Fourの使用](#moddable-features)
- [トラブルシューティング](#troubleshooting)
- [開発リソース](#development-resources)
	- [シミュレーター](#simulator)
	- [サンプル](#examples)
	- [ドキュメント](#documentation)
	- [サポート](#support)
	- [更新](#updates)

<a id="about-moddable-four"></a>
## Moddable Fourについて

<img src="../assets/devices/moddable-four.png">

Moddable Fourは、低消費電力のBluetooth LE開発ボードで、開発者がModdable SDKを使って実験するのを容易にします。[Moddableのウェブサイト](http://www.moddable.com/purchase)で購入できます。

<a id="components"></a>
### コンポーネント

Moddable Fourの主なコンポーネントは、nRF52840モジュールとミラーディスプレイの2つです。nRF52840モジュールには、BLEアンテナ、1 MBフラッシュ、および256 KB RAMが含まれています。シャープのミラーディスプレイは、[`ls013b4dn04`ディスプレイドライバ](../drivers/ls013b4dn04/ls013b4dn04.md)を使用する128x128の白黒ディスプレイです。

また、統合されたLIS3DH加速度計、ジョグダイヤル、およびCR2032バッテリコネクタも含まれています。

<a id="pinout"></a>
### ピン配置

<img src="../assets/devices/moddable-four-pinout.png">

**注意:** LCD-PWR / GPIO23は一般的なGPIOとして使用しないでください。これはセンサーと画面に電力を供給するために使用されます。

- GPIO23に`0`を書き込むと、LCD-PWRに3.3Vが出力され、画面にも電力が供給されます。
- GPIO23に`1`を書き込むと、ピンと画面の電源がオフになります。

<a id="pin-info"></a>
### ピン情報

#### 16ピン外部ヘッダの説明

| 名前 | 機能 | 説明 |
| :---: | :---: | :--- |
| LCD-PWR<BR>GPIO23 | 電源 | GPIO23は、このピンに接続された画面とセンサーの電源をオンにするために使用されます。3.3v |
| GPIO3<BR>ADC1 | I/O<BR>アナログ | nrf52 GPIO3に接続。ADCチャネル1 |
| GPIO4<BR>ADC2 | I/O<BR>アナログ | nrf52 GPIO4に接続。ADCチャネル2 |
| GPIO5<BR>ADC3 | I/O<BR>アナログ | nrf52 GPIO5に接続。ADCチャネル3 |
| GPIO17 | I/O | nrf52 GPIO17に接続 |
| GPIO22 | I/O | nrf52 GPIO22に接続 |
| GPIO28<BR>ADC4 | I/O<BR>アナログ | nrf52 GPIO28に接続。ADCチャネル4 |
| GPIO29<BR>ADC5 | I/O<BR>アナログ | nrf52 GPIO29に接続。ADCチャネル5 |
| GPIO30<BR>ADC6 | I/O<BR>アナログ | nrf52 GPIO30に接続。ADCチャネル6 |
| GPIO31<BR>ADC7 | I/O<BR>アナログ | nrf52 GPIO31に接続。ADCチャネル7 |
| GPIO27<BR>SCL | I/O<BR>I2C | nrf52 GPIO27に接続<BR>I2Cクロック<BR>加速度計 |
| GPIO26<BR>SDA | I/O<BR>I2C | nrf52 GPIO26に接続<BR>I2Cデータ<BR>加速度計 |
| RST | リセット | デバイスをリセット |
| VIN | 電源 | VINはModdable Fourに電力を供給するために使用できる5Vピンです。ボードがUSBで電源供給されている場合、このピンを使用してその5V電力を供給できます。これは未調整のピンであり、VINは5V入力ソースへの直接接続です。他の5V入力およびAP2112K電圧レギュレータに接続します。 |
| GND | GND | GNDに接続 |
| 3.3V | 電源 | 3.3V入力および出力。nRF52 3.3V入力および他の3.3Vデバイスに接続。ボードがマイクロUSB、VIN外部コネクタ、またはUSBで5V電源供給されている場合、調整された出力電力。3.3VはCR2032コインセルでも供給可能。 |

#### 電源

Moddable Fourは3.3Vデバイスです。5V電源はAP2112K-3.3電圧レギュレータによって3.3Vに調整されます（仕様についてはデータシートを参照してください）。Moddable Fourのテストは、一般的な5V 0.5アンペアのUSB電源で行われています。

Moddable Fourには以下の方法で電源を供給できます：

* 5V - Micro USBコネクタ
* 5V - ピン14または外部ヘッダのVIN
* 3.3V - 外部ヘッダのピン16の3.3V
* 3V - CR2032バッテリー

CR2032バッテリーコネクタから供給される電源は、スライドスイッチでオンとオフを切り替えることができます。

#### 6ピンJTAGコネクタ

6ピンJTAGコネクタは、gdbデバッグや新しいブートローダーをModdable Fourにフラッシュするために使用できます。詳細については、[nrf52プラットフォームドキュメント](./nrf52.md#debugging-native-code)を参照してください。

<a id="setup"></a>
## SDKとホスト環境のセットアップ

Moddable Fourでアプリをビルドして実行するには、以下が必要です：

1. [Moddable SDK](./../Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [nRF5ツール](./nrf52.md)をインストールする
3. 以下の**アプリのビルドとデプロイ**セクションの指示に従う

<a id="building-and-deploying-apps"></a>
### アプリのビルドとデプロイ

環境とnRF5ツールをセットアップした後、以下の手順に従ってModdable Fourにアプリケーションをインストールします。

1. Moddable FourをマイクロUSBケーブルでコンピュータに接続します。

	データ同期が可能なケーブルを使用していることを確認してください。電源供給のみのケーブルは使用しないでください。

2. RESETボタンをダブルタップしてデバイスをプログラミングモードにします。

	プログラミングモードは、起動時にLEDインジケーターが定期的に点滅することで示されます。また、デスクトップに`MODDABLE4`という名前のディスクが表示されます。

	![Moddable Four disk](../assets/devices/moddable-four-M4-disk.png)

	> **注意:** 短時間内にデバイスをプログラムしない場合、インストールされたアプリケーションに再起動されます。

3. `mcconfig`を使用してアプリをビルドおよびデプロイします。

	`mcconfig`は、マイクロコントローラおよびシミュレータ上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)にあります。

	Moddable Four用にビルドするには、`mcconfig`でプラットフォーム`-p nrf52/moddable_four`を使用します。[`piu/balls`](../../examples/piu/balls)のサンプルをビルドします：

	```text
	cd $MODDABLE/examples/piu/balls
	mcconfig -d -m -p nrf52/moddable_four
	```

	[examples readme](../../examples) には、画面の回転などによく使用される他の `mcconfig` 引数に関する追加情報が含まれています。

    `mcconfig` でModdable Fourシミュレータ用にビルドするには、プラットフォーム -p `sim/moddable_four` を使用します。

<a id="moddable-features"></a>
### Moddable Four の使用

Moddable Fourのハードウェアとソフトウェアは、追加のハードウェアなしで多くの種類のアプリケーションをサポートするように慎重に設計されています。ハードウェアの特徴には以下が含まれます：

* LED
* バックボタン
* ジョグダイヤル
* ディスプレイ電源
* ディスプレイディザー
* ディスプレイ回転
* 加速度計
* エネルギー管理

#### LED とバックボタン
このコードスニペットは、バックボタンが押されたときにLEDを点灯させるためのModdable FourのHostオブジェクトの使用方法を示しています。

```
let led = new Host.LED.Default;

new Host.Button({
    pin: Host.pins.button,
    onPush() {
        led.write(this.pressed);
    }
});
```

#### ジョグダイヤル

```
new Host.JogDial({
    onTurn(delta) {
	    trace(`Turn ${delta}\n`);
    },
    onPushAndTurn(delta) {
        trace(`Push and Turn ${delta}\n`);
    },
    onPush(value) {
        trace(`Button ${value}\n`);
    }
});
```

**注**: ジョグダイヤルによって提供されるデルタ値は、画面の回転（[ディスプレイ回転](#display-rotation) を参照）を考慮しており、画面の回転が180度の場合には反転されます。

#### ディスプレイの電源
Moddable Fourのディスプレイを使用するには、LCDの電源ピンを有効にする必要があります。`moddable_four/setup-target.js` ファイルでは、`autobacklight` 設定変数が設定されている場合に画面が有効になります：

```
if (config.autobacklight)
    Digital.write(config.lcd_power_pin, 0);
```

#### ディスプレイのディザリング
Moddable Fourのシャープメモリーディスプレイはls013b4dn04ディスプレイドライバを使用しています。デフォルトでは、ディスプレイドライバは8ビットグレーから白黒への変換時に画像をディザリングするように設定されています。ドライバは [Atkinson dither](https://en.wikipedia.org/wiki/Atkinson_dithering) アルゴリズムを使用しており、高速で高品質、アニメーションに適しています。

ディザリングのランタイムコストは低いですが、フレームレートを最大化し、計算を最小限に抑え、コードサイズとRAM使用量を削減したいアプリケーションでは、プロジェクトマニフェストで適切な定義を設定することで、ドライバ内のディザリングを完全に無効にすることができます：

```
    "defines": {
        "ls013b4dn04": {
            "dither": 0
        }
    },
```
一部のアプリケーションでは、特定の画面でディザリングを有効にし、他の画面では無効にしたい場合があります。これらのアプリケーションは、ドライバ内でディザリングを有効にしたままにし、ランタイムでディザリングをオンおよびオフにするためにドライバの `dither` プロパティを使用します。

```
screen.dither = true;
screen.dither = false;
```

<a id="display-rotation"></a>
#### ディスプレイの回転
Moddable Fourのls013b4dn04ディスプレイドライバは、0度と180度の回転をサポートしています。180度の回転は、ディスプレイが逆さまに取り付けられている場合に便利です。デフォルトは0度です。回転はビルド時または実行時に設定できます。

ビルド時に回転を設定するには、マニフェストの`config`セクションに`driverRotation`を含めます：

```
	"config": {
		"driverRotation": 180
	},
```

実行時に回転を設定するには、グローバルの`screen`の`rotation`プロパティを変更します。

```js
screen.rotation = 180
```

実行時に`rotation`を設定しても、ディスプレイはすぐには更新されません。回転を設定した後、アプリケーションは強制的に更新を行う必要があります。

#### 加速度計

```
import Timer from "timer";

let acc = new Host.Accelerometer;

Timer.repeat(() => {
    let sample = acc.sample();
    trace(`sample x: ${sample.x}, y: ${sample.y}, z: ${sample.z}\n`);
}, 100);
```

**注意**: 加速度計の値は画面の回転（[ディスプレイの回転](#display-rotation)を参照）に従うため、画面の回転が180度の場合、`x`と`y`は反転します。

#### エネルギー管理

Moddable Fourは、コイン電池で長期間動作するように設計されています。ハードウェアは最大限のエネルギー効率を達成するために慎重に設計されています。以下は、さまざまな動作モードで使用されるエネルギーです：

- アイドルモード - 3.7 uA - RAMが維持され、ユーザー入力を待機中、画面やセンサーの読み取りの間。
- ディープスリープ - 1.85 uA - ソフトウェアは実行されていません。指定された期間後に自動的にウェイクアップします。保持RAMのみが維持されます。
- デジタルウェイク - 1.9 uA - ディープスリープと同様ですが、デジタル入力の状態変化でもウェイクアップします。
- アナログウェイク - 2.7 uA - ディープスリープと同様ですが、アナログ入力が指定された閾値を超えるとウェイクアップします。

Moddable Fourには多くのエネルギー管理APIが用意されています。これには以下が含まれます：

- ディープスリープ
- リテンションRAM
- デジタルウェイク
- タイマーウェイク
- アナログウェイク
- モーションウェイク（加速度計を使用）

<!--
	そのディレクトリには、例を説明するreadme.mdがあるはずです
-->

詳細については、[nRF52 Low Power Notes](./nRF52-low-power.md)を参照してください。さまざまなスリープおよびウェイクアップモードの例は、`$MODDABLE/build/devices/nrf52/examples/sleep`にあります。

<a id="troubleshooting"></a>
## トラブルシューティング

一般的な問題とその解決方法については、[nRF52 ドキュメント](./nrf52.md#troubleshooting)のトラブルシューティングセクションを参照してください。

<a id="development-resources"></a>
## 開発リソース

<a id="simulator"></a>
### シミュレーター
Moddable SDKシミュレーター、mcsimにはModdable Fourシミュレーターが含まれています。これを使用するには、`mcconfig` を使用してビルドする際に `sim/moddable_four` プラットフォームを使用します：

```
mcconfig -d -m -p sim/moddable_four
```

シミュレーターには、Moddable Fourの多くのユニークなハードウェア機能のコントロールが含まれています。表示メニューの「Show Controls」と「Hide Controls」を使用して、それらの表示を切り替えることができます。

<img src="../assets/devices/moddable-four-simulator.png">

また、コンピューターのキーボードを使用してジョグダイヤルとボタンを操作することもできます：

- ジョグダイヤル時計回り – 上矢印
- ジョグダイヤル反時計回り – 下矢印
- ジョグダイヤル押下 – エンター
- バックボタン – デリート

Moddable Fourシミュレーターは、Moddable Fourハードウェアがオフスクリーンで画像をレンダリングする方法に一致する8ビットグレースケールで画像をレンダリングします。Moddable Fourのディスプレイドライバーは、表示のために8ビットグレースケール画像をモノクローム（1ビット）に変換し、オプションでディザリングを行います。

<a id="examples"></a>
### サンプル

Moddable SDKには、その多くの機能を示す150以上の[サンプルアプリ](../../examples)があります。これらの多くのサンプルはModdable Fourで動作します。

CommodettoとPiuを使用する多くのサンプルは、カラーQVGAスクリーン用に設計されています。これらはModdable Four上でも動作しますが、レンダリング時に色がディザリングされ、一部の画面が切り取られることがあります。すべてのサンプルがModdable Fourハードウェアと互換性があるわけではありません。一部のサンプルは、Moddable Fourディスプレイと互換性のない特定のディスプレイおよびタッチドライバをテストするために設計されており、ビルドエラーが発生します。

<a id="documentation"></a>
### ドキュメント

nRF5デバイスとSDKのドキュメントは、[Nordic Semiconductor Infocenter](https://infocenter.nordicsemi.com/topic/struct_nrf52/struct/nrf52840.html)で見つけることができます。特に興味深いのは、Nordic nRF5 SDK v17.0.2のドキュメントで、[こちら](https://infocenter.nordicsemi.com/topic/struct_sdk/struct/sdk_nrf5_latest.html)から入手できます。

Moddable SDKのドキュメントは[documentation](../)ディレクトリにあります。**documentation**、**examples**、**modules**ディレクトリは共通の構造を持っており、情報を簡単に見つけることができます。以下はそのハイライトの一部です：

- [Using the Moddable SDK with nRF52](./nrf52.md)では、開発のためのセットアップ方法、サポートされているデバイスなどについて説明しています。
- [nRF52 Low Power Notes](./nRF52-low-power.md)では、消費電力を最小限に抑えてバッテリー寿命を最大化するための技術とAPIについて説明しています。
- `commodetto` [directory](../../examples/commodetto)には、2DグラフィックスAPIを提供するビットマップグラフィックスライブラリであるCommodettoと、軽量レンダリングエンジンであるPocoに関連するリソースが含まれています。
- `piu` [directory](../../examples/piu)には、複雑でレスポンシブなレイアウトを作成するためのユーザーインターフェースフレームワークであるPiuに関連するリソースが含まれています。
- `pins` [directory](../../examples/pins)には、サポートされているハードウェアプロトコル（デジタル、アナログ、PWM、I²Cなど）に関連するリソースが含まれています。一般的な市販センサーのドライバと対応するサンプルアプリも多数利用可能です。

<a id="support"></a>
### サポート

質問がある場合は、私たちがサポートします。バグに遭遇した場合は、[issueを開く](https://github.com/Moddable-OpenSource/moddable/issues)ことをお勧めします。質問がある場合は、[ディスカッションを開始](https://github.com/Moddable-OpenSource/moddable/discussions)するか、[Gitter](https://gitter.im/embedded-javascript/moddable#)で私たちに尋ねてください。できるだけ早く対応し、他の開発者もあなたの質問に対する回答から助けを得ることができます。多くの質問はすでに回答されているので、新しい質問をする前に過去のissueやディスカッションを検索してみてください。

<a id="updates"></a>
### 更新情報

私たちの活動をフォローする最良の方法は、Twitter ([@moddabletech](https://twitter.com/moddabletech)) でフォローすることです。新しい投稿に関するお知らせや、[ブログ](http://blog.moddable.com/)の他のModdableニュースを投稿しています。
