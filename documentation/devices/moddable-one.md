# Moddable One 開発者ガイド
Copyright 2019-2022 Moddable Tech, Inc.<BR>
改訂日： 2022年3月22日

このドキュメントは、Moddable Oneに関する情報を提供します。ピンやその他のコンポーネントの詳細、アプリのビルドとデプロイ方法、その他の開発リソースへのリンクが含まれています。

## 目次

- [Moddable Oneについて](#about-moddable-one)
	- [コンポーネント](#components)
	- [ピン配置](#pinout)
- [SDKとホスト環境のセットアップ](#setup)
- [アプリのビルドとデプロイ](#building-and-deploying-apps)
- [トラブルシューティング](#troubleshooting)
- [開発リソース](#development-resources)
	- [サンプル](#examples)
	- [ドキュメント](#documentation)
	- [サポート](#support)
	- [更新](#updates)

<a id="about-moddable-one"></a>
## Moddable Oneについて

<img src="../assets/devices/moddable-one.png">

Moddable Oneは、開発者が安価なハードウェアでModdable SDKを試すのを容易にするハードウェアモジュールです。[Moddableのウェブサイト](http://www.moddable.com/moddable-one)で購入できます。

<a id="components"></a>
### コンポーネント

Moddable Oneの主なコンポーネントは、ESP8266モジュールと静電容量式タッチスクリーンの2つです。ESP8266モジュールにはWi-Fiアンテナと4 MBのフラッシュストレージメモリが含まれています。タッチスクリーンは、FT6206静電容量式タッチコントローラを備えたMIPIディスプレイシリアルインターフェース互換のディスプレイコントローラによって駆動される240 x 320 QVGA IPSディスプレイです。

<a id="pinout"></a>
### ピン配置

<img src="../assets/devices/moddable-one-pinout.png">


#### タッチパネル

タッチパネルはピン4と5を使用してI<sup>2</sup>Cバスに接続します。アプリがタッチパネルを使用する場合、これらのピンをGPIOとして使用することはできません。

アプリがタッチパネルを使用していても、同じアドレスを使用しない限り、他のI<sup>2</sup>Cセンサーを使用することができます。タッチパネルはアドレス0x38（7ビットアドレス）を使用します。

#### ディスプレイ

ディスプレイはピン13と14を使用してSPIバスに接続します。アプリがディスプレイを使用する場合、これらのピンをGPIOとして使用することはできません。

アプリがディスプレイを使用していても、他のSPIセンサーを使用することができます。

<a id="setup"></a>
## SDKとホスト環境のセットアップ

Moddable Oneでアプリをビルドして実行するには、以下の手順が必要です。

1. [Moddable SDK](./../Moddable%20SDK%20-%20Getting%20Started.md)をインストールする
2. [ESP8266ツール](./esp8266.md)をインストールする
3. 以下の**アプリのビルドとデプロイ**セクションの指示に従う

<a id="building-and-deploying-apps"></a>
## アプリのビルドとデプロイ

ホスト環境とESP8266ツールをセットアップした後、以下の手順でModdable Oneにアプリケーションをインストールします。

1. プログラマーをModdable Oneに接続する。

	プログラマーの向きが正しいことを確認してください。向きは以下の画像と一致する必要があります。

	<img src="../assets/devices/moddable-one-programmer.jpg">

	**注意**: Moddable OneのUSBポートは、プログラマーなしで動作する場合に電源を供給するために使用できます。USBポートはModdable Oneのプログラムには使用できません。電源供給のみです。

2. プログラマーをマイクロUSBケーブルでコンピューターに接続する。

	USBケーブルはプログラマーに接続する必要があります。ボード上の電源専用USBポートには接続しないでください。データ同期が可能なケーブルを使用していることを確認してください。電源専用のケーブルではありません。

3. `mcconfig`を使用してアプリをビルドおよびデプロイする。

`mcconfig`は、マイクロコントローラおよびシミュレータ上でModdableアプリをビルドおよび起動するためのコマンドラインツールです。`mcconfig`の完全なドキュメントは[こちら](../tools/tools.md)にあります。

Moddable One用にビルドするには、`mcconfig`でプラットフォーム `-p esp/moddable_one` を使用します。例えば、[`piu/balls`の例](../../examples/piu/balls)をビルドするには：

```text
cd $MODDABLE/examples/piu/balls
mcconfig -d -m -p esp/moddable_one
```

[サンプルのreadme](../../examples)には、画面の回転、Wi-Fi設定など、他の一般的に使用される`mcconfig`引数に関する追加情報が含まれています。

Moddable Oneシミュレータ用にビルドするには、`mcconfig`でプラットフォーム `-p simulator/moddable_one` を使用します。

<a id="troubleshooting"></a>
## トラブルシューティング

一般的な問題とその解決方法については、[ESP8266ドキュメント](./esp8266.md)のトラブルシューティングセクションを参照してください。

<a id="development-resources"></a>
## 開発リソース

<a id="examples"></a>
### サンプル

Moddable SDKには、その多くの機能を使用する方法を示す150以上の[サンプルアプリ](../../examples)があります。これらの例の大部分はModdable Oneで実行されます。

とはいえ、すべてのサンプルがModdable Oneハードウェアと互換性があるわけではありません。たとえば、ESP8266にはBLE機能がないため、BLEの例はビルドも実行もできません。一部のサンプルは、Moddable Oneディスプレイと互換性のない特定のディスプレイおよびタッチドライバをテストするために設計されており、ビルドエラーが発生します。

<a id="documentation"></a>
### ドキュメント

Moddable SDKのすべてのドキュメントは[documentation](../)ディレクトリにあります。**documentation**、**examples**、および**modules**ディレクトリは、情報を簡単に見つけられるように共通の構造を共有しています。いくつかのハイライトを以下に示します：

- `commodetto`サブディレクトリには、2DグラフィックスAPIを提供するビットマップグラフィックスライブラリであるCommodettoと、軽量レンダリングエンジンであるPocoに関連するリソースが含まれています。
- `piu`サブディレクトリには、複雑で応答性の高いレイアウトを簡単に作成できるユーザーインターフェースフレームワークであるPiuに関連するリソースが含まれています。
- `networking`サブディレクトリには、ネットワークソケットおよびHTTP/HTTPS、WebSockets、DNS、SNTP、telnetなどのソケット上に構築されたさまざまな標準の安全なネットワーキングプロトコルに関連するネットワークリソースが含まれています。
- `pins`サブディレクトリには、サポートされているハードウェアプロトコル（デジタル、アナログ、PWM、I2Cなど）に関連するリソースが含まれています。一般的な市販センサー用の多くのドライバと対応する例示アプリも利用可能です。

<a id="support"></a>
### サポート

質問がある場合は、[issueを開く](https://github.com/Moddable-OpenSource/moddable/issues)ことをお勧めします。できるだけ早く対応しますし、他の開発者も助けを提供したり、あなたの質問への回答から利益を得ることができます。多くの質問はすでに回答されているので、新しいissueを開く前に以前のissueを検索してみてください。

<a id="updates"></a>
### 更新情報

私たちの活動を追跡する最良の方法は、Twitter ([@moddabletech](https://twitter.com/moddabletech)) でフォローすることです。新しい投稿に関するお知らせや、その他のModdableニュースを[私たちのブログ](http://blog.moddable.com/)で投稿しています。
