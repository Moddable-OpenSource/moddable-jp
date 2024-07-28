# M5Paper 開発者ガイド
Copyright 2021-2022 Moddable Tech, Inc.<BR>
改訂： 2022年3月22日

このドキュメントは、Moddable SDKを使用してM5Paperを使用する方法、アプリのビルドとデプロイ方法、その他の開発リソースへのリンクについての情報を提供します。

<img src="./../assets/devices/m5paper.png" width=300>

## 目次

- [SDKとホスト環境のセットアップ](#setup)
- [アプリのビルドとデプロイ](#building-and-deploying-apps)
- [トラブルシューティング](#troubleshooting)
- [開発リソース](#development-resources)
	- [ポートステータス](#port-status)
	- [ディスプレイドライバ](#display-driver)
		- [更新モード](#update-modes)
		- [画像フィルタ](#image-filters)
	- [サンプル](#examples)
	- [ドキュメント](#documentation)
	- [サポート](#support)
	- [更新](#updates)

<a id="setup"></a>
## SDKとホスト環境のセットアップ

M5Paperでアプリをビルドして実行するには、以下が必要です：

1. [Moddable SDK](./../Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [ESP32ツール](./esp32.md)をインストールする
3. (macOSユーザーのみ） 以下の**macOS**セクションの指示に従う
4. 以下の**アプリのビルドとデプロイ**セクションの指示に従う

<a id="macOS"></a>
### macOS

macOSでのM5PaperのUSBドライバの状況は少し厄介です。以下の手順を実行してください。

- macOS Big Sur以降を実行する
- この[issue](https://github.com/Xinyuan-LilyGO/LilyGo-T-Call-SIM800/issues/139#issuecomment-904390716)で参照されているドライバをインストールする

<a id="building-and-deploying-apps"></a>
## アプリのビルドとデプロイ

> Moddable SDKには、M5Paperを最大限に活用する方法を示すいくつかのサンプルアプリケーションがあります。詳細は[ePaperブログ](https://blog.moddable.com/blog/epaper#examples)の投稿を参照してください。

ホスト環境とESP32ツールをセットアップした後、以下の手順でM5Paperにアプリケーションをインストールします。

1. デバイスに付属のUSBケーブルでM5Paperをコンピュータに接続します。

2. `mcconfig`を使用してアプリをビルドおよびデプロイします。

	`mcconfig`は、マイクロコントローラおよびシミュレータ上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)で利用できます。

	`mcconfig`を使用してM5Paper用にビルドするには、プラットフォーム`-p esp32/m5paper`を使用します。例えば、[`epaper-photos`のサンプル](../../examples/piu/epaper-photos)をビルドするには次のようにします。

	```text
	cd $MODDABLE/examples/piu/epaper-photos
	mcconfig -d -m -p esp32/m5paper
	```

	[examples readme](../../examples) には、画面の回転、Wi-Fiの設定など、他の一般的に使用される `mcconfig` 引数に関する追加情報が含まれています。

	プラットフォーム `-p simulator/m5paper` を `mcconfig` と共に使用して、M5Paperシミュレータ用にビルドします。

<a id="troubleshooting"></a>
## トラブルシューティング

一般的な問題とその解決方法については、[ESP32ドキュメント](./esp32.md) のトラブルシューティングセクションを参照してください。

<a id="development-resources"></a>
## 開発リソース

<a id="port-status"></a>
### ポートステータス

以下が実装され、動作しています：

- EPDディスプレイドライバ
- GT911タッチドライバ
- SHT30温度/湿度センサー
- A / B / Cボタン
- RTC

> **注意**: GT911タッチコントローラのI2Cアドレスは揺らぎがあります。実装ではアドレス0x14と0x5Dの両方を試みます。これはホストプロバイダのTouchコンストラクタで処理され、ドライバやユーザースクリプトでは処理されません。0x14が失敗すると、0x5Dで再試行する前に例外がスローされます。これに遭遇した場合は、xsbugでGoを押してください。

<a id="display-driver"></a>
### Display Driver

ディスプレイドライバーは[Poco `PixelsOut`](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/commodetto.md#pixelsout-class)の実装です。これにより、Moddable SDKの[Poco](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/poco.md)グラフィックスAPIと[Piu](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/piu/piu.md)ユーザーインターフェースフレームワークの両方を使用することができます。

多くの既存のPocoおよびPiuの例はEPDで動作しますが、その多くは実用的ではありません。これらは高リフレッシュレートの小型カラーLCD用に設計されているため、低リフレッシュレートの大きなグレーディスプレイでは見た目が滑稽になることが多いです。このディスプレイ用に設計されたいくつかの例が必要です。

ディスプレイドライバーは完全にJavaScriptで書かれています。すべてのハードウェアアクセスに[Ecma-419 IO](https://419.ecma-international.org/#-9-io-class-pattern) APIを使用しています。パフォーマンスは優れており、ネイティブのM5Paperライブラリに組み込まれているEPDクラスよりも高速なことが多いです。その理由の1つは、Pocoが4ビットグレーピクセルに直接レンダリングできるため、ピクセルフォーマットの変換が不要になることです。もう一つの理由は、ディスプレイコントローラーへのSPI転送が一度に数千ピクセルのバルク転送で行われるため、4ピクセルずつ転送する場合に比べて転送されるビット数が半分以上削減されることです。

メモリ使用量も非常に低いです。ESP32メモリにはフレームバッファがなく、レンダリングされたピクセルは16ラインのレンダーバッファ（約8 KB）から直接ディスプレイに送信されます。

Pocoの`continue`機能を使用すると、EPDパネルを一度だけリフレッシュしながら画面の複数の領域を更新することが可能です。これにより非常に効率的な更新が可能になります。つまり、転送されるメモリの量が最小限に抑えられ、長いパネルフラッシュが一度だけ発生します。Piuの`balls`のサンプルはこれを実際に見るのに良い方法です。ボールの画像（その周りの空白部分ではなく）のみがディスプレイに転送され、4つのボールを囲む矩形のみがディスプレイパネルでフラッシュします。

ディスプレイコントローラの回転機能がサポートされており、0度、90度、180度、および270度の回転でオーバーヘッドなしの回転が可能です。

<a id="update-modes"></a>
#### 更新モード
ディスプレイコントローラは、いくつかの異なる[更新モード](https://github.com/phoddie/m5paper/blob/f0b79e0a0579c0dbdb1bb4445dc6acf501403681/targets/m5paper/it8951.js#L82-L93)をサポートしています。最適なモードは描画されるコンテンツによって異なります。モードは各フレームで変更することができます。デフォルトのモードは`GLD16`です。モードを変更するには、グローバル`screen`オブジェクトの`config`メソッドを呼び出します。例えば：

```js
screen.config({updateMode: "A2"});
```

新しいアプリをデバイスにインストールすると、前のアプリから画面に残っているアーティファクトが見えることがあります。これを取り除くためには、高品質モード（例：`GC16`）で少なくとも1フレームを完全に描画してから、より高速な更新モード（例：`A2`）に切り替えることが有効です。[epaper-flashcards](https://github.com/Moddable-OpenSource/moddable/blob/public/examples/piu/epaper-flashcards/main.js)のサンプルでは、ほとんどのアプリに適用できるパターンを使用しています：

```js
onDisplaying(application) {
	screen.refresh?.();
	screen.configure?.({updateMode: config.firstDrawMode ?? config.updateMode});
	if (config.firstDrawMode)
		application.defer("onFinishedFirstDraw", config.updateMode);
	this.showNextCard(application, 1); // render the initial screen of the app
}
onFinishedFirstDraw(application, mode) {
	screen.configure({updateMode: mode});
}
```

このパターンを使用すると、デバイスごとの`firstDrawMode`および`updateMode`設定をプロジェクトの`manifest.json`に適用できます：

```json
"platforms": {
	"esp32/m5paper": {
		"config": {
			"firstDrawMode": "GC16",
			"updateMode": "A2"
		}
	}
}
```

<a id="image-filters"></a>
#### 画像フィルター
ディスプレイドライバーは、いくつかの異なる[ピクセルフィルター](https://github.com/phoddie/m5paper/blob/4110701c8084c07d7f777a44e17e970ffd18f729/targets/m5paper/it8951.js#L342-L349)をサポートしています。これらのフィルターはピクセルの輝度を調整します。画像の最適化や特殊効果の適用に役立ちます。デフォルトのフィルターは「なし」です。フィルターは各フレームごとに変更できます。フィルターを変更するには、グローバルな`screen`オブジェクトの`config`メソッドを呼び出します。例えば：

```js
screen.config({filter: "negative"});
```

フィルターは16個の値を持つ `Uint8Array` です。組み込みフィルターの代わりに独自のフィルターを設定するには、次のようにします：

```js
let filter = new Uint8Array(16);
// ここでフィルターを初期化するコード
screen.config({filter});
```

<a id="examples"></a>
### サンプル

Moddable SDKには、その多くの機能を使用する方法を示す150以上の[サンプルアプリ](../../examples)があります。これらのサンプルの多くはM5Paperで動作します。

とはいえ、すべてのサンプルがM5Paperハードウェアと互換性があるわけではありません。例えば、一部のサンプルはM5Paperディスプレイと互換性のない特定のディスプレイおよびタッチドライバをテストするために設計されており、ビルドエラーが発生します。

Moddable SDKには、M5Paperを最大限に活用する方法を示すいくつかのサンプルアプリケーションがあります。詳細については、[ePaperブログ](https://blog.moddable.com/blog/epaper#examples)の投稿を参照してください。

<a id="documentation"></a>
### ドキュメント

Moddable SDKのすべてのドキュメントは[documentation](../)ディレクトリにあります。**documentation**、**examples**、および**modules**ディレクトリは共通の構造を持っており、情報を簡単に見つけることができます。いくつかのハイライトは次のとおりです：

- `commodetto` サブディレクトリには、2DグラフィックスAPIを提供するビットマップ グラフィックス ライブラリであるCommodettoと、軽量レンダリング エンジンであるPocoに関連するリソースが含まれています。
- `piu` サブディレクトリには、複雑で応答性の高いレイアウトを簡単に作成できるようにするユーザー インターフェイス フレームワークであるPiuに関連するリソースが含まれています。
- `networking` サブディレクトリには、BLE、ネットワーク ソケット、およびHTTP/HTTPS、WebSockets、DNS、SNTP、telnetなどのソケット上に構築されたさまざまな標準の安全なネットワーキング プロトコルに関連するネットワーキング リソースが含まれています。
- `pins` サブディレクトリには、サポートされているハードウェア プロトコル (デジタル、アナログ、PWM、I2Cなど） に関連するリソースが含まれています。市販の一般的なセンサー用の多数のドライバーと対応するサンプル アプリも利用できます。

<a id="support"></a>
### サポート

ご質問がある場合は、[issue を開く](https://github.com/Moddable-OpenSource/moddable/issues) ことをお勧めします。できるだけ早く対応し、他の開発者が支援を提供し、質問への回答から利益を得ることができます。多くの質問にはすでに回答されているため、新しいissueを開く前に以前のissueを検索してみてください。

<a id="updates"></a>
### 更新情報

私たちの活動をフォローする最良の方法は、Twitter ([@moddabletech](https://twitter.com/moddabletech)) をフォローすることです。新しい投稿に関するお知らせやその他のModdableニュースを[私たちのブログ](http://blog.moddable.com/)で発表しています。
