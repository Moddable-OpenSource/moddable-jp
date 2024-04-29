# Moddable SDK – 始め方
### Moddable SDKのインストールとツールのビルド方法
Copyright 2016-2023 Moddable Tech, Inc.<BR>
改訂： 2023年10月12日

このドキュメントは、開発用のコンピュータにModdable SDKをインストールし、そのツールをビルドするための手順を提供します。

## 目次

* [概要](#overview) <pl style="background-color:yellow;">(ここから始めましょう！)</pl>
* セットアップ手順

	| [![Apple logo](./assets/moddable/mac-logo.png)](#mac) | [![Windows logo](./assets/moddable/win-logo.png)](#windows) | [![Linux logo](./assets/moddable/lin-logo.png)](#linux) |
	| :--- | :--- | :--- |
	| •  [システム要件](#mac-requirements)<BR>•  [インストール](#mac-instructions)<BR>•  [トラブルシューティング](#mac-troubleshooting)<BR>•  [更新](#mac-update) | •  [システム要件](#win-requirements)<BR>•  [インストール](#win-instructions)<BR>•  [トラブルシューティング](#win-troubleshooting)<BR>•  [更新](#win-update) | •  [システム要件](#lin-requirements)<BR>•  [インストール](#lin-instructions)<BR>•  [更新](#lin-update)

* [次のステップ](#dev-boards-and-mcus): 開発ボードとMCUでアプリをビルドして実行する

<a id="overview"></a>
## 概要

Moddable SDKを始めるには、以下のステップを踏んでください：

#### ステップ 1: Moddable SDKツールをビルドする

以下のセクションにある**インストール**の指示に従って、使用しているOSに該当する手順を実行してください。これらの手順には、`mcconfig`というコマンドラインツールを使用してデスクトップシミュレーター上で`helloworld`サンプルを実行し、セットアップを確認する手順が含まれています。

> デスクトップシミュレーターについての詳細は、[ツールドキュメント](./tools/tools.md#simulator)の**シミュレーター**セクションを参照してください。

また、Moddable SDKツールの一部としてビルドされるJavaScriptソースレベルデバッガーである`xsbug`を実行する方法も学びます。`xsbug`は、XSプラットフォームのモジュールやアプリケーションのデバッグをサポートするフル機能のデバッガーです。

他のデバッガーと同様に、`xsbug`はブレークポイントの設定、ソースコードの閲覧、コールスタックと変数の閲覧をサポートします。さらに、`xsbug`デバッガーはリアルタイムの計測を提供し、メモリ使用量を追跡し、アプリケーションとリソース消費をプロファイルします。

> `xsbug`についての詳細は、[xsbug](./xs/xsbug.md)のドキュメントを参照してください。

#### ステップ2：対象プラットフォームのツールをインストールする

デバイス上でアプリケーションを実行するには、対象プラットフォームの必要なSDK、ドライバー、および開発ツールをインストールする必要があります。`devices`フォルダには、Moddable SDKがサポートするすべてのMCUの手順が含まれています。このドキュメントの[次のステップ](#dev-boards-and-mcus)：開発ボードとMCUでのアプリのビルドと実行セクションを参照して、対象プラットフォームの手順への詳細とリンクを確認してください。

#### ステップ3：いくつかのサンプルを試す

このリポジトリには、Moddable SDKの多くの機能の使用方法を示す[150以上のサンプルアプリ](./../examples)が含まれています。多くのサンプルは、特定の機能の使用方法を示すために、ソースコードが1ページ未満です。

#### ステップ4：独自のアプリを構築する

サンプルのアプリをカスタマイズする準備ができたら、または最初から自分のアプリを構築する準備ができたら、すべてのJavaScript APIのドキュメントとソースコードが利用可能です。すべてのドキュメントはマークダウン形式で提供されています。Moddable SDKのランタイムを構成するソフトウェアモジュールは、[modules](./../modules)ディレクトリにあります。

**documentation**、**examples**、および**modules**ディレクトリは、情報を簡単に見つけられるように共通の構造を共有しています。

- **base**: time、timer、debug、instrumentation、UUIDを含む基本的なランタイム機能
- **commodetto**: ビットマップグラフィックスライブラリ
- **crypt**: 暗号プリミティブとTLS
- **data**: Base64およびhexエンコーディング
- **drivers**: ディスプレイ、タッチ入力、センサー、拡張用のデバイスドライバー
- **files**: ファイル、フラッシュ、設定、リソース、zipを含むストレージ機能
- **network**: ソケットとソケットに基づくプロトコル（HTTP、WebSockets、DNS、SNTP、telnet、TLS）; また、Wi-FiとBLE API。
- **pins**: ハードウェアプロトコル（デジタル（GPIO）、アナログ、PWM、I2Cを含む）
- **piu**: ユーザーインターフェースフレームワーク

<a id="mac"></a>
## macOS

<a id="mac-requirements"></a>
### システム要件

Moddable SDKは、macOS Sierra（バージョン10.12）以降およびXcodeのフルインストール（バージョン9以降）が必要です。

<a id="mac-instructions"></a>
### インストール

1. [Xcode](https://developer.apple.com/xcode/)をダウンロードしてインストールします。プロンプトが表示されたらXcodeを起動して追加のコマンドラインコンポーネントをインストールします。

2. ホームディレクトリに`~/Projects`で`Projects`ディレクトリを作成し、Moddable SDKリポジトリを配置します。

	> 注意: Moddable SDKリポジトリは任意のディレクトリにダウンロードできます。これらの手順では、Moddable SDKが`~/Projects`ディレクトリにダウンロードされることを前提としています。

3. [Moddableリポジトリ](https://github.com/Moddable-OpenSource/moddable)をダウンロードするか、次のように`git`コマンドラインツールを使用します：

	```text
	cd ~/Projects
	git clone https://github.com/Moddable-OpenSource/moddable
	```

4. シェルのスタートアップ/初期化ファイルを開きます。

	macOS Mojave以前ではデフォルトのシェルは`bash`なので、`~/.profile`を開く必要があります。

	```text
	open ~/.profile
	```

	macOS Catalinaからは、[デフォルトのシェルは`zsh`](https://support.apple.com/en-us/HT208050)なので、`~/.zshrc`を開くべきです。

```text
open ~/.zshrc
```

> 注: 上記のコマンドを実行してエラーが表示される場合（シェルの起動/初期化ファイルが存在しないというエラー）、`touch` コマンドを使用して適切なファイルを作成できます。例えば、`touch ~/.zshrc`。

5. 開いたファイルに以下の行を追加して保存します。これにより、`MODDABLE` 環境変数がローカルのModdable SDKリポジトリディレクトリを指すように設定され、`PATH` 環境変数がModdableのビルドツールディレクトリを含むように編集されます。

```text
export MODDABLE="/Users/<user>/Projects/moddable"
export PATH="${MODDABLE}/build/bin/mac/release:$PATH"
```

5. `~/.profile` や `~/.zshrc` にexport文を追加しても、アクティブなシェルインスタンスの環境変数は更新されないため、新しいシェルインスタンスを開く（新しいタブ/ウィンドウを開く）か、シェルでexport文を手動で実行してから進めてください。

6. コマンドラインからModdableコマンドラインツール、シミュレータ、デバッガをビルドします：

```text
cd ${MODDABLE}/build/makefiles/mac
make
```

7. コマンドラインから `xsbug` デバッガを起動します：

```text
open ${MODDABLE}/build/bin/mac/release/xsbug.app
```

8. デスクトップシミュレータターゲット用のスターター `helloworld` アプリケーションをビルドして、ホスト環境のセットアップを確認します：

```text
cd ${MODDABLE}/examples/helloworld
mcconfig -d -m -p mac
```

9. **重要：** これでデスクトップシミュレータ用のアプリケーションをビルドして実行できます。開発ボードやMCUでアプリケーションをビルドして実行するには、対象プラットフォーム用の追加のSDK、ドライバー、および開発ツールをインストールする必要があります。詳細と手順へのリンクについては、このドキュメントの [次のステップ](#dev-boards-and-mcus)（開発ボードとMCUでのアプリケーションのビルドと実行）セクションを参照してください。

<a id="mac-troubleshooting"></a>
### トラブルシューティング

- Moddable SDKのビルドが "`xcode-select: error: tool 'ibtool' requires Xcode, but active developer directory '/Library/Developer/CommandLineTools' is a command line tools instance.`" のようなエラーで失敗する場合、3つの潜在的な問題と修正があります：
	 1. コマンドラインツールのみのXcodeインストールを実行している可能性があります。Moddable SDKのビルドにはXcodeの完全なインストールが必要です。
	 2. Xcodeのライセンス契約を受け入れ、コマンドラインコンポーネントをインストールするためにXcodeアプリケーションを起動する必要があるかもしれません。
	 3. 完全なXcodeインストールを実行しているが、このエラーでビルドが失敗する場合は、`xcode-select` ユーティリティを使用して完全なXcodeインストールを選択する必要があります。これは、環境に応じて必要に応じてパスを調整して、このコマンドで行うことができます： `sudo xcode-select -s /Applications/Xcode.app/Contents/Developer`

<a id="mac-update"></a>
### 更新

Moddable SDKツールは頻繁に改善され、機能が追加されます。以下の手順に従って、定期的にホスト環境を更新する必要があります：

1. [Moddableリポジトリ](https://github.com/Moddable-OpenSource/moddable)のローカルクローンを更新します：

	```text
	cd $MODDABLE
	git pull
	```

	> Moddableリポジトリのファイルにローカル変更がある場合は、変更をスタッシュしてから、プルした後に再適用する必要があるかもしれません：

	```text
	cd $MODDABLE
	git stash push
	git pull
	git stash pop
	```

2. 既存のModdable SDKビルド出力を削除します：

	```text
	cd $MODDABLE
	rm -rf build/bin
   rm -rf build/tmp
	```

3. Moddableコマンドラインツール、シミュレータ、デバッガをビルドします：

	```text
	cd ${MODDABLE}/build/makefiles/mac
	make
	```

4. デスクトップシミュレータターゲット用のスターター`helloworld`アプリケーションをビルドすることで、ホスト環境のセットアップを確認します：

	```text
	cd ${MODDABLE}/examples/helloworld
	mcconfig -d -m -p mac
	```

<a id="windows"></a>
## Windows

<a id="win-requirements"></a>
### システム要件

Moddable SDKは、Windows 8.1以降およびMicrosoft Visual Studio Community 2017以降が必要です。Windows 10またはWindows 11とMicrosoft Visual Studio Community 2022を推奨します。

<a id="win-instructions"></a>
### インストール方法

1. [Microsoft Visual Studio 2022 Communityインストーラー](https://www.visualstudio.com/downloads/)をダウンロードします。

2. インストーラーを起動します。「Workloads」タブで「Desktop development for C++」オプションを選択します。「Individual Components」タブで「Windows 10 SDK (10.0.19041.0)」を選択します（これはWindows 10システムでは事前に選択されていますが、Windows 11システムでは手動で含める必要があります）。設定された通りにインストールを進めます。

3. `%USERPROFILE%`ディレクトリ、例えば`C:\Users\<your-user-name>`に`Projects`ディレクトリを作成し、Moddable SDKリポジトリを配置します。

	> 注意: Moddable SDKリポジトリは任意のディレクトリにダウンロードできます。これらの指示は、Moddable SDKが`%USERPROFILE%`ディレクトリにダウンロードされることを前提としています。

4. [Moddableリポジトリ](https://github.com/Moddable-OpenSource/moddable)をダウンロードするか、以下のように`git`コマンドラインツールを使用します：

	```text
	cd %USERPROFILE%\Projects
	git clone https://github.com/Moddable-OpenSource/moddable
	```

5. [リンク先の手順](https://www.architectryan.com/2018/08/31/how-to-change-environment-variables-on-windows-10/)に従ってコントロールパネルアプリの「環境変数」ダイアログを開きます。そのダイアログから：
 - ユーザー変数として`MODDABLE`を作成し、ローカルのModdable SDKリポジトリディレクトリを指すように設定します。システムに合わせてパスを更新し、「ディレクトリを参照。..」ボタンを使用してModdable SDKフォルダに移動します。
	- 変数名：`MODDABLE`
	- 変数値：`%USERPROFILE%\Projects\moddable`

 - ユーザー変数`Path`を編集して、Moddable SDKツールディレクトリを含むようにします。システムに合わせてパスを更新し、「参照。..」ボタンを使用して正しいフォルダに移動します。
	- 変数名：`Path`
	- 変数値（既存のリストに追加）：`%USERPROFILE%\Projects\moddable\build\bin\win\release`

> 注意: `%USERPROFILE%\Projects\moddable\build\bin\win\release` ディレクトリは次のステップで作成されます。環境変数を今設定しても安全ですし、ステップ6の後で `Path` に追加しても構いません。

> 注意: 環境変数を設定した後は、新しいコマンドプロンプトを開いてください。新しい環境設定は既存のコマンドプロンプトインスタンスでは有効になりません。

6. "x86 Native Tools Command Prompt for VS 2022" コマンドラインコンソールを起動します。コマンドラインからModdableコマンドラインツール、シミュレータ、およびデバッガをビルドします：

	```text
	cd %MODDABLE%\build\makefiles\win
	build
	```

	> 注意: `parallel_build.bat` という代替のビルドバッチファイルがあり、マルチコアシステムで使用すると Moddable SDK のビルド速度を大幅に向上させることができます。ただし、このバッチファイルを使用すると診断エラーメッセージが見にくくなることがあります。システムで `build.bat` が正しく動作する場合は、将来の Moddable SDK ビルドに `parallel_build.bat` の使用を検討してください。

7. コマンドラインから `xsbug` デバッガを起動します：

	```text
	xsbug
	```

8. デスクトップシミュレータターゲット用のスターター `helloworld` アプリケーションをビルドして、ホスト環境のセットアップを確認します：

	```text
	cd %MODDABLE%\examples\helloworld
	mcconfig -d -m -p win
	```

9. **重要：** これでデスクトップシミュレータでアプリケーションをビルドして実行できるようになりました。開発ボードやMCUでアプリケーションをビルドして実行するには、対象プラットフォーム用の追加のSDK、ドライバー、および開発ツールをインストールする必要があります。詳細と指示へのリンクについては、このドキュメントの[次のステップ](#dev-boards-and-mcus)（開発ボードとMCUでのアプリケーションのビルドと実行）セクションを参照してください。

<a id="win-troubleshooting"></a>
### トラブルシューティング

 - Moddable SDKのビルドが "`LINK : fatal error LNK1104: cannot open file '<Moddable path>\build\bin\win\release\<tool name>.exe'`" のようなエラーで失敗する場合、それは[Microsoftのドキュメント](https://docs.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-error-lnk1104?view=vs-2019)で説明されているように、アンチウイルスソフトウェアとの競合を示している可能性が高いです。ビルドプロセス中に `%MODDABLE%` ディレクトリをアンチウイルスソフトウェアのライブスキャン機能から除外してみてください。

- Moddable SDKのビルドが"`'nmake' is not recognized as an internal or external command, operable program or batch file.`"のエラーで失敗した場合、3つの潜在的な問題と修正方法があります：
	1. Visual Studioがインストールされていない可能性があります。[Microsoft Visual Studio 2022 Community Edition installer](https://www.visualstudio.com/downloads/)の使用をお勧めします。
	2. Visual Studioをインストールした際に、「Desktop development for C++」ワークロードオプションを選択していない可能性があります。[インストーラー](https://www.visualstudio.com/downloads/)を再実行し、そのワークロードをインストールするオプションを選択してください。
	3. デフォルトのコマンドプロンプトではなく、「VS 2022用x86ネイティブツールコマンドプロンプト」でビルドを試みた可能性があります。このコマンドプロンプトはビルドに重要な環境変数を設定し、スタートメニューで「x86 Native Tools」と検索することで見つけることができます。

- Moddable SDKのビルドが"`fatal error LNK1112: module machine type 'x64' conflicts with target machine type 'x86'`"のようなエラーで失敗した場合、おそらく「VS 2022用x64ネイティブツールコマンドプロンプト」ではなく、正しい「VS 2022用x86ネイティブツールコマンドプロンプト」でビルドを試みたことが原因です。このコマンドプロンプトはビルドに重要な環境変数を設定し、スタートメニューで「x86 Native Tools」と検索することで見つけることができます。

<a id="win-update"></a>
### 更新

Moddable SDKツールは頻繁に改善や機能追加のために更新されます。以下の手順に従って定期的にホスト環境を更新する必要があります。

	> 注: 以下のコマンドはすべて「VS 2022用 x86 ネイティブツール コマンドプロンプト」コンソールで実行してください。

1. [Moddable リポジトリ](https://github.com/Moddable-OpenSource/moddable)のローカルクローンを更新します：

	```text
	cd %MODDABLE%
	git pull
	```

	> Moddable リポジトリのファイルにローカル変更がある場合は、変更をスタッシュしてからプルした後に再適用する必要があるかもしれません：

	```text
	cd %MODDABLE%
	git stash push
	git pull
	git stash pop
	```

2. 既存のModdable SDKビルド出力を削除します：

	```text
	cd %MODDABLE%\build\makefiles\win
	build clean
	```

3. Moddableコマンドラインツール、シミュレータ、デバッガをビルドします：

	```text
	cd %MODDABLE%\build\makefiles\win
	build
	```

	> 注: マルチコアシステムで Moddable SDK のビルドを大幅に高速化できる `parallel_build.bat` という代替ビルドバッチファイルがあります。ただし、このバッチファイルを使用すると診断エラーメッセージが見にくくなることがあります。`build.bat` がシステムで正しく動作する場合は、将来の Moddable SDK ビルドに `parallel_build.bat` を検討してください。

4. デスクトップシミュレータターゲット用のスターター `helloworld` アプリケーションをビルドして、ホスト環境のセットアップを確認します：

	```text
	cd %MODDABLE%\examples\helloworld
	mcconfig -d -m -p win
	```

<a id="linux"></a>
## Linux

<a id="lin-requirements"></a>
### システム要件

Moddable SDKは、Ubuntu 16.04-22.04 LTS（64ビット）およびRaspberry Pi Desktop（32ビット）オペレーティングシステムでテストされています。これらの指示は、GCCツールチェーンがすでにインストールされていることを前提としています。

<a id="lin-instructions"></a>
### インストール方法

1. コンパイルに必要なパッケージをインストールまたは更新します：

	```text
	sudo apt-get install gcc git wget make libncurses-dev flex bison gperf
	```

2. GTK+ 3ライブラリの開発バージョンをインストールします：

	```text
	sudo apt-get install libgtk-3-dev
	```

	> 注：WSL2をWindowsで使用している場合は、以下のアイコンパックをインストールしてください：

	```text
	sudo apt-get install adwaita-icon-theme-full
	```

3. `~/Projects` に `Projects` ディレクトリを作成し、そこにModdable SDKリポジトリを配置します。

> 注意: Moddable SDK リポジトリは任意のディレクトリにダウンロードできます。これらの指示では、Moddable SDK が `~/Projects` ディレクトリにダウンロードされていると仮定しています。

4. [Moddable リポジトリ](https://github.com/Moddable-OpenSource/moddable)をダウンロードするか、以下のように `git` コマンドラインツールを使用します：

	```text
	cd ~/Projects
	git clone https://github.com/Moddable-OpenSource/moddable
	```

5. `~/.bashrc` ファイルに `MODDABLE` 環境変数を設定し、ローカルのModdable SDKリポジトリディレクトリを指すようにします。Moddableビルドツールディレクトリを `PATH` に追加します：

	```text
	export MODDABLE=~/Projects/moddable
	export PATH=$PATH:$MODDABLE/build/bin/lin/release
	```

	> 注意: 進む前に新しいシェルインスタンスを開くか、手動でシェルにエクスポート文を実行する必要があります。`~/.profile` にエクスポート文を追加しても、アクティブなシェルインスタンスの環境変数は更新されません。

6. コマンドラインからModdableコマンドラインツール、シミュレータ、およびデバッガをビルドします：

```text
cd $MODDABLE/build/makefiles/lin
make
```

7. デスクトップシミュレータと `xsbug` デバッガーアプリケーションをインストールします：

	```text
	cd $MODDABLE/build/makefiles/lin
	make install
	```

	プロンプトが表示されたら、アプリケーションのデスクトップ、実行可能ファイル、アイコンファイルを標準の `/usr/share/applications`, `/usr/bin`, `/usr/share/icon/hicolor` ディレクトリにコピーするために、`sudo` パスワードを入力してください。

8. コマンドラインから `xsbug` デバッガーを起動します：

	```text
	xsbug
	```

9. デスクトップシミュレータターゲット用のスターター `helloworld` アプリケーションをビルドして、ホスト環境のセットアップを確認します：

	```text
	cd $MODDABLE/examples/helloworld
	mcconfig -d -m -p lin
	```

	>  何を期待すべきかの説明のために[このdiscussionを見てください](https://github.com/Moddable-OpenSource/moddable/discussions/1097)。

10. **重要：** これでデスクトップシミュレータでアプリケーションをビルドして実行できるようになりました。開発ボードやMCUでアプリケーションをビルドして実行するには、ターゲットプラットフォーム用の追加のSDK、ドライバー、開発ツールをインストールする必要があります。詳細と指示へのリンクについては、このドキュメントの [次のステップ](#dev-boards-and-mcus)（開発ボードとMCUでのアプリケーションのビルドと実行）セクションを参照してください。

<!--<a id="lin-troubleshooting"></a>
### トラブルシューティング
-->

<a id="lin-update"></a>
### 更新

Moddable SDKツールは頻繁に改善され、機能が追加されます。定期的に以下の手順に従ってホスト環境を更新する必要があります：

1. GTK+ 3ライブラリの開発バージョンがインストールされ、最新の状態であることを確認してください：

	```text
	sudo apt-get update
	sudo apt-get install libgtk-3-dev
	sudo apt-get upgrade libgtk-3-dev
	```

2. [Moddableリポジトリ](https://github.com/Moddable-OpenSource/moddable)のローカルクローンを更新します：

	```text
	cd $MODDABLE
	git pull
	```

	> Moddableリポジトリのファイルにローカル変更がある場合は、変更をスタッシュしてから、プルした後に再適用する必要があるかもしれません：

	```text
	cd $MODDABLE
	git stash push
	git pull
	git stash pop
	```

3. 既存のModdable SDKビルド出力を削除します：

	```text
	cd $MODDABLE
	rm -rf build/tmp
	rm -rf build/bin
	```

4. Moddableのコマンドラインツール、シミュレータ、デバッガーをビルドします：

```text
cd $MODDABLE/build/makefiles/lin
make
```

5. デスクトップシミュレータと `xsbug` デバッガアプリケーションをインストールします：

	```text
	cd $MODDABLE/build/makefiles/lin
	make install
	```

	プロンプトが表示されたら、アプリケーションのデスクトップ、実行可能ファイル、アイコンファイルを標準の `/usr/share/applications`, `/usr/bin`, `/usr/share/icon/hicolor` ディレクトリにコピーするために `sudo` パスワードを入力します。

6. デスクトップシミュレータターゲット用のスターター `helloworld` アプリケーションをビルドして、ホスト環境のセットアップを確認します：

	```text
	cd $MODDABLE/examples/helloworld
	mcconfig -d -m -p lin
	```

<a id="dev-boards-and-mcus"></a>
## 次のステップ：開発ボードとMCUでアプリをビルドして実行する

開発ボードまたはMCUでアプリケーションをビルドして実行するには、ターゲットプラットフォーム用の追加のSDK、ドライバ、開発ツールをインストールする必要があります。これらのツールはMCUの製造元によって提供され、Moddable SDKの一部ではありませんが、`devices` フォルダにはインストールプロセスをガイドするために私たちが書いた手順が含まれています。

以下の表は、ターゲットプラットフォームの指示をすばやく見つけるために、`devices` フォルダ内のいくつかのドキュメントへのリンクを提供します：

| デバイス | ドキュメント |
| :---: | :--- |
| ESP32またはESP32ベースの製品<BR>Moddable Two、NodeMCU ESP32、<BR>およびM5Stack開発ボードを含む | [ESP32でのModdable SDKの使用](./devices/esp32.md)
| ESP8266またはESP8266ベースの製品<BR>Moddable One、Moddable Three、<BR>およびNodeMCU ESP8266を含む | [ESP8266でのModdable SDKの使用](./devices/esp8266.md)
| Giant Gecko、Mighty Gecko、<BR>Thunderboard Sense 2、またはBlue Gecko | [GeckoでのModdable SDKの使用](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/devices/gecko/GeckoBuild.md)
| QCA4020 | [QCA4020でのModdable SDKの使用](./devices/qca4020/README.md)
| Raspberry Pi Pico | [PicoでのModdable SDKの使用](./devices/pico.md) |
| Nordic nRF52 | [nRF52でのModdable SDKの使用](./devices/nrf52.md) |
