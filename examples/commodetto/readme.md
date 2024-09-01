# Moddable SDK - Commodetto サンプル

Copyright 2023 Moddable Tech, Inc.<BR>
改訂： 2023年12月29日

これらのサンプルは、2DグラフィックスAPIを提供するビットマップグラフィックスライブラリである[Commodetto](../../documentation/commodetto/commodetto.md)の機能を使用する方法を示しています。Commodettoには、軽量な[Pocoレンダリングエンジン](../../documentation/commodetto/poco.md)が含まれており、フレームバッファを必要とせずに単一のスキャンラインを効率的にレンダリングするディスプレイリストレンダラーです。

ほとんどのサンプルはQVGA（320x240）画面用に設計されていますが、多くのサンプルはさまざまな画面サイズで動作します。このフォルダー内のすべてのサンプルは、`mini-drag`のサンプルを除いてデスクトップシミュレーターで実行されます。

このドキュメントは、各サンプルの簡単な説明とデスクトップシミュレーターで実行されている各アプリのプレビューを提供します。特定の機能を使用する方法を示すサンプルを探している場合は、以下のリストを参照してください。このリストは、いくつかの推奨事項を提供するだけであり、各機能を使用するサンプルの完全なリストではないことに注意してください。

- **画像:** <a href="#image-frames">`image-frames`</a>, <a href="#docs">`docs`</a>, <a href="#rotated">`rotated`</a>, <a href="#sprite">`sprite`</a>
- **テキスト:** <a href="#text">`text`</a>, <a href="#text-ticker">`text-ticker`</a>, <a href="#cfe8x8">`cfe8x8`</a>, <a href="#cfeNFNT">`cfeNFNT`</a>
- **アニメーション:** <a href="#docs">`docs`</a>, <a href="#progress">`progress`</a>, <a href="#text-ticker">`text-ticker`</a>
- **タッチ入力:** <a href="#mini-drag">`mini-drag`</a>
- **ネットワーキング:** <a href="#jpeghttp-and-jpegstream">`jpeghttp`</a>, <a href="#jpeghttp-and-jpegstream">`jpegstream`</a>, <a href="#epaper-mini-travel-time">`epaper-mini-travel-time`</a>,

***

### `cfe8x8`

<img src="https://www.moddable.com/assets/commodetto-gifs/cfe8x8.gif" width=180>

`cfe8x8`のサンプルは、シンプルな組み込み8 x 8ビットマップフォントの使用方法を示しています。これは、新しいCommodettoフォントエンジンを実装する最も簡単なサンプルです。

***

### `cfeNFNT`

<img src="https://www.moddable.com/assets/commodetto-gifs/cfeNFNT.gif" width=180>

`cfeNFNT`のサンプルは、オリジナルのMacintoshのビットマップフォント形式であるNFNTフォントリソースを使用してテキストをレンダリングします。NFNT用のCommodetto Font Engineを含みます。

***

### `clip`

![](https://www.moddable.com/assets/commodetto-gifs/clipped.png)

`clip`のサンプルは、描画クリップの使用方法を示しています。クリップスタックはPocoレンダリングエンジンによって管理されます。

***

### `clock`

![](https://www.moddable.com/assets/commodetto-gifs/clock.gif)

`clock`のサンプルは、点滅するコロンを持つシンプルな画面上の時計を示しています。任意の画面サイズで動作するアプリケーションを構築するためにテキストを中央に配置する方法を示しています。

***

### `docs`

![](https://www.moddable.com/assets/commodetto-gifs/docs.gif)

`docs`のサンプルには、[Pocoドキュメント](../../documentation/commodetto/poco.md)のすべてのサンプルが含まれています。

***

### `epaper-mini-travel-time`

![](https://www.moddable.com/assets/commodetto-gifs/epaper-mini-travel-time.png)

`epaper-mini-travel-time`は、Moddable Three用に設計された`piu/epaper-travel-time`サンプルのミニチュアバージョンです。

> 詳細については、このサンプルに関する[ブログ記事](https://blog.moddable.com/blog/epaper)「Getting the Most from ePaper Displays」を参照してください。

***

### `fonts`

<img src="https://www.moddable.com/assets/commodetto-gifs/fonts.gif" width=180>

`fonts`のサンプルでは、8つの異なるフォントを使用して英語と日本語のメッセージを表示します。プロジェクトでスケーラブルなTrueTypeおよびOpenTypeフォントを使用する方法を示しています。

> 詳細については、このサンプルに関する[ブログ記事](https://blog.moddable.com/blog/fonts/)「Using More Fonts More Easily in IoT Products」を参照してください。

***

### `gif`

![](https://www.moddable.com/assets/commodetto-gifs/gif.gif)

`gif`のサンプルでは、アニメーションの旗を表示します。アニメーションGIFを直接レンダリングする方法を示しています。

***

### `image-frames`

![](https://www.moddable.com/assets/commodetto-gifs/image-frames.gif)

`image-frames`のサンプルでは、アニメーションの旗を表示します。軽量なカラーフレームエンコードストリームに変換されたアニメーションGIFをレンダリングする方法を示しています。

***

### `jpeghttp` および `jpegstream`

![](https://www.moddable.com/assets/commodetto-gifs/jpeghttp.gif) ![](https://www.moddable.com/assets/commodetto-gifs/jpegstream.gif)

`jpeghttp` と `jpegstream` のサンプルは、moddable.comから画像を取得し、画面に表示します。これらのサンプルは、圧縮されたJPEG画像を保持するのに十分なメモリを持たないESP8266で実行されます。非同期/待機を使用して、JPEG画像が到着するたびにデコードおよびレンダリングを行い、アプリケーションがRAMの制限を克服できるようにします。

***

### `logo`

`logo` のサンプルは、長方形のカラービットマップ画像を描画する方法を示しています。ロゴにはPNG画像を使用します。

***

### `logo-alpha`

`logo` のサンプルは、アルファチャンネルを持つカラービットマップ画像を描画する方法を示しています。ロゴにはPNG画像を使用します。アルファチャンネルはPNG画像から取得されます。背景色はランダムに変わり、アルファチャンネルが使用されていることを示します。

***

### `mini-drag`

`mini-drag` のサンプルは、ディスプレイ上でのタッチテストに最適です。オブジェクトをタッチして画面上をドラッグすることで移動できます。

***

### `origin`

![](https://www.moddable.com/assets/commodetto-gifs/origin.png)

`origin` のサンプルは、Poco描画の起点スタックの使用方法を示しています。

***

### `outline/oscilloscope`

<img src="https://www.moddable.com/assets/commodetto-gifs/outline-oscilloscope.png" width=200>

`outline/oscilloscope` のサンプルは、ストロークされたポリゴンと塗りつぶされたポリゴンの両方を使用してシンプルなオシロスコープをレンダリングする方法を示しています。

***

### `outline/random-ellipses`

<img src="https://www.moddable.com/assets/commodetto-gifs/outline-random-ellipses.png" width=200>

`outline/random-ellipses` のサンプルは、ストロークされたポリゴンと塗りつぶされたポリゴンの両方を使用してシンプルなオシロスコープをレンダリングする方法を示しています。

***

### `outline/random-triangles`

<img src="https://www.moddable.com/assets/commodetto-gifs/outline-random-triangles.png" width=200>

`outline/random-triangles` のサンプルは、ストロークされたポリゴンと塗りつぶされたポリゴンの両方を使用してシンプルなオシロスコープをレンダリングする方法を示しています。

***

### `pngdisplay`

`pngdisplay` を使用すると、curlコマンドラインツールを使用してPNG画像をディスプレイにプッシュできます。ユーザーインターフェースデザイナー向けに作成されており、開発者向けではありません。

> 詳細については、このサンプルに関する[ブログ記事](https://blog.moddable.com/blog/pngdisplay/)「Pushing PNG Images to a Display」を参照してください。

***

### `progress`

![](https://www.moddable.com/assets/commodetto-gifs/progress.gif)

`progress`のサンプルは、ロード画面に役立つプログレスバーとスピナーの高フレームレートアニメーションを表示します。

***

### `rotated`

![](https://www.moddable.com/assets/commodetto-gifs/rotated.png)

`rotated`のサンプルは、画面の回転を定義する方法を示しています。このサンプルでは90度回転しますが、0度、180度、および270度の回転もサポートされています。

***

### `sprite`

![](https://www.moddable.com/assets/commodetto-gifs/sprite.gif)

`sprite`のサンプルは、ロード画面に役立つシンプルなスピニングアニメーションを表示します。これは、アニメーションスプライトを作成するために単一の画像の異なる部分を循環させる方法を示しています。

***

### `static`

![](https://www.moddable.com/assets/commodetto-gifs/static.gif)

`static`のサンプルは、Pocoを拡張して効率的にstatic（テレビの砂嵐）をレンダリングする方法を示しています。

***

### `text-ticker`

<img src="https://www.moddable.com/assets/commodetto-gifs/text-ticker.gif" width=180>

`text-ticker`のサンプルでは、「Greetings from Moddable...」というメッセージがループでアニメーション表示されます。

***

### `text`

<img src="https://www.moddable.com/assets/commodetto-gifs/text.png" width=180>

`text`のサンプルでは、テキストをレンダリングする際のさまざまな切り捨ておよび整列オプションを示しています。

***

