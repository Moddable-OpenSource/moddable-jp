# Base
Copyright 2017-2024 Moddable Tech, Inc.<BR>
改訂： 2024年1月19日

## 目次

* [Timer](#timer)
* [Time](#time)
* [Debug](#debug)
* [UUID](#uuid)
* [deepEqual](#deepequal)
* [structuredClone](#structuredclone)
* [Instrumentation](#instrumentation)
* [Console](#console)
* [CLI](#cli)
* [Worker](#worker)

<a id="timer"></a>
## Timer クラス

- **ソースコード:** [timer](../../modules/base/timer)
- **関連するサンプル:** [timers](../../examples/base/timers/)

`Timer` クラスは、時間ベースのコールバックと遅延関数の両方を提供します。

```js
import Timer from "timer";
```

タイマーコールバックは `this` が `globalThis` に設定された状態で呼び出されます。コールバックの `this` を別の値にバインドするには、アロー関数または `Function.prototype.bind` を使用します。

各タイマーには2つの間隔があります： 初期間隔と繰り返し間隔。間隔はミリ秒単位です。初期間隔はコールバックが最初に呼び出されるまでの時間です。繰り返し間隔は初期間隔の後、コールバックが連続して呼び出される間の時間です。以下のAPIリファレンスでは、各APIが初期間隔と繰り返し間隔をどのように変更するかを示しています。

タイマーのコールバックが返されるときにリピート間隔がゼロの場合、タイマーは自動的にクリアされ、再利用できなくなります。リピート間隔はコールバックによって `Timer.schedule` を使用して変更することができます。

`Timer.set` と `Timer.repeat` 関数は新しいタイマーを作成し、そのタイマーのIDを返します。タイマーIDは不透明であり、`Timer` 関数に渡すためだけに使用されます。

タイマーのコールバックは `setImmediate`、`setTimeout`、および `setInterval` の基本的な動作を提供できます。[Timerのサンプル]()では、その方法を示しています。

### `Timer.set(callback[, initialInterval, repeatInterval])`

`set` 関数は、一定の期間後に関数を一度だけ呼び出すことを要求します。`Timer.set` は新しいタイマーのIDを返します。

即時タイマーは、実行ループの次のサイクルで呼び出されます。即時タイマーを設定するには、引数を1つだけ指定して `set` を呼び出します。

```js
Timer.set(id => trace("immediate fired\n"));
```

ワンショットタイマーは、指定されたミリ秒数の後に一度だけ呼び出されます。ミリ秒数がゼロの場合、ワンショットタイマーは即時タイマーと同等です。

```js
Timer.set(id => trace("one shot fired\n"), 1000);
```

`repeat` を指定して `set` を呼び出すと、最初のコールバックが `interval` 後にトリガーされる繰り返しタイマーと同等です。

```js
Timer.set(id => trace("repeat fired\n"), 1000, 100);
```

コールバック関数は、最初の引数としてタイマーIDを受け取ります。

`Timer.set` が初期間隔と繰り返し間隔なしで呼び出された場合、それは即時のワンショットタイマーです（初期間隔と繰り返し間隔は0に設定されます）。`Timer.set` が初期間隔のみで呼び出された場合、それはワンショットタイマーです（繰り返し間隔は0に設定されます）。`Timer.set` が初期間隔と非ゼロの繰り返し間隔の両方で呼び出された場合、それは繰り返しタイマーです。

***

### `Timer.repeat(callback, repeatInterval)`

繰り返しタイマーは `Timer.clear` 関数を使用して停止されるまで連続して呼び出されます。`Timer.repeat` は新しいタイマーのIDを返します。

```js
Timer.repeat(id => trace("repeat fired\n"), 1000);
```

コールバック関数は、最初の引数としてタイマーIDを受け取ります。

この関数は、初期間隔と繰り返し間隔の両方を `repeatInterval` で指定された値に設定します。初期間隔が繰り返し間隔と異なるタイマーを作成するには、`Timer.set` を使用します。

***

### `Timer.schedule(id [, initialInterval[, repeatInterval]])`

`schedule` 関数は、既存のタイマーを再スケジュールまたはスケジュール解除します。

初期間隔のみを指定して呼び出された場合、タイマーは `Timer.set` で作成されたワンショットタイマーのように動作します。初期間隔とゼロ以外の繰り返し間隔の両方を指定して呼び出された場合、初期間隔と繰り返し間隔の引数を持つ `Timer.set` で作成された繰り返しタイマーのように動作します。間隔引数なしで呼び出された場合、タイマーはスケジュール解除され、`Timer.schedule` を使用して再スケジュールされるまでトリガーされません（スケジュール解除されたタイマーは無限の初期および繰り返し間隔を持つと見なされます）。

次の例では、コールバック関数が1秒間隔で2回トリガーされ、その後2秒ごとに1回再スケジュールされます。

```js
let state = 0;
Timer.repeat(id => {
	if (0 == state)
		state = 1;
	else if (1 == state) {
		state = 2;
		Timer.schedule(id, 2000, 2000);
	}
}, 1000);
```

`Timer.schedule` を使用して初期間隔を設定すると、新しい初期間隔が経過した後に次のコールバックが呼び出されます。

> **注意**: 次のトリガー時間が不明な場合、タイマーを長時間先にスケジュールするよりもスケジュール解除する方が望ましいです。タイマーをスケジュール解除して再スケジュールする方が、タイマーをクリアして新しいものを後で割り当てるよりも効率的な場合があります。

***

### `Timer.clear(id)`

`clear` 関数はタイマーをキャンセルします。`Timer.set` および `Timer.repeat` 関数はタイマーのIDを返し、それを `clear` に渡します。

```js
let aTimer = Timer.set(id => trace("one shot\n"), 1000);
Timer.clear(aTimer);
```

> **注意**: 即時およびワンショットタイマーは、コールバック関数を呼び出した後に自動的にクリアされます。タイマーが発火する前にキャンセルする場合を除き、`clear` を呼び出す必要はありません。

> **注意**: `Timer.clear` に `undefined` または `null` の値がIDとして渡された場合、例外は発生しません。

***

### `Timer.delay(ms)`

`delay` 関数は、指定されたミリ秒数だけ実行を遅延させます。

```js
Timer.delay(500);	// 1/2秒遅延
```

**注意**: 一般的に、JavaScriptプログラミングの推奨スタイルは、実行をブロックする長い遅延を避けることです。`Timer.delay` は、組み込み開発ではハードウェアと対話する際に短い遅延が必要なことが一般的であるため提供されています。長い遅延の場合は、タイマーコールバックやPromiseを使用した非同期実行などの代替手段を使用する方が適切な場合があります。

***

<a id="time"></a>
## Time クラス

- **ソースコード:** [time](../../modules/base/time)
- **ソースコード:** [time](../../modules/base/microseconds)
- **関連するサンプル** [sntp](../../examples/network/sntp/), [ntpclient](../../examples/network/mdns/ntpclient), [ios-time-sync](../../examples/network/ble/ios-time-sync)

`Time` クラスは時間関数とティックカウンターを提供します。

```js
import Time from "time";
```

### `Time.set(seconds)`

`set` 関数はシステム時間を設定します。`seconds` 引数は1970年1月1日から経過した秒数、すなわちUnix時間に対応します。

***

### `timezone` プロパティ

`timezone` プロパティはUTCからの秒単位のタイムゾーンオフセットに設定されます。

```js
Time.timezone = +9 * 60 * 60;	// タイムゾーンをUTC+09:00に設定
```

***

### `dst` プロパティ

`dst` プロパティは秒単位の夏時間（DST）オフセットに設定されます。

```js
Time.dst = 60 * 60;	// DSTを設定
```

***

### `ticks` プロパティ

`ticks` プロパティはミリ秒カウンターの値を返します。返される値は時刻には対応していません。ミリ秒は時間差を計算するために使用されます。

```js
const start = Time.ticks;
for (let i = 0; i < 1000; i++)
	;
const stop = Time.ticks;
trace(`Operation took ${Time.delta(start, stop)} milliseconds\n`);
```

複数の同時実行JavaScript仮想マシンをサポートするデバイス（例えば、Workersを使用する場合）では、`ticks`プロパティの値を決定するために使用されるクロックはすべての仮想マシンで同じです。これにより、あるマシンで作成された`tick`値を別のマシンの値と比較することができます。

値の範囲はホストによって異なります。ほとんどのマイクロコントローラでは、値は符号付き32ビット整数です。シミュレータでは、正の64ビット浮動小数点値です。2つの`ticks`値の差を求めるには、ホストに対して正しい結果を保証する`Time.delta()`を使用します。

***

### `Time.delta(start[, end])`

`delta`関数は、`Time.ticks`によって返される2つの値の差を計算します。値がロールオーバーしても正しい結果を返すことが保証されています。オプションの`end`引数が省略された場合、現在の`Time.ticks`の値が使用されます。

***

### `microseconds`プロパティ  *(オプション)*

`microseconds`プロパティは、マイクロ秒カウンタの値を返します。返される値は時刻に対応していません。マイクロ秒は時間差を計算するために使用されます。


`microseconds` プロパティを使用するには、プロジェクトのマニフェストにそのマニフェストを含めます：

```
	"include": [
		...
		"$(MODDABLE)/modules/base/microseconds/manifest.json",
	],
```

`microseconds` プロパティは `ticks` プロパティと同じ方法で使用されます。`ticks` プロパティと同様に、複数の同時実行仮想マシンがある場合でも単一の時間ソースが使用されます。`microseconds` プロパティの範囲は64ビット浮動小数点値です。

`Time.ticks` とは異なり、`Time.microseconds` によって返される値は常に互いに減算して間隔を計算することができます。

```js
const start = Time.microseconds;
for (let i = 0; i < 1000; i++)
	;
const stop = Time.microseconds;
trace(`Operation took ${stop - start} microseconds\n`);
```

> **注意**: すべてのホストがマイクロ秒を実装しているわけではありません。マイクロ秒を実装していないホストはビルド時にエラーを生成します。

***

<a id="debug"></a>
##  Debug クラス

- **ソースコード:** [debug](../../modules/base/debug)

`Debug` クラスは開発プロセス中に役立つ関数を提供します。

```js
import Debug from "debug";
```

### `Debug.gc([enable])`

`gc` 関数は、JavaScriptガベージコレクタをオンまたはオフにするため、またはオンデマンドでガベージコレクタを実行するために使用できます。

`Debug.gc` を引数なしで呼び出すと、ガベージコレクタが即座に実行されます。

```js
Debug.gc();
```

`Debug.gc` を単一のブール引数で呼び出すと、ガベージコレクタを有効または無効にします。

```js
Debug.gc(true)	// ガベージコレクタを有効にする
Debug.gc(false);	// ガベージコレクタを無効にする
```

***

<a id="uuid"></a>
## UUID　クラス

- **ソースコード:** [uuid](../../modules/base/uuid)

`UUID` クラスは、[UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier) 文字列を生成するための単一の関数を提供します。

```js
import UUID from "uuid";
```

> **注意**: 真にユニークな UUID を生成するには、この関数を実行するデバイスがユニークな MAC アドレスと有効な時間を持っている必要があります。これらの条件はすべてのマイクロコントローラで保証されているわけではありません。製品ソフトウェアのリリースには、デバイスの構成を確認してください。

### `UUID()`

`UUID` 関数は、文字列としてフォーマットされた新しいUUIDを返します。

```js
let value = UUID();	// 1080B49C-59FC-4A32-A38B-DE7E80117842
```

***

<a id="deepequal"></a>
## function deepEqual(a, b [,options])


- **ソースコード:** [deepEqual](../../modules/base/deepEqual)
- **テスト:** [deepEqual](../../tests/modules/base/deepEqual)

`deepEqual` 関数は、2つのJavaScriptオブジェクト間の深い比較を実装します。

```js
import deepEqual from "deepEqual";
```

JavaScriptで2つのオブジェクトを深く比較するための標準アルゴリズムは存在せず、多くの正しいアプローチが考えられます。Moddable SDKは、Node.jsの `assert.deepEqual` および `assert.deepStrictEqual` の動作を `deepEqual` の実装に採用しています。

`deepEqual` 関数にはオプションの第3パラメータとしてオプションオブジェクトがあります。デフォルトでは、`deepEqual` の[比較は非厳密](https://nodejs.org/api/assert.html#assertdeepequalactual-expected-message)であり、比較に `==` を使用するのと似ています。オプションオブジェクトに `{strict: true}` を渡すと、[厳密な比較](https://nodejs.org/api/assert.html#assertdeepstrictequalactual-expected-message)が行われ、比較に `===` を使用するのと似ています。

```js
const a = {a: 1, b: 0, c: "str"};
const b = {a: 1, b: "0", c: "str"};
deepEqual(a, b);		// true
deepEqual(a, b, {strict: true});		// false

```

Moddable SDKの実装とNode.jsの既知の違いは、ほとんどの使用に影響を与えません：

- 読み取り専用オブジェクトをキーとして使用する`WeakMap`と`WeakSet`
- XSはボックス化されたプリミティブを比較するために`valueOf`を呼び出しません

***

<a id="structuredclone"></a>
## function structuredClone(object[, transferables]) {/*examples*/}

- **ソースコード:** [structuredClone](../../modules/base/structuredClone)
- **テスト:** [structuredClone](../../tests/modules/base/structuredClone)

`structuredClone`関数は、JavaScriptオブジェクトのディープコピーを作成します。

```js
import structuredClone from "structuredClone";
```

Moddable SDKの`structuredClone`関数は、循環参照や`transferables`オプションを含む、可能な限りWHATWGによって定義された[アルゴリズム](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API/Structured_clone_algorithm)を実装しています。

```js
const a = {a: 1, b: Uint8Array.of(1, 2, 3,)}
const aCopy = structuredClone(a);
```

Moddable SDKの`structuredClone`の実装は、JavaScript言語標準の一部である[サポートされている型](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API/Structured_clone_algorithm#supported_types)をすべて実装しています。もちろん、`DOMException`のような埋め込みシステムに存在しないWebプラットフォームのオブジェクト型はクローンしません。

<a id="instrumentation"></a>
## Instrumentation クラス

- **ソースコード:** [instrumentation](../../modules/base/instrumentation)
- **関連するサンプル:** [instrumentation](../../examples/base/instrumentation/instrumentation), [xsuse](../../examples/base/instrumentation/xsuse)

`Instrumentation` クラスは、メモリ使用量、オープンファイル数、レンダリングフレームレートなど、ランタイムの動作に関する統計情報を返します。

```js
import Instrumentation from "instrumentation";
```

### `get(what)`

`get` 関数は、`what` パラメータで指定されたインデックスの計測項目の値を返します。計測項目はインデックス1から連続して番号が付けられています。

```js
let pixelsDrawn = Instrumentation.get(1);
```

計測項目のインデックスはホストによって異なり、サポートされている機能に基づいてデバイスごとに異なります。実行中のホストで特定の計測のインデックスを確認するには、`Instrumentation.map()` を使用します。

### `map(name)`

`map` 関数は、名前に対する計測インデックスを返します。

```js
let pixelsDrawnIndex = Instrumentation.map("Pixels Drawn");
let pixelsDrawn = Instrumentation.get(pixelsDrawnIndex);
```

指定された名前の計測項目が利用できない場合、`map`は`undefined`を返します。

### `name(index)`

`name`関数は、指定されたインデックスの計測項目の名前を返します。これを使用して、利用可能なすべての計測項目を反復処理することができます。

```js
for (let i = 1; true; i++) {
	const name = Instrumentation.name(i);
	if (!name)
		break;
	trace(`${name}: ${Instrumentation.get(i)}\n`);
}
```

### 計測項目

以下の表は、利用可能な計測項目を説明しています。次の計測項目は1秒間隔でリセットされます： Pixels Drawn, Frames Drawn, Poco Display List Used, Piu Command List Used, Network Bytes Read, Network Bytes Written, and Garbage Collection Count.

| 名前 | 詳細説明 |
| ---: | :--- |
| `Pixels Drawn` | 現在の間隔中にPocoによってレンダリングされたピクセルの総数。この値には、ディスプレイデバイスに描画されたピクセルとオフスクリーンにレンダリングされたピクセルが含まれます。
| `Frames Drawn` | 最近の間隔中にPocoによってレンダリングされたフレームの総数。フレームはPoco.prototype.end()の呼び出しとPiuフレームの更新によってカウントされます。
| `Network Bytes Read` | 現在の間隔中にSocketモジュールによって受信されたバイトの総数。これにはTCPおよびUDPで受信されたバイトが含まれます。
| `Network Bytes Written` | 現在の間隔中にSocketモジュールによって送信されたバイトの総数。これにはTCPおよびUDPで送信されたバイトが含まれます。
| `Network Sockets` | Socketモジュールによって作成されたアクティブなネットワークソケットの総数。これにはTCPソケット、TCPリスナー、およびUDPソケットが含まれます。
| `Timers` | Timerモジュールによって作成された割り当てられたタイマーの数。
| `Files` | Fileモジュールによって作成された開いているファイルおよびディレクトリイテレータの数。
| `Poco Display List Used` | 現在の間隔中のPocoディスプレイリストのピークサイズ（バイト単位）。
| `Piu Command List Used` | 現在の間隔中のPiuコマンドリストのピークサイズ（バイト単位）。
| `Turns` | 現在の間隔中にイベントループが実行された回数。
| `CPU 0` | 現在の間隔中のCPU 0の負荷。
| `CPU 1` | 現在の間隔中のCPU 1の負荷。
| `System Free Memory` | システムメモリヒープの空きバイト数。この値はシミュレータでは利用できません。
| `XS Slot Heap Used` | プライマリXSマシンのスロットヒープで使用されているバイト数。これらのバイトの一部は次回のガベージコレクタの実行時に解放される可能性があります。
| `XS Chunk Heap Used` | プライマリXSマシンのチャンクヒープで使用されているバイト数。これらのバイトの一部は次回のガベージコレクタの実行時に解放される可能性があります。
| `XS Keys Used` | プライマリXSマシンによって割り当てられたランタイムキーの数。一度割り当てられたキーは決して解放されません。
| `XS Garbage Collection Count` | 現在の間隔中にガベージコレクタが実行された回数。
| `XS Modules Loaded` | 現在プライマリXSマシンにロードされているJavaScriptモジュールの数。この数にはプリロードされたモジュールは含まれません。
| `XS Stack Used` | 現在の間隔中のプライマリXS仮想マシンのスタックの最大深度（バイト単位）。
| `XS Promises Settled` | 解決されたPromiseの数。これはPromise/async/awaitの活動の指標として役立ちます。

***

<a id="console"></a>
##  Console クラス

- **ソースコード:** [console](../../modules/base/console)
- **関連する例:** [console](../../examples/base/console)

`Console` クラスは、デバッグおよび診断のためのシリアルターミナルを実装します。`Console` モジュールは、ターミナルコマンドを実装するために `CLI` モジュールを使用します。

<!-- complete -->

***

<a id="cli"></a>
## CLI クラス

- **ソースコード:** [cli](../../modules/base/cli)
- **関連する例:** [console](../../examples/base/console), [telnet](../../examples/network/telnet)

`CLI` クラスは、コマンドラインインターフェースで使用されるコマンドのプラグインインターフェースです。`CLI` クラスは、`Console` モジュール（シリアルコマンドライン）および `Telnet` モジュール（ネットワークコマンドライン）で使用されるコマンドを実装します。

<!-- complete -->

***

<a id="worker"></a>
## Worker クラス

`Worker` クラスに関する情報は、[Worker ドキュメント](./worker.md) を参照してください。


