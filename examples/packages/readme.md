# パッケージサンプル
2023年10月4日更新

このディレクトリには、ビルド依存関係を記述するために `package.json` を使用するサンプルのプロジェクトが含まれています。これらは、組み込みアプリケーションをパッケージ化するためのModdable SDKのツールである `mcpack` と共に使用することを意図しています。`mcpack` はModdableビルドプロセスのフロントエンドです。`package.json` から始まり、依存関係をスキャンし、Moddableの `manifest.json` ファイルを生成します。このファイルは `mcconfig` を使用してビルド、デプロイ、および実行されます。

## 開発の簡素化
`mcpack` には、開発を効率化し、一般的なJavaScriptの慣習との互換性を提供するためのいくつかの機能があります。これはプロジェクトのソースコードを分析することで実現されます。

- **組み込みモジュール**。プロジェクトが "timer"、"wifi"、"pins/digital"、"piu/MC" などのModdable SDKに組み込まれたモジュールをインポートすると、`mcpack` はそのモジュールをプロジェクトに含めます。組み込みモジュールは、明示的にModdable SDKの組み込みモジュールであることを示すために "moddable:timer" のように "moddable:" で接頭辞を付けることもできます。
- **よく知られたグローバル変数**。JavaScript開発者は、`setTimeout`、`console`、`fetch` などの特定のグローバル変数をJavaScript環境全体で使用することに慣れています。`mcpack` はこれらのグローバル変数の使用を検出し、必要なモジュールを自動的に含め、グローバル変数を定義します。以下のグローバル変数がサポートされています。
	- `clearImmediate`、`setImmediate`、`setInterval`、`setTimeout` - タイマー関数。
	- `console.log()` - デバッグコンソールへの出力。他の `console` の機能は提供されません。
	- `fetch` - HTTPリクエスト。
	- `Headers` - 通常 `fetch()` と共に使用されるHTTPヘッダーのマップ。
	- `structuredClone` - JavaScriptオブジェクトのクローン。
	- `TextDecoder` - バイナリバッファをJavaScript文字列に変換。
	- `TextEncoder` - JavaScript文字列をUTF-8形式のバッファに変換。
	- `URL` - URLの解析。
	- `Worker`、`SharedWorker` - 複数の独立したJavaScript仮想マシン（Web Workers）の使用。
- **トップレベル await (TLA)**。Moddable SDKはTLAをサポートしていますが、プロジェクトで明示的に有効にする必要があります。これにより、機能を使用しないプロジェクトが小さくなります。`mcpack` はTLAの使用を検出し、自動的に `MODDEF_MAIN_ASYNC` ビルドフラグを設定します。

`package.json` のサポートはModdable SDKの `manifest.json` を置き換えるものではありませんが、多くのプロジェクトでは `package.json` だけで十分です。`mcpack` は `package.json` から `manifest.json` を生成し、マニフェストは `package.json` と一緒に使用することもできます。

## サンプルの実行
シミュレータでパッケージのサンプルを実行するには、次の手順を実行します：

```
cd $MODDABLE/examples/package/hello
npm install
mcpack mcconfig -d -m -p sim
```

`npm install` は、外部依存関係を取得するために初回ビルド時にのみ必要です。`mcpack` ツールの呼び出しは通常の `mcconfig` に続きます。同じプロジェクトをESP32 Node-MCUボードで実行するには：

```
mcpack mcconfig -d -m -p esp32/nodemcu
```

ネットワークを必要とするパッケージ（例えば `fetch`）を実行するには、通常通りWi-Fiの認証情報をmcconfigに指定します：

```
mcpack mcconfig -d -m -p esp/moddable_one ssid="My Wi-Fi" password=secret
```

## サンプル
次のセクションでは、各サンプルについての注釈を記載します。注釈では、各サンプルが示す `package.json` と `mcpack` のいくつかの機能について説明します。

### balls
これは、クラシックなModdable SDK Piu [balls](https://github.com/Moddable-OpenSource/moddable/blob/public/examples/piu/balls/main.js) サンプルのpackage.jsonバージョンです。`package.json` に加えて、グラフィカルアセットを含め、タッチ入力処理を無効にするために使用される `manifest.json` があります。manifest.jsonは、その名前と `package.json` と同じディレクトリにある場所に基づいて自動的に含まれます。

main.jsが次のようにしていることに注意してください：

```js
import {} from "piu/MC";
```

このインポートは自動的に検出され、生成されたマニフェストに `manifest_piu.json` が含まれます：

```
> mcpack mcconfig -d -m
# mcpack include: $(MODDABLE)/examples/manifest_piu.json
```

### eventemitter
このサンプルは、有名な [`eventemitter3`](https://www.npmjs.com/package/eventemitter3) npmパッケージのModdable SDKポートである `@moddable/eventemitter3` パッケージの使用を示しています。

### fetch
このサンプルでは、`fetch()` APIを使用していくつかのHTTPリクエストを行います。main.jsでは、アプリケーションが `fetch`、`URL`、`Headers`、および `console.log` グローバルにアクセスするため、これらは `mcpack` によって自動的に含まれます。

```
> mcpack mcconfig -d -m
# mcpack include: $(MODDABLE)/modules/data/url/manifest.json
# mcpack define: URL, URLSearchParams
# mcpack include: $(MODDABLE)/modules/data/headers/manifest.json
# mcpack define: Headers
# mcpack include: $(MODDABLE)/examples/io/tcp/fetch/manifest_fetch.json
# mcpack define: fetch
# mcpack define: console
# xsc globals.xsb
```

`mcpack` はグローバル変数を初期化するために `globals.js` ファイルを生成します。

### hello
このサンプルは、モジュールをプロジェクトに含めるさまざまな方法を示しています。このサンプルではnpmからの外部モジュールを使用するため、`mcpack` を実行する前にそのディレクトリで `npm install` を行ってそれらをダウンロードする必要があります。

`package.json` はnpmから3つのパッケージをインポートします。そのうちの2つはパッケージのインポートとマッピングを示す例です。3つ目は有名なmustacheライブラリで、標準のJavaScriptとサポートされている依存関係を使用して実装されたモジュールが、Moddable SDKのXS JavaScriptエンジンのES2023サポートのおかげで組み込みデバイス上で実行できることを示しています。

`package.json` の `imports` セクションは、2つの重要な機能を示しています：

```json
"imports": {
	"#test": {
	  "moddable": "./test-xs.js",
	  "default": "./test-node.js"
	}
}
```

まず、`"#test"` インポートはプライベートな相対指定子であるため、このパッケージ内でのみモジュールが利用可能になります。次に、package.jsonはターゲットランタイムに応じてモジュールの異なる実装を提供できます。ここでは、キー "moddable" は、`mcpack` を使用してModdable SDK用にビルドする場合にtest-xs.jsが使用され、それ以外の場合にはtest-node.jsが使用されることを意味します。これは、組み込みターゲットおよびその他のターゲットでプロジェクトを実行できるように整理するための強力なツールです。

### mod-balls

これは、`mcpack` を使用して [mod](https://www.moddable.com/documentation/xs/mods) をビルドおよび実行するサンプルです。`mcrun` を使用します。このサンプルを実行する前に、modのホストをインストールするために `run-mod` 例を実行する必要があります。

mod-ballsは、`package.json` を使用したmodとしてのクラシックな [balls](https://github.com/Moddable-OpenSource/moddable/blob/public/examples/piu/balls/main.js) サンプルです。シミュレータで実行するには、次のコマンドを使用します：

```
mcpack mcrun -d -m
```

上記のballsのようにグラフィカルアセットを含めるために外部の `manifest.json` を使用するのではなく、`package.json` 内にマニフェストを埋め込んでいます。埋め込みマニフェストを使用するか外部マニフェストを使用するかは好みの問題であり、機能的な違いはありません。

```json
	"moddable": {
		"manifest": {
			"resources": {
				"*": "./balls"
			}
		}
	}
```

modをビルドするために `mcpack` を使用する場合、ホストがそれを処理すると仮定して、自動インポートやグローバルの初期化は行いません。組み込みのModdable SDKモジュールへの依存関係は、マニフェストに追加することで引き続き含めることができます。

### run-mod
これは [Piu](https://www.moddable.com/documentation/piu/piu) ユーザーインターフェースフレームワークを使用する [mods](https://www.moddable.com/documentation/xs/mods) を実行するためのホストです。

run-mod自体はあまり多くのことをしません。何かを描画するためにはmodに依存します。run-modがインストールされたら、mod-ballsのサンプルを実行してください。

### timers
このサンプルは [timers](https://github.com/Moddable-OpenSource/moddable/blob/public/examples/base/timers/main.js) サンプルのウェブ互換バリエーションです。`timer` モジュールを直接使用する代わりに、グローバルを通じて `setTimeout`（およびその仲間）を使用します。`mcpack` はこれらのよく知られたグローバルの使用を検出し、これらの関数の実装をglobals.jsに自動的に追加してスクリプトで利用できるようにします。これらのグローバルの実装は "timer" モジュールを使用します。

### top-level-await
この例は、プロジェクトの `main.js` でもトップレベルのawaitを使用できる2つの異なる方法を示しています。メインモジュールは "wow" モジュールをインポートし、トップレベルでawaitを使用してその戻り値を初期化します。このプロミスはメインの実行が続行される前に解決されます。メインが実行されると、`setTimeout` のために500ミリ秒待機し、その後「done waiting」を出力します。

### ts-hello
この例では、TypeScriptソースファイルを`mcpack`を使用して`package.json`にどのように使用できるかを示しています。`package.json`には、TypeScriptソースをJavaScriptに変換するための「build」スクリプトが含まれています：

```
  "scripts": {
    "build": "tsc"
   }
```

このスクリプトを`mcpack`ビルドの一部として呼び出すには、次のコマンドラインを使用します：

```
npm run build && mcpack mcconfig -d -m
```

開発者がTypeScriptソースを処理する方法は多岐にわたります。これはそのワークフローの一例です。

ディレクトリには、TypeScriptコンパイラを構成するための`tsconfig.json`が含まれています。これは、TypeScriptコンパイラのターゲット出力をES2022に設定するため重要です。JavaScriptの以前のバージョンをターゲットにすると、TypeScriptが生成するJavaScriptが効率が悪くなり、場合によっては組み込み実行と互換性がなくなることがあります。
