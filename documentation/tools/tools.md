# ツール
Copyright 2017-2024 Moddable Tech, Inc.<BR>
改訂： 2024年1月18日

## このドキュメントについて

このドキュメントでは、Moddableが提供するツールについて説明します。これらのツールは、マイクロコントローラーやModdableシミュレーター上でJavaScriptアプリを構築、デバッグ、実行するために使用されます。

ツールはJavaScriptモジュールをコンパイル・リンクし、特定のプラットフォームや画面に合わせてアセットを準備します。直接使用するツールは**mcconfig**、**mcrun**、**xsbug**のみです。他のツールは、**mcconfig**と**mcrun**によって生成されたmakefileを通じて間接的に使用されますが、何が行われているのかを理解するためにここで紹介します。

ツール自体を構築し、Moddableシミュレーターでアプリを構築・実行するには、標準的な開発ツールのみが必要です。マイクロコントローラー上でアプリを構築・実行するには、Cコードをコンパイル・リンクし、アプリをフラッシュストレージに転送するためのマイクロコントローラーツールチェーンも必要です。Moddable SDKツールを構築する方法の完全な手順については、[Getting Started ドキュメント](../Moddable%20SDK%20-%20Getting%20Started.md)を参照してください。

## 目次

* [mcconfig](#mcconfig)
* [mcrun](#mcrun)
* [mcrez](#mcrez)
* [png2bmp](#png2bmp)
* [xsc](#xsc)
* [xsl](#xsl)
* [mcbundle](#mcbundle)
* [mchex](#mchex)
* [シミュレータ](#simulator) (mcsim)

<a id="mcconfig"></a>
## mcconfig

**mcconfig** はマニフェストに基づいてmakefileを生成し、**make** を実行してマイクロコントローラーやシミュレーターでModdableアプリをビルドして起動するコマンドラインツールです。

例えば：

```shell
cd $MODDABLE/examples/piu/balls
mcconfig -d -m
```

はシミュレーターで [balls のサンプル](../../examples/piu/balls) をビルドして起動し、

```shell
cd $MODDABLE/examples/piu/balls
mcconfig -d -m -p esp/moddable_two
```

はModdable Twoでballsのサンプルをビルドして起動し、

```shell
cd $MODDABLE/examples/network/http/httpgetjson
mcconfig -d -m -p esp ssid="Public Wi-Fi"
```

は "Public Wi-Fi" という名前のオープンなWi-Fiアクセスポイントに接続するためにESP8266ターゲットデバイスを設定し、そのデバイス上でhttpgetjsonのサンプルをビルドして起動します。

いくつかの注意点：

- 最初のアプリのビルドには時間がかかります。ESPおよびXSライブラリもコンパイルする必要があるためです。
- デバッグビルドを実行する場合、**xsbug**がコンピューターで実行されている必要があります。そうでないと起動に成功しません。
- マニフェストについての説明は、[Manifest](./manifest.md)ドキュメントを参照してください。

<a id="mcconfig-arguments"></a>
### 引数

```text
mcconfig [manifest] [-d] [-f format] [-i] [-m] [-o directory] [-p platform] [-r rotation] [-t target] [-v] [-l] [-x xsbug_host:xsbug_port] [ssid="wifi_ssid"] [password="wifi_password"] [screen=screen_driver] [touch=touch_driver]
```

- `manifest`: マニフェストファイル。デフォルトは現在のディレクトリまたは現在のディレクトリの親ディレクトリにある`manifest.json`ファイルです。
- `-d`: デバッグ用のinstrumentedバージョンをビルドし、デフォルトのデバッガー（通常はxsbug）を起動します。
- `-dx`: デバッグ用のinstrumentedバージョンをビルドし、xsbugデバッガーを起動します。
- `-dl`: デバッグ用のinstrumentedバージョンをビルドし、xsbug-logデバッガーを起動します。
- `-dn`: デバッグ用のinstrumentedバージョンをビルドし、デバッガーを起動しません。
- `-f format`: 画面のピクセルフォーマットを選択します：`gray16`, `gray256`, `rgb332`, `rgb565be` または `rgb565le`。デフォルトは `rgb565le`です。詳細は [png2bmp](#png2bmp) を参照してください。
- `-i`: リリースinstrumentedバージョンをビルドします。
- `-l`: xsbugのコンソール出力をxsbugではなくターミナルにログします（下記の注を参照）。`-l`は`-dl`に置き換えられて非推奨です。
- `-x`: デバッグビルドがxsbugに接続するために使用するデフォルトのホストとポート（localhost:5002）を上書きします。
- `-m`: 自動的に`make`を実行します。それ以外の場合、**mcconfig**はmakeファイルを生成するだけです。
- `-o directory`: 出力ディレクトリ。デフォルトは`$MODDABLE/build`ディレクトリです。
- `-p platform`: プラットフォームを選択します。デバイスターゲットのドキュメントを参照して、そのプラットフォーム識別子を確認してください。サポートされている値には、`esp`, `esp/moddable_one`, `esp/moddable_three`, `esp32`, `esp32/moddable_two`, `win`, `lin`, `mac`, `sim/moddable_one`, `sim/moddable_two`, `sim/moddable_three`, および `wasm`が含まれます。ホストビルドプラットフォームにデフォルトされます：`mac`, `win` または `lin`。
- `-r rotation`: 画面の回転を選択します：`0`, `90`, `180` または `270`。デフォルトは `0`です。詳細は [png2bmp](#png2bmp) を参照してください。
- `-t target`: ビルドターゲットを選択します：`build`, `deploy`, `xsbug`, `clean`, または `all`。デフォルトは `all`です。詳細は [Build Targets](#buildtargets) を参照してください。
- `-v`: `make`によって実行されるすべてのコマンドをトレースします。
- `key-=value`または`key="value"`の形式で指定された設定引数。これらはマニフェストの`config`セクションにマージされます。`mc/config`モジュールをインポートしてアクセスします。Moddableが提供するホストは、ネットワーキングおよび/またはディスプレイをサポートしているため、次の設定プロパティを定義しています：
  - `ssid="wifi ssid"`および`password="wifi password"`：ネットワーク認証情報を指定し、アプリを起動する前にネットワークに接続します。
  - `screen=screen_driver`および`touch=touch_driver`：画面またはタッチドライバーを指定します。画面およびタッチドライバーの設定についての詳細は、[examples readme](../../examples/readme.md)を参照してください。

> **注意**: リリースビルドを生成するには、コマンドラインから `-d` と `-i` を除外してください。

> **注意**: `-l` および `-dl` オプションは、ビルドシステムに Node.js が必要です。また、`$MODDABLE/tools/xsbug-log` で `npm install` を実行する必要があります。

> **注意**: `-dn` オプションは現在 Windows ではサポートされていません。近い将来に実装される予定です。

<a id="buildtargets"></a>
**ビルドターゲット**

**mcconfig** はオプショナルな `-t target` 引数を取り、ビルドターゲットを指定します。ターゲットのオプションは以下の通りです：

- `clean`: アプリのビルド出力を削除します
- `build`: アプリをビルドします
- `deploy`: アプリをデプロイします
- `xsbug`: xsbugデバッガに接続します
- `all`: `build`、`deploy`、および `xsbug` のステップを実行します

`-t` フラグが省略された場合、デフォルト値は `all` です。

**mcconfig** を使用して、xsbugでJavaScriptデバッグを行うシリアルポートを使用するマイクロコントローラーを使用する場合、`deploy`、`xsbug`、および `all` ターゲットは、実行中のserial2xsbugのインスタンスを終了します（存在する場合）。

<a id="mcrun"></a>
## mcrun
**mcrun** は、[mod](../xs/mods.md) をビルドするコマンドラインツールです。modは、新しい機能を追加したり、既存の挙動を変更したりするスクリプトで、ユーザーがIoT製品にインストールできます。modをビルドするための入力は、JavaScriptモジュール、アセット、データ、および設定で、これらはマニフェストで指定されます。出力は、JavaScriptバイトコードとリソースデータを含むXSアーカイブファイル（`.xsa` 拡張子）です。

`mcrun`と`mcconfig`の間にはいくつか重要な違いがあります：

- `mcrun`によって使用されるマニフェストは、modがJavaScriptのみを含むことができるため、ネイティブコードにビルドされるファイル（例：`.c`や`.cpp`ファイル）を参照してはいけません
- `mcrun`は`-t`オプションをサポートしていません
- `config`プロパティは`mc/config`の代わりに`mod/config`モジュールから利用可能です（`config`プロパティについての詳細は[マニフェスト](./manifest.md)のドキュメントの`config`セクションを参照してください）

### 引数

```text
mcrun [manifest] [-d] [-f format] [-i] [-m] [-o directory] [-p platform] [-r rotation] [-v] [-x xsbug_host:xsbug_port] [ssid="wifi_ssid"] [password="wifi_password"] [screen=screen_driver] [touch=touch_driver]
```

`mcrun`のコマンドライン引数は`mcconfig`のものとほぼ同じですが、`mcrun`は`-t`オプションをサポートしていません。各引数の説明については、[`mcconfig` 引数](#mcconfig-arguments)セクションを参照してください。

<a id="mcrez"></a>
## mcrez

**mcrez**はリソースマップにアセットを含めるコマンドラインツールです。**mcrez**はアセット自体とそれにアクセスする方法を含むCコードを生成します。

Moddableアプリはファイルシステムを必要としません。アセットは`Resource`モジュールのおかげでリソースとしてアクセスされます。

```js
import Resource from "Resource";
import parseBMP from "commodetto/parseBMP";
let bitmap = parseBMP(new Resource("balls-color.bmp"));
```

ほとんどのアセットはフラッシュストレージから直接使用されることに注意してください。

### 引数

```text
mcrez files... [-o output] [-r name] [-p platform]
```

- `files`: 含めるアセットのパス。
- `-o output`: 出力ディレクトリのパス。デフォルトは現在のディレクトリです。
- `-r name`: 生成されるCファイルの名前。デフォルトは`mc.resources.c`です。
- `-p platform`: プラットフォームを選択するために：`esp`, `esp32`, `win`, `lin` または `mac`。ホストビルドプラットフォームにデフォルト設定されています：`mac`, `win` または `lin`。`esp8266`は`esp`のエイリアスとして使用される場合があります。

<a id="png2bmp"></a>
## png2bmp

**png2bmp**はPNGファイルをModdableアプリがフラッシュストレージから直接使用できるBMPファイルに変換するコマンドラインツールです。

例えば：

```shell
cd $MODDABLE/examples/piu/balls
png2bmp balls.png -o ~/Desktop
```

デスクトップ上に2つのファイルを作成します：

- `balls-alpha.bmp`: アルファチャンネルを定義する8ビットグレービットマップ。
- `balls-color.bmp`: 赤、緑、青のチャンネルを定義する16ビットカラービットマップ。

フラッシュストレージから直接ビットマップを使用するには、ビットマップが画面のピクセルフォーマットと回転に準拠している必要があります。画面のピクセルフォーマットを選択するには `-f` オプションを、画面の回転を選択するには `-r` オプションを使用します。**png2bmp** は画面のピクセルフォーマットに関連する行バイトの制約も処理します。例えば：

```shell
cd $MODDABLE/piu/examples/balls
png2bmp balls.png -o ~/Desktop -f gray256 -r 90
```

![](./../assets/tools/png2bmp.png)

### 引数

```text
png2bmp file.png [-a] [-c] [f format] [-o directory] [-r rotation]
```

- `-a`: アルファビットマップのみを作成します。
- `-c`: カラービットマップのみを作成します。
- `-f format`: 画面のピクセルフォーマットを選択します：`gray16`, `gray256`, `rgb332`, `rgb565be` または `rgb565le`。デフォルトは `rgb565le`。
- `-o directory`: 出力ディレクトリ。デフォルトは現在のディレクトリです。
- `-r rotation`: 画面の回転を選択します：`0`, `90`, `180` または `270`。デフォルトは `0`。

<a id="xsc"></a>
## xsc

**xsc**はXSコンパイラで、コマンドラインツールとして、JavaScriptソースコードを含むファイル（通常は`.js`拡張子のファイルに保存される）を、シンボルとバイトコードを含むXSバイナリファイルにコンパイルします。

デフォルトでは、**xsc**はJavaScriptファイルをECMAScriptモジュールとして解析します。オプションで、互換性と適合性のために、**xsc**はJavaScriptファイルをECMAScriptプログラムとして解析することもできます。ModdableアプリはECMAScriptモジュールのみを使用します。

`-c`オプションを使用すると、**xsc**はホスト関数やホストオブジェクトを参照する`@`構文を受け入れます。例えば：

```c
class Point @ "Point_destructor" {
	constructor(x, y) @ "Point_constructor"
	moveBy(x, y) @ "Point_moveBy"
	get x() @ "Point_get_x"
	get y() @ "Point_get_y"
}
```

`Point`クラスはホストオブジェクトを作成します。ガベージコレクタがそのようなホストオブジェクトを破棄するときには`Point_destructor` C言語の関数が呼ばれます。`new Point(x, y)`でそのようなホストオブジェクトを構築するときには`Point_constructor` C言語の関数が呼ばれます。他のC言語の関数はプロパティにアクセスしたりメソッドを呼び出したりするときに呼ばれます。C言語の関数の実装については[**XS in C**](../xs/XS%20in%20C.md)を参照してください。

`-e` オプションがない場合、**xsc** はXSシンボルを宣言し、ホスト関数のインターフェースを宣言するCコードを生成します。そのようなC言語のコードは、ホスト関数の実装とリンクしてコンパイルされ、動的ライブラリを構築することができます。

`-e` オプションを使用すると、**xsc** はホスト関数とホストオブジェクトへの参照をXSバイナリファイルに埋め込みます。リンカーである **xsl** がすべてのモジュールのC言語のコードを生成します。これがModdableアプリが動作する方法です。

### 引数

```text
xsc file [-c] [-d] [-e] [-o directory] [-p] [-r name] [-t directory]
```

- `file`: コンパイルする `.js` ファイルのパス。
- `-c`: ホスト関数とホストオブジェクトを参照する `@` 構造を受け入れるために使用します。`-c` オプションがあり、`-e` オプションがない場合、**xsc** はXSシンボル、ホスト関数、およびホストオブジェクトを宣言するCコードを生成します。
- `-d`: JavaScriptファイルをデバッグするためにファイルと行のバイトコードを生成します。
- `-e`: ホスト関数とホストオブジェクトへの参照をXSバイナリファイルに埋め込むために使用します。このオプションは、**xsl** がXSアーカイブファイルにリンクできるXSバイナリファイルにJavaScriptファイルをコンパイルするために必要です。
- `-o directory`: 出力ディレクトリのパス。デフォルトは現在のディレクトリです。
- `-p`: JavaScriptファイルをECMAScriptプログラムとして解析します。
- `-r name`: 出力ファイルの名前。デフォルトは入力ファイルの名前です。出力拡張子は常に `.xsb` です。
- `-t directory`: 一時ディレクトリのパス。デフォルトは出力ディレクトリです。`-c` オプションがあり、`-e` オプションがない場合、**xsc** は一時ディレクトリにC言語のコードを生成します。

<a id="xsl"></a>
## xsl

**xsl**はXSリンカーで、コマンドラインツールです。複数のXSバイナリファイルを1つのXSアーカイブファイルにリンクし、XSシンボルとホスト関数のインターフェースを宣言するC言語のコードを生成します。

`-p`オプションを使用すると、**xsl**はモジュールをプリロードし、アプリを実行するためにクローン可能な読み取り専用のXS仮想マシンを定義するCコードも生成できます。これがModdableアプリの動作方法です。

その後、C言語のコードをコンパイルしてホスト関数の実装とリンクし、動的ライブラリまたは実行可能ファイルをビルドします。

### 引数

```text
xsl files... [-a name] [-b directory] [-c creation] [-o directory] [-p modules]... [u url]
```

- `files`: リンクするXSバイナリファイルのパス。
- `-a name`: XSアーカイブファイルの名前。デフォルトは`a`です。
- `-b directory`: ベースディレクトリのパス。デフォルトは出力ディレクトリです。アーカイブ内のモジュールの名前は、ベースディレクトリに対して相対的なXSバイナリファイルのパスです。ベースディレクトリの直接または間接的に内部にないXSバイナリファイルをリンクすることはエラーです。
- `-c creation`: クローンされたマシンを作成するために使用されるパラメータ。
- `-o directory`: 出力ディレクトリのパス。デフォルトは現在のディレクトリです。
- `-p module`: プリロードするモジュールの名前。プリロードするモジュールごとに`-p module`オプションを使用します。
- `-r name`: 出力ファイルの名前。デフォルトは`mc`です。
- `-s feature`: 削除する機能の名前。削除する機能ごとに`-s feature`オプションを使用し、`s *`を使用してすべての未使用機能を削除します。
- `-u url`: アーカイブ内のモジュールのベースURL。デフォルトは`/`です。

<a id="mcbundle"></a>
## mcbundle

**mcbundle** は、Moddable Store用のアプリアーカイブを構築してパッケージするコマンドラインツールです。

**mcbundle** は、[アプリマニフェスト](./manifest.md)の [`bundle` オブジェクト](./manifest.md#store)を使用します。以下は、`countdown` サンプルから取られた `bundle` オブジェクトのサンプルです：

```text
"bundle": {
	"id": "tech.moddable.countdown",
	"devices": [
		"esp/moddable_one",
		"com.moddable.two"
	],
	“custom”: “./store/custom”,
	“icon”: “./store/icon.png”
}
```

`countdown` サンプルのアプリアーカイブをバンドルするには、次の `mcbundle` コマンドを使用します：

```shell
cd $MODDABLE/examples/piu/countdown
mcbundle -d -m -o $MODDABLE/build/tmp
```

これにより、`countdown` アプリが2つのデバイス用に構築され、ターゲットが1つのアプリアーカイブにパッケージされます。

**mcbundle** はシェルスクリプト `tech.moddable.countdown.sh` を生成して実行します。このシェルスクリプトは、アプリを構築するために各ターゲットプラットフォームに対して **mcconfig** を一度ずつ呼び出し、ターゲット、アイコン、カスタム設定ダイアログボックスをアプリアーカイブフォルダにコピーします。アプリアーカイブフォルダの名前は `bundle` オブジェクトの `id` プロパティに一致します：`tech.moddable.countdown`。

```text
tech.moddable.countdown
	com.moddable.one
		main.bin
	com.moddable.two
		bootloader.bin
		partition-table.bin
		xs_esp32.bin
	custom
		config
			icon.png
			index.html
	icon.png
```

最終的に、アプリアーカイブフォルダは `tech.moddable.countdown.zip` というアプリアーカイブファイルに圧縮されます。

シェルスクリプト、アプリアーカイブフォルダ、およびアプリアーカイブファイルは `$MODDABLE/build/tmp` に位置しています。

**mcbundle** は `MODDABLE` 環境変数、ESP32デバイスのための `IDF_PATH` 環境変数、およびWasmシミュレータのための `EMSDK` 環境変数を必要とします。

### 引数

```text
mcbundle [manifest] [-d] [-m] [-o directory]
```

- `manifest`: マニフェストファイル。デフォルトは現在のディレクトリの `manifest.json` ファイルです。
- `-d`: デバッグ用のinstrumentedバージョンでビルドします。
- `-m`: `bash` を自動的に実行するために、それ以外の場合は **mcbundle** はシェルスクリプトを生成するだけです。
- `-o directory`: 出力ディレクトリ。デフォルトは現在のディレクトリです。

<a id="mchex"></a>
## mchex

**mchex** は、ファイルをIntel [Hexadecimal Object File Format](https://archive.org/details/IntelHEXStandard)に変換するコマンドラインツールです。

### 引数

```text
mchex file -a address [-n name] [-o directory]
```

- `file`: 変換するファイル。
- `-n name`: 出力ファイル名。`.hex`が追加されます。
- `-o directory`: 出力ディレクトリ。デフォルトは現在のディレクトリです。

<a id="simulator"></a>
## シミュレータ

ハードウェアシミュレータは、macOS、Linux、Windows上でModdableアプリをシミュレータ画面で実行するためのXSマシンをホストします。シミュレータは`mcsim`と名付けられています。

シミュレータのビデオデモンストレーションは[こちら](https://youtu.be/W1Q2oEuuMwg)で利用可能です。

<img src="./../assets/tools/screen-test.png" width="700"/>

Moddableシミュレータの場合、Moddableアプリはmcconfigによってビルドされた動的ライブラリ（mc.soまたはmc.dll）です。シミュレータはそのような動的ライブラリをロードし、その`main`モジュールを実行します。

**mcconfig**によって生成されたmakefileは自動的にシミュレータでModdableアプリを起動します。

- Viewメニューの**Show/Hide Controls**項目は、シミュレータウィンドウの左側にあるコントロールパネルを切り替えます。表示されるコントロールは使用中のデバイスシミュレータに依存します。
- Viewメニューの**Show/Hide Info**項目は、シミュレータウィンドウの下部にある**Informationバー**を切り替えます。Informationバーには、現在実行中のアプリケーションの名前、画面サイズ、ピクセル形式が表示されます。
- Viewメニューの**0° / 90° / 180° / 270°**項目はデバイスシミュレータを回転させます。右上隅にある**Rotate button**もデバイスシミュレータを回転させます。
- シミュレータウィンドウの左上隅にある**Device pop-up**は、シミュレートするデバイスを選択します。
- 右上隅にある**Mode button**は、ライトモードまたはダークモードを選択します。

シミュレータをデバッグに使用する場合、`xsbug` デバッガーには2つのタブが表示されます。1つはシミュレータで実行中のアプリケーション用、もう1つはデバイスシミュレータ用です。デバイスシミュレータのタブは常に `mcsim` という名前で、アプリケーションのタブの名前は実行中のプロジェクトによって異なり、`balls` や `helloworld` などになります。
