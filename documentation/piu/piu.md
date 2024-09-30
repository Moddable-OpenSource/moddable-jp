# Piu JavaScript リファレンス
Copyright 2017-2023 Moddable Tech, Inc.<BR>
改訂： 2023年8月31日

## このドキュメントについて

Piu は、マイクロコントローラ上で実行するように設計されたユーザー インターフェイス フレームワークです。Piu のプログラミング インターフェイスは、アプリケーションの包含階層、外観、動作、フローを定義するグローバル コンストラクター、関数、およびオブジェクトの JavaScript API です。このドキュメントでは、Piu API を定義するオブジェクトと重要な関連概念について詳しく説明します。

## Table of Contents

  * [継承階層](#継承階層)
  * [重要な概念の紹介](#重要な概念の紹介)
    * [包含階層と外観](#包含階層と外観)
    * [Behavior and Flow](#behavior-and-flow)
  * [グローバルプロパティ](#global-properties)
 	 * [ビルトインプロパティ](#built-in-properties)
 	 * [プロパティの追加](#adding-additional-properties)
  * [プロパティの説明](#descriptions-of-properties)
  	 * [Anchor](#anchor)
 	 * [Color](#color)
 	 * [Coordinates](#coordinates)
 	 * [Duration, Fraction, Interval, Loop, and Time](#duration-fraction-interval-loop-and-time)
 	 * [Font](#font)
 	 * [Tiles](#tiles)
 	 * [Variant, Variants, State, and States](#variant-variants-state-and-states)
  * [Piuオブジェクトリファレンス](#piu-object-reference)
 	 * [Applicationオブジェクト](#application-object)
 	 * [Behaviorオブジェクト](#behavior-object)
 	 * [Columnオブジェクト](#column-object)
 	 * [Containerオブジェクト](#container-object)
 	 * [Contentオブジェクト](#content-object)
 	 * [Dieオブジェクト](#die-object)
 	 * [Imageオブジェクト](#image-object)
 	 * [Labelオブジェクト](#label-object)
 	 * [Layoutオブジェクト](#layout-object)
 	 * [Portオブジェクト](#port-object)
 	 * [Rowオブジェクト](#row-object)
 	 * [Scrollerオブジェクト](#scroller-object)
 	 * [Shapeオブジェクト](../commodetto/outline/Outlines.md)
 	 * [Skinオブジェクト](#skin-object)
 	 * [Soundオブジェクト](#sound-object)
 	 * [Styleオブジェクト](#style-object)
 	 * [Textオブジェクト](#text-object)
 	 * [Textureオブジェクト](#texture-object)
 	 * [Timelineオブジェクト](#timeline-object)
 	 * [Transitionオブジェクト](#transition-object)

## 継承階層

図1は、このドキュメントで説明するオブジェクトの継承階層をまとめたものです。

**図1.** Piu 継承階層

![](../assets/piu/inheritanceHierarchy.png)

Piu アプリケーションのコンテキストにおけるこれらのオブジェクト間の基本的な関係は次のとおりです。

- ユーザーインターフェースのグラフィカル部分はすべて`content`オブジェクトです。
- `skin`、`style`、`texture` オブジェクトは、`content` オブジェクトの外観 (色、フォントなど) をカスタマイズします。
- `behavior`オブジェクトはイベントを処理するために`content`オブジェクトにバインドされます。
- `timeline` および `transition` オブジェクトは `content` オブジェクトをアニメーション化します。


## 重要な概念の紹介

このセクションでは、Piu アプリケーションに関連する重要な概念について説明し、このドキュメントの残りの部分で使用されるいくつかの用語を定義します。

### 包含階層と外観

Piu アプリケーションのグラフィカル ユーザー インターフェイス要素は、`content` オブジェクトの階層で構成されています。基本構造は次のとおりです。

- `application`オブジェクトは包含階層のルートである
- `container`オブジェクトは包含階層のブランチである
- `content`オブジェクトは包含階層のリーフである

アプリケーションはコンストラクタを使用して `content` オブジェクトと `container` オブジェクトを定義します。これらのオブジェクトは、`add`、`insert`、および `replace` 関数を使用して包含階層にアタッチされ、`remove` および `replace` 関数を使用して包含階層から削除されます。 これらのプロパティの説明については、[Containerオブジェクト](#container-object) セクションの [Functions](#container-functions) を参照してください。

#### BoundコンテンツとUnboundコンテンツ

包含階層にアタッチされていないcontentsは*unbound* contentsと呼ばれ、包含階層にアタッチされているcontentsは*bound* contentsと呼ばれます。包含階層の一部であるオブジェクトのみが画面に表示されます。

unbound contentsはレイアウトには参加しません。測定も調整も行われないため、位置とサイズは「未定義」となり、変更できません。

```javascript
let content = new Content();
let before = content.position;	// undefined
application.add(content);
let after = content.position;	// {x: 160, y:120} (assuming the application is 320x240)
```

#### 制約

オブジェクトの座標は、その位置とサイズに対する暗黙的な制約を定義します。

たとえば、中央揃えのコンテンツや、サイズ/位置がコンテナーのサイズ/位置に依存するコンテンツは移動できません。

```javascript
// Cannot move
let centeredContent = new Content(null, {
	width: 10, height: 10
});
let dependentOnContainerContent = new Content(null, {
    top: 0, left: 0, bottom: 100, right: 100,
});

// Can move
let unconstrainedContent = new Content(null, {
    top: 0, left: 0, height: 100, width: 100
});
```

#### MeasureとFit

<a id="measured-size"></a>
##### Measured size

すべてのコンテンツには *measured width* と *measured height* があり、これらはcontent自体によって計算されたデフォルトの幅と高さです。 たとえば、次のcontentオブジェクトのmeasured widthは 100 になります。

```javascript
let sampleContent = new Content(null, {
	width: 100
});
```
次のcontentにはデフォルトの幅がないため、measured widthは0になります。

```javascript
let sampleContent = new Content(null, {
	left: 0, right: 0
});
```

contentオブジェクトの`coordinates`プロパティは、contentのmeasured sizeを反映します。 このプロパティはいつでも変更できます。

```javascript
sampleContent.coordinates = {
	left: 0,
	width: 100,
	top: 0,
	height: 100
};
```

<a id="fitted-size"></a>
##### Fitted size

すべてのcontentには *fitted width* と *fitted height* もあり、これらはcontainerによって計算されたcontentの有効な幅と高さです。

`left` 座標と `right` 座標の両方が定義されている場合、contentはcontainerとともに水平方向に伸縮し、contentのfitted widthはcontainerのfitted widthに依存します。 同様に、`top`座標と`bottom`座標の両方が定義されている場合、contentはcontainerに合わせて垂直方向に伸縮し、contentのfitted heightはcontainerのfitted heightに依存します。 たとえば、ここでの`sampleContent`オブジェクトのfitted widthとfitted heightは両方とも 100 になります。

```javascript
let sampleContent = new Content(null, {
	left: 0, right: 0
});
let sampleContainer = new Container(null, {
	height: 100, width: 100,
	contents: [
		sampleContent
	]
});
```

`left`座標または`right`座標 (両方ではない) が定義されている場合、または`left`も`right`も定義されていない場合、fitted widthはmeasured widthと等しくなります。 同様に、`top`座標または`bottom`座標 (両方ではない) が定義されている場合、または`top`も`bottom`も定義されていない場合、fitted heightはmeasured heightと等しくなります。 たとえば、ここでの`sampleContent`オブジェクトのfitted widthとfitted heightは両方とも 50 になります。

```javascript
let sampleContent = new Content(null, {
	left: 0, top: 0, width: 50, height: 50
});
let sampleContainer = new Container(null, {
	height: 100, width: 100,
	contents: [
		sampleContent
	]
});
```

`content`オブジェクトの`width`プロパティと`height`プロパティは、contentのfitted sizeを反映します。 contentのmeasured widthとmeasured heightは、contentの作成直後に利用可能になります。 fitted widthとfitted heightは、`onDisplaying`イベントがトリガーされるまで利用できません。

```javascript
let sampleContent = new Content(null, {
	left: 0, right: 0, top: 0, height: 50,
	Behavior: class extends Behavior {
		onCreate(content) {
			let measuredHeight = content.coordinates.height; // 50
			let measuredWidth = content.coordinates.width; // undefined
			let fittedWidth = content.width; // undefined
			let fittedHeight = content.height; // 50
		}
		onDisplaying(content) {
			let fittedWidth = content.width; // 320 (assuming the application is 320x240)
			let fittedHeight = content.height; // 50
		}
	}
});
application.add(sampleContent)
```

##### テンプレート

*テンプレート* は、contentをインスタンス化し、包含階層を構築するために必要なスクリプト コードを削減するツールです。テンプレートは、似ているものの、いくつかのわずかに異なるプロパティを持つオブジェクトを作成するためによく使用されます。

たとえば、次の画面を考えてみましょう。

![](../assets/piu/templates.png)

テンプレートを使用しないこれらの画面のコードは、不必要に繰り返しになります。ここでの `screen1` と `screen2` の唯一の違いは、ヘッダーの背景色と文字列です。

```javascript
let screen1 = new Column(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "blue" }), style: sampleStyle,
	contents: [
		Label(null, {
			top: 0, height: 40, left: 0, right: 0,
			string: "Screen 1"
		}),
		Content(null, {
			top: 0, bottom: 0, left: 0, right: 0,
			skin: new Skin({ fill: "white" })
		}),
	]
});

let screen2 = new Column(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "red" }), style: sampleStyle,
	contents: [
		Label(null, {
			top: 0, height: 40, left: 0, right: 0,
			string: "Screen 2"
		}),
		Content(null, {
			top: 0, bottom: 0, left: 0, right: 0,
			skin: new Skin({ fill: "white" })
		}),
	]
});
```

テンプレートを使用すると、2 つの画面の定義に必要なコードの量が大幅に削減されます。

```javascript
let BasicScreen = Column.template($ => ({
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: $.headerColor }), style: sampleStyle,
	contents: [
		Label(null, {
			top: 0, height: 40, left: 0, right: 0,
			string: $.title
		}),
		Content(null, {
			top: 0, bottom: 0, left: 0, right: 0,
			skin: new Skin({ fill: "white" })
		}),
	]
}));

let screen1 = new BasicScreen({ title: "Screen 1", headerColor: "blue" });
let screen2 = new BasicScreen({ title: "Screen 2", headerColor: "red" });
```

上の例で見られるように、コンストラクタには 1 つのパラメーター `$` があり、アプリケーションはこれを使用してデータをテンプレートに渡します。データには任意のプロトタイプが含まれる場合があります。多くの場合、これは「文字列」、「数値」、または JSON オブジェクトです。属性内で、「content」オブジェクトと「container」オブジェクトは「$」を使用してデータ プロパティにアクセスします。 `$` パラメータは、「データ」または「インスタンス化データ」と呼ばれます。

ほとんどの Piu オブジェクトの `template` 関数は、コンストラクターと `template` 関数を返します。唯一の例外は、単にコンストラクターを返す `Skin` オブジェクトと `Texture` オブジェクトです。これらの `template` 関数を使用すると、コンストラクターをさらに特殊化できます。以下の例では、`HeaderWithBehavior` インスタンスは、アクティブで動作することに加えて、`BasicHeader` インスタンスのすべてのプロパティを持ちます。

```javascript
let headerStyle = new Style({ font:"600 28px Open Sans", color: "white", horizontal: "left" });
let headerSkin = new Skin({ fill: "blue" });
let BasicHeader = Label.template($ => ({
	top: 0, height: 40, left: 0, right: 0,
	skin: headerSkin, style: headerStyle, string: $
}));

class HeaderBehavior extends Behavior {
	onTouchEnded(header) {
		trace("Header tapped\n");
	}
}
let HeaderWithBehavior = BasicHeader.template($ => ({
	active: true, Behavior: HeaderBehavior
}));
```
### Behavior and Flow

#### イベント

Piu は *イベント* をアプリケーションの包含階層に配信し、`content` オブジェクトはイベントに応答する関数を含む`behavior`オブジェクトを参照できます。 Piu はイベントを広範囲に使用します。

`content`オブジェクトがイベントを受け取ると、その `behavior`オブジェクトがそのイベントに応答する対応する関数を (プロトタイプ チェーン内で直接的または間接的に) 持っているかどうかをチェックします。存在する場合、`content` オブジェクトは関数を呼び出し、それ自体を最初のパラメーターとして渡します。そうでない場合は、何も起こりません。

##### 低レベルのイベント

Piu は、幅広いターゲット デバイスで役立つ低レベルのイベントを定義します。たとえば、`onTouchBegan`と`onTouchEnded`は、タッチ スクリーンを備えたターゲット デバイスに役立ちます。

##### 高レベルのイベント

アプリケーションは独自の高レベルのイベントを定義することもできます。一般的なイベント伝播パターンを効率的に実装するために、コンテンツには `delegate`、`distribute`、`bubble`、`firstThat`、および `lastThat` プロパティがあります。これらのプロパティの説明については、[Contentオブジェクト](#content-object) セクションの [Functions](#content-functions) を参照してください。

#### アニメーション

時間ベースのアニメーション動作を作成するには、複数の方法があります。 1 つの方法は、このドキュメントの [Duration, Fraction, Interval, Loop, and Time](#duration-fraction-interval-loop-and-time) セクションで説明されているように、`content` オブジェクトをクロックとして使用することです。 この方法は、イベントに応じて単一のコンテンツをアニメーション化する場合 (たとえば、タッチ フィードバックを表示する場合) や、単一のコンテンツを長時間アニメーション化する場合などによく使用されます。

複数のコンテンツやプロパティをアニメーション化する複雑な動作を作成する最も簡単な方法は、`timeline` オブジェクトを使用することです。`timeline` オブジェクトは、一連のトゥイーンアニメーションのシーケンスと実行のメカニズムを提供するため、複数のコンテンツ オブジェクトの段階的なアニメーション (画面のグラフィック要素の切り替えなど) に最適な方法です。詳細については、このドキュメントの [Timelineオブジェクト](#timeline-object) セクションを参照してください。

Piu の `transition` オブジェクトは、画面間を移動したり、コンテンツ オブジェクトを入れ替えたりする単純なアニメーションに最適です。多くの場合、コンテンツを追加または削除することで、包含階層を変更します。`timeline` オブジェクトと同様にオブジェクトのプロパティを変更することもできますが、アニメーションのシーケンスを作成するのはより複雑です。詳細については、このドキュメントの [Transitionオブジェクト](#transition-object) セクションを参照してください。

##### Easing方程式

コンテンツ オブジェクトのプロパティを線形に変更するアニメーションは、見た目がぎこちないことがよくあります。Easing方程式を使用することは、より自然に感じられるアニメーションを実装するための一般的な方法です。

Piu は、JavaScript の `Math` オブジェクトを Robert Penner のオープンソースのEasing方程式で拡張します ([Robert Penner の Programming Macromedia Flash MX](http://robertpenner.com/easing/penner_chapter7_tweening.pdf) のEasingセクションを参照してください)。ソース コードは [`piuTransition.c`][33] にあります。

これらのEasing方程式の Piu 実装はすべて、単一の引数 (範囲 [0, 1] の `number`) を取ります。戻り値は範囲 [0, 1] の `number` で、入力をEasingされた出力にマッピングします。これらの方程式は、あらゆる種類のアニメーションで広く使用されています。一般的には、`content` オブジェクトの `fraction` プロパティまたは `transition` オブジェクトの `onStep` 関数の `fraction` 引数をマッピングするために使用され、多くの `timeline` オブジェクト関数の引数としても使用されます。

## グローバルプロパティ

Piu グローバル オブジェクトのプロパティは、アプリケーション内のどこでも使用できます。

### ビルトインプロパティ

- **Source code:** [`xsGlobal.c`][1]

##### `trace(string)`

| Arguments | Type | Description
| --- | --- |  :--- |
| `string` | `string` | The string to trace

ホストされたアプリケーションの仮想マシンで指定された文字列をトレースします。

> Note: 改行 ("\n") もトレースされるまで、文字列はトレースされません。

```javascript
trace("Hello world\n");

let sampleVariable = 2;
trace(`sampleVariable value is: ${sampleVariable}\n`);    // "sampleVariable value is 2"
```

***

### プロパティの追加

グローバル オブジェクトのプロパティを設定することで、プロパティを追加できます。

```javascript
global.application = new Application(null, {
	displayListLength: 2048, commandListLength: 2048
});
export default global.application;
```

## プロパティの説明

このセクションでは、以降の Piu オブジェクト リファレンス セクション全体で参照される特定のプロパティについて説明します。

### Anchor

各 `content` オブジェクトには、オプションの `anchor` プロパティを設定できます。テンプレートがインスタンス化されると、作成された `content` オブジェクトへの参照がインスタンス化データのプロパティに割り当てられます。プロパティの識別子は `anchor` プロパティの値です。

アンカーを使用すると、アプリケーションは、テンプレートからインスタンス化された包含階層内の特定のcontentとcontainerに直接アクセスできます。これにより、アプリケーションの実行中に変更するオブジェクトへのアクセスが容易になります。たとえば、文字列のラベルを変更したり、ボタンを無効にしたり、テーブルの列に行を追加したりできます。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans", color: "white" });
let SampleContent = Label.template($ => ({
	anchor: "ANCHORED_CONTENT", top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: $.color }), style: sampleStyle,
	string: $.title
}));
let SampleContainer = Container.template($ => ({
	active: true, top: 0, bottom: 0, left: 0, right: 0,
	contents: [
		new SampleContent($)
	],
	Behavior: class extends Behavior {
		onCreate(container, data) {
			this.data = data;
		}
		onTouchEnded(container) {
			this.data["ANCHORED_CONTENT"].string = "Hello!"
		}
	}
}));
application.add(new SampleContainer({ title: "Tap to update", color: "blue" }));
```

![](../assets/piu/anchor.gif)

### Color

`color` パラメータは、`skin` および `style` コンストラクタの辞書に渡すことができます。

色を指定するには、CSS 名 (レベル 2) または 16 進表記 (`"#RGB"`、`"#RGBA"`、`"#RRGGBB"`、`"#RRGGBBAA"`) を使用できます。

```javascript
const whiteSkin = new Skin({ fill:"white" });
const redSkin = new Skin({ fill:"#F00" });
const halfRedSkin = new Skin({ fill:"#F008" });
const greenSkin = new Skin({ fill:"#00FF00" });
const halfGreenSkin = new Skin({ fill:"#00FF0088" });
```

アルファ チャネルで色を指定するには、単なる 16 進数 `0xRRGGBBAA` を使用することもできます。

```javascript
const blueSkin = new Skin({ fill:0x0000FFFF });
const halfBlueSkin = new Skin({ fill:0x0000FF88 });
```

このような 16 進数を構築するために、Piu は CSS 関数表記に似た関数をエクスポートします。

```javascript
import { rgb, rgba, hsl, hsla } from "piu/All";
const yellowSkin = new Skin({ fill:rgb(255, 255, 0) });
const halfYellowSkin = new Skin({ fill:rgba(255, 255, 0, 0.5) });
const cyanSkin = new Skin({ fill:hsl(180, 1, 0.5) });
const halfCyanSkin = new Skin({ fill:hsla(180, 1, 0.5, 0.5) });
```

辞書では、色は単一の色、または 2、3、または 4 色の配列になります。`skin` オブジェクトと `style` オブジェクトは、それらを使用する `content` オブジェクトの状態に基づいて色をブレンドします。詳細については、[Variant, Variants, State, and States](#variant-variants-state-and-states) を参照してください。

### Coordinates

すべての `content` オブジェクトには `coordinates` プロパティがあります。coordinates プロパティは、`left`、`width`、`right`、`top`、`height`、および `bottom` プロパティを持つオブジェクトで、これらはすべて `undefined` にすることができます。contentの座標は、containerと `previous` および `next` プロパティ (同じcontainer内の他の `content` オブジェクト) に対する相対的な位置とサイズを決定します。

contentのcontainerが `application`、`container`、`scroller`、または `layout` オブジェクトの場合:

- `top`、`bottom`、`left`、`right` 座標はすべてcontainerに対する相対座標です。
- `width`、`left`、`right` 座標がすべて指定されている場合、`left` と `right` 座標は無視されます。
- `left` と `right` の両方が指定されていない場合、contentは指定された幅（指定されていない場合は幅 0）でcontainer内で水平方向に中央揃えされます。
- `height`、`top`、`bottom` 座標がすべて指定されている場合、`top` と `bottom` 座標は無視されます。
- `top` と `bottom` の両方が指定されていない場合、contentは指定された高さ（指定されていない場合は高さ 0）でcontainer内で垂直方向に中央揃えされます。

contentのcontainerが `column` オブジェクトの場合:

- `top` と `bottom` の座標は、`previous` と `next` プロパティを基準とします
- `left` と `right` の座標は、containerを基準とします
- `width`、`left`、`right` の座標がすべて指定されている場合、`left` と `right` の座標は無視されます

contentのcontainerが `row` オブジェクトの場合:

- `left` と `right` の座標は、`previous` と `next` プロパティを基準とします
- `top` と `bottom` の座標は、containerを基準とします
- `height`、`top`、`bottom` の座標がすべて指定されている場合、`top` と `bottom` の座標は無視されます

### Duration, Fraction, Interval, Loop, and Time

包含階層内のすべてのcontentは、`duration`、`fraction`、`interval`、および `time` プロパティを使用して時間ベースのアニメーション動作を制御するクロックとして使用できます。

- `duration` プロパティはアニメーションの継続時間で、ミリ秒単位で表されます。
- `time` プロパティはコンテンツの時計の現在の時刻を提供します。
- `fraction` プロパティは、コンテンツの継続時間に対する時計の現在の時刻の比率です。
- `interval` プロパティはアニメーションのフレーム間の時間 (フレーム レート) で、ミリ秒単位で表されます。

コンテンツの `start` および `stop` 関数は、クロックの実行を制御します。現在の時刻が期間に達すると、クロックは自動的に停止します。コンテンツの `loop` プロパティが true に設定されている場合、クロックはリスタートします。

クロックが実行中の場合、そのコンテンツの `onTimeChanged` 関数は、コンテンツの `interval` プロパティで指定された間隔でトリガーされます。コンテンツの時間がその継続時間（`duration`）と等しくなると、`onFinished` 関数がトリガーされます。

```javascript
let animatedContent = new Content(null, {
	height: 100, width: 100, loop: true,
	skin: new Skin({ fill: ["red", "yellow", "blue"] }),
	Behavior: class extends Behavior {
		onCreate(content) {
			this.startAnimation(content);
		}
		startAnimation(content) {
			content.duration = 3000;
			content.time = 0;
			content.start();
		}
		onTimeChanged(content) {
			content.state = content.fraction*2;
		}
	}
});
application.add(animatedContent);
```

![](../assets/piu/contentClockAnimation.gif)

#### Time-based animation

`interval` プロパティは、オブジェクトのクロックに必要なフレーム レートを設定しますが、アプリケーションが過負荷になっている場合は正確ではない可能性があることに注意してください。たとえば、コンテンツの `interval` が 2 で `duration` が 100 の場合、`onFinished` イベントがトリガーされる前に `onTimeChanged` イベントが正確に 50 回トリガーされると想定するのは安全ではありません。したがって、アプリケーションでは `fraction` プロパティまたは `time` プロパティを使用して、アニメーションに表示するフレームを決定することをお勧めします。

たとえば、タップすると両方向に 10 ピクセル拡大および縮小するボタンの 2 つの実装について考えてみましょう。

![](../assets/piu/expandingButton.gif)

次の実装は、`onFinished` の前に `onTimeChanged` が正確に 20 回トリガーされることを前提としているため、**推奨されません**。ほとんどの場合は機能しますが、コンテンツが元のサイズに縮小される前に `onFinished` イベントがトリガーされる可能性があります。

```javascript
class ExpandingBehavior extends Behavior {
	onTouchEnded(content) {
		this.index = 0;
		content.interval = 5;
		content.duration = 100;
		content.time = 0;
		content.start();
	}
	onTimeChanged(content) {
		this.index++;
		if (this.index < 10) content.sizeBy(1, 1);
		else content.sizeBy(-1, -1);
	}
}

let expandingButton = new Content(null, {
	active: true, height: 50, width: 100,
	skin: new Skin({ fill: "blue" }),
	Behavior: ExpandingBehavior
});
```

次の実装は、描画されるフレームの数について想定せず、`onFinished` が呼び出されたときにコンテンツが正しいサイズに縮小されることを保証するため、より優れています。

```javascript
class ExpandingBehavior extends Behavior {
	onTouchEnded(content) {
		this.startingSize = { h: content.height, w: content.width };
		content.interval = 5;
		content.duration = 100;
		content.time = 0;
		content.start();
	}
	onTimeChanged(content) {
		let startingSize = this.startingSize;
		let fraction = content.fraction;
		if (fraction > 0.5) fraction = 1-fraction;
		fraction *= 20;
		content.height = startingSize.h + fraction;
		content.width = startingSize.w + fraction;
	}
	onFinished(content) {
		let startingSize = this.startingSize;
		content.height = startingSize.h;
		content.width = startingSize.w;
	}
}

let expandingButton = new Content(null, {
	active: true, height: 50, width: 100,
	skin: new Skin({ fill: "blue" }),
	Behavior: ExpandingBehavior
});
```

### Font

`font` パラメータを `style` コンストラクタの辞書に渡す必要があります。

Piu はビットマップ フォントを使用します。メトリックはバイナリ FNT ファイルによって提供され、グリフは PNG ファイルによって提供されます。

フォントはアセットであり、マニフェストのリソースで定義する必要があります。フォントには、デフォルトのターゲット、または `-alpha` または `-color` 疑似ターゲットを使用します。

バイナリ FNT ファイル形式は、[AngelCode](http://www.angelcode.com/products/bmfont/doc/file_format.html) によって定義され、Windows 用の [BMFont](http://www.angelcode.com/products/bmfont/) ジェネレーターもリリースされました。Mac では、たとえば [Glyph Designer](https://71squared.com/glyphdesigner) または [bmGlyph](https://www.bmglyph.com) を使用して、このようなファイルを生成できます。

1 つのビットマップ フォントは、1 つのスタイル、1 つの太さ、1 つの伸縮、1 つのサイズ、1 つのファミリに対応します。バリエーションごとに別々のビットマップ フォントが必要になります。

`Style` コンストラクタは、辞書の `font` プロパティを使用してフォントを検索します。`font` プロパティをバイナリ FNT ファイルの名前に設定するだけです。

```javascript
const popStyle = new Style({ font:"popFont" });
```

`popStyle` オブジェクトは、アセット内の `popFont.fnt` ファイルを使用します。

ルックアップは、スタイルを使用する `label` または `text` オブジェクトが表示されている包含​​階層にバインドされているとき、または `Style.prototype.measure` メソッドが呼び出されたときにのみ行われます。

#### Cascading Styles

スタイルをカスケードするには、[CSS フォント ショートカット](https://developer.mozilla.org/en-US/docs/Web/CSS/font) に似たものを使用することをお勧めします。アプリケーションでフォントを定義するこの方法を使用する場合は、上記のようにフォント名だけを使用してアプリケーション内の他のフォントを定義しないでください。

```javascript
const style = new Style({ font:"italic bold 16px Open Sans" });
```

Piu では、フォント プロパティには次の順序で 5 つのオプション部分があります:

1. style: `italic` | `normal` | `inherit`
2. weight: `100` | `ultralight` | `200` | `thin` | `300` | `light` | `400` | `normal` | `500` | `medium` | `600` | `semibold` | `700` | `bold` |  `800` | `heavy` | `900` | `black` | `lighter`  | `bolder`  | `inherit `
3. stretch: `condensed` | `normal` | `inherit `
4. size: `xx-small` | `x-small` | `small` | `medium` | `large` | `x-large` | `xx-large` | `smaller` | `larger ` | `[1-9][0-9]+px` | `[1-9][0-9]+%` | `inherit `
5. family: the rest of the font property if any.

`inherit` 値はデフォルト値であり、フォント プロパティの曖昧さを解消する場合にのみ役立ちます。デフォルトでは、`content` オブジェクトのスタイルは、その `container` オブジェクトのスタイルを継承します。

すべての部分はオプションであり、個別にカスケードされます。したがって、たとえば、アプリケーションに汎用スタイルを定義し、そのコンテンツに特定のスタイルを定義できます：

```javascript
const appStyle = new Style({ font:"16px Fira Sans" });
const menuStyle = new Style({ font:"bold" });
```

スタイルは、それを使用する `label` または `text` オブジェクトが、表示される包含階層にバインドされている場合にのみカスケードされます。

Piu がアセット内の対応するビットマップ フォント ファイルを見つけるには、一般的な方法に基づいて次の規則を採用する必要があります：

* the family, without spaces,
* `-`,
* the capitalized name of the stretch, if not `normal`,
* the capitalized name of the computed weight, if not `normal`,
* the capitalized name of the style, if not `normal`,
* `Regular`, if the stretch, the computed weight and the style are all `normal`,
* `-`,
* the computed size in pixels without units.

ここで、`style` は `OpenSans-BoldItalic-16.fnt` を検索し、`appStyle` は `FiraSans-Regular-16.fnt` を検索し、`menuStyle` は `FiraSans-Bold-16.fnt` を検索します。

### Tiles

アプリケーションは、`tiles` プロパティを使用してスキンをタイル化できます。タイルのサイズは、次のように左、右、上、下の値によって決まります。：

- `tiles` プロパティが未定義の場合、またはその `left`、`right`、`top`、および `bottom` の値がすべて未定義の場合、スキンはアイコンなどの単なる画像になります。
- `left` または `right` の値のみが定義されている場合、スキンは水平方向の 3 部構成のパターンになります。左部分はコンテンツの左側に描画され、右部分はコンテンツの右側に描画され、中央部分はコンテンツの中央を埋めるために繰り返されます。
- `top` または `bottom` 値のみが定義されている場合、スキンは垂直の 3 部パターンになります。上部はコンテンツの上部に描画され、下部はコンテンツの下部に描画され、中央部分はコンテンツの中央を埋めるために繰り返されます。
- それ以外の場合、スキンは 9 つの部分からなるパターンになります。コーナー部分はコンテンツの対応するコーナーに描画され、サイド部分はサイドに繰り返され、中央部分は中央を埋めるために繰り返されます。`left`、`right`、`top`、`bottom` の値をすべて 0 に定義して、スキンでコンテンツを塗りつぶすことができます。

タイリング スキンを使用すると、さまざまなサイズのコンテンツ オブジェクトで 1 つのアセットを共有できます。次の例では、30 x 30 ピクセルの背景を使用して任意のサイズのコンテンツを塗りつぶします。

![](../assets/piu/roundedRectangle.png)

```javascript
let roundedRectangleTexture = new Texture("roundedRectangle.png");
let roundedRectangleSkin = new Skin({
	texture: roundedRectangleTexture,
	x: 0, y: 0, width: 30, height: 30,
	tiles: { left: 5, right: 5, top: 5, bottom: 5 }
});

let sampleStyle = new Style({ font:"600 28px Open Sans", color: "white" });

let smallText = new Label(null, {
	skin: roundedRectangleSkin, top: 20, left: 20, height: 30, width: 30,
	style: sampleStyle, string: "Hi"
});
let bigText = new Text(null, {
	skin: roundedRectangleSkin, top: 70, left: 20, height: 120, width: 100,
	style: sampleStyle, string: "This is a long string"
});

application.add(smallText);
application.add(bigText);
```

![](../assets/piu/skinTiles.png)

### Variant, Variants, State, and States

アプリケーションでは、`content` オブジェクトの `state` プロパティと `variant` プロパティを使用して外観を更新することがよくあります。これらは動的に変更できるため、たとえば `content` オブジェクトをアニメーション化したり、タッチ イベントに視覚的なフィードバックを提供したりするために使用できます。

#### Colored skins and styles

`state` プロパティの一般的な使用方法は、コンテンツの色を更新することです。このドキュメントの [Color section](#color) で説明されているように、`skin` または `style` オブジェクトの `fill` プロパティは色の配列にすることができます。`skin` を使用する `content` オブジェクトの `state` プロパティは、配列のインデックスを決定します。状態が整数でない場合は、周囲の状態の色がブレンドされます。

```javascript
let multiColoredSkin = new Skin({ fill: ["black", "white", "red"] });
let blackContent = new Content(null, {
	top: 20, left: 20, width: 80, height: 80,
	skin: multiColoredSkin, state: 0,
});
let redContent = new Content(null, {
	top: 20, left: 120, width: 80, height: 80,
	skin: multiColoredSkin, state: 2
});
let grayContent = new Content(null, {
	top: 20, left: 220, width: 80, height: 80,
	skin: multiColoredSkin, state: 0.5
});
application.add(blackContent);
application.add(redContent);
application.add(grayContent);
```

![](../assets/piu/states.png)

#### Reusing textures

多くの場合、複数のアイコンやその他のユーザー インターフェイス要素を 1 つのイメージに保存すると便利です。`skin` コンストラクタのディクショナリで `states` プロパティと `variants` プロパティを指定すると、同じテクスチャの異なるセクションを参照できます。これにより、アプリケーションが類似したイメージを参照して複数のスキンを作成する必要がなくなります。

スキンの `states` プロパティと `variants` プロパティは、テクスチャ内の単一要素のサイズを定義するために使用される数値です。`states` プロパティは状態間の垂直オフセットを表し、`variants` プロパティはバリアント間の水平オフセットを表します。次の例は、1 つの画像に 10 個の 28x28 ピクセルのアイコンと、アプリケーションが各アイコンを個別に参照できるようにする `skin` を含むアセットの例です。

![](../assets/piu/wifi-strip.png)

```javascript
const wiFiStripTexture = new Texture({ path:"wifi-strip.png" });
const wiFiSkin = new Skin({
	texture: wiFiStripTexture,
	width: 28, height: 28,
	states: 28, variants: 28
});
```

`texture` オブジェクトの `states` プロパティと `variants` プロパティは、`content` オブジェクトの `state` プロパティと `variant` プロパティと関連していますが、混同しないでください。`content` オブジェクトの `state` と `variant` プロパティは、`skin` のどの領域をレンダリングするかを選択するために使用されます。次の図は、最後の例の `skin` オブジェクトのさまざまな部分を参照するために `content` が使用する `state` プロパティと `variant` プロパティを視覚化したものです。

![](../assets/piu/wifi-strip.gif)

`content` の `state` と `variant` はいつでも更新できます。これは多くの場合、イベントがトリガーされたときに行われます。

```javscript
let WiFiStatusIcon = Content.template($ => ({
    skin: wiFiSkin, state: $.passwordProtected, variant: 0,
    interval: 500, duration: 2500, loop: true,
    active: true,
    Behavior: class extends Behavior {
    	onDisplaying(content) {
    		content.start();
    	}
        onTimeChanged(content) {
        	let variant = content.variant;
        	variant++;
        	if (variant > 4) variant = 0;
        	content.variant = variant;
        }
    }
}));
application.add(new WiFiStatusIcon({ passwordProtected: false }, { top: 20, right: 20 }));
application.add(new WiFiStatusIcon({ passwordProtected: true }, { top: 58, right: 20 }));
```

![](../assets/piu/stateAndVariant.gif)

## Piuオブジェクトリファレンス

このセクションでは、Piu API を定義するオブジェクトの詳細について説明します。各オブジェクトについて、関連する場合は次の情報が提示されます。:

- **Source Code**: オブジェクトのソースコードへのリンク

- **Relevant Examples**: オブジェクトの使用方法を示すサンプルアプリへのリンク

- **コンストラクタの説明**: オブジェクトのコンストラクタの説明

- **Dictionary**: オブジェクトの辞書ベースのコンストラクタに渡される辞書に関する追加情報が必要な場合に存在します。辞書に含まれる可能性があるプロパティについて説明します。辞書パラメーターは、作成されたインスタンスで同じ名前を持つプロパティを設定します (特に指定がない限り)。

- **プロトタイプの説明**: このオブジェクトのプロトタイプが継承するプロトタイプと、このオブジェクトのプロトタイプに固有のプロパティと機能の説明

- **Events**: オブジェクトがトリガーするイベントの説明

完全な JavaScript プログラミング インターフェイスについては、[`piuAll.js`][0] を参照してください。

### Applicationオブジェクト

- **Source code:** [`piuApplication.c`][2]
- **Relevant Examples:** all

すべての Piu アプリケーションは、包含階層のルートに `application` オブジェクトを持っている必要があります。他のすべての `content` オブジェクトは、画面に表示されるように `application` に追加する必要があります。

デフォルトのオブジェクトはないので、自分で作成してメイン モジュールにエクスポートする必要があります。

```javascript
export default new Application();
```

あるいは、`application` オブジェクトを返す関数をエクスポートすることもできます。

```javascript
export default function() {
	return new Application();
}
```

#### コンストラクタの説明

##### `Application([behaviorData, dictionary])`

| Arguments | Type | Description
| --- | --- | :--- |
| `behaviorData` | `*` | このコンテナの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Container.prototype` から継承するオブジェクトである `application` インスタンスを返します。

```javascript
export default new Application(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "blue" }),
});
```

![](../assets/piu/sampleApplication1.png)

##### `Application.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Application.prototype` のインスタンスを作成する関数であるコンストラクターを返します。結果の `prototype` プロパティは `Application.prototype` です。結果には `template` 関数も提供されます。

```javascript
// Taken from the balls example app

...

let BallApplication = Application.template($ => ({
	skin:backgroundSkin,
	contents: [
		Content(6, { left:0, top:0, skin:ballSkin, variant:0, Behavior: BallBehavior } ),
		Content(5, { right:0, top:0, skin:ballSkin, variant:1, Behavior: BallBehavior } ),
		Content(4, { right:0, bottom:0, skin:ballSkin, variant:2, Behavior: BallBehavior } ),
		Content(3, { left:0, bottom:0, skin:ballSkin, variant:3, Behavior: BallBehavior } ),
	]
}));

export default new BallApplication(null, { displayListLength:4096, touchCount:0 });
```

#### Dictionary

`container` オブジェクトの場合と同じです ([Containerオブジェクト](#container-object) セクションの [Dictionary](#container-dictionary) を参照)。さらに次のようになります。

| Parameter | Type | Description |
| --- | --- | :--- |
| `commandListLength ` | `number` | Piu 描画操作を保持するために使用されるコマンド リスト バッファのサイズ (バイト単位)
| `displayListLength ` | `number` | Poco レンダリング エンジンを使用するターゲットのディスプレイ リスト バッファのサイズ (バイト単位)
| `touchCount` | `number` | 同時にトリガーできるタッチイベントの数

#### プロトタイプの説明

プロトタイプは `Container.prototype` から継承します。

### Behaviorオブジェクト

- **Source code:** [`piuBehavior.c`][3]
- **Relevant Examples:** [balls][18], [drag][19]

`behavior` オブジェクトには、`content` オブジェクトによってトリガーされるイベントに対応する関数が含まれています。`content` オブジェクトは、その動作がイベントの名前を持つ関数プロパティを所有または継承しているかどうかを確認し、そうである場合は、その関数を呼び出し、最初のパラメータとして自身を渡します。

#### コンストラクタの説明

アプリケーションは、`Behavior.prototype` から継承し、構築時に `content` オブジェクトに割り当てた `behavior` オブジェクトの独自のコンストラクタを定義します。`content` オブジェクトが構築されると、割り当てられた動作クラスのインスタンスが作成され、動作の `onCreate` イベントがトリガーされます。この関数はデフォルトでは何も行いません。たとえば、動作はこれを使用してプロパティを初期化できます。

```javascript
class SampleBehavior extends Behavior {
	onCreate(content, data) {
		this.name = data.name;
	}
	onTouchEnded(content) {
		trace(`Name is: ${this.name}\n`);	// "Name is: Moddable"
	}
}

let sampleContent = new Content({ name: "Moddable" }, {
    active: true, height: 100, width: 100,
    skin: new Skin({fill: "blue"}),
    Behavior: SampleBehavior
});
application.add(sampleContent);
```

![Behavior Sample](../assets/piu/behaviorSample.gif)

#### プロトタイプの説明

プロトタイプは `Object.prototype` から継承します。

### Columnオブジェクト

- **Source code:** [`piuColumn.c`][4]
- **Relevant Examples:** [keyboard][20], [weather][21]

`column` オブジェクトは、その内容を垂直に配置する `container` オブジェクトです。

#### コンストラクタの説明

##### `Column([behaviorData, dictionary])`

| Arguments | Type | Description
| --- | --- | :--- |
| `behaviorData` | `*` | このコンテンツの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。辞書は `content` オブジェクトの場合と同じです。[コンテンツ オブジェクト](#container-object) の [辞書](#content-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Column .prototype` から継承したオブジェクトである `column` インスタンスを返します。

```javascript
let ColoredSquare = Content.template($ => ({
	left: 0, right: 0, top: 0, bottom: 0,
	skin: new Skin({ fill: $ })
}));

let sampleColumn = new Column(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	contents: [
		new ColoredSquare("red"),
		new ColoredSquare("blue"),
		new ColoredSquare("black"),
	]
});
application.add(sampleColumn);
```

![](../assets/piu/sampleColumn1.png)

##### `Column.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Column.prototype` のインスタンスを作成する関数であるコンストラクターを返します。結果の `prototype` プロパティは `Column.prototype` です。結果には `template` 関数も提供されます。

```javascript
let ColoredSquare = Content.template($ => ({
	left: 0, right: 0, top: 0, bottom: 0,
	skin: new Skin({ fill: $ })
}));

let SampleColumn = Column.template($ => ({
	top: 0, bottom: 0, left: 0, right: 0,
	contents: [
		new ColoredSquare($.firstColor),
		new ColoredSquare($.secondColor),
	],
}));
application.add(new SampleColumn({ firstColor:"red", secondColor:"blue" }));
```

![](../assets/piu/sampleColumn2.png)

#### プロトタイプの説明

プロトタイプは `Container.prototype` から継承します。

<a id="column-events"></a>
#### Events

`container` オブジェクトの場合と同じです ([Containerオブジェクト](#container-object) セクションの [Events](#container-events) を参照してください)

### Containerオブジェクト

- **Source code:** [`piuContainer.c`][5]
- **Relevant Examples:** [drag][19], [transitions][23]

`container` オブジェクトは、他の `content` オブジェクトを含むことができる `content` オブジェクトです。コンテナでは、`content` オブジェクトは二重リンク リストに格納されます。`content` オブジェクトには、`content` プロパティを使用してインデックスまたは名前でアクセスすることもできます (例: `container.content(0)` または `container.content("foo")`)。

#### コンストラクタの説明

##### `Container([behaviorData, dictionary])`

| Arguments | Type | Description
| --- | --- | :--- |
| `behaviorData` | `*` | このコンテナの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#container-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Container.prototype` から継承するオブジェクトである `container` インスタンスを返します。

```javascript
let ColoredSquare = Content.template($ => ({
	height: 100, width: 100,
	skin: new Skin({ fill: $ })
}));

let sampleContainer = new Container(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	contents: [
		new ColoredSquare("blue", { left: 0, top: 0 }),
		new ColoredSquare("red", { right: 0, top: 50 }),
		new ColoredSquare("black", { left: 100, bottom: 0 }),
	]
})
application.add(sampleContainer);
```

![](../assets/piu/sampleContainer1.png)

##### `Container.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Container.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Container.prototype` です。結果には `template` 関数も提供されます。

```javascript
let ColoredSquare = Content.template($ => ({
	height: 100, width: 100,
	skin: new Skin({ fill: $ })
}));

let SampleContainer = Container.template($ => ({
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: $.backgroundColor }),
	contents: [
		new ColoredSquare( $.squareColor ),
	]
}));
application.add(new SampleContainer({ backgroundColor: "white", squareColor: "blue" }));
```

![](../assets/piu/sampleContainer2.png)

<a id="container-dictionary"></a>
#### Dictionary

`content` オブジェクトの場合と同じです ([コンテンツ オブジェクト](#content-object) セクションの [辞書](#content-dictionary) を参照)。さらに次のようになります。

| Parameter | Type | Description |
| --- | --- | :--- |
| `clip` | `boolean` | `true` の場合、このコンテナはその内容をクリップします。

#### プロトタイプの説明

プロトタイプは `Content.prototype` から継承します。

<a id="container-properties"></a>
##### プロパティ

`content` オブジェクトの場合と同じです ([コンテンツ オブジェクト](#content-object) セクションの [プロパティ](#content-properties) を参照)。さらに次のようになります。

| Name | Type | Default Value | Read Only | Description |
| --- | --- | --- | --- | :--- |
| `clip` | `boolean` | `false` | | `true` の場合、このコンテナはその内容をクリップします。
| `first` | `object` |  | ✓ | このコンテナの最初の `content` オブジェクト、またはこのコンテナが空の場合は `null`
| `last` |`object` | | ✓ | このコンテナ内の最後の `content` オブジェクト、またはこのコンテナが空の場合は `null`
| `length` |`number` | | ✓ | このコンテナ内の`content`オブジェクトの数
| `transitioning` | `boolean` | `false` | ✓| `true` の場合、このコンテナは `transition` オブジェクトを実行しています。

<a id="container-functions"></a>
##### Functions

**`add(content)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` | 追加する `content` オブジェクト。バインドされていない必要があります。つまり、そのコンテナーは `null` である必要があります。

指定された `content` オブジェクトをこのコンテナに追加します。`content` オブジェクトは、このコンテナ内の最後の `content` オブジェクトになります。

```javascript
let ColoredSquare = Content.template($ => ({
	height: 100, width: 100,
	skin: new Skin({ fill: $ })
}));
let sampleContainer = new Container(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
});

sampleContainer.add(new ColoredSquare("black", {bottom: 10, left: 15}));
sampleContainer.add(new ColoredSquare("blue", {left: 100}));
sampleContainer.add(new ColoredSquare("green", {top: 10, left: 10}));
application.add(sampleContainer);
```

![](../assets/piu/containerAdd.png)

***

**`content(at)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `at` | `number` or `string` | アクセスしたい`content`オブジェクトの`index`または`name`プロパティ

指定された `content` オブジェクトを返します。指定された `index` または `name` を持つコンテンツがこのコンテナにない場合は `undefined` を返します。

```javascript
let sampleContainer = new Container(null, {
	contents: [
		new Content(null, { name: "Moddable" }),
		new Content(),
	]
});
let namedContent = sampleContainer.content("Moddable");
let unnamedContent = sampleContainer.content(1);
let nonexistentContent = sampleContainer.content("Nonexistent");	// undefined
```

***

**`empty([start, stop])`**

| Argument | Type |Description |
| --- | --- | :--- |
| `start` | `number` | 開始インデックス
| `stop` | `number` |停止インデックス（デフォルトでは`this.length`）

このコンテナから、インデックス `start` から始まり、インデックス `stop` で終わる `content` オブジェクトを削除します。`start` または `stop` が 0 未満の場合、それは `this.length` からのオフセットです。

```javascript
let ColoredSquare = Content.template($ => ({
	height: 100, width: 100,
	skin: new Skin({ fill: $ })
}));

let sampleContainer = new Container(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	contents: [
		new ColoredSquare("blue", { top: 0, left:0 }),
		new ColoredSquare("red", { top: 0, right: 0 })
	]
});

sampleContainer.empty();
application.add(sampleContainer);
```

![](../assets/piu/containerEmpty.png)

***

**`firstThat(id [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `id` | `string` | トリガーするイベントの名前
| `...` | `*` | 0個以上の追加パラメータ

このコンテナ内のすべての `content` オブジェクトが `id` の値で指定されたイベントをトリガーします。トラバーサルの順序は最初から最後までです。分散イベントが `true` を返すと、トラバーサルは停止します。分散イベントの最初のパラメータは、このコンテナではなく、イベントをトリガーする `content` オブジェクトであることに注意してください。イベントの追加パラメータ (存在する場合) は、`firstThat` 関数の追加パラメータです。

```javascript
class SampleBehavior extends Behavior {
	sampleEvent(content) {
		trace(content.name+" triggered\n");
		return true;
	}
}

let sampleContainer = new Container(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	contents: [
		Content(null, { name: "firstContent" }),
		Content(null, { name: "secondContent", Behavior: SampleBehavior }),
		Content(null, { name: "thirdContent", Behavior: SampleBehavior }),
	],
	Behavior: class extends Behavior {
		onDisplaying(container) {
			container.firstThat("sampleEvent");	// "secondContent triggered" will be traced here
		}
	}
});
application.add(sampleContainer);
```

***

**`insert(content, before)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` | 挿入する `content` オブジェクト。そのコンテナーは `null` である必要があります。
| `before` | `object` | 挿入する前の `content` オブジェクト。そのコンテナーはこのコンテナーである必要があります。

パラメータで指定されたとおりに、このコンテナ内の別のオブジェクトの前に 1 つの `content` オブジェクトを挿入します。

```javascript
let ColoredSquare = Content.template($ => ({
    height: 100, width: 100,
    skin: new Skin({ fill: $ })
}));

let sampleContainer = new Container(null, {
    top: 0, bottom: 0, left: 0, right: 0,
    skin: new Skin({ fill: "white" }),
    contents: [
        new ColoredSquare("blue", { top: 0, left:0 }),
    ]
});
let redSquare = new ColoredSquare("red", { top: 20, left: 20, });
sampleContainer.insert(redSquare, sampleContainer.first);
application.add(sampleContainer);
```

![](../assets/piu/containerInsert.png)

***


**`lastThat(id [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `id` | `string` | トリガーするイベントの名前
| `...` | `*` | 0個以上の追加パラメータ

このコンテナ内のすべての `content` オブジェクトが `id` の値で指定されたイベントをトリガーします。トラバーサルの順序は最後から最初です。分散イベントが `true` を返すと、トラバーサルは停止します。分散イベントの最初のパラメータは、このコンテナではなく、イベントをトリガーする `content` オブジェクトであることに注意してください。イベントの追加パラメータ (存在する場合) は、`lastThat` 関数の追加パラメータです。

```javascript
class SampleBehavior extends Behavior {
	sampleEvent(content) {
		trace(content.name+" triggered\n");
		return true;
	}
}

let sampleContainer = new Container(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	contents: [
		Content(null, { name: "firstContent" }),
		Content(null, { name: "secondContent", Behavior: SampleBehavior }),
		Content(null, { name: "thirdContent", Behavior: SampleBehavior }),
	],
	Behavior: class extends Behavior {
		onDisplaying(container) {
			container.lastThat("sampleEvent");	// "thirdContent triggered" will be traced here
		}
	}
});
application.add(sampleContainer);
```

***

**`remove(content)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` | 削除する `content` オブジェクト。そのコンテナーはこのコンテナーである必要があります。

指定された `content` オブジェクトをこのコンテナから削除します

```javascript
let ColoredSquare = Content.template($ => ({
	height: 100, width: 100,
	skin: new Skin({ fill: $ })
}));

let redSquare = new ColoredSquare("red");
let blueSquare = new ColoredSquare("blue");

let sampleContainer = new Container(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	contents: [
		redSquare,
		blueSquare,
	],
});
sampleContainer.remove(redSquare);
application.add(sampleContainer);
```

![](../assets/piu/containerRemove.png)

***

**`replace(content, by)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` | 置き換える `content` オブジェクト。そのコンテナーはこのコンテナーである必要があります。
| `by` | `content` | 置換する `content` オブジェクト。バインドされていない必要があります。つまり、そのコンテナーは `null` である必要があります。

パラメータで指定されたとおりに、コンテナ内の `content` オブジェクトを別のオブジェクトに置き換えます。

```javascript
let ColoredSquare = Content.template($ => ({
	height: 100, width: 100,
	skin: new Skin({ fill: $ })
}));

let redSquare = new ColoredSquare("red");
let blueSquare = new ColoredSquare("blue");

let sampleContainer = new Container(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	contents: [
		blueSquare,
	],
});
sampleContainer.replace(blueSquare, redSquare);
application.add(sampleContainer);
```

![](../assets/piu/containerReplace.png)

***

**`run(transition [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `transition` | `transition` | 実行する`transition`オブジェクト
| `...` | `*` | 0個以上の追加パラメータ

このコンテナで指定された `transition` オブジェクトを実行し、遷移の期間中そのオブジェクトをこのコンテナにバインドします。追加のパラメータは、`transition` オブジェクトの `onBegin` 関数と `onEnd` 関数に渡されます。コンテナは、遷移が開始する前に `onTransitionBeginning` イベントをトリガーし、遷移が終了した後に `onTransitionEnded` イベントをトリガーします。

```javascript
import CombTransition from "piu/CombTransition";
class SwitchScreenBehavior extends Behavior {
	onCreate(content, data) {
		this.data = data;
	}
	onTouchEnded(content) {
		let data = this.data;
		let transition = new CombTransition(250, Math.quadEaseOut, "horizontal", 4);
		let nextScreen = new ColoredScreen({ color: data.nextColor, nextColor: data.color })
		application.run(transition, application.first, nextScreen);
	}
}
let ColoredScreen = Container.template($ => ({
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: $.color }),
	contents: [
		Content($, {
			active: true, height: 100, width: 100,
			skin: new Skin({ fill: $.nextColor }),
			Behavior: SwitchScreenBehavior
		})
	]
}));
application.add(new ColoredScreen({ color: "red", nextColor: "blue" }));
```

![](../assets/piu/containerRun.gif)

***

**`swap(content0, content1)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content0, content1` | `content` | 交換する `content` オブジェクト。両方のオブジェクトのコンテナーはこのコンテナーである必要があります。

このコンテナ内の指定された `content` オブジェクトを交換します。

```javascript
let ColoredSquare = Content.template($ => ({
    height: 100, width: 100,
    skin: new Skin({ fill: $ })
}));

let sampleContainer = new Container(null, {
    active: true, top: 0, bottom: 0, left: 0, right: 0,
    skin: new Skin({ fill: "white" }),
    contents: [
        new ColoredSquare("blue", { top: 0, left:0 }),
        new ColoredSquare("red", { top: 50, left:50 }),
    ],
    Behavior: class extends Behavior {
    	onTouchEnded(container) {
    		container.swap(container.first, container.last);
    	}
    }
});
application.add(sampleContainer);
```

![](../assets/piu/containerSwap.gif)

***

<a id="container-events"></a>
#### Events

`content` オブジェクトの場合と同じです ([Contentオブジェクト](#content-object) セクションの [Events](#content-events) を参照)。さらに次のようになります。

**`onTransitionBeginning(container)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `container` | `container` | イベントをトリガーした`container`オブジェクト

このイベントは、指定された `container` オブジェクト内で `transition` オブジェクトが開始されたときにトリガーされます。

***

**`onTransitionEnded(container)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `container` | `container` | イベントをトリガーした`container`オブジェクト

このイベントは、`transition` オブジェクトが指定された `container` オブジェクト内で終了したときにトリガーされます。

***

### Contentオブジェクト

- **Source code:** [`piuContent.c`][6]
- **Relevant Examples:** [balls][18], [love-js][20]

アプリケーションは、ボタン、アイコン、スライダー、スイッチ、タブなどのユーザー インターフェイスのグラフィカル部分に `content` オブジェクトを使用します。

#### コンストラクタの説明

##### `Content([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` | このコンテンツの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#content-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Content.prototype` から継承したオブジェクトである `content` インスタンスを返します。

```javascript
let sampleContent = new Content("Hello", {
	top: 0, right: 50, height: 100, width: 100,
	skin: new Skin({fill: "blue"}),
	Behavior: class extends Behavior {
		onCreate(content, data) {
			trace(`${data}\n`);		// Prints "Hello" to console
		}
	}
})
application.add(sampleContent);
```

![](../assets/piu/sampleContent1.png)

##### `Content.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Content.prototype` のインスタンスを作成する関数であるコンストラクターを返します。結果の `prototype` プロパティは `Content.prototype` です。結果には `template` 関数も提供されます。

```javascript
let SampleContent = Content.template($ => ({
	height: 100, width: 100,
	skin: new Skin({fill: $.color}),
	Behavior: class extends Behavior {
		onCreate(content, data) {
			trace(`This box is ${data.color}\n`);		// Prints "This box is red" to console
		}
	}
}));
application.add(new SampleContent({color: "red"}, {top: 0, right: 50}));
application.add(new SampleContent({color: "blue"}));
```

![](../assets/piu/sampleContent2.png)

<a id="content-dictionary"></a>
#### Dictionary

| Parameter | Type | Description |
| --- | --- | :--- |
| `active` | `boolean` | `true` の場合、このコンテンツはタッチ可能であり、つまりタッチ イベントがトリガーされます。 |
| `anchor` | `string` | インスタンス化データ内に作成された`content`オブジェクトへの参照であるアンカーを作成します。
| `backgroundTouch ` | `boolean` | `true` の場合、このコンテナはそのコンテンツが受信したすべてのタッチ イベントを受信します。つまり、そのコンテンツの 1 つがタッチされたときにタッチ イベントがトリガーされます。
| `Behavior` | `function` | `Behavior.prototype` のインスタンスを作成する関数。通常は `Behavior` クラスを拡張するクラスです。このコンテンツは、この `behavior` のインスタンスを作成し、その `behavior` パラメータを作成されたインスタンスに設定し、`onCreate` メソッドをトリガーします。
| `bottom` | `number` | このコンテンツの `bottom` 座標（ピクセル単位）（作成されたインスタンスの `coordinates` プロパティで `bottom` を設定）。
| `duration` | `number` |このコンテンツの継続時間（ミリ秒単位）。このコンテンツは、クロックが実行され、その時間が継続時間と等しいときに `onFinished` イベントをトリガーします。
| `exclusiveTouch` | `boolean` | `true` の場合、このコンテンツは常にタッチをキャプチャします。つまり、このコンテンツの `onTouchDown` で `captureTouch` が暗黙的に呼び出されます。`exclusiveTouch` を `true` に設定することは、すべてのタッチ ID の `onTouchDown` イベントに応答して `captureTouch` を呼び出すことと同じです。
| `fraction` | `number` | このコンテンツの割合、つまりその時間と継続時間の比率。
| `height` | `number` | このコンテンツの高さ（ピクセル単位）（作成されたインスタンスの `coordinates` プロパティで `height` を設定）。
| `interval` | `number` | このコンテンツのクロックのティック間の時間、つまり、クロックの実行中にコンテンツの動作の `onTimeChanged` イベントをトリガーする間隔 (ミリ秒数)。
| `left` | `number` | このコンテンツの `left` 座標（ピクセル単位）（作成されたインスタンスの `coordinates` プロパティで `left` を設定）。
| `loop` | `boolean` | `true` の場合、このコンテンツは、その時間が継続時間と等しくなったときにクロックを再開します。
| `multipleTouch` | `boolean` | `true` の場合、このコンテンツは複数のタッチを処理します。
| `name` | `string` | このコンテンツの名前。
| `right` | `number` | このコンテンツの `right` 座標（ピクセル単位）（作成されたインスタンスの `coordinates` プロパティで `right` を設定）。
| `skin` | `skin` | このコンテンツのスキン。
| `Skin` | `function` | `Skin.prototype` のインスタンスを作成する関数。このコンテンツは、この `skin` のインスタンスを作成し、その `skin` パラメータを作成されたインスタンスに設定します。
| `state` | `number` | このコンテンツの状態。このコンテンツのスキンが状態を定義している場合、状態を設定すると、このコンテンツの外観が変わります。
| `style` | `style` | このコンテンツのスタイル。
| `Style` | `function` | `Style.prototype` のインスタンスを作成する関数。このコンテンツは、この `style` のインスタンスを作成し、その `style` パラメータを作成されたインスタンスに設定します。
| `time` | `number` | このコンテンツの時間（ミリ秒単位）。時間を設定すると、このコンテンツは `onTimeChanged` イベントをトリガーします。
| `top` | `number` | このコンテンツの `top` 座標（ピクセル単位）（作成されたインスタンスの `coordinates` プロパティで `top` を設定）。
| `variant` | `number` | このコンテンツのバリアント。このコンテンツのスキンがバリアントを定義している場合、バリアントを設定すると、このコンテンツの外観が変わります。
| `visible` | `boolean` | `true` の場合、このコンテンツは表示されます。
| `width` | `number` | このコンテンツの幅（ピクセル単位）（作成されたインスタンスの `coordinates` プロパティで `width` を設定）。

#### プロトタイプの説明

プロトタイプは `Object.prototype` から継承します。

<a id="content-properties"></a>
##### プロパティ

| Name | Type | Default Value | Read Only | Description |
| --- | --- | --- | --- | :--- |
| `active` | `boolean` | `false` | | `true` の場合、このコンテンツはタッチ可能であり、つまりタッチ イベントがトリガーされます。
| `anchor` | `string` | インスタンス化データ内でこのコンテンツ オブジェクトを参照するプロパティの識別子
| `backgroundTouch ` | `boolean` | `false` | | `true` の場合、このコンテナはそのコンテンツが受信したすべてのタッチ イベントを受信します。つまり、そのコンテンツの 1 つがタッチされたときにタッチ イベントがトリガーされます。
| `behavior` | `object` | `null` |  | このコンテンツの `behavior` オブジェクトまたは `null`。このコンテンツがイベントをトリガーすると、その動作の対応する関数プロパティ（存在する場合）が呼び出されます。
| `bounds` | `object` | | | このコンテンツのグローバル位置とサイズは、ピクセル単位で指定された `x`、`y`、`width`、および `height` 数値プロパティを持つオブジェクトとして表されます。このコンテンツがバインドされていない場合、ゲッターは `undefined` を返し、セッターは無視されます。
| `container` | `object` | |  ✓ | このコンテンツのコンテナ。このコンテンツがバインドされていない場合、つまりコンテナがない場合は `null` になります。
| `coordinates` | `object` | | | このコンテンツの座標。`left`、`width`、`right`、`top`、`height`、または `bottom` 数値プロパティ（ピクセル単位で指定）を持つオブジェクト、またはコンストラクタに座標が渡されなかった場合は空のオブジェクト
| `duration` | `number` | 0 | | このコンテンツの継続時間（ミリ秒単位）。このコンテンツは、クロックが実行され、その時間が継続時間と等しいときに `onFinished` イベントをトリガーします。
| `exclusiveTouch` | `boolean` | `false` | |`true` の場合、このコンテンツは常にタッチをキャプチャします。つまり、このコンテンツの `onTouchDown` で `captureTouch` が暗黙的に呼び出されます。`exclusiveTouch` を `true` に設定することは、すべてのタッチ ID の `onTouchDown` イベントに応答して `captureTouch` を呼び出すことと同じです。
| `fraction` | `number` | `undefined` | | このコンテンツの割合、つまり、その時間とその継続時間の比率です。継続時間が 0 の場合、ゲッターは `undefined` を返し、セッターは無視されます。このコンテンツの割合が設定されると、`onTimeChanged` イベントがトリガーされます。
| `height` | `number` | | | このコンテンツの高さ（ピクセル単位）
| `index` | `number` | | ✓ |コンテナ内のこのコンテンツのインデックス。このコンテンツがバインドされていない場合は -1 になります。
| `interval` | `number` | 1 |  |このコンテンツのクロックのティック間の時間、つまり、クロックの実行中にコンテンツの動作の `onTimeChanged` イベントをトリガーする間隔 (ミリ秒数)。
| `loop` | `boolean` | `false` | | `true` の場合、このコンテンツは、その時間が継続時間と等しくなったときにクロックを再開します。
| `multipleTouch` | `boolean` | `false`| | `true` の場合、このコンテンツは複数のタッチを処理します。
| `name` | `string` | | | このコンテンツの名前。
| `next` | `object` | | ✓ | このコンテンツのコンテナの次の `content` オブジェクト。このコンテンツがこのコンテンツのコンテナの最後の `content` オブジェクトである場合、またはこのコンテンツにコンテナがない場合は `null` になります。
| `offset` | `object` | | | このコンテンツのローカル位置は、ピクセル単位で指定された `x` および `y` 数値プロパティを持つオブジェクトとして表されます。このコンテンツがバインドされていない場合、ゲッターは `undefined` を返し、セッターは無視されます。
| `position` | `object` | | |  このコンテンツのグローバル位置は、ピクセル単位で指定された `x` および `y` 数値プロパティを持つオブジェクトです。このコンテンツがバインドされていない場合、ゲッターは `undefined` を返し、セッターは無視されます。
| `previous` | `object` | | ✓ | このコンテンツのコンテナ内の前の `content` オブジェクト。このコンテンツがこのコンテンツのコンテナの最初の `content` オブジェクトである場合、またはこのコンテンツにコンテナがない場合は `null` になります。
| `running` | `boolean` | | ✓ | `true` の場合、このコンテンツのクロックは実行中です。
| `size` | `object` | | | このコンテンツのサイズは、ピクセル単位で指定された `width` および `height` 数値プロパティを持つオブジェクトとして表されます。
| `skin` | `skin` | `null` | | このコンテンツのスキンまたは `null`。
|`state` | `number` | 0 |  | このコンテンツの状態。このコンテンツのスキンが状態を定義している場合、状態を設定すると、このコンテンツの外観が変わります。
| `style` | `style` | `null` |  | このコンテンツのスタイルまたは `null`
| `time` | `number` | 0 |  |このコンテンツの時間（ミリ秒単位）。時間を設定すると、このコンテンツは `onTimeChanged` イベントをトリガーします。
| `variant` | `number` | 0 |  |このコンテンツのバリアント。このコンテンツのスキンがバリアントを定義している場合、バリアントを設定すると、このコンテンツの外観が変わります。
| `visible` | `boolean` | `true` | |`true` の場合、このコンテンツは表示されます。
| `width` | `number` | |  |このコンテンツの幅（ピクセル単位）。
| `x` | `number` | |  | このコンテンツのグローバル x 位置。このコンテンツがバインドされていない場合、ゲッターは `undefined` を返し、セッターは無視されます。
| `y` | `number` | |  | このコンテンツのグローバル y 位置。このコンテンツがバインドされていない場合、ゲッターは `undefined` を返し、セッターは無視されます。

<a id="content-functions"></a>
##### Functions

**`bubble(id [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `id` | `string` | トリガーするイベントの名前
| `...` | `*` | 0個以上の追加パラメータ

このコンテンツと、包含階層の上位にあるすべての `container` オブジェクトが、`id` の値で指定されたイベントをトリガーします。バブルされたイベントが `true` を返すと、バブリングは停止します。バブルされたイベントの最初のパラメータは、このコンテンツではなく、イベントをトリガーする `container` オブジェクトであることに注意してください。バブルされたイベントの追加パラメータ (ある場合) は、`bubble` 関数の追加パラメータです。

```javascript
let NamedContainer = Container.template($ => ({
	name: $.name, active: true, top: 20, bottom: 20, left: 20, right: 20,
	skin: new Skin({ fill: $.color }),
	Behavior: class extends Behavior {
		printName(container) {
			trace(`${container.name} triggered\n`);
		}
		onTouchEnded(container) {
			trace("\n");
			container.bubble("printName");
		}
	}
}));
let outerContainer = new NamedContainer({ name: "outerContainer", color: "red" });
let middleContainer = new NamedContainer({ name: "middleContainer", color: "blue" });
let innerContainer = new NamedContainer({ name: "innerContainer", color: "black" });
outerContainer.add(middleContainer);
middleContainer.add(innerContainer);
application.add(outerContainer);
```

![](../assets/piu/bubble.gif)

***

**`captureTouch(id, x, y, ticks)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `id` | `number` | タッチの識別子
| `x, y` | `number` | タッチのグローバル位置（ピクセル単位）
| `ticks` | `number` | タッチのグローバルタイム

このコンテンツは `id` という名前のタッチをキャプチャします。つまり、このコンテンツのみが、そのタッチに関連する残りの `onTouchMoved` イベントと `onTouchEnded` イベントをトリガーします。キャプチャされたタッチに関連する他の `content` オブジェクトは、`captureTouch` 関数が呼び出されたときに `onTouchCancelled` イベントをトリガーします。

```javascript
class BlueBehavior extends Behavior {
    onTouchBegan(content, id, x, y, ticks) {
    	trace("Blue touch began\n");
        content.captureTouch(id, x, y, ticks);		// With this line, the redContainer behavior's onTouchCancelled method is called; without this line, its onTouchBegan and onTouchEnded methods would be called
    }
    onTouchEnded(content, id, x, y, ticks) {
    	trace("Blue touch ended\n");
    }
}
let blueCapturingContent = new Content(null, {
    active: true, top: 25, left: 25, height: 50, width: 50,
    skin: new Skin({fill: "blue"}),
    Behavior: BlueBehavior
});

class RedBehavior extends Behavior {
    onTouchBegan(content, id, x, y, ticks) {
        trace("Red touch began\n");
    }
    onTouchCancelled(content) {
    	trace("Red touch cancelled\n");
    }
    onTouchEnded(content, id, x, y, ticks) {
    	trace("Red touch ended\n");
    }
}
let redContainer = new Container(null, {
    active: true, backgroundTouch: true,
    top: 0, left: 0, height: 100, width: 100,
    skin: new Skin({fill: "red"}),
    contents: [
    	blueCapturingContent
    ],
    Behavior: RedBehavior
});

application.add(redContainer);
```

![](../assets/piu/contentCaptureTouch.gif)

***

**`defer(id [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `id` | `string` | トリガーするイベントの名前
| `...` | `*` | 0個以上の追加パラメータ

`defer` 関数は `delegate` 関数に似ています。どちらもこのコンテンツによって `id` の値で指定されたイベントがトリガーされます。違いはタイミングにあります。`delegate` 関数はイベントをすぐに送信しますが、`defer` 関数はイベントをポストします。イベントは、メイン イベント ループの次の反復でトリガーされます。

遅延イベントの最初のパラメータはこのコンテンツです。遅延イベントの追加パラメータ (存在する場合) は、`defer` 関数の追加パラメータです。

***

**`delegate(id [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `id` | `string` | トリガーするイベントの名前
| `...` | `*` | 0個以上の追加パラメータ

このコンテンツにより、`id` の値で指定されたイベントがトリガーされます。委任されたイベントの最初のパラメータはこのコンテンツです。委任されたイベントの追加パラメータ (存在する場合) は、`delegate` 関数の追加パラメータです。

```javascript
let NamedContainer = Container.template($ => ({
	name: $.name, active: true, top: 20, bottom: 20, left: 20, right: 20,
	skin: new Skin({ fill: $.color }),
	Behavior: class extends Behavior {
		printName(container) {
			trace(`${container.name} triggered\n`);
		}
		onTouchEnded(container) {
			trace("\n");
			container.delegate("printName");
		}
	}
}));
let outerContainer = new NamedContainer({ name: "outerContainer", color: "blue" });
let middleContainer = new NamedContainer({ name: "middleContainer", color: "black" });
let innerContainer = new NamedContainer({ name: "innerContainer", color: "red" });
outerContainer.add(middleContainer);
middleContainer.add(innerContainer);
application.add(outerContainer);
```

![](../assets/piu/delegate.gif)

***

**`focus()`**

このコンテンツにフォーカスを当てて、キーボード イベントをトリガーします。一度にフォーカスされるコンテンツ オブジェクトは 1 つだけです。

***

**`distribute(id [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `id` | `string` | イベント名
| `...` | `*` | | 0個以上の追加パラメータ

このコンテナーと、包含階層の下位にあるすべての `content` オブジェクトが、`id` の値で指定されたイベントをトリガーします。トラバーサルの順序は深さ優先です。トリガーされたイベント処理関数の 1 つが `true` を返すと、トラバーサルは停止します。分散イベントの最初のパラメーターは、このコンテナーではなく、イベントをトリガーする `content` オブジェクトであることに注意してください。イベントの追加パラメーター (存在する場合) は、`distribute` 関数の追加パラメーターです。

```javascript
let NamedContainer = Container.template($ => ({
	name: $.name, active: true, top: 20, bottom: 20, left: 20, right: 20,
	skin: new Skin({ fill: $.color }),
	Behavior: class extends Behavior {
		printName(container) {
			trace(`${container.name} triggered\n`);
		}
		onTouchEnded(container) {
			trace("\n");
			container.distribute("printName");
		}
	}
}));
let outerContainer = new NamedContainer({ name: "outerContainer", color: "red" });
let middleContainer = new NamedContainer({ name: "middleContainer", color: "blue" });
let innerContainer = new NamedContainer({ name: "innerContainer", color: "black" });
outerContainer.add(middleContainer);
middleContainer.add(innerContainer);
application.add(outerContainer);
```

![](../assets/piu/distribute.gif)

***

**`hit(x, y)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y` | `number` | テストするグローバル位置（ピクセル単位）

このコンテンツがアクティブで、バインドされていて、位置を含んでいる場合はこのコンテンツを返し、そうでない場合は `undefined` を返します。このコンテンツが `container` インスタンスである場合、コンテンツまたはそれ自体がアクティブで、バインドされていて、位置を含んでいる場合はそのコンテンツの 1 つまたはそれ自体を返し、そうでない場合は `undefined` を返します。

> この関数は、コンテンツが測定され、適合された後にのみ使用する必要があることに注意してください。それ以外の場合は、常に `undefined` が返されます。

```javascript
let sampleContent = new Content(null, {
    active: true, top: 0, left: 0, height: 100, width: 100,
    skin: new Skin({fill: "blue"}),
    Behavior: class extends Behavior {
        onDisplaying(content) {
			let x = content == content.hit(10, 10);		// true
			let y = content.hit(200, 200);			// undefined
        }
    }
});
```

***

**`measure()`**

このコンテンツの[測定サイズ](#measured-size)を、`width` および `height` パラメータを持つオブジェクトとして返します。

Example 1:

```javascript
let sampleContent = new Content(null, {
    top: 0, left: 0, height: 100, width: 100,
    skin: new Skin({fill: "blue"})
});
application.add(sampleContent);

let measuredSize = sampleContent.measure(); 	// {height: 100, width: 100}
let fittedHeight = sampleContent.height;		// 100
let fittedWidth = sampleContent.width;			// 100
```

Example 2:

```javascript
let sampleContent = new Content(null, {
    top: 0, bottom: 0, left: 0, right: 0,
    skin: new Skin({fill: "blue"})
});
application.add(sampleContent);

let measuredSize = sampleContent.measure(); 	// {height: 0, width: 0}
let fittedHeight = sampleContent.height;		// 320 (assuming running on 240x320 screen)
let fittedWidth = sampleContent.width;			// 240 (assuming running on 240x320 screen)
```

***

**`moveBy(x, y)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y` | `number` | このコンテンツを移動する差分（ピクセル単位）

このコンテンツをパラメータで指定されたとおりに移動します。コンテンツの座標によって位置が制約される場合、`moveBy` 関数は対応する水平または垂直の差分を無視します。

```javascript
let unconstrainedContent = new Content(null, {
    top: 0, left: 0, height: 100, width: 100,
    skin: new Skin({fill: "blue"}),
});
application.add(unconstrainedContent);
unconstrainedContent.moveBy(100,100);	// Moves unconstrainedContent 100 pixels in the x and y directions

let constrainedContent = new Content(null, {
    top: 0, left: 0, bottom: 140, right: 220,
    skin: new Skin({fill: "red"}),
});
application.add(constrainedContent);
constrainedContent.moveBy(100,100);	// Does nothing
```

![](../assets/piu/contentMoveBy.png)

***

**`sizeBy(width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `width, height` | `number` | このコンテンツのサイズを決定するための差分（ピクセル単位）

このコンテンツのサイズをパラメータで指定されたとおりに変更します。このコンテンツの座標によってサイズが制限される場合、`sizeBy` 関数は対応する水平または垂直の差分を無視します。

```javascript
let unconstrainedContent = new Content(null, {
    top: 0, left: 0, height: 100, width: 100,
    skin: new Skin({fill: "blue"}),
})
application.add(unconstrainedContent);
unconstrainedContent.sizeBy(100,100);	// Makes unconstrainedContent 100 pixels wider and taller

let constrainedContent = new Content(null, {
    top: 0, left: 0, bottom: 140, right: 220,
    skin: new Skin({fill: "red"}),
})
application.add(constrainedContent);
constrainedContent.sizeBy(100,100);	// Does nothing
```

![](../assets/piu/contentSizeBy.png)

***

**`start()`**

このコンテンツの時計を開始します。

```javascript
class SampleBehavior extends Behavior {
	onDisplaying(content) {
		this.index = 0;
		content.interval = 750;
		content.time = 0;
		content.start();
	}
	onTimeChanged(content) {
		let state = content.state + 1;
		if (state > 3) state = 0;
		content.state = state;
	}
}
let sampleContent = new Content(null, {
	height: 100, width: 100,
	skin: new Skin({fill: ["red", "orange", "yellow", "green"]}),
	Behavior: SampleBehavior
});
application.add(sampleContent);
```

![](../assets/piu/contentStart.gif)

***

**`stop()`**

このコンテンツの時計を停止します。

```javascript
class SampleBehavior extends Behavior {
	onDisplaying(content) {
		this.index = 0;
		content.interval = 750;
		content.time = 0;
		content.start();
	}
	onTimeChanged(content) {
		content.state = !content.state;
	}
	onTouchBegan(content) {
		content.stop();
	}
	onTouchEnded(content) {
		content.start();
	}
}
let sampleContent = new Content(null, {
	active: true, height: 100, width: 100,
	skin: new Skin({ fill: ["blue", "white"]}),
	Behavior: SampleBehavior
});
application.add(sampleContent);
```

![](../assets/piu/contentStop.gif)

<a id="content-events"></a>
#### Events

次の標準イベントは、`content` オブジェクトによってトリガーされます。

**`onCreate(content, data, context)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` | イベントをトリガーした`content`オブジェクト
| `data, context  ` | `object` | 動作を参照または含む`content`オブジェクトのコンストラクタのパラメータ

このイベントは、コンテンツが構築されたときにトリガーされます。

***

**`onDisplaying(content)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` | イベントをトリガーした`content`オブジェクト

このイベントは、指定された `content` オブジェクトが包含階層に追加され、測定および調整された後、ユーザーに表示される前にトリガーされます。これは、座標が計算された後にオブジェクトが受け取る最初のイベントです。

***

**`onFinished(content)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` | イベントをトリガーした`content`オブジェクト

このイベントは、指定された `content` オブジェクトが実行中で、その時間が継続時間と等しいときにトリガーされます。

***

**`onTimeChanged(content)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` |  イベントをトリガーした`content`オブジェクト

このイベントは、指定された `content` オブジェクトの時刻が変更されたときにトリガーされます。

***

**`onTouchBegan(content, id, x, y, ticks)`**

**`onTouchCancelled(content, id)`**

**`onTouchEnded(content, id, x, y, ticks)`**

**`onTouchMoved(content, id, x, y, ticks)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` |  イベントをトリガーした`content`オブジェクト
| `id` | `number` | タッチの識別子
| `x, y` | `number` | イベントのグローバル座標（ピクセル単位）
| `ticks` | `number` | イベントのグローバル時間

これらのイベントは、指定された `content` オブジェクトがアクティブでタッチされたときにトリガーされます。

### Dieオブジェクト

- **Source code:** [`piuDie.c`][7]
- **Relevant Examples:** [cards][24]

`die` オブジェクトは、コンテンツを領域で「ダイカット」して、無効化および更新する領域を最小限に抑えることができる `layout` オブジェクトです。これは、フレームごとにすべての画面ピクセルを更新できない制約のあるデバイスでアニメーションやトランジションを作成する場合に便利です。

`die`オブジェクトは2つの領域を維持する:

- 利用可能な操作が構築する作業領域、
- `die` オブジェクトの内容を切り取るクリップ領域

両方の領域は最初は空です。

#### コンストラクタの説明

##### `Die([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` | このダイスの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。辞書は `content` オブジェクトの場合と同じです。[Contentオブジェクト](#container-object) の [Dictionary](#content-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Die.prototype.` から継承したオブジェクトである `die` インスタンスを返します。

```javascript
let sampleDie = new Die(null, {
    left:0, right:0, top:0, bottom:0,
    Behavior: class extends Behavior {
        onDisplaying(die) {
            die.or(40, 40, die.width-80, die.height-80)
                .cut();
        }
    },
    contents: [
        new Content(null, {
            left:0, right:0, top:0, bottom:0,
            skin: new Skin({ fill: "white" }),
        }),
    ]
});

let sampleScreenWithDie = new Container(null, {
    left:0, right:0, top:0, bottom:0,
    contents: [
    	Content(null, {
    		top: 0, bottom: 120, left: 0, right: 0,
    		skin: new Skin({ fill: "blue" }),
    	}),
    	Content(null, {
    		top: 120, bottom: 0, left: 0, right: 0,
    		skin: new Skin({ fill: "black" }),
    	}),
    	sampleDie
    ]
});
application.add(sampleScreenWithDie);
```

![](../assets/piu/sampleDie1.png)

##### `Die.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Die.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Die.prototype` です。結果には `template` 関数も提供されます。

```javascript
let SampleDie = Die.template($ => ({
    left:0, right:0, top:0, bottom:0,
    Behavior: class extends Behavior {
        onDisplaying(die) {
            die.or(40, 40, die.width-80, die.height-80)
                .cut();
        }
    },
    contents: [
        new Content(null, {
            left:0, right:0, top:0, bottom:0,
            skin: new Skin({ fill: "white" }),
        }),
    ]
}));

let sampleScreenWithDie = new Container(null, {
    left:0, right:0, top:0, bottom:0,
    contents: [
    	Content(null, {
    		top: 0, bottom: 0, left: 0, right: 0,
    		skin: new Skin({ fill: "blue" }),
    	}),
    	Row(null, {
    		top: 0, bottom: 0, left: 0, right: 0,
    		contents: [
    			SampleDie(),
    			SampleDie(),
    		]
    	})
    ]
});
application.add(sampleScreenWithDie);

```

![](../assets/piu/sampleDie2.png)

#### プロトタイプの説明

Prototypeは `Layout.prototype` から継承します。

##### Functions

`layout` オブジェクトの場合と同じです（[Layoutオブジェクト](#layout-object)のセクションの[Functions](#layout-functions)を参照）。さらに:

**`and(x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y, width, height` | `number` | ピクセル単位のローカル矩形

四角形を作業領域と交差させて `this` を返します。

```javascript
let sampleContainer = new Container(null, {
    left:0, right:0, top:0, bottom:0, skin: new Skin({ fill: "blue" }),
    contents: [
        Die($, {
            left:0, right:0, top:0, bottom:0,
            Behavior: class extends Behavior {
                onDisplaying(die) {
                    die.fill()
                    	.and(10, 10, 50, 50)
                    	.cut();
                }
            },
            contents: [
                new Content(null, {
                	left:0, right:0, top:0, bottom:0,
                	skin: new Skin({ fill: "white" }),
                }),
            ]
        }),
    ]
});
application.add(sampleContainer);
```

![](../assets/piu/dieAndOr.png)

***

**`attach(content)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `content` | `content` | アタッチする`content`オブジェクト

コンテンツのコンテナ内の指定された `content` オブジェクトをこの `die` オブジェクトに置き換え、`content` オブジェクトをこの `die` オブジェクトに追加することで、`die` オブジェクトを包含階層にバインドします。

```javascript
let whiteScreen = new Content(null, {
	left:0, right:0, top:0, bottom:0,
	skin: new Skin({ fill: "white" }),
    Behavior: class extends Behavior {
    	onDisplaying(content) {
    		application.insert(blueScreen, content);
    		let die = this.die = new Die();
    		die.attach(content);
    		die.or(0, 0, die.width, die.height/4)
    		die.or(0, die.height/2, die.width, die.height/4)
    			.cut();
    	}
    }
});
let blueScreen = new Content(null, {
	left:0, right:0, top:0, bottom:0,
	skin: new Skin({ fill: "blue" }),
});
application.add(whiteScreen);
```

![](../assets/piu/dieAttach.png)

***

**`cut()`**

作業領域を現在の領域にコピーし、作業領域とクリップ領域の違いのみを無効にします。

***

**`detach()`**

この `die` オブジェクトから最初の `content` オブジェクトを削除し、そのコンテナー内のこの `die` オブジェクトを削除した `content` オブジェクトに置き換えることで、この `die` オブジェクトをコンテンツ階層からアンバインドします。

```
let whiteScreen = new Content(null, {
	active: true, left:0, right:0, top:0, bottom:0,
	skin: new Skin({ fill: "white" }),
	Behavior: class extends Behavior {
    	onTouchBegan(content) {
    		application.add(blueScreen);
    		let die = this.die = new Die();
    		die.attach(blueScreen);
    		die.or(0, 0, die.width, die.height/4)
    		die.or(0, die.height/2, die.width, die.height/4)
    			.cut();
    	}
    	onTouchEnded(content) {
    		let die = this.die;
    		die.detach();
    		application.remove(blueScreen)
    	}
	}
});
let blueScreen = new Content(null, {
	active: true, left:0, right:0, top:0, bottom:0,
	skin: new Skin({ fill: "blue" }),
});
application.add(whiteScreen);
```

![](../assets/piu/layoutDetach.gif)

***

**`empty()`**

作業領域を空にして `this` を返します。

***

**`fill()`**

作業領域をこの `die` オブジェクトの境界に設定し、`this` を返します。

***

**`or(x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y, width, height` | `number` | ピクセル単位のローカル矩形

四角形と作業領域を包括的に結合し、`this` を返します。

```javascript
let sampleContainer = new Container(null, {
    left:0, right:0, top:0, bottom:0, skin: new Skin({ fill: "red" }),
    contents: [
        Die($, {
            left:0, right:0, top:0, bottom:0,
            Behavior: class extends Behavior {
                onDisplaying(die) {
                    die.or(10, 10, 50, 50)
                    die.or(80, 80, 100, 100)
                    	.cut();
                }
            },
            contents: [
                new Content(null, {
                	left:0, right:0, top:0, bottom:0,
                	skin: new Skin({ fill: "white" }),
                }),
            ]
        }),
    ]
});
application.add(sampleContainer);
```

![](../assets/piu/dieOr.png)

***

**`set(x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y, width, height` | `number` | ピクセル単位のローカル矩形

作業領域を四角形に設定し、`this` を返します。

```javascript
let sampleContainer = new Container(null, {
    left:0, right:0, top:0, bottom:0, skin: new Skin({ fill: "blue" }),
    contents: [
        Die($, {
            left:0, right:0, top:0, bottom:0,
            Behavior: class extends Behavior {
                onDisplaying(die) {
                    die.set(50, 50, 220, 140)
                    	.cut();
                }
            },
            contents: [
                new Content(null, {
                	left:0, right:0, top:0, bottom:0,
                	skin: new Skin({ fill: "white" }),
                }),
            ]
        }),
    ]
});
application.add(sampleContainer);
```

![](../assets/piu/dieSet.png)

***

**`sub(x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y, width, height` | `number` | ピクセル単位のローカル矩形


作業領域から四角形を減算し、`this` を返します。

```javascript
let sampleContainer = new Container(null, {
    left:0, right:0, top:0, bottom:0, skin: new Skin({ fill: "blue" }),
    contents: [
        Die($, {
            left:0, right:0, top:0, bottom:0,
            Behavior: class extends Behavior {
                onDisplaying(die) {
                    die.fill()
                    	.sub(0, 0, 50, 50)
                    	.sub(100, 0, 50, 50)
                    	.sub(200, 0, 50, 50)
                    	.cut();
                }
            },
            contents: [
                new Content(null, {
                	left:0, right:0, top:0, bottom:0,
                	skin: new Skin({ fill: "white" }),
                }),
            ]
        }),
    ]
});
application.add(sampleContainer);
```

![](../assets/piu/dieSub.png)

***

**`xor(x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y, width, height` | `number` | ピクセル単位のローカル矩形

作業領域と四角形を排他的に結合し、`this` を返します。

```javascript
let sampleContainer = new Container(null, {
    left:0, right:0, top:0, bottom:0, skin: new Skin({ fill: "blue" }),
    contents: [
        Die($, {
            left:0, right:0, top:0, bottom:0,
            Behavior: class extends Behavior {
                onDisplaying(die) {
                    die.set(0, 0, die.width/2, die.height)
                    	.xor(50, 50, die.width-100, die.height-100)
                    	.cut();
                }
            },
            contents: [
                new Content(null, {
                	left:0, right:0, top:0, bottom:0,
                	skin: new Skin({ fill: "white" }),
                }),
            ]
        }),
    ]
});
application.add(sampleContainer);
```

![](../assets/piu/dieXor.png)

### Imageオブジェクト

- **Source code:** [`piuImage.c`][30]
- **Relevant Examples:** [images][31]

`image` オブジェクトは、画像を表示する `content` オブジェクトです。

画像アセットは、GIF、JPEG、PNG ファイル、または JPEG および PNG ファイルのフォルダーです。[images][31] の例では、サポートされているすべてのタイプの例が示されています。

アセットはマニフェストのリソースで定義する必要があります。JPEG ファイルと PNG ファイルの品質は 1 ～ 100 の値に設定できます。数値が大きいほど品質が高くなります。

```javascript
"resources":{
	"*-image(100)": [
		"$(MODDABLE)/examples/assets/images/screen1",
	],
	"*-image(40)": [
		"$(MODDABLE)/examples/assets/images/screen2",
	]
},
```

フォルダー内の画像は、1 つのアニメーション画像のフレームとして使用されます。フレーム レートはフォルダーの名前によって決まります。たとえば、[サンプル画像フォルダー](../../examples/assets/images) の `fish.15fps` フォルダーは、アプリケーションに 1 秒あたり 15 フレームで実行するように指示します。マニフェストでは、パスから `.15fps` が削除されます。

```javascript
"resources":{
	"*-image": [
		"$(MODDABLE)/examples/assets/images/fish",
	],
}
```

GIF のフレーム レートはファイル自体に設定されており、アプリケーションでは変更できません。

サポートされているすべての画像タイプは、`.cs` 拡張子を持つ単一のリソースに圧縮されます。これらは、アプリケーションのスクリプト コードで参照する必要があるファイルです。たとえば、上記のアセットを使用して `image` オブジェクトを作成するには、アプリケーションはパス `"screen1.cs"`、`"screen2.cs"`、および `"street.cs"` を使用します。

#### コンストラクタの説明

##### `Image([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` | この画像の `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#image-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Image.prototype` から継承したオブジェクトである `image` インスタンスを返します。

```javascript
let sampleImage = new Image(null, ({
	path: "screen1.cs"
}));
application.add(sampleImage);
```

> この例では、[サンプル画像フォルダ](../../examples/assets/images) の `screen1.png` を使用します。

![](../assets/piu/imageSample1.png)

##### `Image.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Image.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Image.prototype` です。結果には `template` 関数も提供されます。

```javascript
class ImageBehavior extends Behavior {
	onDisplaying(image) {
		image.start();
	}
}
let SampleImage = Image.template($ => ({
	path: $, loop: true, Behavior:ImageBehavior,
}));
application.add(new SampleImage("street.cs"));
```

> この例では [the example images folder](../../examples/assets/images)の `street.gif` を使用します。

![](../assets/piu/sampleImage2.gif)

<a id="image-dictionary"></a>
#### Dictionary

`content`オブジェクトの場合と同じです（[Contentオブジェクト](#content-object)のセクションの[Dictionary](#content-dictionary)を参照してください）。さらに:

| Parameter | Type | Description |
| --- | --- | :--- |
| `path` | `string` | 画像ファイルの URL。ファイル URL である必要があります。

#### プロトタイプの説明

プロトタイプは `Content.prototype` から継承します。

##### プロパティ

| Name | Type | Default Value | Read Only | Description |
| --- | --- | --- | --- | :--- |
| `frameCount ` | `number` | | ✓ | この画像に含まれるフレームの総数
| `frameIndex ` | `number` | | | 現在のフレームのインデックス

### Labelオブジェクト

- **Source code:** [`piuLabel.c`][8]
- **Relevant Examples:** [cards][24], [keyboard][20]

`label` オブジェクトは、単一のスタイルで 1 行に文字列をレンダリングする `content` オブジェクトです。文字列が `label` オブジェクトの境界に収まらない場合は切り捨てられます。

#### コンストラクタの説明

##### `Label([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` |このラベルの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#label-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Label.prototype` から継承したオブジェクトである `label` インスタンスを返します。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans", color: "blue" });
let sampleLabel = new Label(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	style: sampleStyle, string: "Hello, World!"
});
application.add(sampleLabel)
```
![](../assets/piu/styleSample1.png)

##### `Label.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Label.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Label.prototype` です。結果には `template` 関数も提供されます。

```javascript
let blueStyle = new Style({ font:"600 28px Open Sans", color: "blue" });
let redStyle = new Style({ font:"600 28px Open Sans", color: "red" });
let SampleLabel = Label.template($ => ({
	style: $.style, string: $.string,
	skin: new Skin({ fill: "white" }),
}));
application.add(new SampleLabel({ string: "Hello, World!", style: blueStyle }, { top: 0, left: 0 }));
application.add(new SampleLabel({ string: "Hello, World!", style: redStyle }, { bottom: 0, right: 0 }));
```

![](../assets/piu/styleSample2.png)

<a id="label-dictionary"></a>
#### Dictionary

`content`オブジェクトの場合と同じです（[Contentオブジェクト](#content-object)のセクションの[Dictionary](#content-dictionary)を参照してください）。さらに:

| Parameter | Type | Description |
| --- | --- | :--- |
| `string` | `string` | このラベルの文字列

#### プロトタイプの説明

プロトタイプは `Content.prototype` から継承します。

##### プロパティ

| Name | Type | Default Value | Read Only | Description |
| --- | --- | --- | --- | :--- |
| `string` | `string` | | | このラベルの文字列

### Layoutオブジェクト

- **Source code:** [`piuLayout.c`][9]
- **Relevant Examples:** N/A

`layout` オブジェクトは、その動作においてその内容の配置とサイズ設定をスクリプトに委任する `container` オブジェクトです。

幅が測定されると、`layout` オブジェクトは `onMeasureHorizo​​ntally` イベントをトリガーし、この動作によって `layout` オブジェクトの測定された幅またはそのコンテンツの座標を変更できます。

高さが測定されると、`layout` オブジェクトは `onMeasureVertically` イベントをトリガーし、動作によって `layout` オブジェクトの測定された高さまたはそのコンテンツの座標を変更できます。

#### コンストラクタの説明

##### `Layout([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` | このレイアウトの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。辞書は `content` オブジェクトの場合と同じです。[Contentオブジェクト](#container-object) の [Dictionary](#content-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Layout.prototype/` から継承したオブジェクトである `layout` インスタンスを返します。

```javascript
let ColoredSquare = Content.template($ => ({
	height: 0, width: 0,
	skin: new Skin({ fill: $ }),
}));

let sampleLayout = new Layout(null, {
	height: application.height, width: application.width,
	skin: new Skin({fill: "black"}),
	contents: [
		new ColoredSquare("red", {top: 0, left: 0}),
		new ColoredSquare("yellow", {top: 0, right: 0}),
		new ColoredSquare("green", {bottom: 0, left: 0}),
		new ColoredSquare("blue", {bottom: 0, right: 0}),
	],
	Behavior: class extends Behavior {
		onMeasureHorizontally(layout, width) {
			let squareWidth = width/2;
			let square = layout.first;
			while (square) {
				square.sizeBy(squareWidth, 0);
				square = square.next;
			}
			return width;
		}
		onMeasureVertically(layout, height) {
			let squareHeight = height/2;
			let square = layout.first;
			while (square) {
				square.sizeBy(0, squareHeight);
				square = square.next;
			}
			return height;
		}
	}
});
application.add(sampleLayout);
```

![](../assets/piu/sampleLayout1.png)

##### `Layout.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Layout.prototype` のインスタンスを作成する関数であるコンストラクターを返します。結果の `prototype` プロパティは `Layout.prototype` です。結果には `template` 関数も提供されます。

```javascript
let ColoredSquare = Content.template($ => ({
	height: 0, width: 0,
	skin: new Skin({ fill: $ }),
}));

let SampleLayout = Layout.template($ => ({
	contents: [
		new ColoredSquare("red", {top: 0, left: 0}),
		new ColoredSquare("yellow", {top: 0, right: 0}),
		new ColoredSquare("green", {bottom: 0, left: 0}),
		new ColoredSquare("blue", {bottom: 0, right: 0}),
	],
	Behavior: class extends Behavior {
		onMeasureHorizontally(layout, width) {
			let squareWidth = width/2;
			let square = layout.first;
			while (square) {
				square.sizeBy(squareWidth, 0);
				square = square.next;
			}
			return width;
		}
		onMeasureVertically(layout, height) {
			let squareHeight = height/2;
			let square = layout.first;
			while (square) {
				square.sizeBy(0, squareHeight);
				square = square.next;
			}
			return height;
		}
	}
}));
application.add(new SampleLayout(null, {top: 0, width: 100, left: 0, height: 100}));
application.add(new SampleLayout(null, {bottom: 0, width: 200, right: 0, height: 100}));
```

![](../assets/piu/sampleLayout2.png)

#### プロトタイプの説明

プロトタイプは `Container.prototype` から継承します。

#### Events

`container` オブジェクトの場合と同じです ([Containerオブジェクト](#container-object) セクションの [Events](#container-events) を参照)。さらに次のようになります。:

**`onFitHorizontally(layout, width)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `layout` | `object` | イベントをトリガーした `layout` オブジェクト
| `width` | `number` | `layout` オブジェクトのフィットした幅（ピクセル単位）

このイベントは、`layout` オブジェクトの [fitted width](#fitted-size) が計算されたときにトリガーされます。これがトリガーされると、behaviorはそのコンテンツの座標を変更できます。`layout` オブジェクトの適合幅をピクセル単位で返します。

***

**`onFitVertically(layout, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `layout` | `object` | イベントをトリガーした `layout` オブジェクト
| `height` | `number` | `layout` オブジェクトのフィットした高さ（ピクセル単位）

このイベントは、`layout` オブジェクトの [fitted height](#fitted-size) が計算されたときにトリガーされます。これがトリガーされると、behaviorはそのコンテンツの座標を変更できます。`layout` オブジェクトの高さをピクセル単位で返します。

***

**`onMeasureHorizontally(layout, width)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `layout` | `object` | イベントをトリガーした `layout` オブジェクト
| `width` | `number` | `layout` オブジェクトの測定された幅（ピクセル単位）

このイベントは、`layout` オブジェクトの [measured width](#measured-size) が計算されたときにトリガーされます。`layout` オブジェクトの測定された幅をピクセル単位で返します。

***

**`onMeasureVertically(layout, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `layout` | `object` | イベントをトリガーした `layout` オブジェクト
| `height` | `number` | `layout` オブジェクトの測定された高さ（ピクセル単位）

このイベントは、`layout` オブジェクトの [measured height](#measured-size) が計算されたときにトリガーされます。`layout` オブジェクトの測定された高さをピクセル単位で返します。

***

### Portオブジェクト

- **Source code:** [`piuPort.c`][10]
- **Relevant Examples:** [countdown][25], [list][26], [spinner][27]

`port` オブジェクトは、単純な Piu グラフィック コマンドを使用して描画する動作で、描画をスクリプトに委任する `content` オブジェクトです。

`port` オブジェクトには、すべての描画に影響するClip Rectangle (最初は `port` オブジェクトの境界) があります。

#### コンストラクタの説明

##### `Port([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` | このコンテンツの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。辞書は `content` オブジェクトの場合と同じです。[Contentオブジェクト](#container-object) の [Dictionary](#content-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Port.prototype` から継承したオブジェクトである `port` インスタンスを返します。

```javascript
let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onDraw(port) {
			port.fillColor("blue", 50, 50, port.width-100, port.height-100);
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/samplePort1.png)

##### `Port.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Port.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Port.prototype` です。結果には `template` 関数も提供されます。

```javascript
let SamplePort = Port.template($ => ({
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onCreate(port, data) {
			this.colors = data;
		}
		onDraw(port) {
			let colors = this.colors;
			let x = 0, y = 0;
			for (let i = 0; i < colors.length; i++) {
				port.fillColor(colors[i], x, y, 75, 75);
				x += 35;
				y += 35;
			}
		}
	}
}));
application.add(new SamplePort(["blue", "red", "black"]));
```

![](../assets/piu/samplePort2.png)

#### プロトタイプの説明

プロトタイプは `Content.prototype` から継承します。

<a id="port-functions"></a>
##### Functions

**`drawContent(x, y, width, height) `**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y, width, height` | `number` | 描画する領域のローカル位置とサイズ（ピクセル単位）

指定された位置にこのポートのスキンを描画します

```javascript
let heartSkin = new Skin({
	texture: new Texture("heart.png"),
	color: "red",
	x: 0, y: 0, width: 60, height: 60,
});

let sampleStyle = new Style({ font:"600 28px Open Sans", color: ["red", "yellow", "green", "blue"] });
let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: heartSkin,
	Behavior: class extends Behavior {
		onDraw(port) {
			let size = 60;
			port.drawContent(0, 0, size, size);
			port.drawContent(port.width-size, 0, size, size);
			port.drawContent(0, port.height-size, size, size);
			port.drawContent(port.width-size, port.height-size, size, size);
		}
	}
});
application.add(samplePort);
```

![](../assets/piu/portDrawContent.png)

***

**`drawLabel(string, x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `string ` | `string ` | 描画する文字列
| `x, y, width, height` | `number` | 描画する領域のローカル位置とサイズ（ピクセル単位）

このポートのスタイルで、`label`インスタンスと同じように文字列を描画します。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans", color: "red" });
let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }), style: sampleStyle,
	Behavior: class extends Behavior {
		onDraw(port) {
			let string = "Hello, World!";
			let size = sampleStyle.measure(string);
			port.drawLabel(string, port.width-size.width, port.height-size.height, size.width, size.height);
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/portDrawLabel.png)

***

**`drawSkin(skin, x, y, width, height [, variant, state])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `skin ` | `skin ` | 描画するスキン
| `x, y, width, height` | `number` | 描画する領域のローカル位置とサイズ（ピクセル単位）
| `variant` | `number` | 描画するスキンのバリアント。指定されたスキンがバリアントを定義している場合、バリアントを設定すると外観が変わります。
| `state` | `number` | 描画するスキンの状態。指定されたスキンが状態を定義している場合、状態を設定すると外観が変わります。

状態、バリアント、位置を指定して、`content` インスタンスと同じようにスキンを描画します。

```javascript
let heartSkin = new Skin({
	texture: new Texture("heart.png"),
	color: ["red", "blue"],
	x: 0, y: 0, width: 60, height: 60,
});

let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	Behavior: class extends Behavior {
		onDraw(port) {
			port.drawSkin(heartSkin, 20, 20, 60, 60, 0, 1);
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/portDrawSkin.png)

***

**`drawString(string, style, color, x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `string` | `string` | 描画する文字列
| `style` | `style` | 文字列を描画するために使用するスタイル
| `color` | `string` | 文字列を描画する色。このドキュメントの[Color](#color)セクションで指定された形式の文字列です。
| `x, y, width, height` | `number` | 描画する領域のローカル位置とサイズ（ピクセル単位）

指定されたスタイル、色、位置を使用して、`label` インスタンスと同じように文字列を描画します。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans"});

let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onDraw(port) {
			port.drawString("Hello, World!", sampleStyle, "blue", 0, 0, port.width, port.height);
		}
	}
});
application.add(samplePort);
```

![](../assets/piu/portDrawString.png)

***

**`drawStyle(string, style, x, y, w, h [, ellipsis, state])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `string` | `string` | 描画する文字列
| `style` | `style` | 文字列を描画するために使用するスタイル
| `x, y, width, height` | `number` | 描画する領域のローカル位置とサイズ（ピクセル単位）
| `ellipsis` | `boolean` | `true` の場合、文字列は指定された境界に収まるように切り捨てられます。 `false` の場合、指定された境界を超えていても文字列全体が描画されます。
| `state` | `number` | 描画するスタイルの状態。指定されたスタイルに複数の塗りつぶし色がある場合は、状態を設定することで使用する色が選択されます。

指定されたスタイル、位置、状態で文字列を描画します。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans", color: ["red", "yellow", "green", "blue"] });
let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	Behavior: class extends Behavior {
		onDraw(port) {
			let string = "Hello, World!";
			let size = sampleStyle.measure(string);
			let w = size.width;
			let h = size.height;
			port.drawStyle(string, sampleStyle, 20, 10, w, h, true, 0);
			port.drawStyle(string, sampleStyle, 20, h+10, w, h, true, 1);
			port.drawStyle(string, sampleStyle, 20, h*2+10, w-10, h, false, 2);	// Not truncated
			port.drawStyle(string, sampleStyle, 20, h*3+10, w-10, h, true, 3);	// Truncated
		}
	}
});
application.add(samplePort);
```

![](../assets/piu/portDrawStyle.png)

***

**`drawTexture(texture, color, x, y, sx, sy, sw, sh)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `texture` | `texture` | 描画するテクスチャ
| `color` | `string` | テクスチャにアルファ ビットマップのみが含まれている場合、ビットマップをカラー化するために `color` プロパティの値が使用されます。このドキュメントの [Color](#color) セクションで指定されている形式の文字列である必要があります。
| `x, y` | `number` | 描画する領域のローカル位置とサイズ（ピクセル単位）
| `sx, sy, sw, sh` | `number` | ソース領域 - ピクセルをコピーする領域の位置とサイズ (ピクセル単位)。デフォルトは画像全体です。 

テクスチャによって参照されるイメージを描画します。

```javascript
let ballTexture = new Texture("balls.png");
let alphaHeartTexture = new Texture("heart.png");

let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onDraw(port) {
			port.drawTexture(ballTexture, "black", 0, 0, 0, 0, 30, 30);
			port.drawTexture(alphaHeartTexture, "red", 0, 30, 0, 0, 60, 60);
			port.drawTexture(alphaHeartTexture, "blue", 0, 90, 0, 0, 60, 60);
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/portDrawTexture.png)

***

**`fillColor(color, x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `color` | `string` | 画像を描画する色。このドキュメントの[Color](#color)セクションで指定された形式の文字列です。
| `x, y, width, height` | `number` | 塗りつぶす領域のローカル位置とサイズ（ピクセル単位）

指定された色で領域を塗りつぶします。

```javascript
let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onDraw(port) {
			port.fillColor("red", 0, 0, port.width/2, port.height);
			port.fillColor("blue", port.width/2, 0, port.width/2, port.height);
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/portFillColor.png)

***

**`fillTexture(texture, color, x, y, width, height, sx, sy, sw, sh)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `texture ` | `texture` | 塗りつぶしに使用する画像
| `x, y, width, height` | `number` | コピー先領域 - ピクセルをコピーする領域のローカル位置とサイズ（ピクセル単位）
| `sx, sy, sw, sh` | `number` | ソース領域 - ピクセルをコピーする領域の位置とサイズ (ピクセル単位)。

領域を画像で塗りつぶします。画像のソース領域が繰り返され、目的の領域がカバーされます。

```javascript
let alphaHeartTexture = new Texture("heart.png");

let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onDraw(port) {
			let w = port.width, h = port.height;
			port.fillTexture(alphaHeartTexture, "red", 0, 0, w, h/2, 0, 0, 60, 60);
			port.fillTexture(alphaHeartTexture, "blue", 0, h/2, w, h/2, 0, 0, 60, 60);
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/portFillTexture.png)

***


**`invalidate([x, y, width, height])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y, width, height` | `number` | 無効化する領域のローカル位置とサイズ（ピクセル単位）

このポートの指定された領域 (領域が指定されていない場合はポート全体) を無効にして、`onDraw` イベントをトリガーします。

```javascript
let samplePort = new Port(null, {
	active: true, top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onCreate(port) {
			this.color = "blue";
		}
		onDraw(port) {
			port.fillColor(this.color, 0, 0, port.width, port.height);
		}
		onTouchEnded(port) {
			this.color = (this.color == "blue")? "red" : "blue";
			port.invalidate();
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/portInvalidate.gif)

***

**`measureString(string, style)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `string` | `string` | 測定する文字列
| `style` | `style` | 文字列を測定するときに使用するスタイル

指定されたスタイルで、`label`インスタンスと同じように文字列を測定します。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans"});
let samplePort = new Port(null, {
	active: true, top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onCreate(port) {
			this.index = 0;
			this.strings = ["H", "He", "Hel", "Hell", "Hello"];
		}
		onDraw(port) {
			let string = this.strings[this.index];
			let stringWidth = port.measureString(string, sampleStyle).width;
			port.drawString(string, sampleStyle, "blue", (port.width-stringWidth)/2, 0, stringWidth, 30);
		}
		onTouchEnded(port) {
			this.index++;
			if (this.index == this.strings.length) this.index = 0;
			port.invalidate();
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/portMeasure.gif)

***

**`popClip()`**

このポートのクリップ矩形スタックから現在のクリップ矩形を復元します

***

**`pushClip([x, y, width, height])`**

| Argument | Type | Description |
| --- | --- | ---
| `x, y, width, height` | `number` | クリップ矩形のローカル位置とサイズ

指定されたクリップ矩形をこのポートのクリップ矩形スタックに保存します

```javascript
let alphaHeartTexture = new Texture("heart.png");
let samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	Behavior: class extends Behavior {
		onDraw(port) {
			port.fillTexture(alphaHeartTexture, "blue", 0, 0, port.width, port.height, 0, 0, 60, 60);
			port.pushClip(0, 0, 200, 100);
			port.fillTexture(alphaHeartTexture, "red", 0, 0, port.width, port.height, 0, 0, 60, 60);
			port.popClip();
		}
	}
})
application.add(samplePort);
```

![](../assets/piu/portPushPopClip.png)

***

<a id="port-events"></a>
#### Events

Same as for `content` object (see [Events](#content-events) in the section [Contentオブジェクト](#content-object)), plus:

`content` オブジェクトの場合と同じです ([Contentオブジェクト](#content-object) セクションの [Events](#content-events) を参照)。さらに次のようになります。

**`onDraw(port, x, y, width, height)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `port` | `port` |イベントをトリガーした`port`オブジェクト
| `x, y, width, height` | `number` | 描画する領域のローカル位置とサイズ（ピクセル単位）

このイベントは、指定された `port` オブジェクトが領域を更新する必要があるとき (無効化されているとき) にトリガーされます。

### Rowオブジェクト

- **Source code:** [`piuRow.c`][11]
- **Relevant Examples:** N/A

`row` オブジェクトは、その内容を水平に配置する `container` オブジェクトです。

#### コンストラクタの説明

##### `Row([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` | このコンテンツの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。辞書は `content` オブジェクトの場合と同じです。[Contentオブジェクト](#container-object) の [Dictionary](#content-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Row.prototype` から継承したオブジェクトである `row` インスタンスを返します。

```javascript
let sampleRow = new Row(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	contents: [
		new ColoredSquare("red"),
		new ColoredSquare("blue"),
		new ColoredSquare("black"),
	]
});
application.add(sampleRow);
```

![](../assets/piu/sampleRow1.png)

##### `Row.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Row.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Row.prototype` です。結果には `template` 関数も提供されます。

```javascript
let ColoredSquare = Content.template($ => ({
	left: 0, right: 0, top: 0, bottom: 0,
	skin: new Skin({ fill: $ })
}));

let SampleRow = Row.template($ => ({
	top: 0, bottom: 0, left: 0, right: 0,
	contents: [
		new ColoredSquare($.firstColor),
		new ColoredSquare($.secondColor),
	],
}));
application.add(new SampleRow({ firstColor:"red", secondColor:"blue" }));
```

![](../assets/piu/sampleRow2.png)

#### プロトタイプの説明

プロトタイプは `Container.prototype` から継承します。

<a id="row-events"></a>
#### Events

`container` オブジェクトの場合と同じです ([Containerオブジェクト](#container-object) セクションの [Events](#container-events) を参照してください)

### Scrollerオブジェクト

- **Source code:** [`piuScroller.c`][12]
- **Relevant Examples:** [scroller][32], [list][26]

`scroller` オブジェクトは、最初の `content` オブジェクトを水平方向および垂直方向にスクロールする `container` オブジェクトです。

#### コンストラクタの説明

##### `Scroller([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` | このコンテンツの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#scroller-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Scroller.prototype` から継承したオブジェクトである `scroller` インスタンスを返します。

```javascript
import { VerticalScrollerBehavior } from "scroller";	// See "scroller.js" from the scroller example

let sampleStyle = new Style({ font:"600 28px Open Sans", color: "white" });
let scrollerSample = new Scroller(null, {
	left: 0, right: 0, top: 0, bottom: 0,
	style: sampleStyle, skin: new Skin({ fill: "blue" }),
	active: true, backgroundTouch: true, clip: true,
	Behavior: VerticalScrollerBehavior,
	contents:[
		Text(null, {
			left: 0, right: 0, top: 0,
			blocks: [
				{spans: [
					"Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
					"Pellentesque a massa et massa rutrum maximus non quis tellus.",
					"Fusce quis eros quis leo sodales vehicula. Praesent pretium massa vel ornare pharetra."
				]}
			]
		}),
	],
});
application.add(scrollerSample);
```

![](../assets/piu/sampleScroller1.gif)

##### `Scroller.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Scroller.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Scroller.prototype` です。結果には `template` 関数も提供されます。

```javascript
import { VerticalScrollerBehavior } from "scroller";	// See "scroller.js" from the scroller example

let sampleStyle = new Style({ font:"600 28px Open Sans", color: "white" });
let ScrollerSample = Scroller.template($ => ({
	left: 0, right: 0, top: 0, bottom: 0, skin: new Skin({ fill: "black" }),
	active: true, backgroundTouch: true, clip: true,
	Behavior: VerticalScrollerBehavior,
	contents:[
		Text(null, {
			left: 0, top: 0, right: 0,
			string: "The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog.",
		}),
	],
}));
let screenWithScrollerSample = new Column(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	style: sampleStyle,
	contents: [
		Text(null, {
			top: 0, height: 40, left: 0, right: 0,
			skin: new Skin({ fill: "blue" }),
			string: "Scroller Example"
		}),
		new ScrollerSample(),
	]
});
application.add(screenWithScrollerSample);
```

![](../assets/piu/scrollerSample2.gif)

<a id="scroller-dictionary"></a>
#### Dictionary

`container` オブジェクトの場合と同じです ([Containerオブジェクト](#container-object) セクションの [Dictionary](#container-dictionary) を参照)。さらに次のようになります。

| Parameter | Type | Definition |
| --- | --- | :--- |
| `loop` | `boolean` | `true`の場合、このスクロールバーは最初の`content`オブジェクトをループします。

#### プロトタイプの説明

プロトタイプは `Container.prototype` から継承します。

##### プロパティ

| Name | Type | Default Value | Read Only | Description |
| --- | --- | --- | --- | :--- |
| `constraint` | `object` | | ✓| このスクローラーの制約されたスクロール オフセット (`x` および `y` 数値プロパティを持つオブジェクト)。このスクローラーが追跡しているときのスクロール オフセットは、制約されたスクロール オフセットとは異なる場合があります。
| `loop` | `boolean` | `false` | | `true` の場合、このコンテンツは、その時間が継続時間と等しくなったときにクロックを再開します。
| `scroll` | `object` | | |  このスクロールバーのスクロール オフセット。ピクセル単位で指定された `x` および `y` 数値プロパティを持つオブジェクトとして表されます。
| `tracking` | `boolean` | `false` | | `true` の場合、このスクローラーは追跡しています。追跡中、スクローラーはスクロール オフセットを制限しません。

##### Functions

**`reveal(bounds)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `bounds` | `object` | 表示する領域のローカル位置とサイズ。ピクセル単位で指定された `x`、`y`、`width`、`height` プロパティを持つオブジェクトとして指定します。

このスクローラーをスクロールして指定された領域を表示します

```javascript
class SampleScrollerBehavior extends Behavior {
	onDisplaying(scroller) {
		let bounds = scroller.bounds;
		bounds.y = scroller.first.last.y;
		scroller.reveal(bounds);
	}
}
let sampleStyle = new Style({ font:"600 28px Open Sans", color: "white" });
let ScrollerItem = Label.template($ => ({
	left: 20, right: 20, bottom: 90, height: 30,
	style: sampleStyle, string: "Item" + $
}));
let scrollerSample = new Scroller(null, {
	left: 0, right: 0, top: 0, bottom: 0,
	skin: new Skin({ fill: "black" }),
	Behavior: SampleScrollerBehavior,
	contents:[
		Column(null, {
			top: 0, left: 0,
			contents: [
				ScrollerItem(1),
				ScrollerItem(2),
				ScrollerItem(3),
				ScrollerItem(4),
				ScrollerItem(5),
			]
		})
	],
});
application.add(scrollerSample);
```

![](../assets/piu/scrollerReveal.png)

***

**`scrollBy(dx, dy)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `dx, dy` | `number` | このスクローラーをスクロールするデルタ（ピクセル単位）

このスクロールバーのスクロールオフセットにデルタを追加します

```javascript
class SampleScrollerBehavior extends Behavior {
	onDisplaying(scroller) {
		this.halfway = scroller.y + scroller.height/2;
	}
	onTouchEnded(scroller, id, x, y, ticks) {
		scroller.scrollBy(0, y-this.halfway);
	}
}

let ColoredSquare = Content.template($ => ({
	left: 20, right: 20, top: 20, height: 80,
	skin: new Skin({ fill: $ })
}));
let scrollerSample = new Scroller(null, {
	left: 0, right: 0, top: 0, bottom: 0,
	skin: new Skin({ fill: "black" }),
	active: true, backgroundTouch: true, clip: true,
	Behavior: SampleScrollerBehavior,
	contents:[
		Column(null, {
			active: true, top: 0, left: 0, right: 0,
			contents: [
				ColoredSquare("red"),
				ColoredSquare("orange"),
				ColoredSquare("yellow"),
				ColoredSquare("green"),
				ColoredSquare("blue"),
				ColoredSquare("purple"),
			]
		})
	],
});
application.add(scrollerSample);
```

![](../assets/piu/scrollBy.gif)

***

**`scrollTo(x, y)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `x, y` | `number` | このスクローラーをスクロールするオフセット（ピクセル単位）

このスクロールバーのスクロールオフセットを変更します

```javascript
let ColoredSquare = Content.template($ => ({
	active: true, top: 20, width: 80, height: 80,
	skin: new Skin({ fill: $ }),
	Behavior: class extends Behavior {
		onDisplaying(content) {
			this.x = content.x - 40;
			this.y = content.y - 40;
		}
		onTouchEnded(content) {
			let scroller = content.container.container;
			scroller.scrollTo(this.x, this.y);
		}
	}
}));
let scrollerSample = new Scroller(null, {
	left: 0, right: 0, top: 0, bottom: 0,
	skin: new Skin({ fill: "black" }),
	contents:[
		Column(null, {
			top: 0, left: 0,
			contents: [
				ColoredSquare("red", {left: 0}),
				ColoredSquare("orange", {left: 40}),
				ColoredSquare("yellow", {left: 80}),
				ColoredSquare("green", {left: 120}),
				ColoredSquare("blue", {left: 160}),
				ColoredSquare("purple", {left: 200}),
			]
		})
	],
});
application.add(scrollerSample);
```

![](../assets/piu/scrollTo.gif)

***

<a id="scroller-events"></a>
#### Events

`container` オブジェクトの場合と同じです ([Containerオブジェクト](#container-object) セクションの [Events](#container-events) を参照)。さらに次のようになります。

##### `onScrolled(scroller)`

| Argument | Type | Description |
| --- | --- | :--- |
| `scroller` | `scroller` | イベントをトリガーした `scroller` オブジェクト

このイベントは、指定された `scroller` オブジェクトがスクロールしたときにトリガーされます。

スクローラーによってトリガーされると、このイベントはスクローラーのすべてのコンテンツによってもトリガーされます。これにより、たとえばスクロールバーの実装が容易になります。

### Skinオブジェクト

- **Source code:** [`piuSkin.c`][13]
- **Relevant Examples:** [balls][18], [cards][24], [drag][19]

`skin` オブジェクトは `content` オブジェクトの外観を定義します。`texture` オブジェクトの一部を使用して `content` オブジェクトを描画または塗りつぶしたり、色を使用して `content` オブジェクトを塗りつぶしたりストロークしたりできます。

#### コンストラクタの説明

##### `Skin([dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#skin-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Skin.prototype` から継承したオブジェクトである `skin` インスタンスを返します。

```javascript
// Texture skin
let ballTexture = new Texture("balls.png");
let ballSkin = new Skin({ texture:ballTexture, x:0, y:0, width:30, height:30, variants:30 });
let ballContent = new Content(null, {
	top: 0, left: 0,
	skin: ballSkin
});
application.add(ballContent);

// Color skin
let borderedSkin = new Skin({ fill: "black", stroke: "blue", borders: { left: 5, right: 5, top: 5, bottom: 5}})
let borderedContent = new Content(null, {
	top: 50, left: 0, height: 50, width: 50,
	skin: borderedSkin
});
application.add(borderedContent);
```

![](../assets/piu/sampleSkin1.png)

##### `Skin.template([dictionary])`

| Arguments | Type | Description
| --- | --- | ---
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#skin-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Skin.prototype` のインスタンスを作成する関数であるコンストラクターを返します。結果の `prototype` プロパティは `Skin.prototype` です。

```javascript
// Texture skin
let ballTexture = new Texture("balls.png");
let BallSkin = Skin.template({ texture:ballTexture, x:0, y:0, width:30, height:30, variants:30 });
let ballContent = new Content(null, {
	top: 0, right: 0, variant: 2,
		Skin: BallSkin		// Note the capital "S" in "Skin"
});
application.add(ballContent);

// Color skin
let BorderedSkin = Skin.template({ fill: "black", stroke: "red", borders: { left: 5, right: 5, top: 5, bottom: 5}})
let borderedContent = new Content(null, {
	top: 50, right: 0, height: 50, width: 50,
	skin: new BorderedSkin()	// Note the lowercase "S" in "Skin"
});
application.add(borderedContent);
```

![](../assets/piu/sampleSkin2.png)

<a id="skin-dictionary"></a>
#### Dictionary

辞書に `texture` または `Texture` プロパティがある場合、コンストラクタはテクスチャ スキンを返します。それ以外の場合は、コンストラクタはカラー スキンを返します。

**Texture skins only**

| Parameter | Type | Description |
| --- | --- | :--- |
| `bottom` | `number` | スキンの下部タイル (作成されたインスタンスの `bottom` パラメータと、作成されたインスタンスの `tiles` プロパティの `bottom` を設定します)
| `color` | `string` or `array` | テクスチャにアルファ ビットマップのみが含まれている場合、ビットマップをカラー化するために `color` プロパティの値が使用されます。このドキュメントの [Color](#color) セクションで指定されている形式の文字列または文字列の配列である必要があります。
| `left` | `number` | スキンの左タイル（作成されたインスタンスの `left` パラメータと、作成されたインスタンスの `tiles` プロパティの `left` を設定します）
| `right` | `number` | スキンの右タイル (作成されたインスタンスの `right` パラメータと、作成されたインスタンスの `tiles` プロパティの `right` を設定します)
| `states` | `number` | このスキンの状態間の垂直オフセット（ピクセル単位）
| `texture` | `texture` | このスキンのテクスチャ
| `Texture` | `function` | `Texture.prototype` のインスタンスを作成する関数。このスキンは、この `texture` のインスタンスを作成し、作成されたインスタンスに `texture` パラメータを設定します。
| `tiles` | `object` |  このスキンのタイルは、ピクセル単位で指定された `left`、`right`、`top`、または `bottom` 数値プロパティを持つオブジェクトです。詳細については、[Tiles](#tiles) を参照してください。
| `top` | `number` | スキンのトップタイル（作成されたインスタンスの `top` パラメータと、作成されたインスタンスの `tiles` プロパティの `top` を設定します）
| `variants` | `number` | このスキンのバリアント間の水平オフセット（ピクセル単位）
| `x, y, width, height` | `number` | 抽出する `texture` オブジェクトの部分 (ピクセル単位) (作成されたインスタンスの `bounds`、`width`、および `height` プロパティを設定します)

**Color skins only**

| Parameter | Type | Description |
| --- | --- | :--- |
| `borders` | `object` | `content` オブジェクトを囲む境界線。ピクセル単位で指定された `left`、`right`、`top`、または `bottom` 数値プロパティを持つオブジェクトです。デフォルトでは境界線はありません。
| `fill` | `string` or `array` | `content` オブジェクトを塗りつぶす色。このドキュメントの [Color](#color) セクションで指定された形式の文字列または文字列の `array` として指定します。
| `stroke` | `string` or `array` | このスキンのストロークの色。このドキュメントの[Color](#color)セクションで指定された形式の文字列または文字列の配列です。

#### プロトタイプの説明

プロトタイプは `Object.prototype` から継承します。

##### プロパティ

`skin` オブジェクトのすべてのプロパティは読み取り専用ですが、コンテンツ オブジェクトのスタイルはいつでも変更できます。

**Texture Skins**

| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| `bottom` | `number` | 0 | スキンの下部タイル
| `color` | `string` | | テクスチャにアルファ ビットマップのみが含まれている場合、ビットマップをカラー化するために `color` プロパティの値が使用されます。このドキュメントの [Color](#color) セクションで指定されている形式の文字列または文字列の配列である必要があります。
| `bounds` | `object` | | `texture` オブジェクトから抽出する部分。`x`、`y`、`width`、`height` の数値プロパティを持つオブジェクトとして、ピクセル単位で指定します。
| `height` | `number` | | このスキンの高さ（ピクセル単位）
| `left` | `number` | 0 | スキンの左タイル
| `right` | `number` | 0 | スキンの右タイル
| `states` | `number` | `undefined` | このスキンの状態間の垂直オフセット（ピクセル単位）
| `texture` | `texture` | | このスキンのテクスチャ
| `tiles` | `object` | `undefined` | このスキンのタイルは、ピクセル単位で指定された `left`、`right`、`top`、`bottom` 数値プロパティを持つオブジェクトとして表されます。
| `top` | `number` | 0 | スキンのトップタイル
| `variants` | `number` | `undefined` | このスキンのバリエーション間の水平オフセット（ピクセル単位）
| `width` | `number` | | このスキンの幅（ピクセル単位）

**Color Skins**

| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| `borders` | `object` | `{left: 0, right: 0, top: 0, bottom: 0}` | このスキンの境界は、ピクセル単位で指定された `left`、`right`、`top`、`bottom` 数値プロパティを持つオブジェクトとして表されます。
| `fill` | `object` | | このスキンの塗りつぶし色は、このドキュメントの [Color](#color) セクションで指定された形式の文字列の `配列` です。<BR><BR>スキンを使用する `content` オブジェクトの `state` プロパティによって配列のインデックスが決まります。`state` が整数でない場合は、周囲の状態の色がブレンドされます。配列ではなく 1 つの文字列として指定された場合は、1 つの項目を持つ配列として扱われます。デフォルトの塗りつぶし色は `transparent` です。
| `stroke` | `object` | |このスキンのストロークの色は、このドキュメントの [Color](#color) セクションで指定された形式の文字列の `配列` です。<BR><BR>スキンを使用する `content` オブジェクトの `state` プロパティによって配列のインデックスが決まります。`state` が整数でない場合は、周囲の状態の色がブレンドされます。配列ではなく 1 つの文字列として指定された場合は、1 つの項目を持つ配列として扱われます。デフォルトのストロークの色は `transparent` です。

***

### Soundオブジェクト

- **Source code:** [`piuSound.c`][37]
- **Relevant Examples:** [sound][38]

`Sound` オブジェクトは、[`AudioOut` class](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/pins/audioout.md) を使用してオーディオを再生します。オーディオ リソースの全体または一部を 1 回または繰り返し再生するために使用できます。`Sound` オブジェクトは、ユーザーの操作に対する音声フィードバックを提供するために短いサウンドを再生するように設計されています。その他のオーディオ再生のニーズには、`AudioOut` クラスを直接使用してください。

#### コンストラクタの説明
再生されるオーディオ リソースごとに、Sound クラスの個別のインスタンスが作成されます。

##### `Sound(dictionary)`

| Argument | Type | Description |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#sound-dictionary) セクションで指定されたパラメータのみが効果を持ち、他のパラメータは無視されます。

`Sound.prototype` から継承したオブジェクトである `sound` インスタンスを返します。

```javascript
let sampleSound = new Sound({ path: "piano.wav" });
```

<a id="sound-dictionary"></a>
#### Dictionary

| Parameter | Type | Description |
| --- | --- | :--- |
| `path` | `string` | オーディオを含むリソースの名前。
| `offset` | `number` | 再生を開始するオーディオのサンプル数。指定しない場合は、最初のサンプルから再生が始まります。
| `size` | `number` | 再生するサンプルの数。指定しない場合は、オーディオ リソースの最後まで再生が続行されます。

#### プロトタイプの説明

プロトタイプは `Object.prototype` から継承します。

##### プロパティ

すべてのプロパティは静的です。`bitsPerSample`、`numChannels`、および `sampleRate` プロパティのデフォルト値は、`AudioOut` モジュールの構成を反映します。

| Name | Type | Default Value | Read Only | Description |
| --- | --- | --- | --- | :--- |
| `bitsPerSample` | `number ` | | ✓ | オーディオ再生のビット数 (8 または 16)。
| `numChannels` | `number ` | | ✓ |  オーディオ再生のチャンネル数。
| `sampleRate` | `number` | | ✓ | オーディオ再生の 1 秒あたりのサンプル数。
| `volume` | `number` | 1 | | 音量。値の範囲は、無音の場合は 0、最大音量の場合は 1 です。

##### Functions

**`static close() `**

再生中のサウンドをすべて停止し、`Sound` オブジェクトによって使用される `AudioOut` インスタンスを閉じます。`Sound` クラスのインスタンスはこの呼び出しによって閉じられたり、その他の影響を受けたりすることはありません。

```javascript
Sound.close();
```

***

**`play([stream, repeat, callback]) `**

| Argument | Type | Description |
| --- | --- | :--- |
| `stream` | `number` | サウンドを再生するストリーム。デフォルトは `0` です。
| `repeat` | `number` | サウンドを繰り返す回数。デフォルトは `1` です。サウンドを無期限に繰り返すには、`repeat` プロパティを `Infinity` に設定します。
| `callback` | `function` | オーディオ リソースの再生が完了した後に呼び出すオプションのコールバック関数。

指定されたストリームでオーディオ サンプルを `repeat` 回再生します。`callback` パラメータが指定されている場合は、オーディオの再生が完了した後に呼び出されます。

```javascript
// Play sampleSound once
sampleSound.play();

// Play sampleSound 5 times
sampleSound.play(0, 5);

// Play sampleSound once and print to the console when finished
sampleSound.play(0, 1, () => {
	trace("Sound finished\n");
});
```

***

### Styleオブジェクト

- **Source code:** [`piuStyle.c`][14]
- **Relevant Examples:** [text][28]

`style` オブジェクトは、`label` オブジェクトと `text` オブジェクト内の文字列の外観を定義します。スタイルは、それを含むオブジェクトのスタイルから継承できます。詳細については、以下の [Cascading Styles](#cascading-styles) セクションを参照してください。

#### コンストラクタの説明

##### `Style(dictionary)`

| Argument | Type | Description |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#style-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Style.prototype` から継承したオブジェクトである `style` インスタンスを返します。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans", color: "blue" });
let sampleLabel = new Label(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	style: sampleStyle, string: "Hello, World!"
});
application.add(sampleLabel)
```

![](../assets/piu/styleSample1.png)

##### `Style.template(dictionary)`

| Argument | Type | Description |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#style-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Style.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Style` です。結果には `template` 関数も提供されます。

```javascript
let RedStyle = Style.template({ font:"600 28px Open Sans", color: "red" });
let BlueStyle = Style.template({ font:"600 28px Open Sans", color: "blue" });
let sampleLabel = new Label(null, {
	top: 0, height:30, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	Style: RedStyle, string: "Hello, World!"	// Note the capital "S" in "Style"
});
let sampleLabel2 = new Label(null, {
	top: 30, height: 30, left: 0, right: 0,
	skin: new Skin({ fill: "white" }),
	style: new BlueStyle, string: "Hello, World!"	// Note the lowercase "s" in "style"
});
application.add(sampleLabel);
application.add(sampleLabel2);
```

![](../assets/piu/sampleStyle2.png)

<a id="style-dictionary"></a>
#### Dictionary

##### Text styles only

| Parameter | Type | Description |
| --- | --- | :--- |
| `bottom` | `number` | このスタイルの下余白 (ピクセル単位) (作成されたインスタンスの `bottom` プロパティと、作成されたインスタンスの `margins` プロパティの `bottom` を設定します)
| `leading` | `number` | このスタイルの行の高さ、または「リーディング」 (ブロックの行間の距離) (ピクセル単位)。0 または指定されていない場合は、自動的に計算されます。行が重なっていても距離を強制するには、負の値を使用します。
| `left` | `number` | このスタイルの左余白 (ピクセル単位) (作成されたインスタンスの `left` プロパティと、作成されたインスタンスの `margins` プロパティの `left` を設定します)
| `right` | `number` | このスタイルの右余白 (ピクセル単位) (作成されたインスタンスの `right` プロパティと、作成されたインスタンスの `margins` プロパティの `right` を設定します)

<!-- TO DO:
| `indentation` | `number` | このスタイルのインデント - ブロックの最初の行のインデント（ピクセル単位）
-->

##### Label styles only

| Parameter | Type | Description |
| --- | --- | :--- |
| `vertical` | `string` | このスタイルの垂直方向の配置は、`top`、`middle`（デフォルト）、`bottom` のいずれかです。

##### Text and Label styles

| Parameter | Type | Description |
| --- | --- | :--- |
| `color` | `string` or `array` | このスタイルの色。このドキュメントの [Color](#color) セクションで指定された形式の文字列または文字列の `array` として表されます
| `font` | `string` | このスタイルのフォント。このドキュメントの [Cascading Styles](#cascading-styles) セクションで指定された形式の文字列として表されます (作成されたインスタンスの `style`、`weight`、`stretch`、`size`、および `family` プロパティを設定します)
| `horizo​​ntal` | `string` | このスタイルの水平方向の配置。`left`、`center` (デフォルト)、`right`、または `justify` として表されます
| `top` | `number` | このスタイルの上余白。ピクセル単位 (作成されたインスタンスの `top` プロパティと、作成されたインスタンスの `margins` プロパティの `top` を設定します)

#### プロトタイプの説明

プロトタイプは `Object.prototype` から継承します。

##### プロパティ

`style` オブジェクトのすべてのプロパティは読み取り専用ですが、コンテンツ オブジェクトのスタイルはいつでも変更できます。

| Name | Type | Default Value | Description |
| --- | --- | --- | --- |
| `bottom` | `number` | 0 | このスタイルの下余白
| `color` | `object` | `undefined` | このスタイルの色 (文字列の配列)。色が指定されていない場合は `undefined`。スタイルを使用する `content` オブジェクトの `state` プロパティによって配列のインデックスが決まります。`state` が整数でない場合は、周囲の状態の色がブレンドされます。
| `family` | `string` | | このスタイルのフォント ファミリ
| `horizo​​ntal` | `string` | | このスタイルの水平方向の配置 (`left`、`center` (デフォルト)、`right`、または `justify`)
| `leading` | `number` | | このスタイルの行の高さ (または「leading」)。ブロックの行間の距離 (ピクセル単位)。`0` または指定されていない場合は、自動的に計算されます
| `left` | `number` | 0 |このスタイルの左余白
| `margins` | `object` | | このスタイルの余白。`left`、`right`、`top`、`bottom` の数値プロパティを持つオブジェクトとして、ピクセル単位で指定します
| `right` | `number` | 0 | このスタイルの右余白
| `size` | `string` | | このスタイルのフォント サイズ (ピクセル単位)。
| `stretch` | `string` | | このスタイルのストレッチ
| `top` | `number` | 0 | このスタイルの上余白
| `vertical` | `string` | | このスタイルの垂直方向の配置。 `top`、`middle` (デフォルト)、または `bottom` として、ピクセル単位で指定します
| `weight` | `number` | | このスタイルのフォントの太さ


<!-- TO DO:
| `indentation` | `number` | | このスタイルのインデント - ブロックの最初の行のインデント（ピクセル単位）
-->

##### Functions

**`measure(string)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `string` | `string` | 計測する文字列

このスタイル（およびこのスタイルが `content` オブジェクトにアタッチされている場合は継承されたスタイル）でレンダリングされた文字列のサイズを計算し、ピクセル単位で指定された `width` および `height` プロパティを持つ `object` として返します。

```javascript
let normalStyle = new Style({ font:"600 28px Open Sans", color: "black" });
let size = normalStyle.measure("Moddable");	// {"width":134,"height":38}
```

***

### Textオブジェクト

- **Source code:** [`piuText.c`][15]
- **Relevant Examples:** [text][28]

`text` オブジェクトは、複数のスタイルで複数行の文字列をレンダリングする `content` オブジェクトです。`text` オブジェクトの文字列を変更する方法は 2 つあります。アプリケーションでは通常、特定の `text` オブジェクトに対してこれらの方法の 1 つだけを使用します。

1. `string` プロパティを設定します。これにより、文字列全体が置き換えられます。
2. `begin` 関数と `end` 関数の呼び出しの間に、ブロックとスパンを使用して文字列を構築します。これにより、文字列に文字が追加されます。この方法では、各スパンに異なるスタイルを設定できるため、最も制御性が向上します。

#### コンストラクタの説明

##### `Text([behaviorData, dictionary])`

| Argument | Type | Description |
| --- | --- | :--- |
| `behaviorData` | `*` | このコンテンツの `behavior` の `onCreate` 関数に渡されるパラメータ。これは、`null` や任意のパラメータを持つ辞書を含む、任意のタイプのオブジェクトにすることができます。
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#text-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Text.prototype` から継承したオブジェクトである `text` インスタンスを返します。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans"});
let redStyle = new Style({ color: "red" });
let blueStyle = new Style({ color: "blue" });

let sampleText = new Text(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({fill: "white"}),
	style: sampleStyle,
	blocks: [
		{ spans: [
			" Lorem ipsum dolor sit amet, ",
			{ style: redStyle, spans: "consectetur adipiscing elit." },
		]},
		{ style: blueStyle, spans: "In dignissim hendrerit ultricies." }
	]
});
application.add(sampleText);
```

![](../assets/piu/sampleText1.png)

##### `Text.template(anonymous)`

| Arguments | Type | Description
| --- | --- | :--- |
| `anonymous` | `function` | 結果が作成するインスタンスを初期化するためのプロパティを持つオブジェクトを返す関数

`Text.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Text.prototype` です。結果には `template` 関数も提供されます。

```javascript
let sampleStyle = new Style({ font:"600 28px Open Sans"});
let redStyle = new Style({ color: "red", horizontal: "left" });
let blueStyle = new Style({ color: "blue", horizontal: "right" });

let SampleText = Text.template($ => ({
	top: 50, bottom: 50, left: 50, right: 50,
	skin: new Skin({fill: "white"}),
	style: $.baseStyle,
	blocks: [
		{ style: redStyle, spans: $.redText },
		{ style: blueStyle, spans: $.blueText }
	]
}));
application.add(new SampleText({ baseStyle: sampleStyle, redText: "Red!", blueText: "Blue!" }));
```

![](../assets/piu/sampleText2.png)

<a id="text-dictionary"></a>
#### Dictionary

`content` オブジェクトの場合と同じです ([Contentオブジェクト](#content-object) セクションの [Dictionary](#content-dictionary) を参照)。さらに次のようになります。

| Parameter | Type | Description |
| --- | --- | :--- |
| `blocks` | `array` | ブロックの配列。ブロックは次のプロパティを持つオブジェクトです:<BR>- `behavior`: オブジェクトまたは `null` (デフォルト)。このテキストがアクティブでブロックに触れると、ブロックはその動作の対応する関数プロパティを呼び出します。<BR>- `style`: `style` インスタンスまたは `null` (デフォルト)。<BR>- `spans`: `string` またはスパンの `array`。<BR><BR>ブロックと同様に、スパンは `behavior`、`style`、および `spans` プロパティを持つオブジェクトです。
| `string` | `string` | このテキストの文字列。この文字列を設定すると、このテキストのスタイルを使用する 1 つのスパンを含む 1 つのブロックが作成されます。     

#### プロトタイプの説明

プロトタイプは `Content.prototype` から継承します。

##### プロパティ

| Name | Type | Read Only | Description |
| --- | --- | --- | --- |
| `blocks` | `array` | | このテキストのブロック
| `string` | `string` | | このテキストの文字列。この文字列を設定すると、このテキストのスタイルを使用する 1 つのスパンを含む 1 つのブロックが作成されます。

##### Functions

> 以下のすべての関数を使用するサンプルコードについては、[text example][28]を参照してください。

**`begin()`**

ブロックとスパンを使用して、このテキストの文字列とスタイルを定義するプロセスを開始します。文字列は空にリセットされます。`begin` 関数は、`beginBlock` または `beginSpan` の前に呼び出す必要があります。文字列とスタイルを定義した後、`end` を呼び出します。

***

**`beginBlock([style, behavior])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `style` | `style` | ブロックのスタイル
| `behavior` | `object` | ブロックの動作

新しいブロックを作成して開きます。`begin` 関数は、対応する `end` なしですでに呼び出されている必要があります。別のブロックがすでに開いていることはできません。

***

**`beginSpan(style [, behavior])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `style` | `style` | スパンのスタイル
| `behavior` | `object` | スパンの動作

新しいスパンを作成して開きます。開いているブロックがなければなりません。別の開いているスパンは存在できません。

***

**`concat(string)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `string` | `string` | 連結する文字列

指定された文字列をこのテキストに連結します。オープンなスパンが必要です。

***

**`end()`**

ブロックとスパンを使用してこのテキストの文字列とスタイルを構築するプロセスを完了します。`begin` の後で、このテキストを測定、フィット、またはレンダリングする前に呼び出す必要があります。

***

**`endBlock()`**

開いているブロックを閉じる

***

**`endSpan()`**

開いているスパンを閉じる

***

<a id="text-events"></a>
#### Events

`content` オブジェクトの場合と同じです ([Contentオブジェクト](#content-object) セクションの [Events](#content-events) を参照してください)。

### Textureオブジェクト

- **Source code:** [`piuTexture.c`][16]
- **Relevant Examples:** [balls][18], [cards][24]

`texture` オブジェクトは、画像ファイルを参照するアセットです。アプリケーションは、`skin` オブジェクトの定義に `texture` オブジェクトを使用します。

#### コンストラクタの説明

##### `Texture(path)`

| Argument | Type | Description |
| --- | --- | :--- |
| `path` | `string` | 画像ファイルの URL。ファイル URL である必要があります。

`Object.prototype` から継承したオブジェクトである `texture` インスタンスを返します。

```javascript
const logoTexture = new Texture("logo.png");
const logoSkin = new Skin({ texture: logoTexture, x: 0, y: 0, width: 100, height: 20 });
```

##### `Texture(dictionary)`

| Argument | Type | Description |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#texture-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Object.prototype` から継承したオブジェクトである `texture` インスタンスを返します。

```javascript
const logoTexture = new Texture({ path: "logo.png" });
const logoSkin = new Skin({ texture: logoTexture, x: 0, y: 0, width: 100, height: 20 });
```

##### `Texture.template(dictionary)`

| Argument | Type | Description |
| --- | --- | :--- |
| `dictionary` | `object` | 結果を初期化するプロパティを持つオブジェクト。以下の [Dictionary](#texture-dictionary) セクションで指定されたパラメータのみが有効になり、他のパラメータは無視されます。

`Texture.prototype` のインスタンスを作成する関数であるコンストラクタを返します。結果の `prototype` プロパティは `Texture.prototype` です。

```javascript
const LogoTexture = Texture.template({ path: "logo.png" });
// Note the capital "T" in "Texture" below
const LogoSkin = Skin.template({ Texture: LogoTexture, x: 0, y: 0, width: 100, height: 20, color: "white" });

const AnotherLogoTexture = Texture.template({ path: "logo.png" });
// Note the lowercase "t" in "texture" below
const AnotherLogoSkin = Skin.template({ texture: new AnotherLogoTexture(), x: 0, y: 0, width: 100, height: 20, color: "white" });

```

<a id="texture-dictionary"></a>
#### Dictionary

| Parameter | Type | Description |
| --- | --- | :--- |
| `path` | `string` | 画像ファイルの URL。ファイル URL である必要があります。

#### プロトタイプの説明

プロトタイプは `Object.prototype` から継承します。

##### プロパティ

| Name | Type | Read Only | Description |
| --- | --- | --- | :--- |
| `height` | `number` | ✓ | このテクスチャの高さ（物理ピクセル単位）          
| `width` | `number` | ✓ | このテクスチャの幅（物理ピクセル単位）

### Timelineオブジェクト

- **Source code:** [`piuTimeline.js`][36]
- **Relevant Examples:** [timeline][35], [easing-equations][34]

`timeline` オブジェクトは、一連のトゥイーンの順序付けと実行を行うメカニズムを提供します。これは、Piu 画面と他のアニメーション間の遷移を管理するのに役立ちます。

トゥイーンは、ターゲット オブジェクトと、プロパティと値のリストを指定するオブジェクトを取り込むことで機能します。トゥイーンが特定の割合に設定されると、ターゲット オブジェクトの適切なプロパティが、初期値と宛先値の間の値で更新されます。この動作の詳細は、トゥイーンのタイプ (以下で説明するように、「on」、「from」、または「to」) と、トゥイーンに使用されるイージング関数によって決まります。

アプリケーションは、タイムライン内の特定のポイントをシークすることで、タイムライン内のすべてのトゥイーンを一度に管理します。これは通常、`content` オブジェクトの `onTimeChanged` 関数によって実行されます。

Piu Timeline の実装は、GreenSock によって開発された [TimelineLite class developed by GreenSock](https://greensock.com/timelinelite) に基づいています。

`Timeline` クラスを使用するには、アプリケーションにインポートする必要があることに注意してください。

```javascript
import Timeline from "piu/Timeline";`
```

#### コンストラクタの説明

##### `Timeline()`

`Timeline` クラスのインスタンスであるタイムライン オブジェクトを返します。

```javascript
let timeline = new Timeline();
```

#### プロトタイプの説明

##### プロパティ

| Name | Type | Default Value | Read Only | Description |
| --- | --- | --- | --- | --- |
| `duration` | `number` | | | このタイムラインが開始してから完了するまでにかかる時間 (ミリ秒)。
| `fraction` | `number` | | | このタイムラインの完了した割合を 0 から 1 までの小数で表します。割合を設定することは、`seekTo(fraction *duration)` を呼び出すことと同じです。
| `time` | `number` | | | タイムラインが完了した時間（ミリ秒単位）。これを直接変更しないでください。代わりに `seekTo(time)` を使用してください。

##### Functions

<!-- The add function doesn't seem possible to use right now, because the Tween sub-class is not exported. Any reason to describe this in the docs?

**`add(tween, when)`** -->

**`from(target, fromProperties, duration, [easing, delay])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `target` | `object` | タイムラインによってプロパティがトゥイーンされるオブジェクト。
| `fromProperties` | `object` | このオブジェクトのキーは、タイムラインによってトゥイーンされる `target` オブジェクトのプロパティです。その値は、トゥイーンの開始時に `target` オブジェクトのプロパティが持つ開始値です。
| `duration` | `number` | トゥイーンの継続時間（ミリ秒）。
| `easing` | `number` | トゥイーンに使用するイージング関数。`null` が指定された場合、トゥイーンでは線形イージング関数が使用されます。
| `delay` | `number` | タイムラインの前のトゥイーンが完了してから、このトゥイーンを開始するまでのミリ秒数。この数値が負の場合、前のトゥイーンが完了する前にトゥイーンが開始されます。期間が指定されていない場合、前のトゥイーンの完了後すぐにトゥイーンが開始されます。

タイムラインに「from トゥイーン」を追加します。「from トゥイーン」は、`target` オブジェクトのプロパティを、`toProperties` オブジェクトで指定された値から `target` オブジェクトの元の値に `duration` ミリ秒かけて変更します。

このタイムラインを返します。複数の `to`、`from`、および `on` 呼び出しを連鎖させるのに役立ちます。

```javascript
let sampleStyle = new Style({ font: "600 28px Open Sans", color: ["blue", "white"] });
let sampleColumn = new Column(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }), style: sampleStyle,
	contents: [
		Label(null, { top: 90, height: 28, left: 80, string: "Hello", state: 0 }),
		Label(null, { top: 2, height: 28, left: 80, string: "Moddable", state: 0 }),
	],
	Behavior: class extends Behavior {
		onDisplaying(column) {
			let timeline = this.timeline = new Timeline();
			timeline.from(column.first, { x: -column.first.width, state: 1 }, 1000, Math.quadEaseOut, 0)
				.from(column.last, { x: application.width, state: 1 }, 800, Math.quadEaseOut, -800);
			column.duration = timeline.duration;
			timeline.seekTo(0);
			column.time = 0;
			column.start();
		}
		onTimeChanged(column) {
			this.timeline.seekTo(column.time);
		}
	}
});
application.add(sampleColumn);
```

![](../assets/piu/timelineFrom.gif)

**`on(target, onProperties, duration, easing, delay, when)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `target` | `object` | タイムラインによってプロパティがトゥイーンされるオブジェクト。
| `onProperties` | `object` | このオブジェクトのキーは、タイムラインによってトゥイーンされる `target` オブジェクトのプロパティです。その値は、タイムラインの継続時間中にトゥイーンが遷移する値の配列です。
| `duration` | `number` | トゥイーンの継続時間（ミリ秒）。
| `easing` | `number` | トゥイーンに使用するイージング関数。`null` が指定された場合、トゥイーンでは線形イージング関数が使用されます。
| `delay` | `number` | タイムラインの前のトゥイーンが完了してから、このトゥイーンを開始するまでのミリ秒数。この数値が負の場合、前のトゥイーンが完了する前にトゥイーンが開始されます。期間が指定されていない場合、前のトゥイーンの完了後すぐにトゥイーンが開始されます。

タイムラインに「on トゥイーン」を追加します。「on トゥイーンは、`onProperties` オブジェクト内の配列で指定された一連のステップを通じて、`duration` ミリ秒にわたって `target` オブジェクトの値をイージングします。

このタイムラインを返します。複数の `to`、`from`、および `on` 呼び出しを連鎖させるのに役立ちます。

```javascript
let sampleContainer = new Container(null, {
	top: 0, bottom: 0, left: 0, right: 0, skin: new Skin({ fill: "white" }),
	contents: [
		Content(null, { top: 0, height: 50, left: 0, width: 50, skin: new Skin({ fill: ["blue", "red"] }) })
	],
	Behavior: class extends Behavior {
		onDisplaying(container) {
			this.startAnimation(container);
		}
		startAnimation(container) {
			let timeline = this.timeline = new Timeline();
			let l=0, r=150, b=150, t=0;
			timeline.on(container.first, { x: [l, r, r, l, l], y: [t, t, b, b, t], state: [0, 1] }, 1000, Math.quadEaseInOut, 500);
			container.duration = timeline.duration;
			timeline.seekTo(0);
			container.time = 0;
			container.start();
		}
		onTimeChanged(container) {
			let time = container.time;
			if (this.reverse) time = container.duration - time;
			this.timeline.seekTo(time);
		}
		onFinished(container) {
			this.reverse = !this.reverse;
			this.startAnimation(container);
		}
	}
});
application.add(sampleContainer);
```

![](../assets/piu/timelineOn.gif)

**`seekTo(time)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `time` | `number` |タイムライン内でシークする位置。

このタイムラインのトゥイーンを指定された時間にジャンプさせます。これにより、トゥイーンのターゲット オブジェクトのプロパティが設定されます。

**`to(target, toProperties, duration, [easing, delay])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `target` | `object` | タイムラインによってプロパティがトゥイーンされるオブジェクト。
| `toProperties` | `object` | このオブジェクトのキーは、タイムラインによってトゥイーンされる `target` オブジェクトのプロパティです。その値は、トゥイーンが完了したときに `target` オブジェクトのプロパティが持つ宛先値です。
| `duration` | `number` | トゥイーンの継続時間（ミリ秒）。
| `easing` | `number` | トゥイーンに使用するイージング関数。`null` が指定された場合、トゥイーンでは線形イージング関数が使用されます。
| `delay` | `number` | タイムラインの前のトゥイーンが完了してから、このトゥイーンを開始するまでのミリ秒数。この数値が負の場合、前のトゥイーンが完了する前にトゥイーンが開始されます。期間が指定されていない場合、前のトゥイーンの完了後すぐにトゥイーンが開始されます。

タイムラインに「to トゥイーン」を追加します。「to トゥイーン」は、`target` オブジェクトのプロパティを、`duration` ミリ秒にわたって現在の値から `toProperties` オブジェクトで指定された目的の値にイージングします。

このタイムラインを返します。複数の `to`、`from`、および `on` 呼び出しを連鎖させるのに役立ちます。

```javascript
let sampleStyle = new Style({ font: "600 28px Open Sans", color: ["blue", "white"] });
let sampleColumn = new Column(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: "white" }), style: sampleStyle,
	contents: [
		Label(null, { top: 90, height: 28, left: 80, string: "Hello", state: 0 }),
		Label(null, { top: 2, height: 28, left: 80, string: "Moddable", state: 0 }),
	],
	Behavior: class extends Behavior {
		onDisplaying(column) {
			let timeline = this.timeline = new Timeline();
			timeline.to(column.first, { x: -column.first.width, state: 1 }, 1000, Math.quadEaseOut, 1000)
				.to(column.last, { x: application.width, state: 1 }, 800, Math.quadEaseOut, -800);
			column.duration = timeline.duration;
			timeline.seekTo(0);
			column.time = 0;
			column.start();
		}
		onTimeChanged(column) {
			this.timeline.seekTo(column.time);
		}
	}
});
application.add(sampleColumn);
```

![](../assets/piu/timelineTo.gif)

### Transitionオブジェクト

- **Source code:** [`piuTransition.c`][17]
- **Relevant Examples:** N/A

`transition` オブジェクトは、包含階層の変更をアニメーション化するために使用されます。

#### コンストラクタの説明

アプリケーションは、`Transition.prototype` から継承し、`onBegin`、`onEnd`、および `onStep` 関数をオーバーライドする `transition` オブジェクトの独自のコンストラクターを定義します。

```javascript
class BasicLeftToRightTransition extends Transition {
	constructor(duration) {
		super(duration);
	}
	onBegin(container, current, next) {
		container.add(next);
		this.die = new Die();
		this.die.attach(next);
		this.width = container.width;
		this.height = container.height;
	}
	onStep(fraction) {
		fraction = Math.quadEaseOut(fraction);
		let left = 0, right =  Math.round(this.width * fraction);
		this.die.set(left, 0, right - left, this.height);
		this.die.cut();
	}
	onEnd(container, current, next) {
		this.die.detach();
		container.remove(current);
	}
}
```

`Container` オブジェクトは、`transition` オブジェクトのインスタンスを `run` 関数のパラメータとして使用します。

```javascript
class SwitchScreenBehavior extends Behavior {
	onCreate(content, data) {
		this.data = data;
	}
	onTouchEnded(content) {
		let data = this.data;
		let transition = new BasicLeftToRightTransition(1000);
		let nextScreen = new ColoredScreen({ color: data.nextColor, nextColor: data.color })
		application.run(transition, application.first, nextScreen);
	}
}
let ColoredScreen = Container.template($ => ({
	top: 0, bottom: 0, left: 0, right: 0,
	skin: new Skin({ fill: $.color }),
	contents: [
		Content($, {
			active: true, height: 100, width: 100,
			skin: new Skin({ fill: $.nextColor }),
			Behavior: SwitchScreenBehavior
		})
	]
}));
application.add(new ColoredScreen({ color: "red", nextColor: "blue" }));
```

![](../assets/piu/transition.gif)

#### プロトタイプの説明

プロトタイプは `Object.prototype` から継承します。

##### プロパティ

| Name | Type | Default Value | Read Only | Description |
| --- | --- | --- | --- | :--- |
| `duration ` | `number` | 250 |  | 遷移の継続時間（ミリ秒単位）

##### Functions

**`onBegin(container [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `container` | `container` | 遷移を実行している`container`オブジェクト
| `...` | `*` | 0個以上の追加パラメータ

この遷移が開始するときに呼び出されます。追加パラメータは、この遷移を指定された `container` オブジェクトにバインドする `run` 関数の呼び出しの追加パラメータです。

***

**`onEnd(container [, ...])`**

| Argument | Type | Description |
| --- | --- | :--- |
| `container` | `container` | 遷移を実行している`container`オブジェクト
| `...` | `*` | 0個以上の追加パラメータ

この遷移が終了したときに呼び出されます。追加パラメータは、この遷移を指定された `container` オブジェクトにバインドする `run` 関数の呼び出しの追加パラメータです。

***

**`onStep(fraction)`**

| Argument | Type | Description |
| --- | --- | :--- |
| `fraction` | `number` | 経過時間とこの遷移の継続時間の比率（0 から 1 まで（両端を含む）の数値）

この遷移の実行中に呼び出されます。少なくとも 2 回 (`fraction` パラメータは 0 と 1)、最大でディスプレイのリフレッシュ レート (たとえば 1 秒あたり 60 回) で呼び出されます。

<!-- Referenced Links -->
[0]: ../../modules/piu/All/piuAll.js "piuAll.js"
[1]: ../../xs/sources/xsGlobal.c "xsGlobal.c"
[2]: ../../modules/piu/All/piuApplication.c "piuApplication.c"
[3]: ../../modules/piu/All/piuBehavior.c "piuBehavior.c"
[4]: ../../modules/piu/All/piuColumn.c "piuColumn.c"
[5]: ../../modules/piu/All/piuContainer.c "piuContainer.c"
[6]: ../../modules/piu/All/piuContent.c "piuContent.c"
[7]: ../../modules/piu/MC/piuDie.c "piuDie.c"
[8]: ../../modules/piu/All/piuLabel.c "piuLabel.c"
[9]: ../../modules/piu/All/piuLayout.c "piuLayout.c"
[10]: ../../modules/piu/All/piuPort.c "piuPort.c"
[11]: ../../modules/piu/All/piuRow.c "piuRow.c"
[12]: ../../modules/piu/All/piuScroller.c "piuScroller.c"
[13]: ../../modules/piu/All/piuSkin.c "piuSkin.c"
[14]: ../../modules/piu/All/piuStyle.c "piuStyle.c"
[15]: ../../modules/piu/All/piuText.c "piuText.c"
[16]: ../../modules/piu/MC/piuTexture.c "piuTexture.c"
[17]: ../../modules/piu/All/piuTransition.c "piuTransition.c"
[30]: ../../modules/piu/MC/colorcell/piuImage.c "piuImage.c"
[33]: ../../modules/piu/All/piuTransition.c "piuTransition.c"
[36]: ../../modules/piu/All/piuTimeline.js "piuTimeline.js"
[37]: ../../modules/piu/MC/piuSound.c "piuSound.c"

[18]: ../../examples/piu/balls "balls"
[19]: ../../examples/piu/drag "drag"
[20]: ../../examples/piu/keyboard "keyboard"
[21]: ../../examples/piu/weather "weather"
[22]: ../../examples/piu/love-js "love-js"
[23]: ../../examples/piu/transitions "transitions"
[24]: ../../examples/piu/cards "cards"
[25]: ../../examples/piu/countdown "countdown"
[26]: ../../examples/piu/list "list"
[27]: ../../examples/piu/spinner "spinner"
[28]: ../../examples/piu/text "text"
[31]: ../../examples/piu/images "images"
[32]: ../../examples/piu/scroller "scroller"
[34]: ../../examples/piu/easing-equations "easing-equations"
[35]: ../../examples/piu/timeline "timeline"
[38]: ../../examples/piu/sound "sound"

[29]: ./PiuContainmentHierarchy.md "Piu Containment Hierarchy"



