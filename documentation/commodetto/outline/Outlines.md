# アウトライン
Copyright 2020-2022 Moddable Tech, Inc.<BR>
改訂： 2022年2月13日

## 概要

Moddable SDKは、2Dベクターグラフィックスをサポートしました。PocoとPiuは、アンチエイリアスされた図形を、ブレンドを使ってカラーでレンダリングできます。

2Dベクターグラフィックスを描画するには、3つのステップがあります。本ドキュメントでは、それぞれについて説明します。

1. 図形のパスを構築します。パスを定義するためにいくつかのAPIが提供されています。
2. パスからアウトラインを作成します。アウトラインは、パスをストロークまたは塗りつぶすことで作成されます。
3. アウトラインを描画します。PiuのユーザーインターフェースフレームワークとPocoレンダラーのどちらもアウトラインを描画できます。

図形を定義するために、馴染みのあるプログラミングインターフェースがサポートされています：

- プログラムで図形を作成するためのCanvasパス
- ベクターグラフィックスツールで作成した図形を使用するためのSVGパス

レンダリングの実装は、FreeTypeの[Outline Processing](https://freetype.org/freetype2/docs/reference/ft2-outline_processing.html) APIに基づいています。

新しいモジュールは3つあります：

- `commodetto/outline` – パスを構築し、パスを塗りつぶすまたはストロークすることでアウトラインを作成する静的メソッドを持つ`Outline`クラスを定義します
- `commodetto/PocoOutline` – Pocoレンダラーを拡張してアウトラインをレンダリングします
- `piu/shape` – Piuにアウトライン表示のための図形オブジェクトを拡張します

**アウトライン**は、FreeTypeのアウトラインデータをカプセル化したホストオブジェクトです。アウトラインは、パスを塗りつぶすかストロークすることで作成されます。アウトラインは、複製、回転、拡大縮小、そして移動が可能です。

**パス**はサブパスの配列です。**サブパス**は曲線と直線のシーケンスで、コマンドとポイントとして配列バッファに格納されます。サブパスは開くことも閉じることもできます。

Piuでアウトラインを使用するプロジェクトは`manifest_outline_piu.json`マニフェストを、Pocoを使用するプロジェクトは`manifest_outline_poco.json`マニフェストを含める必要があります。

> このドキュメントの全てのアニメーション図形は、modsによって生成されています。modのソースコードへのリンクは、生成された図形の横に記載されています。それぞれのmodは、アプリケーション（ホスト）によってPiuの図形に割り当てられるPiuでの振る舞いをエクスポートします。

## パスの構築

`Outline`クラスには、パスを構築するためのいくつかの異なる方法があります。

```
import Outline from "commodetto/outline";
```

#### `Outline.CanvasPath()`

`Outline.CanvasPath.prototype`の新しいインスタンスを返すコンストラクターです。このインスタンスを使用して、[`CanvasPath` ](https://html.spec.whatwg.org/multipage/canvas.html#canvaspath)プログラミングインターフェースを用いたパスを構築します。

`Outline.CanvasPath.prototype`は、以下のメソッドをサポートしています：

- `arc(x, y, radius, startAngle, endAngle, counterclockwise)`
- `arcTo(x1, y1, x2, y2, r)`
- `bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y)`
- `closePath()`
- `ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, counterclockwise)`
- `lineTo(x, y)`
- `moveTo(x, y)`
- `quadraticCurveTo(cpx, cpy, x, y)`
- `rect(x, y, w, h)`

これらのメソッドは、HTML仕様の[Building Paths](https://html.spec.whatwg.org/multipage/canvas.html#building-paths)で文書化されています。

![](./assets/CanvasPath.gif) [source](../../../examples/piu/outline/figures/CanvasPath/mod.js)

#### `Outline.FreeTypePath()`

`Outline.FreeTypePath.prototype`の新しいインスタンスを返すコンストラクターです。このインスタンスを使用して、FreeTypeの[Glyph Stroker](https://freetype.org/freetype2/docs/reference/ft2-glyph_stroker.html) APIにインスパイアされたプログラミングインターフェースでパスを構築します。

`Outline.FreeTypePath.prototype`は、以下のメソッドを備えています：

- `beginSubpath(x, y, open)`:
	- `x`, `y`: 始点
	- `open`: 開いたサブパスを構築する場合は`true`
	- 詳しくは[`FT_Stroker_BeginSubPath`](https://freetype.org/freetype2/docs/reference/ft2-glyph_stroker.html#ft_stroker_beginsubpath)を参照してください
- `conicTo(cx, cy, x, y)`:
	- `cx`, `cy`: ベジェ曲線の制御点
	- `x`, `y`: 終点
	- 詳しくは[`FT_Stroker_ConicTo`](https://freetype.org/freetype2/docs/reference/ft2-glyph_stroker.html#ft_stroker_conicto)を参照してください
- `cubicTo(c1x, c1y, c2x, c2y, x, y`;
	- `c1x`, `c1y`: 1つ目のベジェ曲線の制御点
	- `c2x`, `c2y`: 2つ目のベジェ曲線の制御点
	- `x`, `y`: 終点
	- 詳しくは[`FT_Stroker_CubicTo`](https://freetype.org/freetype2/docs/reference/ft2-glyph_stroker.html#ft_stroker_cubicto)を参照してください
- `endSubpath()`:
	- 詳しくは[`FT_Stroker_EndSubPath`](https://freetype.org/freetype2/docs/reference/ft2-glyph_stroker.html#ft_stroker_endsubpath)を参照してください
- `lineTo(x, y)`:
	- `x`, `y`: 終点
	- 詳しくは[`FT_Stroker_LineTo`](https://freetype.org/freetype2/docs/reference/ft2-glyph_stroker.html#ft_stroker_lineto)を参照してください

![](./assets/FreeTypePath.gif) [source](../../../examples/piu/outline/figures/FreeTypePath/mod.js)

#### `Outline.PolygonPath(x0, y0, x1, y1 /* etc */)`

座標のリストから多角形を作成する静的メソッドです。

| 引数 | 型 | 説明 |
| --- | --- | --- |
| x0, y0, x1, y1 /* etc */ | number | ポリゴンの頂点の座標 |

新しいパスを返します。

![](./assets/PolygonPath.gif) [source](../../../examples/piu/outline/figures/PolygonPath/mod.js)

#### `Outline.RoundRectPath(x, y, width, height, radius)`

角が丸い長方形を作成する静的メソッドです。

| 引数 | 型 | 説明 |
| --- | --- | --- |
| x, y, width, height | number | 長方形の座標 |
| radius | number | 角の丸みの半径 |

新しいパスを返します。

![](./assets/RoundRectPath.gif) [source](../../../examples/piu/outline/figures/RoundRectPath/mod.js)

#### `Outline.SVGPath(data)`

SVGパス文字列からパスを作成する静的メソッド。

| 引数 | 型 | 説明 |
| --- | --- | --- |
| data | string | [the SVG path data](https://svgwg.org/svg2-draft/paths.html#PathData)  |

新しいパスを返します。

SVGパスデータは以下のコマンドをサポートしています：

- パスの閉鎖 [(Z, z)](https://svgwg.org/svg2-draft/paths.html#PathDataClosePathCommand)
- 三次ベジェ曲線 [(C, c, S, s)](https://svgwg.org/svg2-draft/paths.html#PathDataCubicBezierCommands)
- 楕円弧 [(A, a)](https://svgwg.org/svg2-draft/paths.html#PathDataEllipticalArcCommands)
- 直線 [(L, l, H, h, V, v)](https://svgwg.org/svg2-draft/paths.html#PathDataLinetoCommands)
- 移動 [(M, m)](https://svgwg.org/svg2-draft/paths.html#PathDataMovetoCommands)
- 二次ベジェ曲線 [(Q, q, T, t)](https://svgwg.org/svg2-draft/paths.html#PathDataQuadraticBezierCommands)

パスは配列であるため、SVG要素内に複数のパスが存在する場合は、それぞれのパスデータに対して`Outline.SVGPath`を使用し、次に`Array.prototype.concat`を使用してパスを結合します。

![](./assets/SVGPath.gif) [source](../../../examples/piu/outline/figures/SVGPath/mod.js)

## アウトラインの作成

`Outline`クラスには、パスを塗りつぶすかストロークすることでアウトラインを作成する静的メソッドがあります。

#### `Outline.fill(path, rule)`

| 引数 | 型 | 説明 |
| --- | --- | --- |
| path | object | 静的メソッドによって作成されたパス |
| rule | number | [non zero rule](https://en.wikipedia.org/wiki/Nonzero-rule)の場合は`Outline.NON_ZERO_RULE`、[even-odd rule](https://en.wikipedia.org/wiki/Even–odd_rule)の場合は`Outline.EVEN_ODD_RULE`。デフォルトは`Outline.NON_ZERO_RULE` |

新しいアウトライン、`Outline.prototype`のインスタンスを返します。

![](./assets/rules.gif) [source](../../../examples/piu/outline/figures/rules/mod.js)

#### `Outline.stroke(path, weight, linecap, linejoin, miterLimit)`

| 引数 | 型 | 説明 |
| --- | --- | --- |
| path | object | 静的メソッドによって作成されたパス |
| weight | number | 線の太さ、デフォルトは1ピクセル |
| linecap | number | 終点で直線を止める場合は`Outline.LINECAP_BUTT`、終点の周りに半円を描く場合は`Outline.LINECAP_ROUND`、終点の周りに正方形を描く場合は`Outline.LINECAP_SQUARE`。デフォルトは`Outline.LINECAP_ROUND` |
| linejoin | number | ラウンド結合の場合は`Outline.LINEJOIN_ROUND`、ベベル結合の場合は`Outline.LINEJOIN_BEVEL`、マイター結合の場合は`Outline.LINEJOIN_MITER`。デフォルトは`Outline.LINEJOIN_ROUND` |
| miterLimit | number | マイター制限。デフォルトは`weight` |

新しいアウトライン、`Outline.prototype`のインスタンスを返します。

![](./assets/linecaps.gif) [source](../../../examples/piu/outline/figures/linecaps/mod.js)

![](./assets/linejoins.gif) [source](../../../examples/piu/outline/figures/linejoins/mod.js)

## アウトラインの変形

`Outline`プロトタイプには、アウトラインの境界を取得するためのゲッターが1つ、アウトラインを複製するためのメソッドが1つ、そしてアウトラインを変形するためのメソッドが3つあります。

これらの変形操作は、アウトラインのデータを変更するため、座標の精度により元に戻せなくなる場合があります。

#### `Outline.prototype.get bounds()`

アウトラインの境界を表すオブジェクトを返します。このオブジェクトには`x`、`y`、`width`、`height`のプロパティがあります。

#### `Outline.prototype.clone()`

アウトラインのコピーを返します。

#### `Outline.prototype.rotate(angle, cx, cy)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `angle` | number | ラジアン単位の回転角度 |
| `cx`, `cy` | number | 回転の中心。デフォルトは`0, 0` |

アウトラインを返します。

![](./assets/rotate.gif) [source](../../../examples/piu/outline/figures/rotate/mod.js)

#### `Outline.prototype.scale(x, y)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `x` | number | 水平方向のスケーリング係数 |
| `y` | number | 垂直方向のスケーリング係数。デフォルトは`x` |

アウトラインを返します。

![](./assets/scale.gif) [source](../../../examples/piu/outline/figures/scale/mod.js)

#### `Outline.prototype.translate(x, y)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `x` | number | ピクセル単位の水平方向の移動距離 |
| `y` | number | ピクセル単位の垂直方向の移動距離 |

アウトラインを返します。

![](./assets/translate.gif) [source](../../../examples/piu/outline/figures/translate/mod.js)

## Pocoを使用した描画

[Poco renderer](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/poco.md)には、アウトラインを描画するメソッドとポリゴンを描画するメソッドがあります。

#### `Poco.prototype.blendOutline(color, blend, outline, x, y)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `color` | number | レンダリングする色。`Poco.prototype.makeColor`で返される値 |
| `blend` | number | ブレンドレベル。0は透明、255は不透明 |
| `outline` | number | 描画するアウトライン。`Outline.prototype`のインスタンス |
| `x`, `y` | number |	アウトラインを描画する位置 |

#### `Poco.prototype.blendPolygon(color, blend, x0, y0, x1, y1 /* etc */)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `color` | number | レンダリングする色。`Poco.prototype.makeColor`で返される値  |
| `blend` | number | ブレンドレベル。0は透明、255は不透明 |
| `x0, y0, x1, y1 /* etc */` | number | ポリゴンの頂点の座標 |

## Pocoを使用した例

アウトラインを使用して、簡単なオシロスコープを作成してみましょう。

![](./assets/oscilloscope.gif)

まず、モジュールをインポートし、Pocoを初期化し、背景色と前景色を作る必要があります。

```javascript
import Poco from "commodetto/Poco";
import {Outline} from "commodetto/outline";
import Timer from "timer";

let poco = new Poco(screen, {rotation:90});
let background = poco.makeColor(0, 0, 0);
let foreground = poco.makeColor(0, 255, 0);
```

### 第1チャンネル

第1チャンネルでは、ポリゴンパスを塗りつぶしてサンプルを描画する関数を定義します。

```javascript
function drawSamplesPolygon(samples, length, y, h) {
	const delta = poco.width / (length - 1);
	const points = new Array(2 * length);
	for (let i = 0; i < length; i++) {
		let j = i << 1;
		points[j] = delta * i;
		points[j + 1] = y + ((samples[i] / 255) * h);
	}
	const path = Outline.PolygonPath(0, y, ...points, poco.width, y);
	const outline = Outline.fill(path);
	poco.begin(0, 0, poco.width, 120);
		poco.fillRectangle(background, 0, 0, poco.width, poco.height);
		poco.blendOutline(foreground, 255, outline);
	poco.end();
}
```

### 第2チャンネル

第2チャンネルでは、FreeTypeパスをストロークしてサンプルを描画する関数を定義します。このパスは、`conicTo`を使って少し滑らかにしています。

```javascript
function drawSamplesOutline(samples, length, y, h) {
	const dx = poco.width / (length - 1);
	const cx = dx / 3;
	let px = 0;
	let py = y + ((samples[0] / 255) * h);
	const path = new Outline.FreeTypePath();
	path.beginSubpath(px, py, true);
	for (let i = 1; i < length; i++) {
		let qx = px + dx;
		let qy =  y + ((samples[i] / 255) * h);
		path.conicTo(px + cx, py, qx - cx, qy, qx, qy);
		px = qx;
		py = qy;
	}
	path.endSubpath();
	const outline = Outline.stroke(path, 2);
	poco.begin(0, 120, poco.width, 120);
		poco.fillRectangle(background, 0, 120, poco.width, 120);
		poco.blendOutline(foreground, 255, outline);
	poco.end();
}
```

### サンプリング

最終的には、タイマーを使って古いサンプルをシフトし、新しいサンプルを設定します。

```javascript
const h = poco.height / 4;
const length = 33;
const samples = new Int8Array(length).fill(0);
Timer.repeat(function() {
	samples.copyWithin(0, 1);
	samples[length - 1] = Math.floor(255 * Math.random()) - 128;
	drawSamplesPolygon(samples, length, h, h);
	drawSamplesOutline(samples, length, 3 * h, h);
}, 50);
```

この例では、サンプルはランダムな値ですが、もちろんセンサーによって生成することもできます。

## Piu Shapeオブジェクトを使用した描画

Piuの`shape`オブジェクトは、`content`オブジェクトであり、そのスキンの塗りつぶし色とストローク色を使用して、塗りつぶしのアウトラインとストロークのアウトラインを表示します。ストロークのアウトラインは、塗りつぶしのアウトラインの上に表示されます。

デフォルトでは、`shape`オブジェクトは空であり、塗りつぶしのアウトラインとストロークのアウトラインは`null`です。塗りつぶしのアウトラインやストロークのアウトラインを変更するには、`fillOutline`または`strokeOutline`プロパティを設定します。

shapeオブジェクトの測定される幅は、塗りつぶしのアウトラインとストロークのアウトラインの幅の最大値です。測定される高さも、塗りつぶしのアウトラインとストロークのアウトラインの高さの最大値です。そのため、`shape`オブジェクトの塗りつぶしのアウトラインやストロークのアウトラインを変更すると、コンテナのレイアウトが再配置されることがあります。

> 通常、`Outline.fill`を使用して作成した塗りつぶしのアウトラインと、`Outline.strokeを`使用して作成したストロークのアウトラインには、同じパスが使用されます。しかし、`shape`オブジェクトは、この2つのアウトラインの間に何の関係も想定していません。

#### コンストラクタの説明

##### `Shape([behaviorData, dictionary])`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `behaviorData` | `*` | shapeの`behavior`の`onCreate`関数に渡されるパラメータ。これは`null`や任意のパラメータを持つ辞書など、どのようなタイプのオブジェクトでもよい。
| `dictionary` | `object` | 結果を初期化するためのプロパティを持つオブジェクト。`content`オブジェクトと同様。（[Dictionary](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/piu/piu.md#content-dictionary)の[Content Object](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/piu/piu.md#content-object)のセクションを参照してください）

`Shape.prototype`を継承したオブジェクトである、`shape`インスタンスを返します。

#### プロトタイプの説明

プロトタイプは`Content.prototype`を継承します。

##### プロパティ

| 名前 | 型 | デフォルト値 | 読み取り専用 | 説明 |
| --- | --- | --- | --- | :--- |
| `fillOutline` | `object` | `null` | | 塗りつぶしの色で表示するアウトラインオブジェクト。 |
| `strokeOutline` | `object` | `null` | | ストロークの色で表示するアウトラインオブジェクト。 |

## Piuを用いたサンプル

図形を使って簡単なアナログ時計を作成してみましょう。

![](./assets/clock.png)

まず、モジュールをインポートします。

```javascript
import {} from "piu/MC";
import {} from "piu/shape";
import {Outline} from "commodetto/outline";
```
次に、シェイプのアウトラインをbehaviorの`onCreate`メソッドで定義します。

### フレームと文字盤

フレームは単なる白い円です。

```javascript
class FrameBehavior extends Behavior {
	onCreate(shape) {
		const path = new Outline.CanvasPath();
		path.arc(120, 120, 120, 0, 2 * Math.PI);
		shape.fillOutline = Outline.fill(path);
	}
}
```

文字盤は、分目盛り用のアウトラインと時目盛り用のアウトラインを使用します。

```javascript
class DialBehavior extends Behavior {
	onCreate(shape) {
		const fillPath = new Outline.CanvasPath();
		const strokePath = new Outline.CanvasPath();
		for (let i = 0; i < 60; i++) {
			const a = 6 * i  * Math.PI / 180;
			const dx = Math.cos(a);
			const dy = Math.sin(a);
			if (i % 5) {
				fillPath.moveTo(120 * dx, 120 * dy);
				fillPath.lineTo(114 * dx, 114 * dy);
			}
			else {
				strokePath.moveTo(120 * dx, 120 * dy);
				strokePath.lineTo(95 * dx, 95 * dy);
			}
		}
		shape.fillOutline = Outline.stroke(fillPath, 3, Outline.LINECAP_BUTT).translate(120, 120);
		shape.strokeOutline = Outline.stroke(strokePath, 7, Outline.LINECAP_BUTT).translate(120, 120);
	}
}
```
### 針

針はアウトラインを回転させるbehaviorを継承しています。

> 座標を正確に保つために、回転する前にアウトラインが複製されていることに注目してください。

```javascript
class HandBehavior extends Behavior {
	onClockChanged(shape, tick) {
		const a = ((90 - (tick * 6)) % 360) * Math.PI / 180;
		const cx = shape.width >> 1;
		const cy = shape.height >> 1;
		shape.fillOutline = this.outline.clone().rotate(a).translate(cx, cy);
	}
}
```

それぞれの針は異なるアウトラインを持っています。すべてのアウトラインは`0,0`を中心にしており、`onClockChanged`によって回転および移動されます。

```javascript
class HourBehavior extends HandBehavior {
	onCreate(shape) {
		const path = new Outline.CanvasPath();
		path.moveTo(-7, -22);
		path.lineTo(7, -22);
		path.lineTo(6, 65);
		path.lineTo(-6, 65);
		path.lineTo(-7, -22);
		path.closePath();
		this.outline = Outline.fill(path);
	}
}
class MinuteBehavior extends HandBehavior {
	onCreate(shape) {
		const path = new Outline.CanvasPath();
		path.moveTo(-7, -22);
		path.lineTo(7, -22);
		path.lineTo(4, 98);
		path.lineTo(-4, 98);
		path.lineTo(-7, -22);
		path.closePath();
		this.outline = Outline.fill(path);
	}
}
class SecondBehavior extends HandBehavior {
	onCreate(shape) {
		const path = new Outline.CanvasPath();
		path.rect(-1, -30, 2, 96);
		path.arc(0, 0, 4, 0, 2 * Math.PI);
		path.closePath();
		path.arc(0, 67, 6, 0, 2 * Math.PI);
		path.closePath();
		this.outline = Outline.fill(path);
	}
}
```
### アプリケーション

最終的に、アプリケーションのbehaviorとテンプレートを定義します。

アプリケーションのbehaviorは、アンカーを使用して、時針、分針、秒針にそれぞれの目盛りを割り当てます。

```javascript
class ClockApplicationBehavior extends Behavior {
	onCreate(application, $) {
		this.$ = $;
		application.interval = 500;
		application.start();
	}
	onTimeChanged(application) {
		const date = new Date();
		const hours = date.getHours() % 12;
		const minutes = date.getMinutes();
		const seconds = date.getSeconds();
		const $ = this.$;
		$.HOURS.delegate("onClockChanged", (hours * 5) + (minutes / 12));
		$.MINUTES.delegate("onClockChanged", minutes);
		$.SECONDS.delegate("onClockChanged", seconds);
	}
}
```
アプリケーションのテンプレートは、Piuの階層を作成し、shapeをそれぞれのbehaviorとskinにバインドします。

```javascript
let ClockApplication = Application.template($ => ({
	Behavior: ClockApplicationBehavior, skin:{ fill:"black" },
	contents: [
		Shape($, { width:240, height:240, Behavior:FrameBehavior, skin:{ fill:"white", stroke:"black" } } ),
		Shape($, { width:240, height:240, Behavior:DialBehavior, skin:{ fill:"black", stroke:"black" } } ),
		Shape($, { anchor:"HOURS", width:240, height:240, Behavior:HourBehavior, skin:{ fill:"black" } } ),
		Shape($, { anchor:"MINUTES", width:240, height:240, Behavior:MinuteBehavior, skin:{ fill:"black" } } ),
		Shape($, { anchor:"SECONDS", width:240, height:240, Behavior:SecondBehavior, skin:{ fill:"red" } } ),
	]
}));
export default new ClockApplication({}, { displayListLength:4096, touchCount:1, pixels: 240 * 64 });
```

