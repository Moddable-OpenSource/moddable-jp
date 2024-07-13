# Moddable Three 開発者ガイド
Copyright 2019-2022 Moddable Tech, Inc.<BR>
改訂日： 2022年3月22日

このドキュメントは、Moddable Threeに関する情報を提供します。ピンやその他のコンポーネントの詳細、アプリのビルドとデプロイ方法、その他の開発リソースへのリンクが含まれています。

## 目次

- [Moddable Three について](#about-moddable-three)
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

<a id="about-moddable-three"></a>
## Moddable Three について

<img src="../assets/devices/moddable-three.png">

Moddable Threeは、開発者が安価なハードウェアでModdable SDKを試すのを容易にするハードウェアモジュールです。[Moddable のウェブサイト](http://www.moddable.com/moddable-three)で購入できます。

<a id="components"></a>
### コンポーネント

Moddable Threeの主なコンポーネントは、ESP8266モジュールとePaperスクリーンの2つです。ESP8266モジュールにはWi-Fiアンテナと4 MBのフラッシュストレージメモリが含まれています。ePaperスクリーンは122 x 250の白黒ディスプレイです。

<a id="pinout"></a>
### ピン配置

<img src="../assets/devices/moddable-three-pinout.png">

<a id="setup"></a>
## SDK とホスト環境のセットアップ

Moddable Threeでアプリをビルドして実行するには、以下の手順が必要です：

1. [Moddable SDK](./../Moddable%20SDK%20-%20Getting%20Started.md) をインストールする
2. [ESP8266 ツール](./esp8266.md) をインストールする
3. 以下の **アプリのビルドとデプロイ** セクションの指示に従う

<a id="building-and-deploying-apps"></a>
## アプリのビルドとデプロイ

ホスト環境とESP8266ツールをセットアップした後、以下の手順でModdable Threeにアプリケーションをインストールします。

1. Moddable Threeをmicro USBケーブルでコンピュータに接続します。

	データ同期が可能なケーブルを使用していることを確認してください。電源供給のみのケーブルは使用しないでください。

3. `mcconfig`を使ってアプリをビルドしてデプロイする。

`mcconfig`は、マイクロコントローラおよびシミュレータ上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)にあります。

Moddable Three用にビルドするには、`mcconfig`でプラットフォーム`-p esp/moddable_three`を使用します。例えば、[`piu/love-e-ink`のサンプル](../../examples/piu/love-e-ink)をビルドするには：

```text
	cd $MODDABLE/examples/piu/love-e-ink
	mcconfig -d -m -p esp/moddable_three
```

[examples readme](../../examples)には、画面の回転、Wi-Fiの設定など、他の一般的に使用される`mcconfig`引数に関する追加情報が含まれています。

Moddable Threeシミュレータ用にビルドするには、`mcconfig`でプラットフォーム`-p simulator/moddable_three`を使用します。

<a id="troubleshooting"></a>
## トラブルシューティング

一般的な問題とその解決方法については、[ESP8266ドキュメント](./esp8266.md)のトラブルシューティングセクションを参照してください。

<a id="development-resources"></a>
## 開発リソース

<a id="examples"></a>
### サンプル

Moddable SDKには、その多くの機能を示す150以上の[サンプルアプリ](../../examples)があります。これらの例のほとんどはModdable Threeで動作します。

とはいえ、CommodettoやPiuを使用する多くのサンプルは、より高速なリフレッシュレートを持つカラースクリーン用に設計されています。さらに、すべてのサンプルがModdable Threeハードウェアと互換性があるわけではありません。例えば、ESP8266にはBLE機能がないため、BLEの例はビルドも実行もできません。一部の例は、Moddable Threeディスプレイと互換性のない特定のディスプレイおよびタッチドライバをテストするために設計されており、ビルドエラーを引き起こします。

<a id="documentation"></a>
### ドキュメント

Moddable SDKのすべてのドキュメントは[documentation](../)ディレクトリにあります。**documentation**、**examples**、および**modules**ディレクトリは共通の構造を持っており、情報を簡単に見つけることができます。いくつかのハイライトは以下の通りです：

- `commodetto`サブディレクトリには、2DグラフィックスAPIを提供するビットマップグラフィックスライブラリであるCommodettoと、軽量なレンダリングエンジンであるPocoに関連するリソースが含まれています。
- `piu`サブディレクトリには、複雑でレスポンシブなレイアウトを簡単に作成できるようにするユーザーインターフェースフレームワークであるPiuに関連するリソースが含まれています。
- `networking`サブディレクトリには、ネットワークソケットおよびHTTP/HTTPS、WebSockets、DNS、SNTP、telnetなどのソケット上に構築されたさまざまな標準的で安全なネットワーキングプロトコルに関連するネットワークリソースが含まれています。
- `pins`サブディレクトリには、サポートされているハードウェアプロトコル（デジタル、アナログ、PWM、I2Cなど）に関連するリソースが含まれています。一般的な市販センサーの多くのドライバと対応する例示アプリも利用可能です。

<a id="support"></a>
### サポート

質問がある場合は、[issueを開く](https://github.com/Moddable-OpenSource/moddable/issues)ことをお勧めします。できるだけ早く対応しますし、他の開発者も助けを提供したり、あなたの質問への回答から利益を得ることができます。多くの質問はすでに回答されているので、新しいissueを開く前に以前のissueを検索してみてください。

<a id="updates"></a>
### 更新情報

私たちの活動を追跡する最良の方法は、Twitterで私たちをフォローすることです ([@moddabletech](https://twitter.com/moddabletech))。新しい投稿に関する発表や、その他のModdableニュースを[私たちのブログ](http://blog.moddable.com/)でお知らせしています。
