# Raspberry Pi Pico の使い方
Copyright 2021-2023 Moddable Tech, Inc.<BR>
改訂： 2023年5月30日

このドキュメントは、Raspberry Pi Pico用のModdableアプリケーションの構築を開始する方法を説明します。ホストビルド環境の設定方法、アプリのビルドおよびデプロイ方法に関する情報を提供し、外部開発リソースへのリンクを含みます。

> 注: Pico ポートは確かであり、ほぼ完成しています。将来的な作業には以下が含まれる可能性があります:
>
> - Mods
> - JavaScript Atomics
> - セカンドコアを利用する Web Workers
> - PIO 統合
> - Windows でのビルド

## 目次

- [Raspberry Pi Pico について](#about-pico)
- [概要](#overview)
- [プラットフォーム](#platforms)
- [macOS](#mac)
  - [SDK とホスト環境のセットアップ - macOS](#macos-setup)
  - [アプリのビルドとデプロイ - macOS](#macOS-building-and-deploying-apps)
  - [トラブルシューティング](#mac-troubleshooting)
- [Windows](#win)
  - [SDK とホスト環境のセットアップ - Windows](#windows-setup)
  - [アプリのビルドとデプロイ - Windows](#windows-building-and-deploying-apps)
- [Linux](#lin)
  - [SDK とホスト環境のセットアップ - Linux](#linux-setup)
  - [アプリのビルドとデプロイ - Linux](#linux-building-and-deploying-apps)
- [ネイティブコードのデバッグ](#debugging-native-code)
- [参考文書](#reference)

<a id="about-pico"></a>
## Raspberry Pi PicoおよびPico Wについて

<img src="../assets/devices/pi-pico.png" width="300">
<img src="../assets/devices/pi-pico_w.png" height="300">

詳細については、[Raspberry Pi Picoのドキュメント](https://www.raspberrypi.org/documentation/pico/getting-started/)を参照してください。

<a id="overview"></a>
## 概要

アプリケーションをビルドする前に、以下を行う必要があります：

- Moddable SDKをインストールし、そのツールをビルドする
- Raspberry Pi Picoプラットフォーム用の必要な開発ツールをインストールする

以下の手順では、コマンドラインツール`mcconfig`を使用してデバイス上で`helloworld`の例を実行することで、セットアップを確認します。

> `mcconfig`に関する詳細は、[ツールのドキュメント](./../tools/tools.md)を参照してください

`mcconfig`を使用してビルドする際には、開発ボードの**プラットフォーム識別子**を`-p`引数に指定してデバイスターゲットを指定します。例えば、Pico Display用にビルドするには、次のコマンドを使用します：

```text
mcconfig -d -m -p pico/pico_display
```

利用可能なPicoサブプラットフォームとそのプラットフォーム識別子のリストは、以下の**プラットフォーム**セクションに記載されています。

<a id="platforms"></a>
## プラットフォーム

### Pico

Raspberry Pi Picoには以下の機能があります：

- 最大133 MHzで動作するRP2040マイクロコントローラー
- デュアルコアARM Cortex M0+
- 264 KB RAM
- 2 MBフラッシュ

Moddable SDKはPicoで構築されたデバイスをサポートしています。以下の表は、各デバイス、そのプラットフォーム識別子、デバイス固有の主要機能のリスト、および追加リソースへのリンクを示しています。

| 名前 | プラットフォーム識別子 | 主要機能 | リンク |
| :---: | :--- | :--- | :--- |
| <img src="../assets/devices/pi-pico.png" width=220><BR>Rasberry Pi<BR>Pico | `pico` | LED、26外部ピン  | <li>[Raspberry Pi Pico ドキュメント](https://www.raspberrypi.org/documentation/pico/getting-started/)</li> |
| <img src="../assets/devices/pi-pico_w.png" width=65><BR>Rasberry Pi<BR>Pico W | `pico/pico_w` | Wi-Fi、LED、26外部ピン  | <li>[Raspberry Pi Pico ドキュメント](https://www.raspberrypi.org/documentation/pico/getting-started/)</li> |
| <img src="../assets/devices/pico-display.png" width=80></a><BR>Pimoroni<BR>Pico Display | `pico/pico_display`<BR>`simulator/pico_display` | **1.4" IPSディスプレイ**<BR>135 x 240<BR>16ビットカラー<BR>4ボタン<BR>RGB LED | <li>[Pimoroni Pico Display](https://pimoroni.com/picodisplay)</li> |
| <img src="../assets/devices/pico-display-2.png" width=200></a><BR>Pimoroni<BR>Pico Display 2 | `pico/pico_display_2`<BR>`simulator/pico_display_2` | **2.0" IPSディスプレイ**<BR>320 x 240<BR>16ビットカラー<BR>4ボタン<BR>RGB LED | <li>[Pimoroni Pico Display 2](https://shop.pimoroni.com/products/pico-display-pack-2-0)</li> |
| <img src="../assets/devices/pico-lcd-1.3.png" width=220></a><BR>Waveshare<BR>Pico LCD 1.3 | `pico/pico_lcd_1.3` | **1.3" IPSディスプレイ**<BR>240 x 240<BR>16ビットカラー<BR>4ボタン<BR>1ジョイスティック | <li>[Waveshare Pico LCD 1.3](https://www.waveshare.com/wiki/Pico-LCD-1.3)</li> |
| <img src="../assets/devices/pico-adafruit-itsyBitsy-rp2040.png" width=220></a><BR>Adafruit<BR>ItsyBitsy RP2040 | `pico/itsybitsy` | Neopixel、1ボタン | <li>[Adafruit 製品ページ](https://www.adafruit.com/product/4888)</li> |
| <img src="../assets/devices/pico-lilygo-t-display-rp2040.png" width=220></a><BR>LILYGO<BR>T-Display RP240 | `pico/lilygo_t_display` | **1.14" ST7789**、2ボタン、赤LED | <li>[LilyGO T-Display GitHub リポジトリ](https://github.com/Xinyuan-LilyGO/LILYGO-T-display-RP2040)</li> |
| <img src="../assets/devices/pico-pimoroni-picoSystem.png" width=220></a><BR>Pimoroni<BR>PicoSystem | `pico/picosystem` | **1.54" IPS LCD**、240 x 240、Dパッド & 4ボタン、RGB LED | <li>[Pimoroni 製品ページ](https://shop.pimoroni.com/products/picosystem?variant=32369546985555)</li> |
| <img src="../assets/devices/pico-sparkfun-pro-micro-rp2040.png" width=220></a><br>Sparkfun<br>Pro Micro RP2040 | `pico/pro_micro` | Qwiic/STEMMAコネクタ、Neopixel | <li>[Sparkfun 製品ページ](https://www.sparkfun.com/products/18288)</li> |
| <img src="../assets/devices/pico-adafruit-qt-py-rp2040.png" width=150></a><br>Adafruit<br>QT Py | `pico/qtpy` | STEMMA/Qwiicコネクタ、Neopixel、1ボタン | <li>[Adafruit 製品ページ](https://www.adafruit.com/product/4900)</li> |
| <img src="../assets/devices/pico-adafruit-qt-trinkey.png" width=150></a><br>Adafruit<br>Trinkey QT2040 | `pico/qt_trinkey` | STEMMA/Qwiicコネクタ、Neopixel、1ボタン | <li>[Adafruit 製品ページ](https://www.adafruit.com/product/5056)</li> |
| <img src="../assets/devices/pico-pimoroni-tiny-2040.png" width=150></a><br>Pimoroni<br>Tiny 2040 | `pico/tiny2040` | RGB LED、1ボタン| <li>[Pimoroni 製品ページ](https://shop.pimoroni.com/products/tiny-2040?variant=39560012234835)</li> |
| <img src="../assets/devices/ws_round.png" width=150></a><br>WAVESHARE<br>1.28inch Round LCD | `pico/ws_round`<BR>`pico/ws_round_touch` | 1.28" IPS 240×240 ラウンドディスプレイ| <li>[WAVESHARE 製品ページ](https://www.waveshare.com/rp2040-lcd-1.28.htm)</li><li>[タッチLCDバージョン](https://www.waveshare.com/product/rp2040-touch-lcd-1.28.htm)</li> |
| <img src="../assets/devices/pico-seeed-studio-xiao-rf2040.png" width=150></a><br>Seeed Studio<br>XIAO RP2040 | `pico/xiao_rp2040` | Neopixel | <li>[Seeed Studio 製品ページ](https://www.seeedstudio.com/XIAO-RP2040-v1-0-p-5026.html)</li> |
| <img src="../assets/devices/pico-xiao-ili9341.png" width=140></a><BR>ili9341 | `pico/xiao_ili9341` | ili9341 QVGAディスプレイ<BR>320 x 240<BR>16ビットカラー | <li>[配線ガイド - Pico](../displays/images/xiao-qtpy-ili9341-wiring.png)</li> |
| <img src="../assets/devices/pico-ili9341.png" width=140></a><BR>ili9341 | `pico/ili9341` | ili9341 QVGAディスプレイ<BR>320 x 240<BR>16ビットカラー | <li>[汎用2.4" & 2.8"ディスプレイ（抵抗タッチ）配線ガイド - Pico](../displays/wiring-guide-generic-2.4-spi-pico.md)</li> |
| <img src="../assets/devices/pico-ili9341-i2s-thumb.png" width=140></a><BR>ili9341 | `pico/ili9341_i2s` | ili9341 QVGAディスプレイ<BR>320 x 240<BR>16ビットカラー<br>ポテンショメータ、ボタン<br>i2sオーディオ | [配線ガイド](../displays/images/pico-ili9341-i2s-wiring.png) |

pico-ili9341-i2s-thumb.png

<a id="setup"></a>
## SDKとホスト環境のセットアップ

<a id="mac"></a>

### macOSのセットアップ

> これらの手順のほとんどは[Raspberry Pi Pico C SDK][picosdkdoc]ドキュメントから要約されています。詳細についてはドキュメントを参照してください。

1. [Moddable SDK Getting Startedドキュメント](../Moddable%20SDK%20-%20Getting%20Started.md)には、ホストビルド環境の設定方法や必要なSDK、ドライバ、開発ツールのインストール方法が記載されています。macOSの場合は、[ホスト環境のセットアップ](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/Moddable%20SDK%20-%20Getting%20Started.md#host-mac)セクションの指示に従ってください。

2. Pico SDKのセットアップ
	ホームディレクトリに`~/pico`という名前のディレクトリを作成し、必要なサードパーティのSDKやツールを配置します。

	```text
	cd $HOME
	mkdir pico
	```

3. `brew`を使用して必要なコンポーネントをインストールします。

	```text
	brew install cmake
	brew tap ArmMbed/homebrew-formulae
	brew install arm-none-eabi-gcc
	```

4. `PICO_GCC_ROOT`環境変数を`arm-none-eabi`ツールチェーンの`bin`ディレクトリを指すように設定します。macOSの場合、これは通常`brew --prefix`で設定されます。x86_64アーキテクチャでは通常`/usr/local`、arm64では`/opt/homebrew`です。

	```text
	export PICO_GCC_ROOT=$(brew --prefix)
	```

5. __pico__ SDK、extras、およびexamplesをインストールします：

	```text
	cd $HOME/pico
	git clone -b 1.5.0 https://github.com/raspberrypi/pico-sdk
	cd pico-sdk
	git submodule update --init
	```

	```text
	cd $HOME/pico
	git clone -b sdk-1.5.0 https://github.com/raspberrypi/pico-extras
	```

	```text
	cd $HOME/pico
	git clone -b sdk-1.5.0 https://github.com/raspberrypi/pico-examples
	```

6. `PICO_SDK_DIR` 環境変数をPico SDKディレクトリを指すように設定します：

	```text
	export PICO_SDK_DIR=$HOME/pico/pico-sdk
	```

7. いくつかの `pico` ツールをビルドします：

	```text
	cd $PICO_SDK_DIR
	mkdir build
	cd build
	cmake ..
	make
	```


<a id="macOS-building-and-deploying-apps"></a>
#### macOS でのアプリのビルドとデプロイ

macOSホスト環境をセットアップした後、以下の手順でPicoにアプリケーションをインストールします。

1. `mcconfig` を使用してアプリをビルドおよびデプロイします。

	`mcconfig` は、マイクロコントローラーおよびシミュレーター上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig` の完全なドキュメントは [こちら](../tools/tools.md) にあります。

	`mcconfig` でプラットフォーム `-p pico` を指定してPico用にビルドします。[`helloworld`](../../examples/helloworld) の例をビルドします：

	```text
	cd $MODDABLE/examples/helloworld
	mcconfig -d -m -p pico
	```

アプリがビルドされ、インストールされます。数秒後に `xsbug` が起動し、Picoに接続されます。

> 注: デバイスが応答しない場合、次のメッセージが表示されることがあります:
>
>	```text
>	BOOTSEL ボタンを押しながらデバイスの電源を入れ直してください。
>	/Volumes/RPI-RPI2 を待っています.....
>	```
>
>  Pico の電源を入れる際に __BOOTSEL__ ボタンを押し続けることで、デバイスをプログラミングモードにします。
>
>    デスクトップに `RPI-RP2` という名前のディスクが表示されると、プログラミングモードがアクティブになっていることがわかります。

<a id="mac-troubleshooting"></a>
### トラブルシューティング

- macOSの **DISK NOT EJECTED PROPERLY** が画面に残っている場合は、[`ejectfix.py`](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/ejectfix.py) ツールをダウンロードして使用することで、自動的に消去することができます。

   詳細については、[Adafruit ブログの記事](https://blog.adafruit.com/2021/05/11/how-to-tone-down-macos-big-surs-circuitpy-eject-notifications/) を参照してください。

<a id="win"></a>

### Windows Setup

まだ利用できません。

<a id="win-building-and-deploying-apps"></a>
#### Windowsでのアプリのビルドとデプロイ

まだ利用できません。


<a id="lin"></a>

### Linuxセットアップ

> 注: このセットアップはVirtualBoxを使用してUbuntu 20 VM上で実行されました。

> これらの手順のほとんどは[Raspberry Pi Pico C SDK][picosdkdoc]ドキュメントから要約されています。詳細についてはドキュメントを参照してください。

1. [Moddable SDK Getting Startedドキュメント](../Moddable%20SDK%20-%20Getting%20Started.md)には、ホストビルド環境の設定方法や必要なSDK、ドライバ、開発ツールのインストール方法が記載されています。Linuxの[ホスト環境セットアップ](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/Moddable%20SDK%20-%20Getting%20Started.md#lin-instructions)セクションの指示に従ってください。

2. Pico SDKをセットアップする

	ホームディレクトリの`~/pico`に、必要なサードパーティSDKとツール用の`pico`ディレクトリを作成します。

	```text
	cd $HOME
	mkdir pico
	```

3. `apt`を使用して必要なコンポーネントをインストールします。

	```text
	sudo apt update
	sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
	```

4. `PICO_GCC_ROOT` 環境変数を `arm-none-eabi` ツールチェーンの `bin` ディレクトリを指すように設定します。Ubuntu 20の場合、それは `/usr` に設定されます。

	```text
	export PICO_GCC_ROOT=/usr
	```

5. __pico__ SDK、extras、およびexamplesをインストールします：

	```text
	cd $HOME/pico
	git clone -b 1.5.0 https://github.com/raspberrypi/pico-sdk
	cd pico-sdk
	git submodule update --init
	```

	```text
	cd $HOME/pico
	git clone -b sdk-1.5.0 https://github.com/raspberrypi/pico-extras
	```

	```text
	cd $HOME/pico
	git clone -b sdk-1.5.0 https://github.com/raspberrypi/pico-examples
	```

6. `PICO_SDK_DIR` 環境変数をPico SDKディレクトリを指すように設定します：

	```text
	export PICO_SDK_DIR=$HOME/pico/pico-sdk
	```

7. いくつかの `pico` ツールをビルドします：

	```text
	cd $PICO_SDK_DIR
	mkdir build
	cd build
	cmake ..
	make
	```


<a id="linux-building-and-deploying-apps"></a>
### Linuxでのアプリのビルドとデプロイ

Linuxホスト環境を設定した後、以下の手順に従ってPicoにアプリケーションをインストールします。

1. Picoを電源オンにする際に__BOOTSEL__ボタンを押し続けて、デバイスをプログラミングモードにします。

	データ同期が可能なケーブルを使用していることを確認してください。電源供給のみのケーブルは使用しないでください。

	> 注: 電源スイッチ付きのUSBハブがあると非常に便利です。

	デスクトップに`RPI-RP2`という名前のディスクが表示されると、プログラミングモードがアクティブになっていることがわかります。

	<img src="../assets/devices/pico-on-linux.png" width="175">

	> 注: 仮想マシンを使用する場合、最良の結果を得るためには、Picoデバイスをブートモード状態と実行状態の両方でキャプチャしてください。以下の画像はVirtualBoxでの設定を示しています：

	<img src="../assets/devices/pico-vbox-usb.png" width="400">

3. `mcconfig`を使用してアプリをビルドおよびデプロイします。

	`mcconfig`は、マイクロコントローラーおよびシミュレーター上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)で利用できます。

	Pico用にビルドするには、`mcconfig`でプラットフォーム`-p pico`を指定します。[`helloworld`](../../examples/helloworld)の例をビルドします：

	```text
	cd $MODDABLE/examples/helloworld
	mcconfig -d -m -p pico
	```

アプリがビルドされ、インストールされます。数秒後に `xsbug` が起動し、Picoに接続されます。



<a id="debugging-native-code"></a>
## ネイティブコードのデバッグ

ハードウェアのセットアップ手順については、[Getting Started With Pico][picogettingstarteddoc] を参照してください。

これらの手順は、Appendix A: Using Picoprobeに記載されている2つのPico SWDセットアップを使用してmacOSホストでテストされています。

1. ドキュメントに記載されているようにpico-openocdをビルドします。

2. コンソールで `openocd` を開始し、コンソールをこのままにします。

	```text
	cd ~/pico/openocd
	~/pico/openocd/src/openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -s tcl
	```

3. 別のコンソールで、ビルド結果ディレクトリに移動し、`gdb` を開始します。

	```text
	cd $MODDABLE/build/bin/pico/debug/<app>
	arm-none-eabi-gdb xs_pico.elf
	```

4. `openocd` に接続します。アプリを `load` します。デバイスを `reset` します。

	```text
	(gdb) target remote localhost:3333
	(gdb) load
	(gdb) monitor reset init
	(gdb) continue
	```

<a id="reference"></a>
## 参考文献

[Getting started with Raspberry Pi Pico][picogettingstarteddoc]

[Hardware Design with RP2040][picohwdoc]

[Raspberry Pi Pico C SDK][picosdkdoc]


[picogettingstarteddoc]:https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf
[picohwdoc]:https://datasheets.raspberrypi.org/rp2040/hardware-design-with-rp2040.pdf
[picosdkdoc]:https://datasheets.raspberrypi.org/pico/raspberry-pi-pico-c-sdk.pdf
