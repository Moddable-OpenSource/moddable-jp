# キーボード

キーボードモジュールは、Piuで使用するためのレスポンシブレイアウトを備えたタッチスクリーンキーボードを実装します。

- **ソースコード:** [`keyboard.js`](../../modules/input/keyboard/keyboard.js)
- **関連するサンプル:** [keyboard](../../examples/piu/keyboard/main.js)

キーボードは、親コンテナを自動的に埋める[`Port`](./piu.md#port-object)オブジェクトを使用して実装されており、アプリケーションによって制御される方法でリフローすることができます。コンストラクタに渡される辞書は、キーボードのプロパティを構成します。

キー押下は、アプリケーションの動作でキャプチャできるイベントをトリガーします。キーボードのテキストのスタイル（フォントとウェイト）は、呼び出し元が提供する[`Style`](./piu.md#style-object)オブジェクトによって駆動されます。これにより、`Style`テンプレートを使用することができます。

キーボードは、キーボードを画面外に移行させるためにトリガーできる`doKeyboardTransitionOut`イベントを実装しています。移行が完了すると、キーボードはアプリケーションに通知するイベントをトリガーします。

## モジュールエクスポート


| エクスポート | 型 |  説明 |
| :---: | :---: | :--- |
| `Keyboard` | `constructor` | キーボードインスタンスを作成するために使用されるコンストラクタ。 |
| `BACKSPACE` | `string` | バックスペースキーが押されたことを示すために使用される定数。 |
| `SUBMIT` | `string` | サブミットキーが押されたことを示すために使用される定数。 |

```js
import {Keyboard, BACKSPACE, SUBMIT} from "keyboard";
```

## キーボードオブジェクト

### コンストラクタの説明

#### `Keyboard(behaviorData, dictionary)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `behaviorData`	| `*` |	キーボードの `behavior` の `onCreate` 関数に渡されるパラメータ。これは `null` や任意のパラメータを持つ辞書を含む任意のタイプのオブジェクトである可能性があります。 |
| `dictionary` | `object` | 結果として得られるキーボードを構成するためのプロパティを持つオブジェクト。以下の [Dictionary](#keyboard-dictionary) セクションで指定されたパラメータのみが効果を持ち、他のパラメータは無視されます。

`keyboard` インスタンス、つまりその動作として `KeyboardBehavior` クラスのインスタンスを使用する `Port` オブジェクトを返します。

```js
let OpenSans18 = new Style({ font: "semibold 18px Open Sans", color: "black" });
let keyboard = new Keyboard(null, {style: OpenSans18, doTransition: false})
```

![](../assets/piu/keyboard.png)

<a id="keyboard-dictionary"></a>
### 辞書

| パラメータ | 型 | デフォルト値 | 説明 |
| :---: | :---: | :---: | :--- |
| `style` | `style` | n/a | **必須。** キーのテキストに使用されるPiu Styleオブジェクト。 |
| `bgColor` | `string` | `"#5b5b5b"`| 背景の塗りつぶし色。 |
| `doTransition` | `boolean` | `false`| キーボードが最初に表示されるときにトランジションするかどうか。 |
| `keyColor` | `string` | `"#d8d8d8"`| 押されていないときの文字キーの色。 |
| `keyDownColor` | `string` | `"#999999"`| 押されているときの文字キーの色。 |
| `keyToggledColor` | `string` | `"#7b7b7b"`| 押されているときの文字キーの色。 |
| `specialKeyColor` | `string` | `"#999999"`| 押されていないときの特殊キー（シフト、シンボル、バックスペース、送信）の色。 |
| `specialTextColor` | `string` | `"#ffffff"`| 特殊キー（シフト、シンボル、バックスペース、送信）のテキストの色。 |
| `submit` | `string` | `"OK"`| 送信キーに表示される文字列。 |
| `textColor` | `string` | `"#000000"`| 文字キーのテキストの色。 |
| `transitionTime` | `number` | `250`| キーボードのイン/アウトトランジションの持続時間（ミリ秒）。 |

<a id="key-callback"></a>
### トリガーされたイベント

#### `onKeyboardTransitionFinished()`
キーボードが画面外に移行し終わったときに、このイベントがバブルされます。`onKeyboardTransitionFinished` 関数は通常、呼び出し元アプリケーションの動作で実装され、トリガーされます。

***

#### `onKeyDown(key)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `key` | `string` | ほとんどの場合、文字列は押されたキーの値になります（例: `"a"`, `"3"`, `"$"`）。また、モジュールによってエクスポートされる2つの定数のいずれかである場合もあります: `BACKSPACE` または `SUBMIT`。これらはキーボード上でそのキーが押されたことを示します。|

キーボードがキーを押されたときに、このイベントがバブルされます。`onKeyDown` 関数は通常、呼び出し元アプリケーションの動作で実装され、トリガーされます。

***

#### `onKeyUp(key)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `key` | `string` | ほとんどの場合、文字列は放されたキーの値になります（例: `"a"`, `"3"`, `"$"`）。また、モジュールによってエクスポートされる2つの定数のいずれかである場合もあります: `BACKSPACE` または `SUBMIT`。これらはキーボード上でそのキーが放されたことを示します。|

キーボードはキーが放されたときに `onKeyUp` イベントをバブルします。`onKeyUp` 関数は通常、呼び出し元アプリケーションの動作で実装され、トリガーされます。

***

### 受信イベント

#### `doKeyboardTransitionOut(keyboard)` {#examples}

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `keyboard` | `keyboard` | イベントを受信した `keyboard` オブジェクト。 |

この関数は、キーボードを画面外に移行させるためにトリガーされることがあります。移行が完了すると、キーボードによって `onKeyboardTransitionFinished` イベントがバブルされます。

***
