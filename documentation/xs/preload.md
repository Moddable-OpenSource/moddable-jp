# XSプリロードを使用してアプリケーションを最適化する
Copyright 2019-2023 Moddable Tech, Inc.<BR>
改訂： 2024年10月1日

モジュールのプリロードは、XS JavaScriptエンジンのユニークな機能です。プリロードは、アプリケーションが対象デバイスにダウンロードされる前のビルドプロセス中に、JavaScriptアプリケーションの一部を実行します。これには2つの主な利点があります：

- ビルド中に初期化を実行することで、ターゲットマイクロコントローラ上でアプリケーションが起動するたびに実行する必要がなくなり、アプリケーションの起動速度が向上します。

- プリロード中に作成されたオブジェクトのメモリをRAMからフラッシュ（ROM）に移動することで、スクリプトが使用できるメモリが増加します。

すべてのモジュールをプリロードすることはできません。なぜなら、ビルドマシンで実行できる操作には限りがあるからです。例えば、デジタルピンの初期化やWi-Fiネットワークへの接続などです。Moddable SDKのほとんどのモジュールは、プリロードをサポートするように設計および実装されています。このドキュメントでは、XSのプリロード機能について詳しく説明し、プロジェクトでの使用方法と独自のモジュールへの適用方法について説明します。

## モジュールのプリロードを指定する
プロジェクトのビルドマニフェスト（通常は `manifest.json` という名前のファイル）は、[多くの他のオプション](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/tools/manifest.md)と共にインクルードするモジュールをリストアップします。マニフェストの任意の部分には、プリロードモジュールのリストがあります。

```json
"modules": {
	"*": [
		"./main",
		"$(MODULES)/network/http/*"
	]
},
"preload": [
	"http"
]
```

この例では、`http` ネットワークプロトコルモジュールがプリロードされますが、`main` モジュールはプリロードされません。便宜上、Moddable SDKのほとんどの例では `main` モジュールをプリロードしていませんが、少し手を加えれば可能です。その方法の詳細は以下に記述されています。

## モジュールの実行
モジュールを実行するとはどういうことかを理解するために、簡単なモジュールを見てみましょう：

```js
class CountingLog {
	constructor() {
		this.count = 1;
	}
	log(msg) {
		trace(`${this.count++}: ${msg}\n`);
	}
}

export default CountingLog;
```

このモジュールにはクラス定義のみが含まれているため、事前読み込みから得られる利点がないように思えるかもしれません。JavaScriptでは、クラスはコンパイル時ではなく、クラスステートメントが実行されるときに動的に構築されます。JavaScriptコンパイラは `CountingLog` クラスを構築するためのバイトコードを出力します。そのバイトコードはモジュールがロードされたときに実行されます。クラスとその各メソッドの作成はバイトコードの実行（時間がかかる）とオブジェクトの割り当て（メモリを消費する）を行います。`CountingLog` モジュールをプリロードすることで、時間とメモリの両方が節約されます。

こちらは別の例のモジュールで、`CountingLog`をインポートし、各ログ行の先頭に現在の日付と時刻を印刷するように拡張しています。

```js
import CountingLog from "countinglog";

class CountingDateLog extends CountingLog {
	log(msg) {
		trace(`${new Date} `);
		super.log(msg);
	}
}

export default CountingDateLog;
```

`CountingDateLog`が`CountingLog`をインポートするとき、インポートは解決されるため、時間がかかり、インポートを追跡するためにメモリを使用します。`CountingDateLog`をプリロードすることで、インポート文はビルド時に実行され、このメモリをRAMではなくフラッシュメモリに保持することができます。

`CountingDateLog`がプリロードされると、マニフェストの`preload`配列に含まれているかどうかに関わらず、`CountingLog`もプリロードされます。したがって、モジュールをプリロードする場合、プリロードするすべてのモジュールもプリロードをサポートする必要があります。

## フリージング
JavaScript言語は、オブジェクトをいつでも変更できるように許可しています。プリロードはオブジェクトをフラッシュメモリに置き、実用上、フラッシュメモリは読み取り専用メモリです。XSエンジンは、ROM内のオブジェクトが変更される可能性がある場合、変更をRAMに保存することで、フラッシュに保存されたオブジェクトを変更できるようにします。これは、変更可能な各オブジェクトに対してRAM内にポインタを維持することで達成されます。典型的な32ビットマイクロコントローラでは、各ポインタは4バイトを占めます。

### クラスのフリーズ
XSはフラッシュに保存されたオブジェクトを変更できるため、以下の例のようにクラスのプロトタイプを変更するコードが機能します。

```js
import CountingLog from "countinglog";

CountingLog.prototype.reset = function() {
	this.count = 0;
}
```

上記のモジュールがプリロードされていないと仮定すると、`reset` プロパティとそれが参照する関数オブジェクトはRAMに保存されます。場合によっては、そのような振る舞いが望ましいこともあります。しかし、その振る舞いを可能にするためには、XSは `CountingLog` がパッチを適用できるようにするためにRAMに4バイトのポインタを予約する必要があります。多くの場合、モジュールの開発者はこのようなオブジェクトの変更を望まないことが多いです。なぜなら、それによって信頼性の問題やセキュリティ問題が発生する可能性があるからです。

JavaScript言語は、既存のプロパティの変更を防ぎ、新しいプロパティの追加を防ぐために `Object.freeze` 関数を提供しています。ここでは、そのプロトタイプに `Object.freeze` を使用して修正された `CountingLog` モジュールを示します。

```js
class CountingLog {
	constructor() {
		this.count = 1;
	}
	log(msg) {
		trace(`${this.count++}: ${msg}\n`);
	}
}
Object.freeze(CountingLog.prototype);

export default CountingLog;
```

この変更により、プロトタイプに `reset` 関数を追加するための上記のコードは例外を投げます。`CountingDateLog` の例は、`CountingLog` をサブクラス化して `CountingLog.prototype` を参照しますが、それを変更することはありませんので、引き続き機能します。

XSエンジンは、フリーズされたオブジェクトは変更できないと認識しているため、それらに対して4バイトのポインタを割り当てません。これにより、ランタイムのRAM使用量が削減されます。4バイトは小さな利点のように思えるかもしれませんが、数十KBのRAMしかないデバイスで多くのクラスがあるプロジェクトでは、その影響は意味があります。

### データのフリーズ
JavaScriptアプリケーションは、データを保存するためにオブジェクトをよく使用します。ここには、電球で実行されるスクリプトからの例があります。

```js
const Colors = [
	{name: "blue", value: 0x0000FF},
	{name: "white", value: 0xFFFF00},
	{name: "red", value: 0xFF0000},
	{name: "green", value: 0x00FF00},
	{name: "purple", value: 0xFF00FF},
	{name: "yellow", value: 0xFFFF00},
	{name: "cyan", value: 0x00FFFF},
];
```

`Colors`オブジェクトは7つのエントリを持つ`Array`です。XSは上記のように`Colors`への変更を追跡するためのポインタを予約します。`Array`を凍結すると、そのポインタが削除されます：

```js
Object.freeze(Colors);
```

しかし、配列には7つのオブジェクトが含まれており、それぞれに対してもポインタを予約する必要があり、追加で28バイトが必要です。これらのオブジェクトも凍結する必要があります。こちらがその方法の1つです。

```js
const Colors = [
	Object.freeze({name: "blue", value: 0x0000FF}),
	Object.freeze({white", value: 0xFFFF00}),
	*..*.
```

残念ながら、それはデータを不明瞭にします。こちらが別のアプローチです：

```js
const Colors = [
	{name: "blue", value: 0x0000FF},
	{name: "white", value: 0xFFFF00},
	...
];
Object.freeze(Colors);
Colors.forEach(color => Object.freeze(color));
```

これはより良いですが、コード量が多くなります。さらに、これらのオブジェクトが自分のサブオブジェクトを持っていた場合、追加のコードが必要になります。

### ディープフリーズ
オブジェクトをフリーズすることは、他のJavaScript環境よりもXSでよく行われます。開発者がオブジェクトを確実にフリーズできるようにするために、XSは`Object.freeze`を拡張して、オブジェクトを再帰的にフリーズすることを要求するオプションの第二引数を追加しました。これにより、上記の`Colors`オブジェクトを単一の呼び出しで完全にフリーズすることができます。

```js
const Colors = [
	{name: "blue", value: 0x0000FF},
	{name: "white", value: 0xFFFF00},
	...
];
Object.freeze(Colors, true);
```

この拡張はJavaScript言語の一部ではないため、XSエンジン専用のコードでのみ使用するよう注意が必要です。同等の機能が[`harden`](https://github.com/Agoric/Harden)のような標準的な方法で利用可能になった場合、XSはそのメカニズムをだけを使用するように移行します。

Hardened JavaScriptでは、`harden()`グローバル関数として再帰的フリーズが正式に提供されています。XSはHardened JavaScript（以前のセキュアECMAScript）サポートの一部として`harden()`を実装していますが、現時点ではModdable SDKのビルドには含まれていません。

### 組み込みの自動フリーズ
プリロードビルドフェーズの後、XSリンカーは以下をフリーズします：

- 組み込みオブジェクトのプロトタイプ -- 例えば `Object`、`Math`、`Date`、`Proxy` など -- が凍結されます。
- 組み込み関数およびプリロードされたモジュールの一部であるすべての関数。これにはクラスコンストラクタも含まれます。

このステップの結果は、[Frozen Realmsのプロポーザル](https://github.com/tc39/proposal-frozen-realms)と共通の特徴を持つランタイム環境を生成します。既に説明されたメモリ節約に加えて、スクリプトはJavaScript言語仕様によって定義された組み込みオブジェクトが実行中にランタイムパッチによって変更されないことを知るため、信頼性の高い実行環境を提供します。ランタイムオブジェクトのパッチを排除することも、安全な実行環境を提供するのに寄与します。

## モジュールスコープ
モジュールは、単一のクラスインスタンスに依存しない情報をそのライフタイム全体で維持する必要があることがあります。これらの変数はモジュールのクロージャの一部であり、モジュールの本体にレキシカルスコープされています。モジュールが実行されるときに作成されます。以下の `CountingLog` の改訂版は、すべてのインスタンス間で単一のカウンタ変数を共有します。

```js
let count = 1;

class CountingLog {
	log(msg) {
		trace(`${count++}: ${msg}\n`);
	}
}
Object.freeze(CountingLog.prototype);

export default CountingLog;
```

このモジュールがプリロードされると、`count` 変数の値はモジュールのクロージャの一部としてROMに固定されます。オブジェクトと同様に、XSは変更をRAMに保存することで、そのような変数を変更できるようにします。これは、変更可能なROM内の各変数に対してRAM内にポインタを維持することによって達成されます。各ポインタは、典型的な32ビットマイクロコントローラーで4バイトを消費します。

ランタイムで変更されることが意図されていないモジュール変数を宣言するには `const` を使用します。`const` でモジュール変数を宣言することは、XSにその変数が変更できないことを伝えます。これにより、変数を変更できるようにするために必要なポインタを省略することでRAMを節約します。

## プリロードできないもの
プリロードはビルドマシンで行われ、ターゲットデバイスでは行われません。これにより、プリロード中に実行できる操作が制限されます。

### ネイティブ関数
ビルドはターゲットデバイス用であるため、ビルドマシン用ではなく、異なる環境や場合によっては異なる命令セットを期待するネイティブ関数は実行できません。モジュールがネイティブ関数を呼び出そうとすると、ビルド中にエラーが生成されます。

たとえば、以下の例では `Digital.write` がネイティブ関数であるため、プリロードに失敗します。

```js
import Digital from "pins/digital";

Digital.write(1, 0);
```

この画像は、このモジュールをプリロードする際にビルド中に生成されるエラーを示しています。

![](./../assets/preload/build-error.png)

ネイティブ関数をプリロード中に定義することは安全です。以下の例を参照してください。

```js
class Example {
	static aNativeFunction() @ "xs_nativefunction";
}
```

ネイティブ関数を呼び出すことはできないため、これはビルド時にエラーを生成します：

```js
class Example {
	static aNativeFunction() @ "xs_nativefunction";
}
Example.aNativeFunction();
```

### いくつかのJavaScript組み込み機能
基本的なJavaScriptの型やオブジェクトはビルド時に作成可能で、作成されたオブジェクトをフラッシュメモリに保存できます。安全に使用できるものには以下が含まれます：

- Array
- ArrayBuffer
- BigInt
- Boolean
- class
- DataView
- Date
- Error
- FinalizationGroup
- Function
- Map
- null
- Number
- Object
- Promise
- RegExp
- Set
- String
- Symbol
- TypedArrays（例：`Uint8Array`）
- undefined
- WeakMap
- WeakRef
- WeakSet

これらのオブジェクトはフラッシュメモリに保存できません：

- AsyncGenerator
- Generator
- SharedArrayBuffer

将来的にXSは追加の組み込みオブジェクトをフラッシュメモリに保存するサポートが可能になるかもしれません。フラッシュメモリに保存される組み込みオブジェクトの詳細については、[XSリンカーの警告](./XS%20linker%20warnings.md)ドキュメントを参照してください。

これらのオブジェクトはフラッシュに保存できません。しかし、保存する必要がない限り、プリロード中に使用することは可能です。例えば、プリロードの一部として実行されるコードは、プリロードフェーズが終了する時に正規表現のインスタンスが残っていない限り、安全に`RegExp`を使用できます。

## 組み込みオブジェクトの振る舞いの変更

組み込みオブジェクトはプリロードが完了するまで凍結されないため、プリロードされたモジュールを使用してその挙動を変更することが可能です。これにより、組み込みオブジェクトの動作を拡張できます。

例えば、`Error.protoype.name` プロパティは通常のプロパティであり（[ECMA-262](https://tc39.es/ecma262/#sec-error.prototype.name)で指定されているように）, 凍結されると、`name`に書き込みを試みた際にエラーが発生します。


```js
class MyError extends Error {
    constructor(message) {
        super(message);
        this.name = "MyError"; // throws "# Exception: set name: not writable (in MyError)!"
    }
}

const err = new MyError("My error message");
```

これに対処するために、クラス `MyError` の実装にnameプロパティを追加できます。ただし、`Error` のプロトタイプで `name` を書き込み可能にする必要がある場合（たとえば、サードパーティのNPMモジュールを使用する際など）、`name` が凍結される前にその挙動を変更するプリロードされたモジュールを作成することができます。

```js
Object.defineProperty(Error.prototype, "name", {
	get: function() {return "Error";},
	set: function(value) {
		Object.defineProperty(this, "name", {value, writable: true, configurable: true});
	}
});
```

## プリロード `main`
`main`モジュールは最初に実行されるアプリケーションスクリプトです。その作業を行うために、`main`モジュールは通常他のモジュールをインポートします。プロジェクトの`main`モジュールは、通常、プリロードに設定されていない唯一のモジュールです。これは便宜上行われ、Moddable SDKの例のような小さなプロジェクトでは、通常問題にはなりません。アプリケーションの`main`モジュールは必ずネイティブ関数を呼び出し、Wi-Fiに接続したり、画像を表示したり、デジタルピンを切り替えたりします。上記のように、ネイティブ関数はプリロード中には呼び出せません。

以下は、起動時にデジタルピンを使用して1つのLEDを点灯させ、繰り返しタイマーを設定して別のLEDの状態を切り替える簡単なアプリケーションの例です。

```js
import Digital from "pins/digital";

let toggle = false;

Digital.write(1, true);
Timer.repeat(() => {
	toggle = !toggle;
	Digital.write(2, toggle)
}
```

Moddable SDKランタイムでは、`main` モジュールが関数を返す場合、その関数は直ちに実行されます。これを利用して、`main` がプリロードをサポートするようにすることができます。ここにそのような単純な例を示します：

```js
import Digital from "pins/digital";

export default function() {
	let toggle = false;

	Digital.write(1, true);
	Timer.repeat(() => {
		toggle = !toggle;
		Digital.write(2, toggle)
	}, 1000);
}
```

時には、`main` をエクスポートされた関数からインスタンス化されるシンプルなクラスで整理すると便利です。これにより、コードがよりクリーンに構造化され、上記の例の `toggle` のように必要な状態がインスタンスの状態の一部として `this` を使用してアクセスされます。

```js
import Digital from "pins/digital";

class App {
	constructor() {
		Digital.write(1, true);

		this.toggle = false;
		Timer.repeat(this.blink.bind(this), 1000);
	}
	blink() {
		this.toggle = !this.toggle;
		Digital.write(2, this.toggle)
	}
}

export default function() {
	new App;
}
```

## プリロード中の事前計算
マイクロコントローラはパフォーマンスが比較的遅いため、一般的な最適化技術として、複雑な計算をマイクロコントローラ上で実行する必要を最小限に抑えるために、事前計算された値のテーブルを使用することがあります。値は通常、別のプログラムによって計算され、その後配列などのデータ構造に入力されます。

たとえば、浮動小数点の平方根関数は比較的複雑であり、パフォーマンスが重要な状況で使用される場合、最適化の良い候補となります。次の例は、単純化のために作られたもので、0から10までの整数の平方根を含む配列をルックアップテーブルとして使用しています。

```js
const roots = [
	0,
	1,
	1.4142135624,
	1.7320508076,
	2,
	2.2360679775,
	2.4494897428,
	2.6457513111,
	2.8284271247,
	3,
	3.1622776602
];
Object.freeze(roots);

function fastSquareRootToTen(x) {
	return roots[x];
}
```

11個の要素を持つテーブルはすでに扱いにくいです。100や1000になると、さらに扱いにくくなります。さらに悪いことに、視覚的な検査によって値が正しいかどうかを確認する方法がないため、気づかぬうちにエラーが発生する可能性があります。ビルド時にプログラムでテーブルを生成することで、これらの問題を解決することができます。

```js
const roots = [];
for (let i = 0; i <= 10; i++)
	roots[i] = Math.sqrt(i)
Object.freeze(roots);

function fastSquareRootToTen(x) {
	return roots[x];
}
```

この技術は、より複雑な計算を行ったり、配列よりも複雑なデータ構造を生成するために適用することができます。

## プリロードされたモジュールをチェックするためにxsbugを使用する
プロジェクト内のすべてのモジュールがプリロードに設定されているかどうかをソースコードを検査するだけでは判断が難しいです。xsbugデバッガーには、これを支援するための2つの機能があります。

インストゥルメントペインには、「Modules loaded」というエリアがあり、実行の各秒ごとにロードされたモジュールの数を表示します。ほとんどのプロジェクトでは、この数は1または0であるべきです。

モジュールペインは、すべてのロードされたモジュールのリストを表示し、どのモジュールがプリロードされているかを色で示します。プリロードされたモジュールは青で表示され、実行時にロードされたものは黒で表示されます。

下の画像では、インストゥルメントがブレークポイントの時点で1つのモジュールがロードされていることを示し、モジュールペインは `main` モジュールが実行時にロードされたことを示しています。

![](./../assets/preload//xsbug.png)

## 追加の注釈

プリロードされたオブジェクトは `JSON.stringify()` を使用してシリアライズすることはできません。これを試みると「読み取り専用値」の例外が発生します。これは `JSON.stringify()` の実装がオブジェクトがRAM内にあることを検出するために依存しているためです。回避策としては、`structuredClone` を使用してオブジェクトのディープコピーを作成し、それを `JSON.stringify()` に渡すことができます。

## 結論
モジュールのプリロードは、XS JavaScriptエンジンのユニークな機能であり、マイクロコントローラーの限られたRAMとパフォーマンスのより効率的な使用を可能にします。Moddable SDKで提供されるモジュールに広くサポートされているため、開発者はプリロードを完全に理解していなくてもその利点を享受できます。プリロードメカニズムを理解することで、開発者は自身のコードに対するその利点を実現することができます。これらの利点には：

- マイクロコントローラからビルドマシンへの初期化コードの移動による起動時間の短縮
- 初期化中に作成されたオブジェクトをフラッシュ（ROM）メモリに移動することで、プロジェクトコードのための空きメモリを増加
- モジュールのほぼ瞬時のインポート
- ビルド時により多くのエラーを検出
- 事前計算技術を使用して埋め込みルックアップテーブルやデータ構造を簡素化

#### 謝辞

このドキュメントは、Twitterの[@hori__hiro](https://twitter.com/moddabletech/status/1086084032008413184)からのリクエストに応えて作成されました。

このドキュメントの初稿はLizzie Praderによって書かれ、このドキュメントの編集を手伝ってくれました。

初回投稿のモジュールスコープセクションの冒頭の用語使用は不正確でした。改善を提案してくれたAllen Wirfs-Brockに感謝します。
