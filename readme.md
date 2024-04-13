# Moddable SDK

Copyright 2017-2020 Moddable Tech, Inc., 2024 Shinya Ishikawa<BR>
改訂： 2020年2月11日<BR>
翻訳： 2024年4月13日


## マイクロコントローラーのためのモダンなソフトウェア開発

**Moddable SDKは、マイクロコントローラー用のアプリケーションを作成するための開発ツールとランタイムソフトウェアの組み合わせです。**

マイクロコントローラーは、現代のコンピューターやモバイルデバイスと比べると、大幅に制約のあるデバイスです。Moddable SDKが対象とする代表的なマイクロコントローラーは、約45 KBの空きメモリ、1 MBのフラッシュROMを持ち、80 MHzで動作します。Moddable SDKは、こうした制約の中でも効率的に動作するために、ビルド時とランタイムの両方で様々な技術を使用します。

![JS logo](./documentation/assets/moddable/js.png)

開発の主要なプログラミング言語はJavaScriptです。Moddable SDKの中核をなすXS JavaScriptエンジンは、[2019年のJavaScript言語標準](http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-262.pdf)に99％以上準拠しています。<sup>[[1](#footnotes)]</sup> ただし、対象マイクロコントローラーの制約により、1つのアプリケーションで使用できる言語機能の組み合わせには制限がある場合があります。

Moddable SDKで実装されているJavaScript言語は、WebページやNode.jsで使用されるものと同じですが、スクリプトが実行されるマイクロコントローラーは、パソコンやサーバー、モバイルデバイスとは大きく異なります。こうした違いから、JavaScriptの使い方にも異なるアプローチが必要となることが少なくありません。Moddable SDKのAPIやオブジェクトは、メモリ使用を最小限に抑えることを目的として設計されており、かなり異なる特徴を持っています。JavaScriptに関する既存の経験を活かしつつ、パフォーマンス、コードサイズ、メモリ使用について異なる考え方をもつ必要があります。<sup>[[2](#footnotes)]</sup>

可能な限り、Moddable SDKはJavaScriptで実装されています。パフォーマンス向上やネイティブAPIへの直接アクセスのために、Moddable SDKの一部はCで実装されています。<sup>[[3](#footnotes)]</sup> C++は使用されていません。

マイクロコントローラー向けに効率的なソフトウェアを構築する上で重要な部分は、ビルド時の処理です。Moddable SDKには、ビルドプロセスのための多様なツールとオプションが盛り込まれています。<sup>[[4](#footnotes)]</sup> 最良の結果を得るために、これらについて学ぶ時間を取ることをおすすめします。

<a id="footnotes"></a>
> <sup>[1]</sup> *参照: [XS 準拠](./documentation/xs/XS%20Conformance.md)*<BR>
<sup>[2]</sup> *参照: [XS の違い](./documentation/xs/XS%20Differences.md)*<BR>
<sup>[3]</sup> *参照: [C言語でのXS](./documentation/xs/XS%20in%20C.md)*<BR>
<sup>[4]</sup> *参照: [ツール](./documentation/tools/tools.md), [マニフェスト](documentation/tools/manifest.md)*

## 主要機能

### 通信

Moddable SDKはネットワークソケットと、HTTP/HTTPS、WebSocket、DNS、SNTP、telnetを含む様々な標準的で安全な通信プロトコルを実装しています。

また、Bluetooth Low Energy（BLE）プロトコルのサポートもあり、BLEペリフェラルおよびセントラルデバイスの開発が可能です。

### グラフィックス

現代的なユーザーインターフェースを構築するための2つのAPIが利用可能です：

- **Commodetto**、ビットマップグラフィックスライブラリで、2DグラフィックスAPIを提供します。Commodettoには、軽量な**Poco**レンダリングエンジンが含まれており、フレームバッファを必要とせずに一度に1つのスキャンラインを効率的にレンダリングするディスプレイリストレンダラーです。
- **Piu**、Commodettoの上に構築されたユーザーインターフェースフレームワークです。Piuはオブジェクトベースのフレームワークで、複雑でレスポンスの良いレイアウトを簡単に作成できます。

Moddable SDKには、画像形式の変換、画像圧縮、画像回転、フォント圧縮などのコマンドラインツールも含まれています。ビルドシステムは自動的にこれらのツールを使用します。

### ハードウェア

Moddable SDKは、デジタル（GPIO）、アナログ、PWM、I2Cを含む様々なハードウェアプロトコルを実装しています。一般的な市販のセンサー用の多数のドライバーと[対応するサンプルアプリ](./examples/drivers)も利用可能です。

### デバッガー

`xsbug` JavaScriptデバッガーは、XSプラットフォームのモジュールやアプリケーションのデバッグをサポートするフル機能のデバッガーです。

他のデバッガーと同様に、`xsbug`はブレークポイントの設定、ソースコードの閲覧、コールスタックと変数の検査をサポートします。さらに、`xsbug`デバッガーはリアルタイムの計測を提供し、メモリ使用量を追跡し、アプリケーションとリソース消費をプロファイリングします。

> *参照: [xsbug ドキュメント](./documentation/xs/xsbug.md)*

## 入門

### ステップ 1: 環境を設定する

[Getting Started](documentation/Moddable%20SDK%20-%20Getting%20Started.md) ドキュメントは、macOS、Linux、Windows用のModdable SDKのインストール、設定、ビルドの手順をステップバイステップで提供しています。また、ModdableハードウェアモジュールやESP8266、ESP32マイクロコントローラー用のサンプルアプリのビルド方法も含まれています。

Silicon Labs Geckoデバイスのビルドと設定の指示は [Gecko Build](documentation/devices/gecko/GeckoBuild.md) にあります。

### ステップ 2: サンプルアプリを試す

[examples](./examples) フォルダには、Moddable SDKの多くの機能の使い方を示す150以上のサンプルアプリが含まれています。多くの例は、特定の機能の使い方を示すために1ページ未満のソースコードからなります。

すべてのサンプルがすべてのデバイスと互換性があるわけではないことに注意してください。[readme.md](examples/readme.md) ドキュメントは、サンプルのビルド方法と、目的のサンプルを見つけるのに役立つ情報を提供します。

Moddable SDKがサポートするマイクロコントローラーと開発ボードについての情報は **サポートされるハードウェア** セクションを参照してください。まだハードウェアを持っていない場合、Moddable SDKにはmacOS、Linux、Windows上で動作するシミュレータも含まれています。これらは始めるのに最適であり、開発を加速するのに役立ちます。

### ステップ 3: 独自のアプリをビルドする

サンプルアプリをカスタマイズする準備ができたら、または最初から自分のアプリをビルドする準備ができたら、[documentation](./documentation) とすべてのJavaScript APIのソースコードが使えます。すべてのドキュメントはマークダウン形式で提供されています。Moddable SDKのランタイムを構成するソフトウェアモジュールは [modules](./modules) ディレクトリにあります。

**documentation**、**examples**、**modules** ディレクトリは、情報を簡単に見つけるために共通の構造を持っています。

- **base**: 時刻、タイマー、デバッグ、計測、UUIDを含む基本的なランタイム機能
- **commodetto**: ビットマップグラフィックスライブラリ
- **crypt**: 暗号プリミティブとTLS
- **data**: Base64およびhexエンコーディング
- **drivers**: ディスプレイ、タッチ入力、センサー、エキスパンダーのデバイスドライバー
- **files**: ファイル、フラッシュ、設定、リソース、zipを含むストレージ機能
- **network**: ソケットとソケットをベースにしたプロトコル（HTTP、WebSockets、DNS、SNTP、telnet、TLS）; また、Wi-FiとBLE API
- **pins**: ハードウェアプロトコルを含むデジタル（GPIO）、アナログ、PWM、I2C
- **piu**: ユーザーインターフェースフレームワーク

## 対応ハードウェア

![Moddable hardware](./documentation/assets/moddable/moddable-hardware.png)

**Moddable One**、**Moddable Two**、**Moddable Three** は、開発者が容易にModdable SDKを探索できる、手頃な価格のハードウェアモジュールです。各デバイスのGetting Startedガイドは以下で利用可能です：

- [Moddable One](./documentation/devices/moddable-one.md)
- [Moddable Two](./documentation/devices/moddable-two.md)
- [Moddable Three](./documentation/devices/moddable-three.md)

![Platforms](./documentation/assets/moddable/platforms.png)

Moddable SDKは、EspressifのESP8266とESP32、Silicon LabsのGecko 4モデル、およびQualcommのQCA4020 CDBもサポートしています。

サポートされているプラットフォームの完全なリストについては、examplesフォルダの [readme.md](examples/readme.md) ドキュメントの **ターゲットプラットフォーム** セクションを参照してください。

## ディスプレイ

![Displays](./documentation/assets/moddable/displays.jpg)

ESP8266とESP32で様々なSPIディスプレイをテストしました。これらのディスプレイのビデオデモは[当社のウェブサイトで](http://www.moddable.com/display)公開されています。配線ガイドは [documentation/displays](./documentation/displays) フォルダにあります。対応するディスプレイおよびタッチドライバのソースコードは [modules/drivers](./modules/drivers) フォルダにあります。

## ソースツリー

Moddable SDKリポジトリには、以下のトップレベルディレクトリが含まれています。

- **build**: 特定のマイクロコントローラーターゲットに必要なファイル群、シミュレーター、および `tools` ディレクトリのビルドツール用のmakefileです。
- **documentation**: Moddable SDKのすべてのドキュメント。ドキュメントはマークダウン形式で提供されます。
- **examples**: Moddable SDKのさまざまな機能のサンプルアプリ。[readme.md](examples/readme.md) ドキュメントは、サンプルを構築するためのガイドを提供し、求める種類のサンプルを見つけるのに役立つ情報を提供します。
- **license**: Moddable SDKに提供されるソフトウェアのライセンス契約。ここにはコントリビューターライセンス契約もあります。
- **modules**: Moddable SDKのランタイムを構成するソフトウェアモジュール。これには、通信、グラフィックス、ユーザーインターフェース、ハードウェアアクセス、暗号プリミティブ、およびデバイスドライバーが含まれます。すべてのモジュールにはJavaScript APIがあります。多くのモジュールは部分的にCで実装されています。
- **tools**: Moddable SDKを使用してアプリケーションを構築するためのツール。これには、画像形式の変換、画像圧縮、画像回転、フォント圧縮、ローカライゼーション文字列の処理、リソースのコンパイル、JSONマニフェストファイルからのアプリケーションのビルド用のコマンドラインツールが含まれます。さらに、XSのデバッガーであるxsbugもここにあります。
- **xs**: XS JavaScriptエンジンを含むそのコンパイラとリンカー、およびtest262の実行シェル。

## ライセンス

Moddable SDKは、GPL 3.0、LGPL 3.0、Apache 2.0、およびCreative Commons Attribution 4.0ライセンスの組み合わせの下で提供されています。[license](./licenses)ディレクトリには、使用されているライセンスとライセンスオプションに関する追加情報が含まれています。Moddableのウェブサイトにある[licensing article](http://www.moddable.com/license)では、追加の背景情報と商用ライセンスオプションについて説明しています。

## セキュリティ問題

私たちはModdable SDKにセキュリティホールがないように努力していますが、すべてを排除することはほぼ不可能です。セキュリティ研究者が私たちのソフトウェアに潜在的なセキュリティ問題を特定した場合、[issue](./issues)を開くことを奨励します。これらにはできるだけ迅速に対応します。希望する場合は、[メール](mailto:info@moddable.com)でセキュリティ問題を報告することもできます。

## 質問がありますか？気軽にお問い合わせください

個人開発者の場合は[issue](./issues)を開くことをお勧めします。できるだけ迅速に対応し、他の開発者もあなたの質問に対する回答から利益を得ることができます。

<a href="http://www.moddable.com">![Moddable logo](./documentation/assets/moddable/moddable.png)</a>

Moddableについてもっと知りたい場合は、[私達のWebサイト](http://www.moddable.com)をご覧ください。

自社製品へのJavaScriptとModdable SDKの活用に関心のある企業のために、Moddableは導入支援コンサルティングサービスを提供しています。設計、実装、トレーニング、サポートを支援します。

Twitter（[@moddabletech](https://twitter.com/moddabletech)）でもお問い合わせいただけます。Twitterでフォローすることで、私たちがの最新の取り組みを知ることができます。新しいブログ記事の告知やその他のModdableに関するニュースも投稿しています。

直接メールでお問い合わせいただくこともできます：[info@moddable.com](mailto:info@moddable.com).
