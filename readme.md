# Moddable SDK

Copyright 2017-2024 Moddable Tech, Inc., 2024 Shinya Ishikawa<BR>
改訂: 2024年4月3日

## マイクロコントローラーのための現代的なソフトウェア開発

**Moddable SDKは、マイクロコントローラー用のアプリケーションを作成するための開発ツールとランタイムソフトウェアの組み合わせです。**

マイクロコントローラーは、現代のコンピューターやモバイルデバイスと比較して大幅に制約があるデバイスです。Moddable SDKで使用される典型的なマイクロコントローラーは、約45 KBの空きメモリ、1 MBのフラッシュROMを持ち、80 MHzで動作します。Moddable SDKは、これらのデバイスで効率的に動作するために、ビルド時とランタイムの両方で多くの異なる技術を使用します。

<a href="https://tc39.es/ecma262/">![JS logo](./documentation/assets/moddable/js.png)</a>

開発の主要なプログラミング言語はJavaScriptです。Moddable SDKの中心にあるXS JavaScriptエンジンは、[2023年のJavaScript言語標準](https://262.ecma-international.org/14.0/)を99％以上準拠しています。<sup>[[1](#footnotes)]</sup> ターゲットマイクロコントローラーの制約により、単一のアプリケーションで組み合わせて使用できる言語機能の数が制限される場合があります。

Moddable SDKで実装されているJavaScript言語は、WebページやNode.jsで使用されるのと同じ言語です。ただし、スクリプトが実行されるマイクロコントローラーは、個人用コンピューター、サーバー、またはモバイルデバイスとは非常に異なります。これらの違いは、JavaScriptの使用方法に異なるアプローチを必要とすることが多いです。Moddable SDKのAPIやオブジェクトは、メモリ使用を最小限に抑えることを目的として設計されており、かなり異なります。JavaScriptに関する既存の経験を活かしつつ、パフォーマンス、コードサイズ、メモリ使用について異なる方法で考える準備をしてください。<sup>[[2](#footnotes)]</sup>

可能な限り、Moddable SDKはJavaScriptで実装されています。パフォーマンス向上やネイティブAPIへの直接アクセスのために、Moddable SDKの一部はCで実装されています。<sup>[[3](#footnotes)]</sup> C++は使用されていません。

マイクロコントローラー用の効率的なソフトウェアを構築する重要な部分は、ビルド時に発生します。Moddable SDKには、ビルドプロセスのための多くのツールとオプションが含まれています。<sup>[[4](#footnotes)]</sup> これらについて学ぶことで、最良の結果を得ることができます。

<a id="footnotes"></a>
> <sup>[1]</sup> *参照: [XS 準拠](./documentation/xs/XS%20Conformance.md)*<BR>
<sup>[2]</sup> *参照: [XS の違い](./documentation/xs/XS%20Differences.md)*<BR>
<sup>[3]</sup> *参照: [C言語でのXS](./documentation/xs/XS%20in%20C.md)*<BR>
<sup>[4]</sup> *参照: [ツール](./documentation/tools/tools.md), [マニフェスト](documentation/tools/manifest.md)*

## 主要機能

### ネットワーキング

Moddable SDKはネットワークソケットと、HTTP/HTTPS、WebSockets、MQTT、mDNS、DNS、SNTP、およびtelnetを含む様々な標準的なセキュアなネットワーキングプロトコルを実装しています。

また、BLE周辺機器および中央デバイス開発のためのBluetooth Low Energy（BLE）プロトコルのサポートもあります。

### グラフィックス

現代のユーザーインターフェースを構築するための2つのAPIが利用可能です：

- **Commodetto**、ビットマップグラフィックスライブラリで、2DグラフィックスAPIを提供します。Commodettoには、軽量な**Poco**レンダリングエンジンが含まれており、フレームバッファを必要とせずに一度に1つのスキャンラインを効率的にレンダリングするディスプレイリストレンダラーです。
- **Piu**、Commodettoの上に構築されたユーザーインターフェースフレームワークです。Piuはオブジェクトベースのフレームワークで、複雑でレスポンシブなレイアウトを作成するのを容易にします。

Moddable SDKには、画像形式の変換、画像圧縮、画像回転、フォント圧縮、ローカリゼーションなどのコマンドラインツールも含まれています。ビルドシステムは自動的にこれらのツールを使用します。

### ハードウェア

Moddable SDKはデジタル（GPIO）、アナログ、PWM、およびI2Cを含む様々なハードウェアプロトコルを実装しています。一般的な市販のセンサー用の多数のドライバーと[対応する例示アプリ](./examples/drivers)も利用可能です。

### ソースレベルデバッガー

`xsbug` JavaScriptソースレベルデバッガーは、XSプラットフォームのモジュールおよびアプリケーションのデバッグをサポートするフル機能を備えたデバッガーです。

他のデバッガーと同様に、`xsbug`はブレークポイントの設定、ソースコードの閲覧、コールスタックと変数の検査をサポートします。さらに、`xsbug`デバッガーはリアルタイムの計測を提供し、メモリ使用量を追跡し、アプリケーションとリソース消費のプロファイルを提供します。

> *参照: [xsbug ドキュメント](./documentation/xs/xsbug.md)*

## 入門 {/*examples*/}

1. Moddable SDK を使用するには、まずコンピュータにインストールする必要があります。これには、このリポジトリのダウンロード、開発ツールのインストール、コマンドラインでの設定の構成、および Moddable SDK ツールのビルドが含まれます。

	`documentation` ディレクトリの [入門ガイド](./documentation/Moddable%20SDK%20-%20Getting%20Started.md) では、Moddable SDK のインストールプロセス全体を詳しく説明しています。

2. Moddable SDK をインストールしたら、ハードウェアシミュレータでアプリをビルドして実行できます。

3. 特定のデバイスで開発するには、そのデバイス用の追加ツールと SDK をインストールする必要があります。各デバイスのセットアッププロセスは異なりますが、通常は追加の SDK、ドライバー、および開発ツールをインストールすることを含みます。

特定のデバイスの SDK とツールは Moddable によって作成または維持されていませんが、それらをインストールし、Moddable SDK での開発を開始するための詳細な指示を提供しています。以下のセクションでは、サポートしているデバイスのいくつかのセットアップ手順/開発ガイドへのリンクを提供しています。

### ハードウェアシミュレータ

<a href="./documentation/tools/tools.md#simulator"><img src="./documentation/assets/tools/screen-test.png" width=100></a>

Moddable SDK には、macOS、Linux、Windows で動作するシミュレータが含まれています。これらは入門に最適であり、開発の加速にも非常に価値があります。

シミュレータを使用するには、[Moddable SDK](./documentation/Moddable%20SDK%20-%20Getting%20Started.md) をインストールするだけです。

シミュレータに関する [ビデオデモンストレーション](https://www.youtube.com/watch?v=7GKOm3Tayjs) および情報は、[ツールドキュメント](./documentation/tools/tools.md) の **シミュレータ** セクションで利用可能です。

### Espressif の ESP8266

ESP8266 ベースのデバイスで Moddable SDK を使用するには：

1. [Moddable SDK](./documentation/Moddable%20SDK%20-%20Getting%20Started.md) をインストールします。
2. [ESP8266 ツール](./documentation/devices/esp8266.md) をインストールします。

Moddable SDKはESP8266をベースにした[多くのデバイス](./documentation/devices/esp8266.md#platforms)をサポートしており、以下に示すデバイスが含まれます。

| | | |
| :---: | :---: | :---: |
| <a href="./documentation/devices/moddable-one.md"><img src="./documentation/assets/devices/moddable-one.png" width=125></a><BR>Moddable One<sup>[[5](#footnotes2)]</sup> | <a href="./documentation/devices/esp8266.md"><img src="./documentation/assets/devices/esp8266.png" width=125></a><BR>Node MCU ESP8266<sup>[[6](#footnotes2)]</sup> | <a href="./documentation/devices/moddable-three.md"><img src="./documentation/assets/devices/moddable-three.png" width=125></a><BR>Moddable Three<sup>[[7](#footnotes2)]</sup>

<a id="footnotes2"></a>
> <sup>[5]</sup> *参照: [Moddable One 開発者ガイド](./documentation/devices/moddable-one.md), Moddable [製品ページ](https://www.moddable.com/product)*<BR>
<sup>[6]</sup> *参照: [ESP8266でModdable SDKを使用する](./documentation/devices/esp8266.md)*<BR>
<sup>[7]</sup> *参照: [Moddable Three 開発者ガイド](./documentation/devices/moddable-three.md)*<BR>

### EspressifのESP32

ESP32ベースのデバイスでModdable SDKを使用するには、以下の手順が必要です:

1. [Moddable SDKをインストールする](./documentation/Moddable%20SDK%20-%20Getting%20Started.md)
2. [ESP32ツールをインストールする](./documentation/devices/esp32.md)

Moddable SDKはESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-H2をベースにした[多くのデバイス](./documentation/devices/esp32.md#platforms)をサポートしており、以下に示すデバイスが含まれます。

| | | |
| :---: | :---: | :---: |
| <a href="./documentation/devices/moddable-two.md"><img src="./documentation/assets/devices/moddable-two.png" width=125></a><BR>Moddable Two<sup>[[8](#footnotes3)]</sup> | <a href="./documentation/devices/esp32.md"><img src="./documentation/assets/devices/esp32.png" width=125></a><BR>Node MCU ESP32<sup>[[9](#footnotes3)]</sup> | <a href="./documentation/devices/esp32.md#platforms"><img src="./documentation/assets/devices/m5stack.png" width=125></a><BR>M5Stack
| <a href="./documentation/devices/esp32.md#platforms"><img src="./documentation/assets/devices/m5stack-fire.png" width=125></a><BR>M5Stack Fire | <a href="./documentation/devices/esp32.md#platforms"><img src="./documentation/assets/devices/m5stick-c.png" width=125></a><BR>M5Stick C |  <a href="./documentation/devices/esp32.md#platforms"><img src="./documentation/assets/devices/m5atom.png" width=125></a><BR>M5Atom Matrix

<a id="footnotes3"></a>
> <sup>[8]</sup> *参照: [Moddable Two 開発者ガイド](./documentation/devices/moddable-two.md), Moddable [製品ページ](https://www.moddable.com/product)*<BR>
<sup>[9]</sup> *参照: [ESP32でModdable SDKを使用する](./documentation/devices/esp32.md)*<BR>

### Raspberry PiのPico

PicoデバイスでModdable SDKを使用するには、以下の手順が必要です:

1. [Moddable SDK](./documentation/Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [Picoツール](./documentation/devices/pico.md)をインストールする

Moddable SDKは、以下に示すボードを含むRaspberry Pi Picoで構築された[多くのデバイス](./documentation/devices/pico.md#platforms)をサポートしています:

| | | |
| :---: | :---: | :---: |
| <a href="./documentation/devices/pico.md"><img src="./documentation/assets/devices/pi-pico_w.png" width=65></a><BR>Raspberry Pi<BR>Pico W | <a href="./documentation/devices/pico.md"><img src="./documentation/assets/devices/pico-display.png" width=65></a><BR>Pimoroni<BR>Pico Display | <a href="./documentation/devices/pico.md"><img src="./documentation/assets/devices/pico-ili9341.png" width=120></a><BR>ili9341
| <a href="./documentation/devices/pico.md"><img src="./documentation/assets/devices/pico-adafruit-qt-py-rp2040.png" width=85></a><BR>Adafruit QT Py | <a href="./documentation/devices/pico.md"><img src="./documentation/assets/devices/pico-sparkfun-pro-micro-rp2040.png" width=160></a><BR>Sparkfun Pro Micro | <a href="./documentation/devices/pico.md"><img src="./documentation/assets/devices/pico-lilygo-t-display-rp2040.png" width=160></a><BR> LILYGO T-Display


### Nordic SemiconductorのnRF52

nRF52デバイスでModdable SDKを使用するには、以下の手順が必要です:

1. [Moddable SDK](./documentation/Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [nRF5ツール](./documentation/devices/nrf52.md)をインストールする
3. [ブートローダー](./documentation/devices/nrf52.md#install-bootloader)をインストールする。これはModdable Fourには不要です。Moddable Fourにはブートローダーがプリインストールされています。

Moddable SDKは、以下に示すボードを含むnRF52で構築された[多くのデバイス](./documentation/devices/nrf52.md#platforms)をサポートしています:

| | | |
| :---: | :---: | :---: |
| <a href="./documentation/devices/moddable-four.md"><img src="./documentation/assets/devices/moddable-four.png" width=125></a><BR>Moddable Four<sup>[[10](#footnotes4)]</sup> | <a href="./documentation/devices/nrf52.md"><img src="./documentation/assets/devices/nrf52-pca10056.png" width=125></a><BR>Nordic DK pca10056 | <a href="./documentation/devices/nrf52.md"><img src="./documentation/assets/devices/nrf52-sparkfun.png" width=125></a><BR>Sparkfun Pro nRF52840 |
| <a href="./documentation/devices/nrf52.md"><img src="./documentation/assets/devices/nrf52-makerdiary.png" width=125></a><BR>Makerdiary nRF52840 MDK | <a href="./documentation/devices/nrf52.md"><img src="./documentation/assets/devices/nrf52-xiao.png" width=125></a><BR>seeed XIAO-nRF52840 | <a href="./documentation/devices/nrf52.md"><img src="./documentation/assets/devices/nrf52-itsybitsy.png" width=125></a><BR>Adafruit ItsyBitsy nRF52840 |

<a id="footnotes4"></a>
> <sup>[10]</sup> *参照: [Moddable Four 開発者ガイド](./documentation/devices/moddable-four.md), Moddable [製品ページ](https://www.moddable.com/product)*<BR>


### シリコンラボのGecko

Moddable SDKをGeckoデバイスで使用するには、以下の手順が必要です:

1. [Moddable SDKのインストール](./documentation/Moddable%20SDK%20-%20Getting%20Started.md)
2. [Geckoツールのインストール](./documentation/devices/gecko/GeckoBuild.md)

以下の開発者リソースも利用可能です:

- [Silicon Labs GeckoのModdable SDKサポート](https://blog.moddable.com/blog/gecko/) ブログポスト
- [Moddable SDKでBluetooth Low Energyサポートが利用可能に](https://blog.moddable.com/blog/ble/) ブログポスト

Moddable SDKは以下の四つのGeckoボードをサポートしています。

| | | | |
| :---: | :---: | :---: | :---: |
| <a href="./documentation/devices/gecko/GeckoBuild.md"><img src="https://www.silabs.com/content/dam/siliconlabs/images/products/microcontrollers/32-bit_mcus/giant_gecko/giant-gecko-starter-kit.jpg" width=125></a><BR>Giant Gecko | <a href="./documentation/devices/gecko/GeckoBuild.md"><img src="https://blog.moddable.com/blog/wp-content/uploads/2018/05/gecko-1-300x179.jpg" width=125></a><BR>Mighty Gecko | <a href="./documentation/devices/gecko/GeckoBuild.md"><img src="https://siliconlabs-h.assetsadobe.com/is/image//content/dam/siliconlabs/images/products/Bluetooth/thunderboard/thunderbord-sense-down-2.jpg?" width=125></a><BR>Thunderboard Sense 2 | <a href="./documentation/devices/gecko/GeckoBuild.md"><img src="https://siliconlabs-h.assetsadobe.com/is/image//content/dam/siliconlabs/images/products/Bluetooth/bluetooth_low_energy/bluegiga_bluetooth_low_energy_modules/blue-gecko-soc-kit.jpg?" width=125></a><BR>Blue Gecko

### QualcommのQCA4020

<a href="./documentation/devices/qca4020/README.md"><img src="https://blog.moddable.com/blog/wp-content/uploads/2019/04/qca4020-image.png" width=175></a>

Moddable SDKをQCA4020で使用するには、以下の手順が必要です：

1. [Moddable SDKのインストール](./documentation/Moddable%20SDK%20-%20Getting%20Started.md)
2. [QCA4020ツールのインストール](./documentation/devices/qca4020/README.md)

以下の開発者リソースも利用可能です：

- Moddableによる[Qualcomm QCA4020がModdable SDKに対応](https://blog.moddable.com/blog/qca4020/)したブログ投稿
- Qualcomm Developer Network上の[QCA4020 Moddable SDKでの入門](https://developer.qualcomm.com/project/qca4020-getting-started-moddable-sdk)ブログ投稿
- Qualcomm Developer Network上の[QCA4020 Moddable SDKを使用した現代的なUIアプリケーション開発](https://developer.qualcomm.com/project/qca4020-modern-ui-application-development-moddable-sdk)ブログ投稿

### WebAssembly (Wasm)

<a href="./documentation/devices/wasm.md">![](./documentation/assets/devices/wasm.gif)</a>

WebAssemblyは、コンピュータやモバイルデバイスの現代のWebブラウザでサポートされています。Moddable SDKのWasmサポートは、Safari、FireFox、Chromeブラウザと互換性があります。

Moddable SDKをWebAssemblyで使用するには、以下の手順が必要です：

1. ホストプラットフォーム用の[Moddable SDKのインストール](./documentation/Moddable%20SDK%20-%20Getting%20Started.md)
2. [Wasmツールのインストール](./documentation/devices/wasm.md)

## ディスプレイ

<a href="http://www.moddable.com/display">![Displays](./documentation/assets/moddable/displays.jpg)</a>

ESP8266とESP32で様々なSPIディスプレイをテストしました。これらのディスプレイのビデオデモは[当社のウェブサイトにあります](http://www.moddable.com/display)。配線ガイドは[documentation/displays](./documentation/displays)ディレクトリにあります。対応するディスプレイおよびタッチドライバのソースコードは[modules/drivers](./modules/drivers)ディレクトリにあります。

## ソースツリー

Moddable SDKリポジトリには、以下のトップレベルディレクトリが含まれています：

- [**build**](./build): 特定のマイクロコントローラーターゲット、シミュレーター、および `tools` ディレクトリのビルドツール用の make ファイルが必要なファイル。
- [**contributed**](./contributed): Moddable API を使用するための有用な技術を示す非公式のプロジェクトとモジュール。
- [**documentation**](./documentation): Moddable SDK のすべてのドキュメント。ドキュメントはマークダウン形式で提供されます。
- [**examples**](./examples): Moddable SDK のさまざまな機能のサンプルアプリ。[readme.md](examples/readme.md) ドキュメントは、サンプルのビルド方法と、目的の種類のサンプルを見つけるのに役立つ情報を提供します。
- [**license**](./license): Moddable SDK に含まれるソフトウェアのライセンス契約。コントリビューターライセンス契約もここにあります。
- [**modules**](./modules): Moddable SDK のランタイムを構成するソフトウェアモジュール。これには、通信、グラフィックス、ユーザーインターフェース、ハードウェアアクセス、暗号プリミティブ、およびデバイスドライバーが含まれます。すべてのモジュールには JavaScript API があります。多くのモジュールは部分的に C で実装されています。
- [**tools**](./tools): Moddable SDK を使用してアプリケーションを構築するためのツール。これには、画像形式の変換、画像圧縮、画像回転、フォント圧縮、ローカライゼーション文字列の処理、リソースのコンパイル、JSON マニフェストファイルからのアプリケーションのビルド用のコマンドラインツールが含まれます。さらに、XSデバッガーである xsbug もここにあります。
- [**xs**](./xs): コンパイラーとリンカーを含む XS JavaScript エンジン、および test262 実行シェル。

## API ドキュメント

Moddable SDK によってサポートされる JavaScript API は、[documentation](./documentation) ディレクトリの一連のドキュメントで文書化されています。ドキュメントは広範なリファレンスで、多数の例があります。主要な [Piu ドキュメント](./documentation/piu/piu.md) だけで100ページ以上あります。すべてのドキュメントはマークダウン形式で提供されます。

そのディレクトリの [readme](./documentation#api-documentation-for-modules) ドキュメントで API ドキュメントの概要をご覧ください。

## リソース {/*resources*/}

このリポジトリのドキュメントと例に加えて、開発者向けのいくつかのリソースがあります。

| | |
| :---: | :---|
|<img src="https://miro.medium.com/fit/c/262/262/0*Vrsgi6r4Y0T_P9nd.png" width=50> | 私たちの [Gitterチャットルーム](https://gitter.im/embedded-javascript/moddable) は、Moddableチームのメンバーや他の開発者とModdable SDKについて質問したり議論したりするのに最適な場所です。 |
| <img src="https://static.thenounproject.com/png/1453176-200.png" width=60> | [Moddableブログ](https://blog.moddable.com) には、Moddableの新機能や興味深いプロジェクト、Moddable SDKやXS JavaScriptエンジンの重要なアップデートについての詳細な投稿が掲載されています。
| <img src="https://www.moddable.com/images/book/book-hero-full-vert.png" width=200> | [IoT Development for ESP32 and ESP8266 with JavaScript](https://www.moddable.com/book) は、Moddableチームのメンバーである [Peter Hoddie](https://www.moddable.com/peter-hoddie) と [Lizzie Prader](https://www.moddable.com/lizzie-prader) によって書かれた本です。
| <img src="https://m.media-amazon.com/images/I/51ak-nNiHdL.jpg" width=70> | [実践Moddable JavaScriptではじめるIoTアプリケーション (技術の泉シリーズ](https://meganetaaan.hatenablog.com/entry/2020/09/13/011403) は、開発者の石川真也によって書かれた本です。
| <img src="https://static.thenounproject.com/png/2067146-200.png" width=50> | Moddable SDKのアップデートに関する [詳細なリリースノート](https://github.com/Moddable-OpenSource/moddable/releases) は、定期的にGitHubに投稿されます。 |
| <img src="./documentation/assets/getting-started/ts-logo-256.svg" width=50> | Moddable SDKのビルドツールはTypeScriptをサポートしているため、組み込みコードでビルド時の型チェックを使用できます。詳細は [ブログ](https://blog.moddable.com/blog/typescript/) にて。

## ライセンス {/*licensing*/}

Moddable SDKは、GPL 3.0、LGPL 3.0、Apache 2.0、およびCreative Commons Attribution 4.0ライセンスを含むライセンスの組み合わせの下で提供されています。[ライセンス](./licenses) ディレクトリには使用されているライセンスとライセンスオプションに関する追加情報が含まれています。Moddableウェブサイトの [ライセンス記事](http://www.moddable.com/license) では、追加の背景と商用ライセンスオプションを説明しています。

## セキュリティ問題 {#security-issues}

Moddable SDKにセキュリティホールがないように努めています。しかし、すべてを排除することはほぼ不可能です。セキュリティ研究者の方々が当社のソフトウェアに潜在的なセキュリティ問題を特定した場合は、[問題を報告する](./issues)ことを奨励します。これらにはできるだけ迅速に対応します。もし希望される場合は、[メール](mailto:info@moddable.com)でセキュリティ問題を報告することもできます。

## 質問がありますか？気軽にお問い合わせください {#questions-we-are-here-to-help}

個人開発者の場合、質問があるか、プロジェクトについて話したい場合は、[ディスカッションを開始する](./discussions)か、[Gitterチャットルーム](https://gitter.im/embedded-javascript/moddable)に参加することをお勧めします。始めるのに困ったり、バグを見つけたりした場合は、[問題を報告してください](./issues)。できるだけ早く対応しますし、他の開発者もあなたの質問に対する回答から利益を得ることができます。

<a href="http://www.moddable.com">![Moddableロゴ](./documentation/assets/moddable/moddable.png)</a>

Moddableについてもっと知るには、[当社のウェブサイト](http://www.moddable.com)をご覧ください。

自社製品へのJavaScriptとModdable SDKの活用に関心のある企業向けに、Moddableはコンサルティングサービスを提供しています。設計、実装、トレーニング、サポートを支援します。

Twitter（[@moddabletech](https://twitter.com/moddabletech)）でもお問い合わせいただけます。新しいブログ投稿のアナウンスなど、Moddableの最新情報を知る最良の方法です。

直接メールでお問い合わせも可能です：[info@moddable.com](mailto:info@moddable.com).
