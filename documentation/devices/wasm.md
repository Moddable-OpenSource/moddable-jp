# Wasm
Copyright 2019-2022 Moddable Tech, Inc.<BR>
改訂日： 2022年1月27日

このドキュメントは、Wasmプラットフォーム用のModdable SDKアプリをビルドおよび実行するための手順を提供します。

![](./../assets/devices/wasm.gif)

## 目次

- セットアップ手順

	| [![Apple logo](./../assets/moddable/mac-logo.png)](#mac) | [![Linux logo](./../assets/moddable/lin-logo.png)](#lin) |
	| :---: | :---: |
	| [セットアップ手順](#mac) | [セットアップ手順](#lin)

- [制限事項](#limitations)

<a id="mac"></a>
## macOS

1. まだ[Emscripten](https://emscripten.org/)リポジトリを持っていない場合は、リポジトリを`~/Projects`ディレクトリにクローンします。

	```text
	cd ~/Projects
	git clone https://github.com/emscripten-core/emsdk.git
	```

	すでにEmscriptenリポジトリを持っている場合は、以下のコマンドを使用して最新バージョンにアップグレードします。

	```text
	cd ~/Projects/emsdk
	git pull
	```

2. ホストプラットフォームにリストされているすべてのEmscriptenの前提条件を、[Emscriptenダウンロードおよびインストールのウェブページ](https://emscripten.org/docs/getting_started/downloads.html#platform-notes-installation-instructions-sdk)の**Platform-specific notes**セクションで確認してください。

3. 最新バージョンのEmscriptenをインストールしてアクティベートします。

	```text
	cd ~/Projects/emsdk
	./emsdk install latest
	./emsdk activate latest
	```

	> 私たちはバージョン3.1.1（コミット `0f0ea34526515d0b2caa262ab5915bc1a7e5dd71`）で最後にテストしました。

4. まだ[Binaryen](https://github.com/WebAssembly/binaryen)リポジトリを持っていない場合は、`~/Projects`ディレクトリにリポジトリをクローンします。

	```text
	cd ~/Projects
	git clone --recursive https://github.com/WebAssembly/binaryen.git
	```

	すでにBinaryenリポジトリを持っている場合は、以下のコマンドを使用して最新バージョンにアップグレードします：

	```text
	cd ~/Projects/binaryen
	git pull origin main --recurse-submodules
	```

	Binaryenリポジトリの更新中に問題が発生した場合は、単にbinaryenディレクトリを削除して再クローンすることができます：

	```text
	cd ~/Projects
	rm -rf binaryen
	git clone --recursive https://github.com/WebAssembly/binaryen.git
	```

5. Binaryenツールをビルドします。

	```text
	cd ~/Projects/binaryen
	cmake . && make
	```

	> 最後にテストしたのは、wasm-opt バージョン 105（コミット `060442225165d0423d06ea33ab865e850b54f61b`）を使用したときです。
6. 	次のコマンドを `~/.profile` に貼り付けて、`PATH` とその他の環境変数を設定します。最初のコマンドは、Emscriptenの環境変数を設定するシェルスクリプトをソースします。2つ目のコマンドは、BinaryEnを含むように `PATH` を更新します。

	```text
	source ~/Projects/emsdk/emsdk_env.sh
	export PATH=~/Projects/binaryen/bin:$PATH
	```

	> 注: これらの指示は、新しいターミナルが開かれたときにシェルが `~/.profile` からソースすることを前提としています。使用するシェルや設定方法によっては、そうならない場合があります。macOS Catalina 以降では、[デフォルトのシェルは `zsh`](https://support.apple.com/en-us/HT208050) であり、`~/.zshrc` を使用します。

	> 注: 続行する前に、新しいシェルインスタンスを開くか、シェルでエクスポート文を手動で実行する必要があります。エクスポート文を `~/.profile` に追加しても、アクティブなシェルインスタンスの環境変数は更新されません。

7. コマンドラインからModdable Wasmツールをビルドします：

	```text
	cd ${MODDABLE}/build/makefiles/wasm
	make
	```

8. テストするために、`wasm`ターゲット用の`balls`サンプルをビルドします。

	```text
	cd $MODDABLE/examples/piu/balls
	mcconfig -d -m -p wasm
	```

	ローカルHTTPサーバーでホスティングすることで、ブラウザでビルドされたアプリを実行できます。Pythonはこれを行うための簡単なツールを提供します。

	Python 2を使用している場合：

	```text
	cd $MODDABLE/build/bin/wasm/debug/balls
	python -m SimpleHTTPServer
	```

	Python 3を使用している場合：

	```text
	cd $MODDABLE/build/bin/wasm/debug/balls
	python3 -m http.server
	```

	ブラウザで[`localhost:8000`](http://localhost:8000)にアクセスします。シミュレーターが実行されている`balls`のウェブページが表示されるはずです。

<a id="lin"></a>
## Linux

1. まだ[EmscriptenPlatform-specific notes](https://emscripten.org/)リポジトリを`~/Projects`ディレクトリに持っていない場合は、最新バージョンをアクティベートします。

	```text
	cd ~/Projects
	git clone https://github.com/emscripten-core/emsdk.git
	cd emsdk
	```

既にEmscriptenリポジトリを持っている場合は、以下のコマンドを使用して最新バージョンにアップグレードしてください。

```text
cd ~/Projects/emsdk
git pull
```

2. ホストプラットフォームに必要なすべてのEmscriptenの前提条件が、[Emscriptenダウンロードおよびインストールのウェブページ](https://emscripten.org/docs/getting_started/downloads.html#platform-notes-installation-instructions-sdk)の**Platform-specific notes**セクションにリストされていることを確認してください。

3. 最新バージョンのEmscriptenをインストールしてアクティブ化します。

```text
cd ~/Projects/emsdk
./emsdk install latest
./emsdk activate latest
```

> 最後にテストしたバージョンは3.1.2（コミット `476a14d60d0d25ff5a1bfee18af73a4b9bfbd385`）です。

4. まだ[Binaryen](https://github.com/WebAssembly/binaryen)リポジトリを持っていない場合は、リポジトリを`~/Projects`ディレクトリにクローンします。

```text
cd ~/Projects
git clone --recursive https://github.com/WebAssembly/binaryen.git
```

既にBinaryenリポジトリを持っている場合は、以下のコマンドを使用して最新バージョンにアップグレードしてください。

	```text
	cd ~/Projects/binaryen
	git pull origin main --recurse-submodules
	```

	Binaryenリポジトリの更新中に問題が発生した場合は、単にbinaryenディレクトリを削除して再クローンすることができます：

	```text
	cd ~/Projects
	rm -rf binaryen
	git clone --recursive https://github.com/WebAssembly/binaryen.git
	```

5. Binaryenツールをビルドします。

	```text
	cd ~/Projects/binaryen
	cmake . && make
	```

	> wasm-optバージョン105（コミット `707be2b55075dccaaf0a70e23352c972fce5aa76`）で最後にテストしました。
6. `PATH`およびその他の環境変数を設定するために、以下のコマンドを`~/.bashrc`に貼り付けます。最初のコマンドは、Emscriptenの環境変数を設定するシェルスクリプトをソースします。2番目のコマンドは、BinaryEnを含むように`PATH`を更新します。

	```text
	source ~/Projects/emsdk/emsdk_env.sh
	export PATH=~/Projects/binaryen/bin:$PATH
	```

	> 注意: 続行する前に、新しいシェルインスタンスを開くか、シェルでエクスポート文を手動で実行する必要があります。エクスポート文を`~/.profile`に追加しても、アクティブなシェルインスタンスの環境変数は更新されません。

4. コマンドラインからModdable Wasmツールをビルドします：

	```text
	cd ${MODDABLE}/build/makefiles/wasm
	make
	```

5. テストするために、`wasm`ターゲット用の`balls`サンプルをビルドします。

	```text
	cd $MODDABLE/examples/piu/balls
	mcconfig -d -m -p wasm
	```

	ローカルHTTPサーバーでホスティングすることで、ブラウザでビルドされたアプリを実行できます。Pythonはこれを行うための簡単なツールを提供しています。

	Python 2を使用している場合：

	```text
	cd $MODDABLE/build/bin/wasm/debug/balls
	python -m SimpleHTTPServer
	```

	Python 3を使用している場合：

	```text
	cd $MODDABLE/build/bin/wasm/debug/balls
	python3 -m http.server
	```

	ブラウザで[`localhost:8000`](http://localhost:8000)にアクセスします。`balls`が実行されているシミュレータを含むウェブページが表示されるはずです。

<a id="limitations"></a>
## 制限事項

Moddable SDKのすべての機能がWasmシミュレータでサポートされているわけではありません。

現在サポートされている機能は次のとおりです：

- XSがサポートするすべてのJavaScript機能
- Piuユーザーインターフェースフレームワーク
- Commodettoグラフィックスライブラリ

サポートされていない機能は次のとおりです：

- ファイル
- ソケット
- BLE
- ピン/IO
