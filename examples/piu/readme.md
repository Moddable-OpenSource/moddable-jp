# Moddable SDK - Piuサンプル

Copyright 2021 Moddable Tech, Inc.<BR>
改訂： 2021年11月4日

これらのサンプルは、[Piuユーザーインターフェースフレームワーク](../../documentation/piu/piu.md)の機能を使用する方法を示しています。Piuはオブジェクトベースのフレームワークで、複雑でレスポンシブなレイアウトを簡単に作成できるようにします。

ほとんどのサンプルはQVGA（320x240）画面用に設計されていますが、多くのサンプルはさまざまな画面サイズで動作するレスポンシブレイアウトを特徴としています。このフォルダー内のすべてのサンプルは、`backlight`、`epaper-travel-time`、`one-line`、および`one-line-keyboard`のサンプルを除いてデスクトップシミュレーターで実行されます。

このドキュメントは、各サンプルの簡単な説明とデスクトップシミュレーターで実行されている各アプリのプレビューを提供します。特定の機能を使用する方法を示すサンプルを探している場合は、以下のリストを参照してください。このリストはあくまでいくつかの推奨事項を提供するものであり、各機能を使用するサンプルの完全なリストではないことに注意してください。

- **画像:** <a href="#images">`images`</a>, <a href="#balls">`balls`</a>, <a href="#neon-light">`neon-light`</a>
- **テキスト:** <a href="#text">`text`</a>, <a href="#cards">`cards`</a>, <a href="#localization">`localization`</a>
- **アニメーション:** <a href="#easing-equations">`easing-equations`</a>, <a href="#transitions">`transitions`</a>, <a href="#timeline">`timeline`</a>
- **タッチ入力:** <a href="#drag">`drag`</a>, <a href="#keyboard">`keyboard`</a>, <a href="#map-puzzle">`map-puzzle`</a>
- **スクロールコンテンツ:** <a href="#scroller">`scroller`</a>, <a href="#list">`list`</a>
- **ネットワーキング:** <a href="#wifi-config">`wifi-config`</a>, <a href="#weather-and-mini-weather">`weather` and `mini-weather`</a>, <a href="#one-line-and-one-line-keyboard">`one-line` and `one-line-keyboard`</a>

***

### `backlight`

`backlight`のサンプルでは、Moddable Twoのバックライトの明るさを調整することができます。

> バックライトの詳細については、[Moddable Twoのドキュメント](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/devices/moddable-two.md#backlight)の**バックライト**セクションを参照してください。

***

### `balls`

![](http://www.moddable.com/assets/piu-gifs/balls.gif)

`balls`のサンプルでは、画面上で跳ねるボールのフルフレームレートアニメーションを表示します。特定の画面サイズに合わせて設計されていないため、ディスプレイのテストに役立ちます。

***

### `bars`

![](http://www.moddable.com/assets/piu-gifs/bars.gif)

`bars`のサンプルでは、白黒画像をレンダリングし、Piuの`Port`オブジェクトを使用してバーを描画します。また、アニメーションを作成する2つの異なる方法である`Timeline`オブジェクトと`Transition`オブジェクトの使用方法も示しています。

***

### `cards`

![](http://www.moddable.com/assets/piu-gifs/cards.gif)

`cards`のサンプルでは、Piuの`Timeline`オブジェクトを使用してModdableの従業員の名刺をアニメーション化します。

***

<!--
### `clut`

***
-->

### `color-picker`

![](http://www.moddable.com/assets/piu-gifs/color-picker.gif)

`color-picker`のサンプルは、色を選択するためのシンプルなユーザーインターフェースを提供します。カラフルな画像の上でピッカーをドラッグして色を選択します。選択された色はヘッダーバーに表示されます。

> カラーピッカーの実装に関する詳細は、ブログ記事 [A Color Picker for Microcontrollers](https://blog.moddable.com/blog/colorpicker/) を参照してください。

***

### `countdown`

![](http://www.moddable.com/assets/piu-gifs/countdown.gif)

`countdown`のサンプルは、コードで指定された日付までのカウントダウンを行います。数字とテキストは異なる色にかすかにフェードします。

***

### `drag` と `drag-color`

![](http://www.moddable.com/assets/piu-gifs/drag.gif) ![](http://www.moddable.com/assets/piu-gifs/drag-color.gif)

`drag`と`drag-color`のサンプルは、ディスプレイ上のタッチをテストするのに最適です。オブジェクトをタッチして画面上をドラッグすることで移動できます。マルチタッチもサポートされています。

***

### `easing-equations`

![](http://www.moddable.com/assets/piu-gifs/easing-equations.gif)

`easing-equations`のサンプルは、Moddable SDKに組み込まれているイージング方程式と、アニメーションを作成するためのPiu `Timeline`オブジェクトの使用を示しています。イージング方程式は、滑らかで自然な見た目のアニメーションを作成するのに役立ちます。

***

### `epaper-flashcards`

<img src="https://www.moddable.com/assets/piu-gifs/epaper-flashcards.gif" width=250>

`epaper-flashcards`のサンプルは、一連のフラッシュカードを表示します。タップすると答えが表示されます。左にスワイプまたは右にスワイプすると、前のカードまたは次のカードに移動します。

> このサンプルとePaperディスプレイの詳細については、ブログ記事[Getting the Most from ePaper Displays](https://blog.moddable.com/blog/epaper)をご覧ください。

***

### `epaper-photos`

<img src="http://www.moddable.com/assets/piu-gifs/epaper-photos.gif" width=180>

`epaper-photos`のサンプルは、写真のスライドショーを表示します。

> このサンプルとePaperディスプレイの詳細については、ブログ記事[Getting the Most from ePaper Displays](https://blog.moddable.com/blog/epaper)をご覧ください。

***

### `epaper-travel-time`

`epaper-epaper-travel-time`のサンプルは、自宅と職場の間の移動時間を表示します。現在の移動時間を決定するためにGoogle Maps Web APIを使用します。

> このサンプルとePaperディスプレイの詳細については、私たちのブログ記事[Getting the Most from ePaper Displays](https://blog.moddable.com/blog/epaper)をご覧ください。

***

### `hardware-rotation`

<img src="https://www.moddable.com/assets/piu-gifs/hardware-rotation.png" width=180>

`hardware-rotation`のサンプルは、ディスプレイ上の画像とテキストを3秒ごとに回転させます。このサンプルは、ハードウェア回転をサポートするディスプレイコントローラーと互換性があります。

> ハードウェア回転の詳細については、私たちのブログ記事[Run-time Display Rotation](https://blog.moddable.com/blog/rotate/)をご覧ください。

***

### `heartrate`

![](http://www.moddable.com/assets/piu-gifs/heartrate.gif)

`heartrate`のサンプルは、毎秒ランダムな数値を生成し、それを心拍数モニターのサンプルUIの一部として表示します。

***

### `horizontal-expanding-keyboard`

![](http://www.moddable.com/assets/piu-gifs/horizontal-expanding-keyboard.gif)

`horizontal-expanding-keyboard`のサンプルは、拡張キーボードモジュールを使用してタッチスクリーン用のオンスクリーンキーボードを作成する方法を示しています。水平拡張キーボードは、320x240ディスプレイでのタッチ入力を容易にするように設計されています。

*`keyboard`および`vertical-expanding-keyboard`のサンプルも参照してください。*

> 拡張キーボードの詳細については、ブログ記事[Introducing an Expanding Keyboard for Small Screens](https://blog.moddable.com/blog/expanding-keyboard/)をご覧ください。

***

### `images`

![](http://www.moddable.com/assets/piu-gifs/images.gif)

`images`のサンプルは、GIF、JPEG、およびPNGをレンダリングする方法を示しています。

***

### `keyboard`

![](http://www.moddable.com/assets/piu-gifs/keyboard.gif)

`keyboard`のサンプルは、キーボードモジュールを使用してタッチスクリーン用のオンスクリーンキーボードを作成する方法を示しています。キーボードモジュールは多くの画面サイズで使用できます。

*`horizontal-expanding-keyboard` と `vertical-expanding-keyboard` のサンプルも参照してください。*

***

### `list`

![](http://www.moddable.com/assets/piu-gifs/list.gif)

`list` のサンプルでは、Piuの `Port` オブジェクトを使用して、タップ可能なアイテムのスクロールリストを作成します。

***

### `localization`

![](http://www.moddable.com/assets/piu-gifs/localization.gif)

`localization` のサンプルでは、選択された言語に画面上のテキストを翻訳します。Moddable SDKでのローカライゼーションの実装方法については [こちら](../..//documentation/piu/localization.md) を参照してください。

***

### `love-e-ink`

![](http://www.moddable.com/assets/piu-gifs/love-e-ink.gif)

`love-e-ink` のサンプルは、128x296 Crystalfontz ePaperディスプレイ用に設計されています。画面の小さな部分を一度に更新し、「Moddable ♥ Eink」というメッセージを表示します。

***

### `love-js`

![](http://www.moddable.com/assets/piu-gifs/love-js.gif)

`love-js` のサンプルは、128x128ピクセルの画面用に設計されています。3つの異なる画像のアニメーションを再生して、「Moddable ♥ JavaScript」というメッセージを表示します。

***

### `map-puzzle`

![](http://www.moddable.com/assets/piu-gifs/map-puzzle.gif)

`map-puzzle`のサンプルは、タッチスクリーン用のシンプルなパズルを実装しています。画面をドラッグしながらパズルのピースをタッチして移動させます。

***

### `neon-light`

![](http://www.moddable.com/assets/piu-gifs/neon-light.gif)

`neon-light`のサンプルは、カラフルでアニメーションの背景に英語と日本語のテキストを表示します。

***

### `one-line` と `one-line-keyboard`

![](http://www.moddable.com/assets/piu-gifs/one-line.gif)  ![](http://www.moddable.com/assets/piu-gifs/one-line-keyboard.gif)

`one-line`と`one-line-keyboard`のサンプルは一緒に動作するように設計されています。`one-line`のサンプルはWebSocketサーバーを開き、画面にIPアドレスを表示します。`one-line-keyboard`のサンプルは`one-line`サーバーに接続するWebSocketクライアントを開き、ユーザーが入力したテキストを送信して表示させます。

***

### `outline/clock`

<img src="http://www.moddable.com/assets/piu-gifs/outline-clock.png" width=200>

`outline/clock` のサンプルは、Canvasのアウトラインを使用してアナログ時計をレンダリングします。

***

### `outline/figures`

<img src="http://www.moddable.com/assets/piu-gifs/outline-figures.png" width=200>

`outline/figures` のサンプルには、Outlineドキュメントのすべての図形を描画するためのモジュールが含まれています。

***

### `outline/shapes`

<img src="http://www.moddable.com/assets/piu-gifs/outline-shapes.png" width=160>

`outline/shapes` のサンプルは、ボールのサンプルと同様の方法で4つの異なるアウトライン形状をレンダリングします。

***

### `preferences`

<img src="https://www.moddable.com/assets/piu-gifs/preferences.png" width=190>

`preferences` のサンプルは、ブート間で保存される設定を行う方法を示しています。色をタップして背景色を変更し、背景色の設定を行います。デバイスが再起動されると、背景色は最後に選択された色になります。

***

### `scroller`

![](http://www.moddable.com/assets/piu-gifs/scroller.gif)

`scroller` のサンプルは、垂直および水平のスクロールコンテンツを作成する方法を示しています。タイトルバーをタップして、2つの方向を切り替えます。

***

### `sound`

<img src="http://www.moddable.com/assets/piu-gifs/sound.png" width=190>

`sound` のサンプルは、Piuの `Sound` オブジェクトの使い方を示しています。再生ボタンをタップして音を再生します。音量ボタンをタップして音量を調整します。

***

### `spinner`

![](http://www.moddable.com/assets/piu-gifs/spinner.gif)

`spinner` のサンプルは、読み込み画面に便利なシンプルなスピニングアニメーションを実装しています。

***

### `spiral`

![](http://www.moddable.com/assets/piu-gifs/spiral.gif)

`spiral` のサンプルは、Piuの `Port` オブジェクトを使用して、ランダムなサイズのスパイラルを画面に描画します。

***

### `text`

![](http://www.moddable.com/assets/piu-gifs/text.gif)

`text` のサンプルは、Piuの `Text` オブジェクトを使用して、さまざまなサイズ、色、配置のテキストスタイルをレンダリングします。

***

### `timeline`

![](http://www.moddable.com/assets/piu-gifs/timeline.gif)

`timeline` のサンプルは、Piuの `Timeline` オブジェクトの `to`、`from`、および `on` 関数の使用方法を示しています。対応する関数を使用したアニメーションを見るには、オプションをタップします。

***

### `transitions`

![](http://www.moddable.com/assets/piu-gifs/transitions.gif)

`transitions`のサンプルは、Piuのcombおよびwipeトランジションの使用方法を示しています。これらはフルスクリーントランジションを作成するのに役立ちます。

***

### `vertical-expanding-keyboard`

![](http://www.moddable.com/assets/piu-gifs/vertical-expanding-keyboard.gif)

`vertical-expanding-keyboard`のサンプルは、拡張キーボードモジュールを使用してタッチスクリーン用のオンスクリーンキーボードを作成する方法を示しています。縦に拡張するキーボードは、240x320ディスプレイでのタッチ入力を容易にするように設計されています。

*`keyboard`および`horizontal-expanding-keyboard`のサンプルも参照してください。*

> 拡張キーボードの詳細については、ブログ記事[Introducing an Expanding Keyboard for Small Screens](https://blog.moddable.com/blog/expanding-keyboard/)をご覧ください。

***

### `weather` and `mini-weather`

![](http://www.moddable.com/assets/piu-gifs/weather.gif)

`weather`および`mini-weather`のサンプルは、5つの異なる都市の天気予報を表示します。予報はクラウドサービスにHTTPリクエストを送信して取得されます。

***

### `wifi-config`

![](http://www.moddable.com/assets/piu-gifs/wifi-config.gif)

`wifi-config`のサンプルでは、利用可能なネットワークのリストから選択してWi-Fiネットワークを設定することができます。画面上のキーボードを使用して、セキュアネットワークのパスワードを入力します。

***
