# NeoStrand driver
Copyright 2018 Moddable Tech, Inc.

NeoStrandドライバは、NeoPixel (WS2811/WS2812のような） LEDのストリングやストリップで動的なカラーディスプレイのためのエフェクトエンジンを追加するための[NeoPixel](http://blog.moddable.com/blog/neopixels/)ドライバのサブクラスです。

用語：

* **ストランド**は、NeoPixelのストリップです。
* **エフェクト**は、NeoPixelストリップの状態を変更するか、他のアクションを実行する設定とアクション関数のグループです。
* **スキーム**は、ストリップに適用されるエフェクトのセットです。

## プロジェクトにNeoStrandを追加する

NeoStrand機能をプロジェクトに追加するには、**manifest.json**にモジュールのパスを追加します。

	"modules": {
		"*": [
			/* 他のモジュールはこちら */
			"$(MODDABLE)/modules/drivers/neostrand/*",
       ],
    },

### NeoStrandの使用

NeoStrandクラスをインポートします：

```js
import NeoStrand from "neostrand";
```

NeoStrandはNeoPixelのサブクラスなので、LEDストリップのデータ信号に接続されたピンの長さ、ピン、およびピクセルの順序を設定するために同じコンストラクタ辞書を使用します。

```js
const strand = new NeoStrand({length: 50, pin: 22, order: "RGB"});
```

> 注: ちらつきが発生する場合は、ストリップのタイミングを調整する必要があるかもしれません。仕様書を確認してください。期間は50ns単位です。**NeoPixel**はカスタムタイミングを許可するように拡張されています。
>

```
const Timing_WS2811 = {
    mark:  { level0: 1, duration0: 800,  level1: 0, duration1: 450, },
    space: { level0: 1, duration0: 400,   level1: 0, duration1: 800, },
    reset: { level0: 0, duration0: 30000, level1: 0, duration1: 30000, } };
>
const strand = new NeoStrand({length: 50, pin: 22, order: "RGB",
							  timing: Timing_WS2812B});
```

&nbsp;

> 注: 開発中に目を傷めないように、ライトの明るさを抑えることをお勧めします。
>
> ```js
> strand.brightness = 10;
> ```
> これにより色の範囲が減少するため、注意して使用してください。デフォルトの明るさは*64*で、最大は255です。

### スキーム

スキームは、ストランドに適用するエフェクトのセットです。スキームには、指定された順序で1つ以上のエフェクトが適用されます。新しいスキームを設定すると、ストランドがすでに実行中であっても即座に効果が現れます。

```js
let schemes = [];

let marqueeDictionary = { strand };

schemes.push( [ new NeoStrand.Marquee( marqueeDictionary ) ]);

strand.setScheme(schemes[0]);
strand.start(50);
```

`start`に送信されるパラメータは、エフェクトの更新間隔を定義します。これは、エフェクト関数の呼び出し間のミリ秒数です。数値を減らすと、CPU負荷の代わりに一部のエフェクトがよりスムーズにレンダリングされる場合があります。数値が高すぎると、長いストランドが完全に更新されないか、正しく更新されない可能性があります。
### エフェクトの辞書

エフェクトのパラメータは辞書で指定されます。

```js
let marqueeDictionary = { strand, start: 0, end: (strand.length / 2) };
```
辞書にはストランドが含まれている必要があります。`start`と`end`は、エフェクトの影響を受けるピクセルの範囲を定義します。エフェクトのバリエーションを作成したい場合は、`Object.assign`を使用して辞書をコピーします。

```js
let marqee2 = Object.assign({}, marqueeDictionary,
		{ start: (strand.length / 2) + 1, end: strand.length, reverse: 1 });
```

スキームには複数のエフェクトを含めることができます。

```js
schemes.push( [ new NeoStrand.Marquee( marqueeDictionary ),
                new NeoStrand.Marquee( marquee2 ) ]);
```

### コントロール

スキームはいつでも開始および停止できます。ストランドの状態は保持されます。スキームを停止した後、すべてのLEDを0に設定することをお勧めします。
```js
strand.stop();
strand.fill(strand.makeRGB(0, 0, 0));
strand.update();
```

### エフェクト

エフェクトはスキームを構成する要素です。通常、エフェクトは時間の経過とともに1つ以上のLEDの状態を変化させます。しかし、エフェクトのアクション関数はJavaScriptであるため、GPIOのトリガー、音の再生、大砲の発射など、ほぼ何でも行うことができます。

エフェクトは `NeoStrandEffect` からサブクラス化され、設定パラメータの辞書を受け取ります。設定辞書には、このエフェクトが適用される `strand` を含める必要があります。

```js
    let baseDictionary = { strand, start: 0, end: strand.length, loop: 1 };
```
ほとんどのエフェクトで使用される一般的なパラメータがいくつかあります。エフェクトは独自のパラメータを指定することもできます。

[ビルトインのエフェクトの一覧](#builtin) はこのドキュメントの後に出てきます。

#### 共通エフェクトパラメータ

| パラメータ | デフォルト値 | 範囲 | 説明 |
|---|---|---|---|
strand | (なし) | | このエフェクトが適用されるNeoStrand
start | `0` | [0..strand.length] | エフェクトの最初のピクセルのインデックス
end | `strand.length` | [0..strand.length] | エフェクトの最後のピクセルのインデックス
size | `strand.length` | [0..strand.length] | 1つの色相サイクルの長さ（ピクセル単位）
duration | `1000` | | エフェクトの1つの完全なサイクルの期間（ミリ秒単位）
reverse | `0` | [0, 1] | エフェクトを逆方向に実行するには1に設定します。つまり、エフェクトのタイムラインを逆に実行します
loop | `1` | [0, 1] | エフェクトをループさせるには1に設定します

`start` と `end` は、このエフェクトが操作するピクセルの範囲を定義します。`duration` はエフェクトの1サイクルの長さです。

エフェクトの1サイクルが完了すると、`loop` パラメータが `1` に設定されている場合、エフェクトは再起動します。

ループが `0` の場合、エフェクトの `onComplete()` 関数が呼び出されます。

`reverse`は、終了状態から開始状態に戻る効果を持ちます。

### カスタムエフェクト

エフェクトは**NeoStrandEffect**クラスからサブクラス化されています。コンストラクタは提供された辞書からパラメータを設定します。`activate`メソッドは[`timeline`](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/piu/piu.md#timeline-object)を設定します。以下の例では、`effectValue`は0から`effect.dur`まで変化し、その値が変わると呼び出されます。

```js
class MyEffect extends NeoStrandEffect {
    constructor(dictionary) {
        super(dictionary);
        this.name = "MyEffect"
}

activate(effect) {
    effect.timeline.on(effect, { effectValue: [ 0, effect.dur ] }, effect.dur, null, 0);
    effect.reset(effect);
}

set effectValue(value) {
    for (let i=this.start; i<this.end; i++) {
    	//
    	// 各セグメントのピクセルに対して、ピクセル操作を実行します
    	// valueは[ 0, effect.dur ]の範囲内になります。または、
    	// activate()でタイムラインを作成するときにエフェクトが指定する
    	// 任意の範囲になります。
    	// ...
    }
}
```

カスタムエフェクトは、希望するパラメータでインスタンスを作成し、それを`changeScheme`関数に送信する配列に含めることで使用します。

```js
strand.setScheme( [ new MyEffect(strand) ] );
```

<a id="builtin"></a>
## ビルトインエフェクト

以下のエフェクトがビルトインされています。

* [Marquee](#marquee)
* [Hue Span](#huespan)
* [Sine](#sine)
* [Pulse](#pulse)
* [Pattern](#pattern)
* [Dim](#dim)


<a id="marquee"></a>
### Marquee

`Marquee`エフェクトは一般的なアリの行進のような照明効果です。

`sizeA` と `sizeB` パラメータはパターンを定義します。例えば、{ sizeA: 1, sizeB: 3 } は繰り返しパターンを生成します：[A, B, B, B, A, B, B, B, A ...]

| パラメータ | デフォルト値 | 範囲 | 説明 |
|---|---|---|---|
strand | (なし) | | このエフェクトが適用される NeoStrand
start | `0` | [0..strand.length] | エフェクトの最初のピクセルのインデックス
end | `strand.length` | [0..strand.length] | エフェクトの最後のピクセルのインデックス
size | `strand.length` | [0..strand.length] | 1つの色相サイクルの長さ（ピクセル単位）
duration | 1000 | | パターンの1つの完全なサイクルの時間（ミリ秒単位）
reverse | `0` | [0, 1] | エフェクトを逆に実行するには1に設定します。つまり、エフェクトのタイムラインを逆に実行します
loop | `1` | [0, 1] | エフェクトをループさせるには1に設定します
sizeA | 1 | [1..strand.length] | パターンのA部分のピクセル数
sizeB | 3 | [1..strand.length] | パターンのB部分のピクセル数
rgbA | { r: 0, g: 0, b: 0x13 } | | RGB A カラー要素
rgbB | { r:0xff, g:0xff, b:0xff } | | RGB B カラー要素

<a id="huespan"></a>
### Hue Span

`HueSpan`エフェクトは、色の間をスムーズにフェードする効果です。カラーホイールのように、HSVカラースペースの色相を循環します。

| パラメータ | デフォルト値 | 範囲 | 説明 |
|---|---|---|---|
strand | (なし) | | このエフェクトが適用されるNeoStrand
start | `0` | [0..strand.length] | エフェクトの最初のピクセルのインデックス
end | `strand.length` | [0..strand.length] | エフェクトの最後のピクセルのインデックス
size | `strand.length` | [0..strand.length] | 1つの色相サイクルの長さ（ピクセル単位）
duration | 1000 | | カラーホイールの1つの完全なサイクルの時間（ミリ秒単位）
reverse | `0` | [0, 1] | エフェクトを逆方向に実行するには1に設定します。つまり、エフェクトのタイムラインを逆方向に実行します
loop | `1` | [0, 1] | エフェクトをループさせるには1に設定します
speed | 1.0 | | スピードの倍率
position | 0 | [0..1] | 開始時のHSV色相位置
saturation | 1.0 | [0..1] | HSV彩度
value | 1.0 | [0..1] | HSV明度

<a id="sine"></a>
### Sine

`Sine`エフェクトは、色のコンポーネントを正弦波のように変化させます。

`value` = y = (sin(_t_) + pos) * amplitude

| パラメータ | デフォルト値 | 範囲 | 説明 |
|---|---|---|---|
strand | (なし) | | このエフェクトが適用されるNeoStrand
start | `0` | [0..strand.length] | エフェクトの最初のピクセルのインデックス
end | `strand.length` | [0..strand.length] | エフェクトの最後のピクセルのインデックス
size | `strand.length` | [0..strand.length] | 1つの色相サイクルの長さ（ピクセル単位）
duration | 1000 | | 1つの完全なサイン波サイクルの時間（ミリ秒単位）
reverse | `0` | [0, 1] | エフェクトを逆方向に実行するには1に設定
loop | `1` | [0, 1] | エフェクトをループさせるには1に設定
speed | 1.0 | | 速度の倍率
loop | 1 | [0, 1] | エフェクトをループ
position | 0 | [0..1] | 開始位置（x座標）
vary | "b" | ["r","g","b",<br>"h","s","v"] | 変化させる色成分
value | 1.0 | [0..1] | HSV値



<a id="pulse"></a>
### Pulse

`Pulse`エフェクトは、指定された位置に`size`個のピクセルを設定し、それを一方または両方の端に向かって移動させます。`mode`パラメータは、ピクセルのRGB値を追加、減算、または設定するかどうかを指定します。

| パラメータ | デフォルト値 | 範囲 | 説明 |
|---|---|---|---|
strand | (なし) | | このエフェクトが適用される NeoStrand
start | `0` | [0..strand.length] | エフェクトの最初のピクセルのインデックス
end | `strand.length` | [0..strand.length] | エフェクトの最後のピクセルのインデックス
size | `strand.length` | [0..strand.length] | 1つの色相サイクルの長さ（ピクセル単位）
duration | 3000 | | 1つのパルスサイクルの時間（ミリ秒）
reverse | `0` | [0, 1] | エフェクトを逆に実行するには1に設定します。つまり、エフェクトのタイムラインを逆に実行します
loop | `1` | [0, 1] | エフェクトをループさせるには1に設定します
position | "random" | [-strand.length..strand.length] | パルス開始ピクセルのインデックス<br>負の数はストランド外でも問題ありません<br>"random"はランダムな開始位置を選びます
mode | 1 | [-1, 0, 1] | **1** は追加、**-1** は減算、**0** はピクセルカラーを設定
fade | 0.2 | [0..1] | 末尾がどれだけ早くフェードするか
rgb | { r: 0x80, g: 0x80, b: 0x80 } | | RGBカラー要素


<a id="pattern"></a>
### パターン

`Pattern` はピクセルの固定RGBパターンを設定します。

| パラメータ | デフォルト値 | 範囲 | 説明 |
|---|---|---|---|
strand | (なし) | | このエフェクトが適用されるNeoStrand
start | 0 | [0..strand.length] | エフェクトの最初のピクセルのインデックス
end | strand.length | [0..strand.length] | エフェクトの最後のピクセルのインデックス
pattern | [ 0x130000, 0x131313 ] | | RGBカラーの配列
mode | 1 | [-1, 0, 1] | ピクセルの色を追加するには**1**、減算するには**-1**、設定するには**0**

<a id="dim"></a>
### Dim

`Dim`エフェクトは、`duration`の間にスパン内の各ピクセルの色を減少させます。例えば、ピクセルが完全にオン（つまり0xFFFFFF）の場合、`duration`ミリ秒が経過すると徐々に完全にオフになります。

| パラメータ | デフォルト値 | 範囲 | 説明 |
|---|---|---|---|
strand | (なし) | | このエフェクトが適用されるNeoStrand
start | 0 | [0..strand.length] | エフェクトの最初のピクセルのインデックス
end | strand.length | [0..strand.length] | エフェクトの最後のピクセルのインデックス
duration | 1000 | | 明るさ最大のピクセルをオフにする時間（ミリ秒）


## カスタムエフェクトの作成

自分のエフェクトを作成するには、自分のビジョンに最も近い既存のエフェクトを選び、そこから始めます。

ランダムカラーエフェクトを作成しましょう。これは `size` ピクセルをランダムな色に設定し、`duration` ミリ秒ごとに色を変更します。

<a id="Random"></a>

| パラメータ | デフォルト値 | 範囲 | 説明 |
|---|---|---|---|
strand | (なし) | | このエフェクトが適用される NeoStrand
start | 0 | [0..strand.length] | エフェクトの最初のピクセルのインデックス
end | strand.length | [0..strand.length] | エフェクトの最後のピクセルのインデックス
duration | 1000 | | 色の変更間隔の長さ（ミリ秒）
size | 5 | [0..strand.length] | 各色のサイズ
max | 127 | [0..255] | ランダムなRGB成分の最大値

*Pattern* を出発点として、クラス名とコンストラクタを変更し、`activate` でタイムラインを設定し、変化する `effectValue` のセッターを提供します。ループエフェクトが開始または再開される前に `loopPrepare` 関数が呼び出されます。

```js
class RandomColor extends NeoStrandEffect {
    constructor(dictionary) {
        super(dictionary);
        this.name = "RandomColor"
        this.size = dictionary.size ? dictionary.size : 5;
        this.max = dictionary.max ? dictionary.max : 127;
        this.loop = 1;			// force loop
    }

    loopPrepare(effect) { effect.colors_set = 0; }

    activate(effect) {
        effect.timeline.on(effect, { effectValue: [ 0, effect.dur ] }, effect.dur, null, 0);
        effect.reset(effect);
    }

    set effectValue(value) {
        if (0 == this.colors_set) {
            for (let i=this.start; i<this.end; i++) {
                if (0 == (i % this.size))
                    this.color = this.strand.makeRGB(Math.random() * this.max, Math.random() * this.max, Math.random() * this.max);
                this.strand.set(i, this.color, this.start, this.end);
            }
            this.colors_set = 1;
        }
    }
}
```

次に、エフェクトをスキームリストに追加して試してみてください。

```js
let randomColorScheme = [ new RandomColor({ strand }) ];
manySchemes.push( randomColorScheme );
```

## タイミング

LEDのさまざまなドライバーチップのタイミング仕様は、以下の参考資料に含まれています。

```js
const Timing_WS2812B = {
    mark:  { level0: 1, duration0: 900,   level1: 0, duration1: 350, },
    space: { level0: 1, duration0: 350,   level1: 0, duration1: 900, },
    reset: { level0: 0, duration0: 30000, level1: 0, duration1: 30000, } };

const Timing_WS2811 = {
    mark:  { level0: 1, duration0: 800,   level1: 0, duration1: 450, },
    space: { level0: 1, duration0: 400,   level1: 0, duration1: 800, },
    reset: { level0: 0, duration0: 30000, level1: 0, duration1: 30000, } };
```
