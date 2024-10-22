# 組み込みデバイス上でXSエンジンを使用する際のJavaScript言語の考慮事項

Copyright 2018-2023 Moddable Tech, Inc.<BR>
改訂： 2023年9月22日

## 超軽量JavaScript

XSはModdableアプリケーションとツールの中核をなすJavaScriptエンジンです。XSは今世紀の初めから存在しています。最新バージョンは[GitHub](https://github.com/Moddable-OpenSource/moddable)で入手できます。

他のJavaScriptエンジンは主にクライアントまたはサーバーサイドのWeb開発に用いられます。主な焦点は速度であり、速度を得るために多大なリソースを消費します。

XSエンジンは、[ESP8266](https://www.espressif.com/en/products/hardware/esp8266ex/overview)や[ESP32](https://www.espressif.com/en/products/hardware/esp32/overview)といったマイクロコントローラを中心に構築された組み込みプラットフォームを対象にしています。通常のデスクトップやモバイルプラットフォームでも動作します。

組み込み開発の課題はよく知られています：限られたメモリと限られた性能です。通常のWebでクライアントやサーバーサイドでJavaScriptを実行するハードウェアと比較すると、その差は何パーセントかではなく桁違いで測定されます。加えて、バッテリー寿命も非常に重要です。

こういった制約があるにもかかわらず、また、マイクロコントローラ上で利用可能な他のスクリプトライブラリとは異なり、XSは非常に広範な[ECMAScript 言語仕様](https://tc39.github.io/ecma262/)を完全に実装するよう常に努めています。

これらの制約には影響があります。このドキュメントでは、組み込みデバイスに焦点を当てたXSと、ウェブ開発に重点を置いたJavaScriptエンジンとの主な違いを説明します。

## 実行時リソースの節約

組み込みプラットフォームは単独で存在するものではありません。アプリケーション開発にはコンピュータが必要です。したがって、実行の前に、つまりコンパイル時に、実行時リソースを節約するためにできる限りの準備をする必要があります。

Webでは、JavaScriptエンジンがスクリプトやモジュールをそのソースコードから実行しています。マイクロコントローラの場合、XSコンパイラがコンピュータ上でモジュールのソースコードをバイトコードに変換するため、マイクロコントローラ上のXSエンジンはバイトコードを実行するだけで済みます。

Moddableでは、このアプローチをスクリプトだけでなく、すべての種類のアセットに一般化しました。フォント、動画、画像、音声、およびテクスチャは、常にコンピュータ上で特定のハードウェアターゲットに最適な形式にエンコードされます。

> アセットが特定のデバイスに適応されているかどうかに基づいて、Web アプリケーションを静的または動的とするのはよくある誤解です。たとえば、コンテンツネゴシエーションを使用することで、Web アプリケーションは必要なだけ動的にしながらも、最適化されたモジュールとアセットを利用できます。この場合、コンパイル時はサーバー側で行われ、実行時はクライアント側で行われ、キャッシュがその間に存在します。

JavaScriptに基づいているにもかかわらず、XSを使用した組み込み開発は、Web開発よりもモバイル開発に似ています。XSで構築されたアプリケーションは、マニフェストで記述され、XSコンパイラおよびリンカー、アセットエンコーダ、Cコンパイラおよびリンカー、デバッガー、およびROMフラッシャーを含むツールチェーンで構築されます。

## 環境の準備

組み込みプラットフォームでは、RAMの量は非常に小さく、しばしば64KB以下です。ROMの量はより大きく、512KBから4MBで、これは現代の典型的なウェブページがダウンロードするデータ量とほぼ同じです。アプリケーションのネイティブコードとデータはROMにフラッシュされます。

JavaScriptモジュールはバイトコードにコンパイルされ、エンコードされたアセットはネイティブデータの一部としてフラッシュROMに保存されます。これにより、バイトコードやアセットを最初にRAMにコピーすることなく、そのまま使用できます。例えば、XSエンジンはバイトコードをROMから直接実行します。それでも、アプリケーションが動作するJavaScript環境にはかなりの量のRAMが必要です。例えば：

- `Object`, `Function`, `Boolean`, `Symbol`, `Error`, `Number`, `Math`, `Date`, `String`, `RegExp`, `Array`, `TypedArray`, `Map`, `Set`, `WeakMap`, `WeakSet`, `ArrayBuffer`, `SharedArrayBuffer`, `DataView`, `Atomics`, `JSON`, `Generator`, `AsyncGenerator`, `AsyncFunction`, `Promise`, `Reflect`, `Proxy`などのJavaScriptのビルトイン。
- ユーザーインターフェースフレームワークやセキュアネットワークフレームワークなど、アプリケーションが何か便利なことをするために使うモジュール。

ビルトインの構築やモジュールのロードは、多くのクラス、関数、およびプロトタイプオブジェクトを作成します。これらのオブジェクトは、マイクロコントローラに搭載されている控えめなRAMよりも多くのRAMを必要とすることがよくあります。

この問題を解決するために、XSリンカーはコンピュータ上でJavaScript環境を準備することを可能にします。XSリンカーはすべてのビルトインを構築し、ほとんどのモジュールを事前にロードし、その結果をネイティブデータとして保存し、ROMにフラッシュします。

この方法の利点は大きいです：

- ほとんど何もROMからRAMにコピーされないため、アプリケーションは少量のRAMで動作します。
- すべてがROMで準備されているため、アプリケーションは瞬時に起動します。

> XS リンカーは、マイクロコントローラでのみ利用可能なネイティブ関数を呼び出すボディを持つモジュールを事前にロードすることはできません。通常、そういったアプリケーションを開始するためのモジュールは 1 つだけです。

## ほとんどのオブジェクトを凍結する

しかし、XSリンカーがROMに準備したオブジェクトを、アプリケーションが変更したい場合はどうなりますか？

XSエンジンは、最初は空のエイリアステーブルを保持します。ROM内のエイリアス可能オブジェクトは、そのテーブルにインデックスを持ちます。アプリケーションがエイリアス可能オブジェクトを変更すると、そのオブジェクトはRAMにクローンされ、エイリアス可能オブジェクトをオーバーライドするためにテーブルに挿入されます。

このようなメカニズムにはメモリ容量とパフォーマンスのコストがかかりますが、JavaScript言語仕様に準拠するためには不可欠です。しかし、JavaScriptにはオブジェクトを変更不可に指定する機能があります：`Object.freeze` です。オブジェクトが凍結されると、XSリンカーはそれらをエイリアス可能としてインデックス付けしません。

モジュールはその本体で `Object.freeze` を使用して、XSリンカーにどのオブジェクトをエイリアス可能としてインデックス付けする必要がないかを指示できます。各オブジェクトに対してこれを呼び出すのは面倒なので、XSリンカーは自動的にすべてのクラス、関数、およびプロトタイプオブジェクト、ならびに `Atomics`、`JSON`、`Math`、`Reflect` などの他の組み込みオブジェクトを凍結することができます。

ECMAScriptの用語では、これは[frozen realm](https://github.com/tc39/proposal-frozen-realms)に関連しています。

> ほとんどのオブジェクトを凍結することは、特に動的アプリケーションにとって健全です。JavaScript 環境を何も変更できないことが確実だからです。

## 未使用機能の削除

前述のように、JavaScriptは多くの組み込み機能を定義しており、それらはすべてネイティブコードでXSエンジンに実装されています。多くの場合、アプリケーションがすべての組み込み言語機能を使用するわけではありません。

モジュールのバイトコードに基づいて、XSリンカーはXSエンジン自体から未使用のネイティブコードを削除することができます。これにより、ROM占有量を減らすように調整された独自バージョンのXSエンジンを実行することになります。

自己完結型で全体更新されるアプリケーションでは自動で行われます。一貫性、安全性、セキュリティを目的としたこの方法は組み込みプラットフォームでは一般的です。

個別のモジュールでカスタマイズや更新が予想されるアプリケーションでは、XSエンジンから削除する組み込み機能を手動で指定し、プロファイリングされたJavaScript環境を文書化することができます。例えば、`eval` や `Function` などを削除して、XSパーサーやバイトコーダーを取り除くことができます。

>JavaScript にはプロファイルという概念はありませんが、あなたのアプリケーションで独自のプロファイルを定義することができます。

## ネイティブコードの使用

JavaScriptエンジンについて話しているときに、「ネイティブコードの使用」は矛盾しているように思えるかもしれません！しかし、XSのCプログラミングインターフェースである [XS in C](./XS%20in%20C.md) のシンプルさは、効率的なアプリケーションを開発するために常に重要です。

Web開発はしばしば「純粋な」JavaScriptであると主張しますが、実際にはWebブラウザを実装するために必要な大量のネイティブコードに依存しています。もちろん、Web開発はクライアント側ではJavaScriptに限定されています。

Moddableでは、必要な場合にのみネイティブコードを使用します。例えば、ドライバを作成する場合や、メモリのフットプリントやパフォーマンスの向上が明らかな場合、グラフィックスライブラリやユーザーインターフェースフレームワークを用いる場合です。

実際、リソースを節約する合理的な解決策は、JavaScriptの代わりに時々ネイティブコードを使用することです。あなたのアプリケーションはすべてを管理していることを覚えておいてください。

> ツールチェーンは常にネイティブコードのコンパイルとリンクを必要とするため、開発サイクルにオーバーヘッドはありません

## 適合性

環境が凍結されておらず、エンジンがストリップされていない場合、XSは [Official ECMAScript Conformance Test Suite](https://github.com/tc39/test262) の**99.5%**に合格します。[XS 適合性](./XS%20Conformance.md) のドキュメントでは、結果とテスト手順について説明しています。

以下は、いくつかの互換性の問題に関する注意点です：

- **Function**: XSは関数のソースコードを保存しないため、`Function.prototype.toString` は常に失敗します。

- **RegExp**: デフォルトではUnicodeプロパティエスケープは、それらが必要とするテーブルのサイズのために組み込まれていません。

- **String**: デフォルトでは文字列は内部でUTF-8にエンコードされており、その長さは含まれるコード単位数ではなくコードポイント数であり、コード単位ではなくコードポイントによってインデックスが付けられています。`mxCESU8`ビルドフラグを定義すると、文字列は完全な適合性を提供する[CESU-8](https://en.wikipedia.org/wiki/CESU-8)を使用してエンコードされます。CESU-8を使用する場合、XSはNUL文字をJavaの[Modified UTF-8](https://en.wikipedia.org/wiki/UTF-8#Modified_UTF-8)を使用してエンコードします。

- **Tagged Template**: XSはタグ付きテンプレートをサポートしていますが、タグ付きテンプレートキャッシュは現在実装していません。

XSのホストはウェブブラウザではないため、ECMAScript言語仕様のAnnex Bはサポートされていません。ただし、XSは `Date.prototype.getYear`、`Date.prototype.setYear`、`Object.prototype.__proto__`、`String.prototype.substr`、`escape`、および `unescape` を実装しています。

ECMAScript国際化API仕様はECMAScript言語仕様とは別のものであり、XSではサポートされていません。
