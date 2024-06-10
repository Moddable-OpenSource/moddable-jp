# Moddable SDKでnRF52を使用する

Copyright 2021-2024 Moddable Tech, Inc.<BR>
改訂： 2024年1月19日

このドキュメントは、Moddable SDKを使用してNordicのnRF52840 SoC向けにアプリを構築するためのガイドです。

## 目次

* [概要](#overview)
* [プラットフォーム](#platforms)
	* [nrf52](#platforms-nrf52)
* [ビルドタイプ](#builds)
	* [デバッグ](#build-debug)
	* [instrumented](#build-instrumented)
	* [リリース](#build-release)
* セットアップ手順

    | [![Apple logo](./../assets/moddable/mac-logo.png)](#mac) | [![Windows logo](./../assets/moddable/win-logo.png)](#win) | [![Linux logo](./../assets/moddable/lin-logo.png)](#lin) |
    | :--- | :--- | :--- |
    | •  [インストール](#mac-instructions)<BR>•  [トラブルシューティング](#mac-troubleshooting) | •  [インストール](#win-instructions)<BR>•  [トラブルシューティング](#win-troubleshooting) | •  [インストール](#lin-instructions)<BR>•  [トラブルシューティング](#lin-troubleshooting)

* [トラブルシューティング](#troubleshooting)
* [シリアル経由のデバッグ](#serial-debugging)
* [BLE経由の更新](#ble-update)
	* [nRF52デバイスにバージョン8のブートローダーがインストールされていることを確認](#ble-update-1)
	* [nRF52ブートローダーの更新](#ble-update-bootloader)
	* [更新パッケージのビルド](#ble-update-2)
	* [パッケージをモバイルに転送](#ble-update-3)
	* [デバイスをDFU OTAモードにする](#ble-update-4)
	* [nRF Connect for Mobileを使用してデバイスをワイヤレスで更新](#ble-update-5)
* [シリアル経由でのアプリのインストール](#install-apps-via-serial)
* [ネイティブコードのデバッグ](#debugging-native-code)
* [ブートローダー](#bootloader)
	* [ブートローダーのインストール](#install-bootloader)
	* [ブートローダーの更新](#update-bootloader)
* [nRF5 SDKの変更](#nrf5-sdk-mods)

<a id="overview"></a>
## 概要

アプリケーションをビルドする前に、以下のことを行う必要があります：

- Moddable SDKをインストールし、そのツールをビルドする
- nRF52プラットフォーム用の必要なドライバと開発ツールをインストールする

以下の手順では、Moddable SDKを使用してアプリケーションをビルドおよび実行するためのコマンドラインツールである`mcconfig`を使用して、デバイス上で`helloworld`の例を実行することでセットアップを確認します。

> `mcconfig`の詳細については、[ツールのドキュメント](./../tools/tools.md)を参照してください。

Moddable Four用にビルドするには、**プラットフォーム識別子**として`nrf52/moddable_four`を指定して`mcconfig`を実行します：

```text
mcconfig -d -m -p nrf52/moddable_four
```

利用可能なnRF52サブプラットフォームとそのプラットフォーム識別子のリストは、以下の**プラットフォーム**セクションにあります。

<a id="platforms"></a>
## プラットフォーム

<a id="platforms-nrf52"></a>
### nRF52

nRF52840には以下の機能があります：

- 64 MHz Cortex-M4 with FPU
- BLE
- 256 KB RAM
- 1 MB Flash

| 名前 | プラットフォーム識別子 | 主な機能 | リンク |
| :---: | :--- | :--- | :--- |
| <img src="./../assets/devices/moddable-four.png" width=125><BR>Moddable Four | `nrf52/moddable_four`<BR>`simulator/moddable_four` | - **1.28" 128x128 モノクローム**<BR>- シャープミラーディスプレイ<BR>- BLE<BR>- ジョグダイヤル<BR>- 加速度計<BR>- ボタンとLED<BR>- CR2032コインセル電源<BR>- 12 外部GPIOピン  | <li>[Moddable Four開発者ガイド](./moddable-four.md)</li><li>[Moddable製品ページ](https://www.moddable.com/hardware)</li> |
| <img src="./../assets/devices/moddable-display-4.png" height=150><BR>Moddable Display 4 | `nrf52/moddable_display_4`<BR>`simulator/moddable_four` | - **1.28" 128x128 モノクローム**<BR>- シャープミラーディスプレイ<BR>- BLE<BR>- ジョグダイヤル<BR>- 加速度計<BR>- ボタンとLED<BR>- CR2032コインセル電源<BR>- 12 外部GPIOピン  | <li>[Moddable Display開発者ガイド](./moddable-display.md)</li><li>[Moddable製品ページ](https://www.moddable.com/hardware)</li> |
| <img src="./../assets/devices/nrf52-pca10056.png" width=125><BR>Nordic nRF52840 DK pca10056 | `nrf52/dk` | - 4つのLED<BR>- 4つのボタン<BR>- すべてのピンにアクセス可能<BR>- BLE<BR>- CR2032コインセル電源  | <li>[製品ページ](https://www.nordicsemi.com/Products/Development-hardware/nrf52840-dk)</li> |
| <img src="./../assets/devices/nrf52-sparkfun.png" width=125><BR>Sparkfun Pro nRF52840 Mini | `nrf52/sparkfun` | - 1つのLED<BR>- 1つのボタン<BR>- BLE<BR>- JST電源コネクタ<BR>- Qwiicコネクタ<BR>- 17 GPIOピン  | <li>[製品ページ](https://www.sparkfun.com/products/15025)</li> |
| <img src="./../assets/devices/nrf52-makerdiary.png" width=125><BR>Makerdiary nRF58240 MDK | `nrf52/makerdiary` | - 1つの3色LED<BR>- 1つのボタン<BR>- BLE<BR>- 12 GPIOピン  | <li>[製品ページ](https://makerdiary.com/products/nrf52840-mdk-usb-dongle)</li> |
| <img src="./../assets/devices/nrf52-xiao.png" width=125><BR>Seeed Studio XIAO nRF52840 | `nrf52/xiao` | - 1つの3色LED<BR>- 1つのボタン<BR>- BLE<BR>- 11 GPIOピン  | <li>[製品ページ](https://www.seeedstudio.com/Seeed-XIAO-BLE-nRF52840-p-5201.html)</li> |
| <img src="./../assets/devices/nrf52-itsybitsy.png" width=125><BR>Adafruit ItsyBitsy nRF52840 Express | `nrf52/itsybitsy` | - 1つのLED<BR>- 1つのボタン<BR>- BLE<BR>- 21 GPIOピン  | <li>[製品ページ](https://www.adafruit.com/product/4481)</li> |
| <img src="../assets/devices/xiao-qtpy-ili9341-thumbnail.png" width=140></a><BR>ili9341 | `nrf52/xiao_ili9341` | ili9341 QVGAディスプレイ<BR>320 x 240<BR>16ビットカラー | <li>[配線ガイド](../displays/images/xiao-qtpy-ili9341-wiring.png)</li> |

<a id="builds"></a>
## ビルドタイプ
nRF52は、デバッグ、instrumented、およびリリースの3種類のビルドをサポートしています。それぞれは製品開発プロセスの異なる段階に適しています。`mcconfig`を実行する際にコマンドラインからどの種類のビルドを行うか選択します。

> **注**: ディープスリープAPIはinstrumentedおよびリリースビルドでのみ利用可能です。

<a id="build-debug"></a>
### デバッグ
デバッグビルドはJavaScriptのデバッグに使用されます。デバッグビルドでは、デバイスは起動時にUSBまたはシリアル経由でxsbugに接続しようとします（デバイスの設定によります）。ネイティブgdbデバッグ用のシンボルが含まれます。

`mcconfig`コマンドラインの`-d`オプションでデバッグビルドを選択します。

<a id="build-instrumented"></a>
### instrumented
instrumentedビルドはネイティブコードのデバッグに使用されます。instrumentedビルドでは、JavaScriptデバッガーが無効になります。通常xsbugで利用可能な計測データは、1秒ごとにシリアルコンソールに出力されます。ディープスリープAPIはinstrumentedビルドで利用可能です。

`mcconfig` コマンドラインの `-i` オプションは、計測ビルドを選択します。

<a id="build-release"></a>
### リリース
リリースビルドは、製品用です。リリースビルドでは、JavaScriptデバッガーが無効になり、計測統計が収集されず、シリアルコンソール出力が抑制されます。リリースビルドでは、ディープスリープAPIが利用可能です。

`mcconfig` コマンドラインで `-d` と `-i` の両方のオプションを省略すると、リリースが選択されます。`-r` はリリースビルドの選択ではなく、ディスプレイの回転を指定することに注意してください。

<a id="setup"></a>
<a id="mac"></a>
## macOS

Moddable SDKのnRF52用ビルドは、現在Nordic nRF5 SDK v17.0.2を使用しています。

<a id="mac-instructions"></a>
### インストール

1. [Getting Started ドキュメント](./../Moddable%20SDK%20-%20Getting%20Started.md) の指示に従って、Moddable SDKツールをインストールします。

2. 必要なサードパーティSDKとツールのために、ホームディレクトリに `~/nrf5` ディレクトリを作成します。

3. macOS Catalina (バージョン10.15) 以降を使用している場合、システムのセキュリティポリシーを満たさないソフトウェアをローカルで実行できるように、ターミナル (または選択した別のターミナルアプリケーション） に例外を追加します。この設定がないと、次のステップでダウンロードされる事前コンパイル済みのGNU Arm Embedded Toolchainを実行することができません。

    ターミナルのセキュリティポリシーの例外を設定するには、セキュリティとプライバシーのシステム環境設定に移動し、プライバシータブを選択し、左側のリストから開発者ツールを選び、ターミナルまたはModdable SDKアプリをビルドするために使用する代替ターミナルアプリケーションのチェックボックスをオンにします。最終的には次のようになります：

    ![Catalina Developer Options](../assets/getting-started/catalina-security.png)

4. **x86** Macの場合、GNU Arm Embedded Toolchainのバージョン12.2.1 [AArch32 bare-metal target (arm-none-eabi)](https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-darwin-x86_64-arm-none-eabi.tar.xz) を[Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)ウェブサイトからダウンロードします。アーカイブを解凍し、`arm-gnu-toolchain-12.2.rel1-darwin-x86_64-arm-none-eabi`ディレクトリを`nrf5`ディレクトリに移動します。

5. **Armベース**のMacの場合、GNU Arm Embedded Toolchainのバージョン12.2.1 [AArch32 bare-metal target (arm-none-eabi)](https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-darwin-arm64-arm-none-eabi.tar.xz) を[Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)ウェブサイトからダウンロードします。アーカイブを解凍し、`arm-gnu-toolchain-12.2.rel1-darwin-arm64-arm-none-eabi`ディレクトリを`nrf5`ディレクトリに移動します。

    <!-- is this brew step necessary with the v 12.2.1? You will also need some items from `brew`:

    `brew install gmp mpfr libmpc isl libelf`
     -->

6. Moddable Fourは、デバイスにファームウェアをフラッシュするためのUF2ファイル形式をサポートする修正された[Adafruit nRF52 Bootloader](https://github.com/Moddable-OpenSource/Adafruit_nRF52_Bootloader)を使用します。Moddableは、デバイスへの転送用にUF2バイナリをパッケージ化するMicrosoftの`uf2conv.py` Pythonツールを使用します。[uf2conv](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/uf2conv.py)ツールをダウンロードしてください。`uf2conv.py`ファイルを`nrf5`ディレクトリに移動またはコピーします。

    `chmod`を使用して`uf2conv`のアクセス権限を変更し、実行可能にします。

    ```text
    cd ~/nrf5
    chmod 755 uf2conv.py
    ```

6. Moddable Fourの修正が加えられた[Nordic nRF5 SDK](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/nRF5_SDK_17.0.2_d674dde-mod.zip)をダウンロードします。

    アーカイブを解凍し、`nRF5_SDK_17.0.2_d674dde`ディレクトリを`nrf5`ディレクトリにコピーします。

    > FYI – nRF5 SDKの変更に関する情報については、[nRF5 SDK modifications](#nrf5-sdk-mods)のセクションを参照してください。これらの変更は、先ほどダウンロードしたアーカイブに既に適用されています。

7. `NRF_SDK_DIR`環境変数をnRF5 SDKディレクトリに設定します：

    ```text
    export NRF_SDK_DIR=$HOME/nrf5/nRF5_SDK_17.0.2_d674dde
    ```

8. デバイスターゲット用に`helloworld`をビルドしてセットアップを確認します：

    ```text
    cd ${MODDABLE}/examples/helloworld
    mcconfig -d -m -p nrf52/<YOUR_SUBPLATFORM_HERE>
    ```

<a id="mac-troubleshooting"></a>
### トラブルシューティング

- macOSで**DISK NOT EJECTED PROPERLY**が画面に残る場合は、[`ejectfix.py`](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/ejectfix.py)ツールをダウンロードして使用することで、自動的に解除できます。

   詳細については、[Adafruitブログの記事](https://blog.adafruit.com/2021/05/11/how-to-tone-down-macos-big-surs-circuitpy-eject-notifications/)を参照してください。

<a id="win"></a>
## Windows

<a id="win-instructions"></a>
### インストール

1. [開始ガイド](./../Moddable%20SDK%20-%20Getting%20Started.md)の指示に従って、Moddable SDKツールをインストールします。

2. 必要なサードパーティSDKおよびツールのために、`%USERPROFILE%`ディレクトリに`nrf5`ディレクトリを作成します。例：`C:\Users\<your-user-name>`。

    ```text
    cd %USERPROFILE%
    mkdir nrf5
    cd nrf5
    ```

3. [Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)ウェブサイトから、GNU Arm Embedded Toolchainのバージョン12.2.1 [`AArch32 bare-metal target (arm-none-eabi)`](https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-mingw-w64-i686-arm-none-eabi.zip)をダウンロードします。アーカイブを解凍し、`arm-gnu-toolchain-12.2.rel1-mingw-w64-i686-arm-none-eabi`ディレクトリを`nrf5`ディレクトリに移動します。

4. Moddable Fourは、デバイスにファームウェアをフラッシュするためのUF2ファイル形式をサポートする修正された[Adafruit nRF52 Bootloader](https://github.com/Moddable-OpenSource/Adafruit_nRF52_Bootloader)を使用します。`uf2conv.py`は、デバイスへの転送用にUF2バイナリをパッケージ化するMicrosoftのPythonツールです。[uf2conv](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/uf2conv.py)ツールをダウンロードします。`uf2conv.py`ファイルを`nrf5`ディレクトリに移動またはコピーします。

5. Moddable Fourの修正が加えられた[Nordic nRF5 SDK](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/nRF5_SDK_17.0.2_d674dde-mod.zip)をダウンロードします。

    アーカイブを解凍し、`nRF5_SDK_17.0.2_d674dde`ディレクトリを`nrf5`ディレクトリにコピーします。

    > FYI – nRF5 SDKの修正に関する情報は、[nRF5 SDK modifications](#nrf5-sdk-mods)のセクションを参照してください。これらの修正は、今ダウンロードしたアーカイブに既に適用されています。

6. `NRF52_SDK_PATH`環境変数をnRF5 SDKディレクトリに設定します：

    ```text
    set NRF52_SDK_PATH = %USERPROFILE%\nrf5\nRF5_SDK_17.0.2_d674dde
    ```

7. Windows用の[Pythonインストーラー](https://www.python.org/ftp/python/2.7.15/python-2.7.15.msi)をダウンロードして実行します。デフォルトのオプションを選択します。

8. システムの`PATH`環境変数を編集して、Pythonディレクトリを含めます：

    ```text
    C:\Python27
    C:\Python27\Scripts
    ```

9. デバイスターゲット用に`helloworld`をビルドしてセットアップを確認します：

    ```text
    cd %MODDABLE%\examples\piu\balls
    mcconfig -d -m -p nrf52/moddable_four
    ```

<a id="win-troubleshooting"></a>
### トラブルシューティング

<a id="lin"></a>
## Linux

<a id="lin-instructions"></a>
### インストール

1. [Getting Started document](./../Moddable%20SDK%20-%20Getting%20Started.md) の指示に従って、Moddable SDKツールをインストールします。

2. 必要なサードパーティのSDKとツールのために、ホームディレクトリに `nrf5` ディレクトリを作成します。

    ```text
    cd $HOME
    mkdir nrf5
    cd nrf5
    ```

3. GNU Arm Embedded Toolchainのバージョン12.2.1 [AArch32 bare-metal target (arm-none-eabi)](https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz) を [Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) ウェブサイトからダウンロードします。アーカイブを解凍し、`arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi` ディレクトリを `nrf5` ディレクトリに移動します。

4. Moddable Fourは、デバイスにファームウェアをフラッシュするためのUF2ファイル形式をサポートする修正された [Adafruit nRF52 Bootloader](https://github.com/Moddable-OpenSource/Adafruit_nRF52_Bootloader) を使用します。`uf2conv.py` は、デバイスへの転送用にUF2バイナリをパッケージ化するMicrosoftのPythonツールです。[uf2conv](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/uf2conv.py) ツールをダウンロードします。`uf2conv.py` ファイルを `nrf5` ディレクトリに移動またはコピーします。

5. Moddable Fourの修正が加えられた[Nordic nRF5 SDK](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/nRF5_SDK_17.0.2_d674dde-mod.zip)をダウンロードします。

    アーカイブを解凍し、`nRF5_SDK_17.0.2_d674dde`ディレクトリを`nrf5`ディレクトリにコピーします。

    > FYI – nRF5 SDKの修正に関する情報は、[nRF5 SDK modifications](#nrf5-sdk-mods)のセクションを参照してください。これらの修正は、ダウンロードしたアーカイブに既に適用されています。

6. `NRF_SDK_DIR`環境変数をnRF5 SDKディレクトリに設定します：

    ```text
    export NRF_SDK_DIR=$HOME/nrf5/nRF5_SDK_17.0.2_d674dde
    ```

7. デバイスターゲット用に`helloworld`をビルドしてセットアップを確認します：

    ```text
    cd ${MODDABLE}/examples/helloworld
    mcconfig -d -m -p nrf52/<YOUR_SUBPLATFORM_HERE>
    ```

<a id="lin-troubleshooting"></a>
### Linuxのトラブルシューティング

アプリケーションをインストールしようとすると、エラーや警告の形で障害に遭遇することがあります。このセクションでは、Linux上での一般的な問題とその解決方法について説明します。

macOS、Windows、Linuxで共通のその他の問題については、このドキュメントの下部にある[トラブルシューティングセクション](#troubleshooting)を参照してください。


#### 許可が拒否されました

nrf52はttyACM0デバイスを介してLinuxホストと通信します。Ubuntu Linuxでは、ttyACM0デバイスは`dialout`グループによって所有されています。`xsbug`デバッガに接続しようとしたときに**許可が拒否されましたエラー**が発生した場合は、ユーザーを`dialout`グループに追加してください：

```text
sudo adduser <username> dialout
sudo reboot
```

#### `MODDABLE4`またはその他の`nrf52`ディスクの許可の問題

nrf52デバイスがプログラミングモード（つまり、リセットボタンをダブルプレス）にある場合、デスクトップに表示されるはずです。許可の問題がある場合は、次のコマンドを試してディスクをマウントしてください：

```text
udisksctl mount -b /dev/sdb -t FAT
```


<a id="troubleshooting"></a>
## トラブルシューティング

### スタックオーバーフロー

	region RAM overflowed with stack

アプリケーションをビルドしていて、リンクが`arm-none-eabi/bin/ld: region RAM overflowed with stack`エラーで失敗する場合、ヒープに割り当てられるRAMの量を減らす必要があります。

デフォルトでは、`NRF52_HEAP_SIZE` は0x35000に設定されています。

アプリケーションマニフェストで、`NRF52_HEAP_SIZE` の割り当てを変更できます：

```text
    "build": {
        "NRF52_HEAP_SIZE": "0x30000"
     }
```

### アプリケーションが大きすぎる

	ld: region FLASH overflowed with .data and user data
	section '.text' will not fit in region 'FLASH'
	ld: region `FLASH' overflowed by 2285384 bytes

アプリケーションをビルドしていて、上記のようなエラーでリンクに失敗する場合、アプリケーションが大きすぎます。アプリケーションリソースのサイズを減らすか、コードを再構成してください。


<a id="advanced"></a>
## 高度な設定

<a id="serial-debugging"></a>
### シリアル経由でのデバッグ

カスタムデバイスや一部の開発ボードには、デバッグ用のUSBポートがない場合があります。

Moddable SDKはシリアル接続を介して `xsbug` に接続できます。

#### `manifest.json` の変更

`build` セクションで、以下の設定を確認してください：

```
"USE_USB": "0",
"FTDI_TRACE": "-DUSE_FTDI_TRACE=0"
```

`defines` セクションで、ピンとボーレートを設定する `debugger` 条項を設定します：

```
"debugger": {
    "tx_pin": "NRF_GPIO_PIN_MAP(0,30)",
    "rx_pin": "NRF_GPIO_PIN_MAP(0,31)",
    "baudrate": "NRF_UARTE_BAUDRATE_460800"
},
```

#### 環境の変更

`DEBUGGER_PORT` 環境変数をシリアルアダプタに設定し、`DEBUGGER_SPEED` を設定します。

```
export DEBUGGER_PORT=/dev/cu.usbserial-0001
export DEBUGGER_SPEED=460800
```

アプリケーションをビルドしてインストールします。


#### デバッガに接続する

`xsbug` が実行されていない場合は、起動します：

macOS:

```
open $MODDABLE/build/bin/mac/release/xsbug.app
```

WindowsとLinuxでは、次のコマンドを入力するだけです：

```
xsbug
```

接続するために `serial2xsbug` を実行します：

```
serial2xsbug $DEBUGGER_PORT $DEBUGGER_SPEED 8N1
```

デバイスをリセットすると、`xsbug` に接続されます。

<a id="install-apps-via-serial"></a>
### シリアル経由でアプリをインストールする

ブートローダーとModdable SDKは、シリアルポートを使用したファームウェアのインストールをサポートしています。ブートローダーのビルドは、USB経由またはシリアル経由のいずれかのプログラミングをサポートしますが、両方はサポートしません。

シリアル経由でインストールするには、次の手順を実行する必要があります：

1. ブートローダーの `board.h` ファイルを修正する
2. ブートローダーをビルドしてインストールする
3. 特別なターゲットでModdableアプリをビルドする

これらの手順は以下で詳しく説明されています。

#### ブートローダー

ブートローダーは、ターゲットデバイスに特化してビルドする必要があります。`BOARD` 定義ファイルを修正し、ビルド定義を使用する必要があります。

##### `BOARD` 定義

このセクションを `src/boards/<boardname>/board.h` ファイルに追加します：

```
//--------------------------------------------------------------------+
// UART update
//--------------------------------------------------------------------+
#define RX_PIN_NUMBER      31
#define TX_PIN_NUMBER      30
#define CTS_PIN_NUMBER     0
#define RTS_PIN_NUMBER     0
#define HWFC               false
```

`RX_PIN_NUMBER` と `TX_PIN_NUMBER` をボードに適した値に設定します。

ステータスLEDは、デバイスがプログラミングモードにあるときに急速に点滅するため便利です。このファイルでは `LED_PRIMARY_PIN` として定義されています。

##### ビルドライン

ブートローダーをビルドする際に、ビルドラインに `SERIAL_DFU=1` を追加します。例えば：

```
make BOARD=<boardname> SERIAL_DFU=1 flash
```

ブートローダーのビルドに関する詳細は、[ブートローダー](#bootloader) を参照してください。

#### Moddable アプリケーションビルドターゲット

USBではなくシリアルポート経由でインストールするためには、`installDFU` または `debugDFU` のいずれかのターゲットを使用します。

```
mcconfig -d -m -p nrf52/<boardname> -t debugDFU
```

`installDFU` は単にアプリをデバイスにインストールします。

`debugDFU` はアプリをインストールし、xsbugを起動し、serial2xsbugで接続します。

インストールはAdafruitのadafruit-nrfutilを使用してmcconfigによって行われます。

#### Setup

Adafruitの `adafruit-nrfutil` をGitHubリポジトリで説明されているようにインストールします：

[`https://github.com/adafruit/Adafruit_nRF52_nrfutil`](https://github.com/adafruit/Adafruit_nRF52_nrfutil)

環境変数 `UPLOAD_PORT` をデバイスに接続されているシリアルポートに設定します。

```
export UPLOAD_PORT=/dev/cu.usbserial-0001
```

#### Device target

デバイスターゲットの `manifest.json` ファイルで、デバッガーのtxおよびrxピンとボーレートが定義されていることを確認します。

manifest.jsonファイルは `$MODDABLE/build/devices/nrf52/targets/<boardname>/manifest.json` にあります。

`"defines"` セクションで：

```
"defines": {
	"debugger": {
		"tx_pin": "30",
		"rx_pin": "31",
		"baudrate": "NRF_UARTE_BAUDRATE_921600"
	},
...
```

#### Build and install

インストールを受け取るためには、デバイスをファームウェア更新モードにする必要があります。リセットボタンをダブルタップしてデバイスをプログラミングモードにします。ステータスLEDが急速に点滅します。

```
mcconfig -d -m -p nrf52/<boardname> -t debugDFU
```

ビルド情報がスクロールした後、コンソールはインストールに進みます：

```
Sending DFU start packet
Sending DFU init packet
Sending firmware file
########################################
########################################
...
########################################
###########################
Activating new firmware

DFU upgrade took 69.43805122375488s
Device programmed.
```

<a id="ble-update"></a>
## nRF52のBLE経由での更新 (DFU OTA)

Moddable SDKは、iOSおよびAndroidのNordicの「nRF Connect for Mobile」アプリを使用して、BLE経由でnRF52ファームウェアを更新することをサポートしています。これは、Moddable Fourおよび他のサポートされているnRF52搭載ボードで動作します。

デバイスを準備し、BLE経由でnRF52ファームウェアを更新するための5つのステップは次のとおりです。

1. [nRF52デバイスがAdaFruitブートローダーのModdableフォークのバージョン8](#ble-update-1)（またはそれ以降）を持っていることを確認します。(https://github.com/Moddable-OpenSource/Adafruit_nRF52_Bootloader)
2. [プロジェクトファームウェアをビルド](#ble-update-2)して更新パッケージにします
3. [更新パッケージをモバイルデバイスに転送](#ble-update-3)します
4. [ターゲットデバイスをDFU OTAモードにします](#ble-update-4)
5. [「nRF Connect for Mobile」を使用してファームウェアをインストール](#ble-update-5)し、BLEでデバイスにワイヤレスでインストールします

以下のセクションでは、これらの手順を詳細に説明します。

> 注: OTAファームウェアの更新に失敗した場合、デバイスはソフトウェアが正常に更新されるまでDFU OTAモードで再起動します。

<a id="ble-update-1"></a>
### 1) nRF52デバイスにバージョン8.1のブートローダーがインストールされていることを確認する

デバイスをプログラミングモードにして（リセットボタンをダブルタップ）、デスクトップに表示されるボリュームを開きます。INFO_UF2.TXTファイルを開きます。以下のような情報を探します。

```
Bootloader: Moddable 8.1
Date: Nov  8 2023
```

バージョンが8.1よりも前の場合は、ブートローダーを更新してください。

<a id="ble-update-bootloader"></a>
#### nRF52ブートローダーの更新

Moddable nRF52ブートローダーは、ボード用に事前にビルドされたバージョンで更新することも、自分でカスタマイズしてビルドすることもできます。

[Moddable Four Bootloader](https://github.com/Moddable-OpenSource/moddable/tree/public/build/devices/nrf52/bootloader)は、リポジトリの`$MODDABLE/build/devices/nrf52/bootloader/`にあります。デバイスをプログラミングモードにして、current.uf2ファイルをデバイスにコピーします。

もし異なるデバイスをお持ちの場合は、まずブートローダーを構成し、次にブートローダーをビルドして、更新されたブートローダーをnRF52デバイスにインストールしてください。

[ModdableのAdafruit nRF52ブートローダーのフォーク](https://github.com/Moddable-OpenSource/Adafruit_nRF52_Bootloader)を使用してください。DFU OTAを使用するための最低バージョンはバージョン8です。

<a id="configure-the-bootloader"></a>
#### ブートローダーの構成

ブート中にGPIOピンの状態をチェックしてデバイスをDFU OTAモードにするようにブートローダーを構成できます。

`board.h`ファイルで`BUTTON_DFU`定義を設定して、使用するGPIOを指定します。`board.h`ファイルは`Adafruit_nRF52_Bootloader/src/boards/<boardname>/board.h`にあります。

GPIOを定義しない場合は、プログラムでデバイスをDFU OTAモードで再起動するように設定できます。[以下を参照](#dfu-software-switch)

#### USBを使用してブートローダーをビルドおよびインストールする

USB経由で通信するデバイスの場合、ブートローダー更新ファイルをビルドします：

```
cd .../Adafruit_nRF52_Bootloader
rm -rf _build
git pull --rebase
make BOARD=moddable_four bootloaderuf2
```
デバイスをプログラミングモードにして、`current.uf2`ファイルをデバイスにコピーします：

```
cp current.uf2 /Volumes/MODDABLE4
```

#### または、UARTデバイス用のJTAGを使用してブートローダーをビルドおよびインストールする

シリアル経由で更新するデバイスの場合、新しいブートローダーをインストールするためにJTAGを使用します：

```
cd .../Adafruit_nRF52_Bootloader
rm -rf _build
git pull --rebase
make SERIAL_DFU=1 BOARD=test flash
```

> 注: `SERIAL_DFU=1` を使用すると、上記の例はUSBではなくシリアルを使用してプログラムするデバイス用です。このブートローダーをインストールすると、USB経由での更新が無効になります。

<a id="ble-update-2"></a>
### 2) 更新パッケージをビルドする

`-t ble-package` ターゲットを使用してアプリケーションをビルドします。ビルドが完了し、`ble-package.zip` ファイルの場所が表示されます。

```
 % cd .../my_app
 % mcconfig -d -m -p nrf52/moddable_four -t ble-package
 ....
 # Packaging .../my_app/xs_nrf52.hex for BLE
Zip created at .../my_app/ble-package.zip
```

<a id="ble-update-3"></a>
### 3) パッケージをモバイルに転送する

`ble-package.zip` ファイルをモバイルデバイスに転送し、「nRF Connect for Mobile」でアクセスできるようにします。

<a id="ble-update-4"></a>
### 4) デバイスをDFU OTAモードにする

デバイスがDFU OTAモードにあると、ステータスLEDが定期的にダブルブリンクします。

<a id="dfu-software-switch"></a>
#### nRF52を更新モードにする（プログラム的に）

nRF52デバイスをBLE DFUアップデートモードにするには、`nrf52_rebootToOTA()` C関数を呼び出します。`$(MODDABLE)/build/devices/nrf52/examples/BLE_DFU`アプリは、アプリからそれを使用する方法の例です。

```
cd $MODDABLE/build/devices/nrf52/examples/BLE_DFU
mcconfig -d -m -p nrf52/moddable_four
```
#### nRF52をアップデートモードにする (GPIO)

デバイスとブートローダーに`BUTTON_DFU`として定義されたボタンがある場合、そのボタンを押しながらデバイスをリセットします。

<a id="ble-update-5"></a>
#### 5) nRF Connect for Mobileを使用してデバイスをワイヤレスでアップデートする

nRF52がBLE DFUアップデートモードになったら、[nRF Connect for Mobile](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile)を使用して、`ble-package.zip`ファイルに含まれるファームウェアをnRF52デバイスに転送します。

アプリケーションを起動し、以下の手順に従います：

1. フィルターを開く
2. Nordic DFUサービスを有効にする
3. "Remove Unconnectable"を有効にする
4. AdaDFUデバイスに接続する<br>
	<img src=../assets/dfu/nrfConnect0.jpeg width=40%>&nbsp;<img src=../assets/dfu/nrfConnect1.jpeg width=40%>
5. DFUタブを選択する
6. "Connect"ボタンをクリックする
デバイスが接続されたら、
7. "Open Document Picker"をクリックする<br>
	<img src=../assets/dfu/nrfConnect2.jpeg width=40%>&nbsp;<img src=../assets/dfu/nrfConnect3.jpeg width=40%>
8. アップロードパッケージを選択する
9. "Start"ボタンを押す<br>
	<img src=../assets/dfu/nrfConnect4.jpeg width=40%>&nbsp;<img src=../assets/dfu/nrfConnect5.jpeg width=40%>
10. フラッシュ領域が消去される間、ステータスエリアに「Starting」と表示されます。
11. 領域が消去された後、ステータスが「Uploading」に変わり、アップロードが続行されると進行状況が表示されます。<br>
	<img src=../assets/dfu/nrfConnect6.jpeg width=40%>&nbsp;<img src=../assets/dfu/nrfConnect7.jpeg width=40%>
12. 転送が完了すると、「Success!」と表示されます。デバイスは新しくインストールされたファームウェアイメージに再起動します。

	<img src=../assets/dfu/nrfConnect8.jpeg width=40%>

<a id="debugging-native-code"></a>
### ネイティブコードのデバッグ

すべてのModdableプラットフォームと同様に、Moddable FourではUSBシリアルインターフェースを介して`xsbug`を使用してスクリプトコードをデバッグできます。詳細については、[`xsbug`のドキュメント](../xs/xsbug.md)を参照してください。ネイティブコードのソースレベルデバッグには、[GDB](https://www.gnu.org/software/gdb/documentation/)を使用できます。

Moddable Fourでネイティブコードをデバッグするには、[Nordic nRF52840-DKボード](https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK)、[Segger J-Link Plus](https://www.segger.com/products/debug-probes/j-link/models/j-link-plus/)または互換デバイスが必要です。

<a id="j-link-connection"></a>
例えば、Moddable FourをnRF52840-DKボードに次のように接続します：

| nRF52840 DK | Moddable Four |  |
| :---: | :---: | :---
| SWD CLK | SWDCLK |
| SWD IO | SWDIO |
| RESET | RESET | (オプション)
| GND DETECT | GND |
| VTG | 3V3 |

<img src="../assets/devices/moddable-four-dk-pinout.png" width="100%"><BR>

セガー J-Link Plusをお持ちの場合、J-Linkへの接続は以下の通りです：

| Moddable Four | J-Link | |
| :---: | :---: | :---
| SWDCLK | TCK |
| SWDIO | TMS |
| RESET | RESET | (オプション)
| GND | GND |
| 3V3 | VTref |



<img src="../assets/devices/nrf52-jlink.png" width="50%"><BR>



[GDB](https://www.gnu.org/software/gdb/documentation/) は、Unix系のビルドホストでネイティブコードをデバッグするために広く使用されているGNUデバッガです。GDBは、[SDKおよびホスト環境のセットアップ](#setup)ステップでダウンロードされたArm Embedded Toolchainアーカイブに含まれています。

GDBは、nRF52840-DKボード、セガー J-Link Plusまたは他のJ-Link互換デバイスのJ-Link接続を介してnRF58240デバイスと通信します。必要なツールをインストール/構成し、GDBを起動するために次の手順を実行してください：

1. [nRFコマンドラインツール](https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Command-Line-Tools)をインストールします。`JLinkGDBServer`が`$PATH`のどこかにあることを確認してください。

2. `nrf5`ディレクトリに、以下の内容でGDBスタートアップコマンドテキストファイル `gdb_cmds.txt` を作成します：

    ```text
    target remote localhost:2331
    mon speed 10000
    mon flash download=1
    load
    break main
    mon reset 0
    continue
    ```

3. デバイスとデバッガーの両方のUSBポートをコンピュータに接続します。両方のUSBポートはUSBハブを介してコンピュータに接続できます。

4. デバッグする予定のネイティブコードを含むModdableアプリをビルドします。この例では、BLEの[heart-rate-server](https://github.com/Moddable-OpenSource/moddable/tree/public/examples/network/ble/heart-rate-server)の例をビルドします：

    ```text
    cd $MODDABLE/examples/network/ble/heart-rate-server
    mcconfig -d -m -p nrf52/moddable_four -t build
    ```

5. コマンドラインコンソールからJ-Link GDBサーバーを起動します：

    ```text
    JLinkGDBServer -device nRF52840_xxAA -if swd -port 2331
    ```

    GDBサーバーはnRF52840-DKターゲットに接続し、クライアント接続を待ちます：

    ```text
    Connecting to J-Link...
    J-Link is connected.
    Firmware: J-Link OB-SAM3U128-V2-NordicSemi compiled Mar 17 2020 14:43:00
    Hardware: V1.00
    S/N: 683214408
    Checking target voltage...
    Target voltage: 3.30 V
    Listening on TCP/IP port 2331
    Connecting to target...Connected to target
    Waiting for GDB connection...
    ```

6. 別のコマンドラインコンソールからGDBクライアントを起動し、アプリケーションELFとGDBスタートアップコマンドテキストファイルをコマンドライン引数として渡します：

    ```text
    arm-none-eabi-gdb $MODDABLE/build/tmp/nrf52/moddable_four/debug/heart-rate-server/xs_nrf52.out -x ~/nrf5/gdb_cmds.txt
    ```

GDBサーバーはクライアントと接続し、アプリケーションをダウンロードし、GDBセットアップコマンドファイルで指定されたブレークポイント`main`で停止します：

```text
Breakpoint 1 at 0x46550: file /Users/<user>/Projects/moddable/build/devices/nrf52/xsProj/main.c, line 149.
    Resets core & peripherals via SYSRESETREQ & VECTRESET bit.

    Breakpoint 1, main () at /Users/<user>/Projects/moddable/build/devices/nrf52/xsProj/main.c:149

    149     clock_init();
    (gdb)
```

7. `(gdb)`プロンプトで、`c`と入力して実行を続行するか、他のブレークポイントを設定します。

<a id="bootloader"></a>
## ブートローダー

nRF52 SoC上で動作するModdable SDKを使用するアプリケーションは、通常、デバイスにファームウェアをフラッシュするためのUF2ファイル形式をサポートする修正された[Adafruit nRF52 Bootloader](https://github.com/Moddable-OpenSource/Adafruit_nRF52_Bootloader)を使用します。

### プログラミングモード

デバイスにModdable SDKアプリのデバッグビルドがインストールされている場合、Moddableツールはデバイスを自動的にプログラミングモードに設定するため、ボードを手動でリセットする必要はありません。

それ以外の場合は、デバイスのリセットボタンを**ダブルタップ**してプログラミングモードにします。オンボードLEDが毎秒点滅し、**MODDABLE4**という名前のUSBディスクがデスクトップに表示されます。

`.uf2`ファイルを**MODDABLE4**ディスクにドラッグしてプログラムします。

> 注: ブートローダーは同じ方法で更新できます。

> 注: 表示されるディスクは **MODDABLEnRF** と名付けられている場合があります。

<a id="install-bootloader"></a>
### 初めてのブートローダーのインストール

Moddable SDKを使用するためには、nRF52840デバイスにブートローダーをインストールする必要があります。これにより、以前のブートローダーの機能が置き換えられます。

> 注: Moddable Fourにはブートローダーが事前にインストールされています。

> 注: デバイスが壊れる可能性があります。

初めてブートローダーをプログラムするには、Segger J-Linkまたは同等のものが必要です。一度Moddableブートローダーがインストールされると、UF2インストール方法を使用できます。

1. デバイスをJ-Linkに接続します。デバッガーと同じ方法で接続します。詳細は
[ネイティブコードのデバッグ](#debugging-native-code)
セクションを参照してください。

2. ブートローダーリポジトリを取得します：

   ```
   git clone https://github.com/Moddable-OpenSource/Adafruit_nRF52_Bootloader --recurse-submodules
   ```

3. デバイス用にビルドします

   ```
   cd Adafruit_nRF52_Bootloader
   make BOARD=moddable_four
   ```

> 注: 以下のBOARD構成はModdableをサポートするように更新されました。
- `moddable_four`
- `moddable_itsybitsy_nrf52`
- `moddable_makerdiary_nrf52`
- `moddable_pca10056`
- `moddable_sparkfun52840`
- `moddable_xiao`

4. デバイスにインストールします。まずSoftDeviceをインストールし、次にブートローダーをフラッシュします：

   ```
   make BOARD=moddable_four sd
   make BOARD=moddable_four flash
   ```

5. リセットボタンをダブルタップしてデバイスをプログラミングモードに設定します。LEDが定期的に点滅し、デスクトップに`MODDABLEnRF`ボリュームが表示されます。

   これでデバイスをプログラムできます。

   <a id="bootloader-update-file"></a>
6. デバイスにModdableブートローダーがインストールされたら、**bootloaderuf2** Makefileターゲットを使用して更新ファイルをビルドし、そのファイルをデバイスにコピーできます。

   `bootloaderuf2`ターゲットでブートローダーをビルドします

   ```
   cd .../Adafruit_nRF52_Bootloader
   make BOARD=moddable_four bootloaderuf2
   ```

   デバイスをプログラミングモードにして、`current.uf2`ファイルをデバイスにコピーします。

----

<a id="nrf5-sdk-mods"></a>
## nRF5 SDKの変更点

Moddable Fourには、Nordic nRF5 SDKにいくつかの小さな調整が必要です。準備されたSDKは[Nordic nRF5 SDK](https://github.com/Moddable-OpenSource/tools/releases/download/v1.0.0/nRF5_SDK_17.0.2_d674dde-mod.zip)から使用できます。

または、以下の手順に従ってSDKを変更することで、自分で作成することもできます：

1. 次の手順に従って[Nordic nRF5 SDK](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download)をダウンロードします：

    - nRF5 SDKバージョンセクションから`v17.0.2`を選択します。

        ![](../assets/devices/nrf5-sdk-versions.png)

    - すべてのSoftDevicesのチェックを外します。

        ![](../assets/devices/softdevices.png)

    - ページの下部にある**Download Files**ボタンをクリックします。以下の画像と同じ選択が表示されるはずです。

        ![](../assets/devices/nrf5-sdk-selected.png)

    ダウンロードされたアーカイブは`DeviceDownload.zip`という名前です。アーカイブを解凍し、`nRF5_SDK_17.0.2_d674dde`ディレクトリを`nrf5`ディレクトリにコピーします。

2. `NRF_SDK_DIR` 環境変数をnRF5 SDKディレクトリに設定します：

    ```text
    export NRF_SDK_DIR=$HOME/nrf5/nRF5_SDK_17.0.2_d674dde
    ```

3. Moddable Fourのボード定義ファイルをNordic nRF5 SDKに追加します。ボード定義ファイルにはModdable FourのLED、ボタン、およびピンの定義が含まれています。Moddable Fourボード定義ファイルを追加するには、次の手順を実行します：

    - `moddable_four.h` ボード定義ファイルは `$MODDABLE/build/devices/nrf52/config/moddable_four.h` にあります。`moddable_four.h` ファイルをNordic nRF5 SDKの `components/boards/` ディレクトリにコピーします。

    ```text
    cp $MODDABLE/build/devices/nrf52/config/moddable_four.h $NRF_SDK_DIR/components/boards
    ```

    - `$NRF_SDK_DIR/components/boards/boards.h` を修正し、次の内容を `#elif defined(BOARD_CUSTOM)` の前に追加します：

    ```c
    #elif defined (BOARD_MODDABLE_FOUR)
      #include "moddable_four.h"
    ```

4. `SPIM3` サポートを追加します：

    nRF5 SDKには `integration/nrfx/legacy/apply_old_config.h` というファイルがあり、少し変更が必要です。以下の行に示すように、`NRFX_SPIM2_ENABLED` の後に `|| NRFX_SPIM3_ENABLED` を追加します：

```text
#define NRFX_SPIM_ENABLED \
(SPI_ENABLED && (NRFX_SPIM0_ENABLED || NRFX_SPIM1_ENABLED || NRFX_SPIM2_ENABLED || NRFX_SPIM3_ENABLED))
```

5. LEセキュア接続サポートを有効にする：

    Nordic SDKの`nrf_stack_info.h`ファイル内の`nrf_stack_info_overflowed`関数でスタックオーバーフローチェックを無効にします：

```c
__STATIC_INLINE bool nrf_stack_info_overflowed(void)
    {
    #if 0
        if (NRF_STACK_INFO_GET_SP() < NRF_STACK_INFO_BASE)
        {
            return true;
        }
    #endif
        return false;
    }
```
