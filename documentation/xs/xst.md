# xst
更新日: 2024年6月3日

`xst`はXSテストエンジンで、Linux、macOS、WindowsでXSをテストするためのJavaScriptエンジンです。

## ビルド

### Linux

	cd $MODDABLE/xs/makefiles/lin
	make

### macOS

	cd $MODDABLE/xs/makefiles/mac
	make

### Windows

	cd %MODDABLE%\xs\makefiles\win
	build

## ダウンロード

[jsvu CLI](https://github.com/GoogleChromeLabs/jsvu)を使用して**xst**をインストールまたは更新できます。

`PATH`上のディレクトリに[moddable-xst](https://github.com/Moddable-OpenSource/moddable-xst/releases)リポジトリから`xst`の最新版をダウンロードすることもできます。


## 使用方法

	xst [-h] [-e] [-m] [-s] [-v] [-l] [-lc] [-b] [-j] strings...

- `-h`: このヘルプメッセージを表示
- `-e`: `strings`を評価
- `-m`: `strings`はモジュールへのパス
- `-s`: `strings`はスクリプトへのパス
- `-v`: XSバージョンと設定（スロットとIDサイズ）を表示
- `-l`: Hardened JavaScriptの`lockdown`下でスクリプトを実行
- `-lc`: Hardened JavaScriptの`lockdown`下で`Compartment`内でスクリプトを実行
- `-b` - スクリプトはUTF-8データのバイナリバッファ。パース前にTextDecoderを通す
- `-j` - スクリプトはJSONで、実行する代わりに`JSON.parse()`に渡す


`-e`、`-m`、または`-s`オプションがない場合、`strings`は**test262**ケースまたはディレクトリへのパスです。

### eshost

**eshost**でXSをテストするには、[eshost CLI](https://github.com/bterlson/eshost-cli)をインストールします。その後、XSをホストに追加します：

	eshost --add 'XS' xs ~/.jsvu/xst

**eshost**は**xst**の`-s`オプションを使用します。

### test262

**test262**でXSをテストするには、[test262](https://github.com/tc39/test262)をクローンし、`test262`ディレクトリ内の`test`ディレクトリに移動します。例えば：

	cd ~/test262/test
	xst language/block-scope
	xst built-ins/TypedArrays/buffer-arg-*

**XS**が現在**test262**ケースをどのようにパスしているかの詳細については、[XS準拠性](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/xs/XS%20Conformance.md)を参照してください。
