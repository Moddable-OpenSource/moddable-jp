# Moddable SDK - サンプル

Copyright 2018-2023 Moddable Tech, Inc.<BR>
改訂： 2023年4月6日

サンプルはModdable SDKの多くの機能を使用する方法を示しています。各ターゲットプラットフォームはユニークであるため、すべてのサンプルがすべてのプラットフォームやデバイスで実行されるわけではありません。

このドキュメントは、Moddable SDKでサンプルアプリケーションを構築するためのガイドです。`mcconfig` コマンドラインツールを使用してアプリを構築する方法、ターゲットプラットフォームのビルドを設定する方法、およびマイクロコントローラデバイスターゲットの異なる画面/タッチドライバとWi-Fi設定をサポートするためのビルドの変更方法について説明します。

> **注意**: このドキュメントは、開発マシンにModdable SDKがインストールされていることを前提としています。[Getting Started](../documentation/Moddable%20SDK%20-%20Getting%20Started.md)ドキュメントでは、SDKのインストール方法について説明しています。

## 目次

* [アプリのビルド](#building-apps)
* [ターゲットプラットフォームの指定](#target-platforms)
* [画面ピクセルフォーマット](#screen-pixel-formats)
* [画面回転](#screen-rotation)
* [Wi-Fi設定](#wifi-configuration)
* [画面ドライバー設定](#screen-driver-configuration)
* [プラットフォーム固有の例](#platform-specific)

<a id="building-apps"></a>
## アプリのビルド

`mcconfig` コマンドラインツールはModdableアプリをビルドします。使用するには、ターミナルウィンドウを開き、アプリのディレクトリに移動してビルドします。例えば、macOSまたはLinuxで `balls` アプリを実行するには：

	cd $MODDABLE/examples/piu/balls
	mcconfig -m

Windowsでは、**VS 2017用の開発者コマンドプロンプト**を使用してアプリをビルドします：

	cd %MODDABLE%\examples\piu\balls
	mcconfig -m

デフォルトでは、`mcconfig` はホストプラットフォームを対象としたリリースビルドを生成します。これらは一般的なコマンドラインオプションです：

- `-d`: デバッグ用のinstrumentedバージョンをビルド
- `-i`: リリース用のinstrumentedバージョンをビルド
- `-m`: 自動的にmakeを実行、それ以外の場合は `mcconfig` はmakeファイルを生成するだけ

例えば、ホストプラットフォームを対象としたアプリのデバッグバージョンをビルドするには：

	mcconfig -d -m

デバッグアプリのビルドのみが `xsbug`、XS JavaScriptソースレベルデバッガに接続されることに注意してください。

また、`mcconfig` は自動的にアプリを対象デバイスにデプロイします。多くのプラットフォームでは、デバイスにデバッグアプリをデプロイする際に `xsbug` デバッガが起動されます。

> `mcconfig`の詳細については、[tools.md](../documentation/tools/tools.md)ドキュメントを参照してください。

<a id="target-platforms"></a>
### ターゲットプラットフォームの指定

`-p` コマンドラインオプションは、ビルド対象のプラットフォーム/サブプラットフォームを指定します。例えば、Moddable Oneをターゲットにしたリリースアプリをビルドするには：

	mcconfig -m -p esp/moddable_one

ESP32デバイスをターゲットにしたデバッグアプリをビルドするには：

	mcconfig -d -m -p esp32

利用可能なプラットフォーム/サブプラットフォームの完全なリストについては、[Getting Started Guide](../documentation/Moddable%20SDK%20-%20Getting%20Started.md)の**プラットフォーム**セクションを参照してください。

<a id="screen-pixel-formats"></a>
### 画面ピクセルフォーマット

`mcconfig`はデフォルトで16ビットRGB565リトルエンディアンピクセルのビルドを行います。一部のサンプルアプリは、異なるピクセルフォーマットのeペーパーディスプレイやLCDスクリーンで実行するように設計されています。目的の画面ピクセルフォーマットを設定するには、`-f` コマンドラインオプションを使用します：

- `-f rgb565le`: 16ビットRGB565リトルエンディアン（デフォルト）
- `-f rgb332`: 8ビットRGB332
- `-f gray256`: 8ビットグレー
- `-f clut16`: 4ビットインデックスカラー
- `-f gray16`: 4ビットグレー

たとえば、ホストプラットフォーム用の8ビットグレースケールピクセルフォーマットで `balls` サンプルアプリのデバッグバージョンをビルドするには：

	cd $MODDABLE/examples/piu/balls
	mcconfig -d -m -f gray256

<a id="screen-rotation"></a>
### 画面回転

一部のサンプルアプリは、回転した画面でレンダリングするように設計されています。ターゲットの回転角度を指定するには、コマンドラインオプション `-r` を使用します：

- `-r 0`: 回転なし（デフォルト）
- `-r 90`: 90度回転
- `-r 180`: 180度回転
- `-r 270`: 270度回転

たとえば、`rotated` アプリは90度回転で動作するように設計されています。[Moddable Two](../documentation/devices/moddable-two.md) 用のデバッグバージョンをビルドするには：

	cd $MODDABLE/examples/commodetto/rotated
	mcconfig -d -m -r 90 -p esp32/moddable_two

ホストプラットフォームで180度回転して `progress` アプリをビルドするには：

	cd $MODDABLE/examples/commodetto/progress
	mcconfig -d -m -r 180

<a id="wifi-configuration"></a>
### Wi-Fi設定

ネットワーク接続を必要とするサンプルアプリは、ターゲットデバイスがWi-Fiアクセスポイントに接続するように設定する必要があります。アクセスポイントの認証情報を指定するには、`ssid` と `password` の設定オプションを使用します。Moddable SDKランタイムは、アプリの起動前に指定されたアクセスポイントに自動的に接続を試みます。

> **注意**: Wi-Fi設定はマイクロコントローラーターゲットのみでサポートされています。

例えば、ネットワーク`friedkin`でパスワード`tacos`を使用して`httpget`アプリをビルドして実行するには：

	cd $MODDABLE/examples/network/httpget
	mcconfig -d -m -p esp ssid=friedkin password=tacos

オープンネットワーク`Free Wifi`で`sntp`アプリをビルドして実行するには：

	cd $MODDABLE/examples/network/sntp
	mcconfig -d -m -p esp ssid="Free WiFi"

<a id="screen-driver-configuration"></a>
### スクリーンドライバの設定

スクリーンドライバの設定はマイクロコントローラーターゲットでサポートされています。`screen`および`touch`設定オプションを使用して、スクリーンまたはタッチドライバを指定します。

上記の**ターゲットプラットフォーム**セクションにリストされているプラットフォームのいずれかでアプリケーションをビルドする場合、`screen`および`touch`設定オプションを使用すべきではありません。これらは自動的に設定されます。しかし、異なるディスプレイまたはタッチドライバを使用したい場合は、次の2つのステップが必要です：

1. スクリーンまたはタッチドライバを指定するために`screen`および`touch`設定オプションを変更する
2. ドライバを含むようにアプリケーションのマニフェストを更新する

このセクションの残りは、これらのステップを実行する方法を説明します。

#### ステップ 1: `screen` と `touch` の設定オプションを使用する

[LPM013M126A](../modules/drivers/lpm013m126a) 8色ディスプレイを搭載したESP8266デバイスで `text` アプリを実行するには：

	cd $MODDABLE/examples/commodetto/text
	mcconfig -d -m -p esp screen=lpm013m126a
[FT6206](../modules/drivers/ft6206) マルチタッチコントローラーを搭載したESP8266デバイスで `map-puzzle` アプリを実行するには：

	cd $MODDABLE/examples/piu/map-puzzle
	mcconfig -d -m -p esp touch=ft6206

#### ステップ 2: ドライバを含む

各アプリケーションには、そのアプリを構築するために必要なモジュールとリソースを記述した `manifest.json` ファイルが含まれています。`config` セクションでは、アプリケーションで使用されるスクリーンとタッチドライバを指定します。コマンドラインで指定された `screen` と `touch` の設定オプションは、マニフェストに記載されているものを上書きします：

	"config": {
		"screen": "ili9341",
		"touch": "xpt2046"
	}

`include` セクションでは、アプリケーションのために構築されるドライバモジュールを指定します：

`ft6206` タッチコントローラドライバ用のアプリをビルドするためには、`manifest.json` ファイルの `config` と `include` セクションを変更します：

	"config": {
		"screen": "ili9341",
		"touch": "ft6206"
	}

	"include": [
		"$(MODDABLE)/modules/drivers/ili9341/manifest.json",
		"$(MODDABLE)/modules/drivers/ft6206/manifest.json"
	]

> **注記**: `manifest.json` ファイルの詳細については、[manifest.md](../documentation/tools/manifest.md) ドキュメントを参照してください。

<a id="platform-specific"></a>
## プラットフォーム固有の例

`$MODDABLE/build/device/<platform>/examples` にはプラットフォーム固有の機能の例が含まれています。

* [nRF52](../../build/devices/nrf52/examples)
* [ESP32](../../build/devices/esp32/examples)
