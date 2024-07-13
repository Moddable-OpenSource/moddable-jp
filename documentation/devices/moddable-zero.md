# Moddable Zero 開発者ガイド
Copyright 2019-2020 Moddable Tech, Inc.<BR>
改訂日： 2020年10月21日

このドキュメントは、Moddable Zeroに関する情報を提供します。ピンやその他のコンポーネントの詳細、アプリのビルドとデプロイ方法、その他の開発リソースへのリンクが含まれています。

## 目次

- [Moddable Zero について](#about-moddable-zero)
	- [コンポーネント](#components)
	- [ピン配置](#pinout)
- [SDK とホスト環境のセットアップ](#setup)
- [アプリのビルドとデプロイ](#building-and-deploying-apps)
- [トラブルシューティング](#troubleshooting)
- [開発リソース](#development-resources)
	- [サンプル](#examples)
	- [ドキュメント](#documentation)
	- [サポート](#support)
	- [更新](#updates)

<a id="about-moddable-zero"></a>
## Moddable Zero について

<img src="../assets/moddable/moddable-zero-sm.jpg">

Moddable Zeroは、開発者が安価なハードウェアでModdable SDKを試すのを容易にするハードウェアモジュールです。Moddable Zeroは販売されていませんが、サポートは継続されています。[こちらの手順](../displays/wiring-guide-generic-2.4-spi.md)に従って自分でModdable Zeroを作成するか、その代替品である [Moddable One](./moddable-one.md) を購入することができます。

<a id="components"></a>
### コンポーネント

Moddable Zeroの主なコンポーネントは、ESP8266モジュールと抵抗膜式タッチスクリーンの2つです。ESP8266モジュールにはWi-Fiアンテナと4MBのフラッシュストレージメモリが含まれています。タッチスクリーンは、ILI9341ディスプレイコントローラとXPT2046抵抗膜式タッチコントローラによって駆動される、240 x 320 QVGAディスプレイで、16ビットピクセルを持っています。

<a id="pinout"></a>
### ピン配置

<img src="../assets/devices/moddable-zero-pinout.png">

<a id="setup"></a>
## SDKとホスト環境のセットアップ

Moddable Zeroでアプリをビルドして実行するには、以下の手順が必要です：

1. [Moddable SDK](./../Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [ESP8266ツール](./esp8266.md)をインストールする
3. 以下の**アプリのビルドとデプロイ**セクションの指示に従う

<a id="building-and-deploying-apps"></a>
## アプリのビルドとデプロイ

ホスト環境をセットアップした後、以下の手順でModdable Zeroにアプリケーションをインストールします。

1. Moddable ZeroをマイクロUSBケーブルでコンピュータに接続します。

	データ同期対応のケーブルを使用していることを確認してください。電源供給のみのケーブルではありません。

2. `mcconfig`を使用してアプリをビルドおよびデプロイします。

	`mcconfig`は、マイクロコントローラーおよびシミュレーター上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)にあります。

	Moddable Zero用にビルドするには、`mcconfig`でプラットフォーム`-p esp/moddable_zero`を使用します。例えば、[`piu/balls`のサンプル](../../examples/piu/balls)をビルドするには：

	```
	cd $MODDABLE/examples/piu/balls
	mcconfig -d -m -p esp/moddable_zero
	```

	[サンプルのreadme](../../examples)には、画面の回転、Wi-Fiの設定など、他の一般的に使用される`mcconfig`引数に関する追加情報が含まれています。

<a id="troubleshooting"></a>
## トラブルシューティング

一般的な問題とその解決方法については、[ESP8266ドキュメント](./esp8266.md)のトラブルシューティングセクションを参照してください。

<a id="development-resources"></a>
## 開発リソース

<a id="examples"></a>
### サンプル

Moddable SDKには、その多くの機能を示す150以上の[サンプルアプリ](../../examples)があります。これらのサンプルの大部分はModdable Zeroで動作します。

とはいえ、すべてのサンプルがModdable Zeroハードウェアと互換性があるわけではありません。例えば、ESP8266にはBLE機能がないため、BLEのサンプルはビルドも実行もできません。一部のサンプルは、Moddable Zeroのディスプレイと互換性のない特定のディスプレイやタッチドライバをテストするために設計されており、ビルドエラーが発生します。

<a id="documentation"></a>
### ドキュメント {/*documentation*/}

Moddable SDKのすべてのドキュメントは[documentation](../)ディレクトリにあります。**documentation**、**examples**、および**modules**ディレクトリは共通の構造を持っており、情報を簡単に見つけることができます。いくつかのハイライトは次のとおりです：

- `commodetto`サブディレクトリには、2DグラフィックスAPIを提供するビットマップグラフィックスライブラリであるCommodettoと、軽量レンダリングエンジンであるPocoに関連するリソースが含まれています。
- `piu`サブディレクトリには、複雑でレスポンシブなレイアウトを簡単に作成できるユーザーインターフェースフレームワークであるPiuに関連するリソースが含まれています。
- `networking`サブディレクトリには、ネットワークソケットおよびHTTP/HTTPS、WebSockets、DNS、SNTP、telnetなどのソケット上に構築されたさまざまな標準的で安全なネットワーキングプロトコルに関連するネットワークリソースが含まれています。
- `pins`サブディレクトリには、サポートされているハードウェアプロトコル（デジタル、アナログ、PWM、I2Cなど）に関連するリソースが含まれています。一般的な市販センサー用の多くのドライバと対応するサンプルアプリも利用可能です。

<a id="support"></a>
### サポート

質問がある場合は、[issueを開く](https://github.com/Moddable-OpenSource/moddable/issues)ことをお勧めします。できるだけ早く対応し、他の開発者も助けを提供し、あなたの質問への回答から利益を得ることができます。多くの質問はすでに回答されているので、新しいissueを開く前に以前のissueを検索してみてください。

<a id="updates"></a>
### 更新情報

私たちの活動を追跡する最良の方法は、Twitter ([@moddabletech](https://twitter.com/moddabletech))でフォローすることです。新しい投稿に関する発表や、その他のModdableニュースを[私たちのブログ](http://blog.moddable.com/)で公開しています。
