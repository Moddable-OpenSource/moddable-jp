# M5Core Ink 開発者ガイド
Copyright 2021 Moddable Tech, Inc.<BR>
改訂： 2021年11月2日

このドキュメントは、Moddable SDKを使用してM5Core Inkを使用する方法、アプリのビルドとデプロイ方法、その他の開発リソースへのリンクについての情報を提供します。

<img src="./../assets/devices/m5coreink.png" width=250>

## 目次

- [SDKとホスト環境のセットアップ](#setup)
- [アプリのビルドとデプロイ](#building-and-deploying-apps)
- [トラブルシューティング](#troubleshooting)
- [開発リソース](#development-resources)
	- [ポートステータス](#port-status)
	- [ディスプレイドライバ](#display-driver)
	- [ボタン](#buttons)
	- [LED](#led)
	- [ブザー](#buzzer)
	- [バッテリー電圧](#battery-voltage)
	- [バッテリー駆動の操作](#battery-powered-operation)
	- [サンプル](#examples)
	- [ドキュメント](#documentation)
	- [サポート](#support)
	- [更新](#updates)

<a id="setup"></a>
## SDKとホスト環境のセットアップ

M5Core Inkでアプリをビルドして実行するには、以下が必要です：

1. [Moddable SDK](./../Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [ESP32ツール](./esp32.md)をインストールする
3. 以下の**アプリのビルドとデプロイ**セクションの指示に従う

<a id="building-and-deploying-apps"></a>
## アプリのビルドとデプロイ

> Moddable SDKには、M5Core Inkを最大限に活用する方法を示すいくつかのサンプルアプリケーションがあります。詳細については、[ePaperブログ](https://blog.moddable.com/blog/epaper#examples)の投稿を参照してください。

ホスト環境とESP32ツールをセットアップした後、以下の手順に従ってM5Core Inkにアプリケーションをインストールします。

1. デバイスに付属のUSBケーブルを使用して、M5Core Inkをコンピュータに接続します。

2. `mcconfig`を使用してアプリをビルドおよびデプロイします。

	`mcconfig`は、マイクロコントローラおよびシミュレータ上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)で利用できます。

	`mcconfig`を使用してM5Core Ink用にビルドするには、プラットフォーム`-p esp32/m5core_ink`を使用します。例えば、[`epaper-photos`のサンプル](../../examples/piu/epaper-photos)をビルドするには、次のようにします：

	```text
	cd $MODDABLE/examples/piu/epaper-photos
	mcconfig -d -m -p esp32/m5core_ink
	```

	[サンプルのreadme](../../examples)には、画面の回転、Wi-Fi設定など、他の一般的に使用される`mcconfig`引数に関する追加情報が含まれています。

<a id="troubleshooting"></a>
## トラブルシューティング

一般的な問題とその解決方法については、[ESP32 ドキュメント](./esp32.md)のトラブルシューティングセクションを参照してください。

<a id="development-resources"></a>
## 開発リソース

<a id="port-status"></a>
### ポートステータス

以下が実装され、動作しています：

- EPDディスプレイドライバ (GDEW0154M09)
- RTC (PCF8563 / BM8563)
- 上 / 下 / 中央 / 電源 / 外部ボタン
- LED
- ブザー
- バッテリー電圧

<a id="display-driver"></a>
### ディスプレイドライバ

ディスプレイドライバは [Poco `PixelsOut`](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/commodetto.md#pixelsout-class) の実装です。これにより、[Poco](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/poco.md) グラフィックスAPIとModdable SDKの[ Piu](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/piu/piu.md) ユーザーインターフェースフレームワークの両方を使用することができます。

ディスプレイドライバは完全にJavaScriptで書かれています。すべてのハードウェアアクセスに [Ecma-419 IO](https://419.ecma-international.org/#-9-io-class-pattern) APIを使用しています。パフォーマンスは非常に優れており、ネイティブのM5Core Inkライブラリに組み込まれているEPDクラスよりも高速なことが多いです。

ディスプレイドライバはディザリングを実装しており、白黒のピクセルのみを使用して多くのグレーレベルを表示することができます。デフォルトのディザリングアルゴリズムは古典的な[Atkinson dither](https://twitter.com/phoddie/status/1274054345969950720)です。Burkesに変更するか、ディザリングを無効にするには次のようにします：

```js
screen.dither = "burkes";
screen.dither = "none"
```

ディザリングは新しい `commodetto/Dither` モジュールを使用して実行されます。

ディザリングをサポートするために、ドライバはPocoが8ビットグレー（256グレー）でレンダリングすることを要求します。また、ドライバはディザリングを使用するとシームが表示されるため、部分更新ではなくフルスクリーン更新のみをサポートします。ディザリングを使用しない場合には部分更新がサポートされるべきであり、実際に有用です。

ディスプレイドライバはバックバッファを維持します。これはディスプレイコントローラの動作方式のため、ほぼ必須です。幸いなことに、バッファは1ビットピクセルを使用できるため、わずか5000バイトです。

ディスプレイドライバはインスタンス化後の最初の描画時にフルスクリーンリフレッシュを行います。これを無効にするには、`refresh` をfalseに設定します。

```js
screen.configure({refresh: false});
```

ディスプレイドライバは最初のフレームの後、部分更新を使用します。フルスクリーン更新を強制するには、次のようにします。

```js
screen.configure({refresh: true});
```

初回のフルスクリーンフラッシュを避けるために、電源投入時に部分更新を行うことがあります。このためには、前のフレームをコントローラのメモリに戻すために再描画する必要があります。これを行うには、まず `previous` をtrueに設定して前のフレームを描画し、その後通常通り次のフレームを描画します。ドライバは1フレーム描画後に `previous` の値をfalseにリセットします。

```js
screen.configure({previous: true, refresh: false});
// draw previous
// draw next

```

ハードウェア回転はサポートされていません。サポートすることは可能ですが、ディスプレイが正方形であるため、明らかに有用ではありません。PocoとPiuの両方がビルド時にソフトウェア回転をサポートしているため、必要に応じて回転は可能ですが、実行時には利用できません。

<a id="buttons"></a>
### ボタン

すべてのボタンは変更時にコールバックを使用して利用可能です。各ボタンにコールバックをインストールする基本的なテストは次の通りです。

```js
for (let name in device.peripheral.button) {
	new device.peripheral.button[name]({
		onPush() {
			trace(`Button ${name}: ${this.pressed}\n`);
		}
	})
}
```

<a id="led"></a>
### LED

ユニットの上部にある緑色のLEDは使用可能です：

```js
const led = new device.peripheral.led.Default;
led.on = 1;		// full strength
led.on = 0;		// off
led.on = 0.5;		// half strength
```

<a id="buzzer"></a>
## ブザー

ブザーは音を鳴らすために実装されています。M5スピーカーAPIと同様に、音は即座に再生されます。

ブザーをインスタンス化します：

```js
const buzzer = new device.peripheral.tone.Default;
```

音名とオクターブで音を鳴らします：

```js
buzzer.note("C", 4);
```

音名とオクターブでミリ秒単位の固定時間音を鳴らします：

```js
buzzer.note("Bb", 4, 500);
```

周波数で音を鳴らします：

```js
buzzer.tone(262);
```

周波数でミリ秒単位の固定時間音を鳴らします：

```js
buzzer.tone(262, 500);
```

ブザーをミュートします（次の音やトーンが再生されると自動的にミュート解除されます）:

```js
buzzer.mute();
```

ブザーを閉じます：

```js
buzzer.close();
```

<a id="battery-voltage"></a>
### バッテリー電圧

バッテリー電圧を取得するには：

```js
const battery = new device.peripheral.battery.Default;
const voltage = battery.read();
```

<a id="battery-powered-operation"></a>
### バッテリー駆動操作

M5Core Inkをバッテリーで動作させるには、パワーホールドラインを1に設定する必要があります。これは`setup/target`モジュールでデフォルトで行われます。

```js
power.main.write(1);
```

バッテリーで動作しているときにデバイスをオフにするには、パワーホールドラインを0に設定します。

```js
power.main.write(0);
```

<a id="examples"></a>
### サンプル

Moddable SDKには、その多くの機能を使用する方法を示す150以上の[サンプルアプリ](../../examples)があります。これらの多くのサンプルはM5Core Inkで動作します。

とはいえ、すべてのサンプルがM5Core Inkハードウェアと互換性があるわけではありません。例えば、一部のサンプルはM5Core Inkディスプレイと互換性のない特定のディスプレイおよびタッチドライバをテストするために設計されており、ビルドエラーが発生します。

タッチに依存しないModdable SDK Piuのサンプルは、一般的にそのまま動作するようですが、いくつかはePaperディスプレイでは見栄えが良くありません。M5Core Inkのディスプレイリフレッシュレートは約3 FPSなので、`balls`のようなサンプルは見栄えが良くありません。

M5Core Inkを最大限に活用する方法を示すいくつかのサンプルアプリケーションがModdable SDKにあります。詳細については[ePaperブログ](https://blog.moddable.com/blog/epaper#examples)の投稿を参照してください。

<a id="documentation"></a>
### ドキュメント

Moddable SDKのすべてのドキュメントは [documentation](../) ディレクトリにあります。**documentation**、**examples**、および **modules** ディレクトリは共通の構造を持っており、情報を簡単に見つけることができます。いくつかのハイライトは次のとおりです：

- `commodetto` サブディレクトリには、2DグラフィックスAPIを提供するビットマップ グラフィックス ライブラリであるCommodettoと、軽量レンダリング エンジンであるPocoに関連するリソースが含まれています。
- `piu` サブディレクトリには、複雑で応答性の高いレイアウトを簡単に作成できるユーザー インターフェイス フレームワークであるPiuに関連するリソースが含まれています。
- `networking` サブディレクトリには、BLE、ネットワーク ソケット、およびHTTP/HTTPS、WebSockets、DNS、SNTP、telnetなどのソケット上に構築されたさまざまな標準の安全なネットワーク プロトコルに関連するネットワークリソースが含まれています。
- `pins` サブディレクトリには、サポートされているハードウェア プロトコル (デジタル、アナログ、PWM、I2Cなど） に関連するリソースが含まれています。一般的な市販センサー用の多くのドライバーと対応するサンプル アプリも利用可能です。

<a id="support"></a>
### サポート

質問がある場合は、[issueを開く](https://github.com/Moddable-OpenSource/moddable/issues)ことをお勧めします。できるだけ早く対応しますし、他の開発者も助けを提供したり、あなたの質問への回答から利益を得ることができます。多くの質問はすでに回答されているので、新しいissueを開く前に以前のissueを検索してみてください。

<a id="updates"></a>
### 更新情報

私たちの活動を追跡する最良の方法は、Twitterでフォローすることです ([@moddabletech](https://twitter.com/moddabletech))。新しい投稿に関する発表や、その他のModdableニュースを[私たちのブログ](http://blog.moddable.com/)で公開しています。
