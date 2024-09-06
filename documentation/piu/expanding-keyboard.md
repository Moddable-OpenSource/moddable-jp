# 拡張キーボードのリファレンス
Copyright 2019 Moddable Tech, Inc.<BR>
改訂： July 2, 2019

縦型および横型の拡張キーボードモジュールは、Moddable OneおよびModdable Two [製品](https://www.moddable.com/product.php)上で[Piu](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/piu/piu.md)と共に使用するためのタッチスクリーンキーボードを実装します。キーはタップすると自動的に拡大し、スタイラスを必要としません。両方のキーボードは同じAPIを実装しています。

- **ソースコード:** [`vertical/keyboard.js`](../../modules/input/expanding-keyboard/vertical/keyboard.js) [`horizontal/keyboard.js`](../../modules/input/expanding-keyboard/horizontal/keyboard.js)
- **関連するサンプル:** [vertical-expanding-keyboard](../../examples/piu/vertical-expanding-keyboard/main.js) [horizontal-expanding-keyboard](../../examples/piu/horizontal-expanding-keyboard/main.js)

キーボードは240 x 320の画面にフィットするように設計されています。縦型キーボードの高さは185ピクセルです。横型キーボードの高さは164ピクセルで、横向きで動作するように設計されています。どちらの向きでもアプリケーション画面の幅を埋めます。

キー押下はアプリケーションの動作でキャプチャできるイベントをトリガーします。キーボードのテキストのスタイル（フォントとウェイト）は、呼び出し元が提供する[`Style`](./piu.md#style-object)オブジェクトによって定義されます。これにより、`Style`テンプレートを使用することができます。

さらに、キーボードをアプリに統合するための`KeyboardField`コンテナと関連する動作が提供されています。このコンテナは、押されたキーと点滅するIビームカーソルを表示します。`KeyboardField`の高さは、フィールドテキストから外れたIビームカーソルが収まるのに十分な高さである必要があります。

## キーボードモジュールのエクスポート

| エクスポート | タイプ | 説明 |
| :---: | :---: | :--- |
| `VerticalExpandingKeyboard` | `constructor` | 垂直に拡張するキーボードインスタンスを作成するためのコンストラクタ。 |
| `HorizontalExpandingKeyboard` | `constructor` | 水平方向に拡張するキーボードインスタンスを作成するためのコンストラクタ。 |

```js
import {VerticalExpandingKeyboard} from "keyboard";
import {HorizontalExpandingKeyboard} from "keyboard";
```

## KeyboardField モジュールのエクスポート

| エクスポート | タイプ | 説明 |
| :---: | :---: | :--- |
| `KeyboardField` | `constructor` | キーボードフィールドインスタンスを作成するために使用されるコンストラクタ。 |

```js
import {KeyboardField} from "common/keyboard";
```

## キーボードオブジェクト

### コンストラクタの説明

#### `VerticalExpandingKeyboard(behaviorData, dictionary)`
#### `HorizontalExpandingKeyboard(behaviorData, dictionary)`

| 引数 | タイプ | 説明 |
| :---: | :---: | :--- |
| `behaviorData` | `*` | キーボードの `behavior` の `onCreate` 関数に渡されるパラメータ。これは `null` や任意のパラメータを持つ辞書を含む任意のタイプのオブジェクトである可能性があります。 |
| `dictionary` | `object` | 結果として得られるキーボードを構成するプロパティを持つオブジェクト。以下の [Dictionary](#keyboard-dictionary) セクションで指定されたパラメータのみが効果を持ち、他のパラメータは無視されます。 |

`VerticalExpandingKeyboard` または `HorizontalExpandingKeyboard` の `Container` インスタンスを返します。

```js
const KeyboardStyle = Style.template({ font:"18px Roboto", color:"black" });
let keyboard = new VerticalExpandingKeyboard(null, { Style:KeyboardStyle, target:this.data["FIELD"] });
```

![](../assets/piu/vertical-expanding-keyboard.png)

<a id="keyboard-dictionary"></a>
### 辞書

| パラメータ | 型 | デフォルト値 | 説明 |
| :---: | :---: | :---: | :--- |
| `Style` | `style` | n/a | **必須。** キーのテキストに使用されるPiuスタイルオブジェクト。 |
| `target` | `object` | n/a | **必須。** `onKeyUp`イベントを受け取るPiuコンテナオブジェクト。 |
| `doTransition` | `boolean` | `false`| キーボードが最初に表示されるときにトランジションし、閉じられるときにトランジションするかどうか。 |

<a id="key-callback"></a>
### トリガーイベント

#### `onKeyboardOK(container, text)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `container` | `object` | ビヘイビアのコンテナオブジェクト |
| `text` | `string` | フィールドに入力された完全な文字列 |

OKボタンが押されたときにキーボードがこのイベントをバブルします。

#### `onKeyboardRowsContracted(container)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `container` | `object` | ビヘイビアのコンテナオブジェクト |

キーボードが水平方向にキーボード行を元のズームされていないビューに戻すと、このイベントがバブルされます。

#### `onKeyboardRowsExpanded(container)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `container` | `object` | ビヘイビアのコンテナオブジェクト |

キーボードが水平方向にキーボード行をズームされたビューに拡張すると、このイベントがバブルされます。

#### `onKeyboardTransitionFinished(container, out)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `container` | `object` | ビヘイビアのコンテナオブジェクト |
| `out` | `boolean` | キーボードがビューから外れるときに `true` を設定し、キーボードがビューに入るときに `false` を設定します |

キーボードが画面のオンとオフを切り替えると、このイベントがバブルされます。`onKeyboardTransitionFinished` 関数は通常、呼び出し元アプリケーションのビヘイビアで実装され、トリガーされます。

#### `onKeyUp(container, key)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `container` | `object` | ビヘイビアのコンテナオブジェクト |
| `key` | `string` | 押されたキーの文字列値（例：`'a'`、`'3'`、`'$'`）。バックスペースの場合は `\b`、送信ボタンの場合は `\r` も可能です。|

キーボードはキーが放されたときに `onKeyUp` イベントをバブルします。`onKeyUp` 関数は通常、呼び出し元アプリケーションのビヘイビアで実装され、トリガーされます。

***

## KeyboardField オブジェクト

### コンストラクタの説明

#### `KeyboardField(behaviorData, dictionary)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `behaviorData`	| `*` |	キーボードフィールドの `behavior` の `onCreate` 関数に渡されるパラメータ。このパラメータは任意の型のオブジェクトであり得ます。`null` や任意のパラメータを持つ辞書も含まれます。 |
| `dictionary` | `object` | 結果として得られるキーボードフィールドを構成するプロパティを持つオブジェクト。以下の [Dictionary](#keyboard-field-dictionary) セクションで指定されたパラメータのみが効果を持ちます。他のパラメータは無視されます。 |

`KeyboardField` の `Container` インスタンスを返します。

```js
const FieldStyle = Style.template({ font: "20px Open Sans", color: "black", horizontal:"left", vertical:"middle" });
let keyboardField = new KeyboardField(null, { Style:FieldStyle });
```

<a id="keyboard-field-dictionary"></a>
### Dictionary

| パラメータ | 型 | デフォルト値 | 説明 |
| :---: | :---: | :---: | :--- |
| `Style` | `style` | n/a | **必須。** キーボード入力テキストに使用されるPiuスタイルオブジェクト。 |
| `password` | `boolean` | `false` | パスワードモードを有効にするには`true`を設定します。パスワードモードでは、短時間表示された後に各文字が隠されます。 |

### トリガーイベント

#### `onKeyboardOK(container, text)`

| 引数 | 型 | 説明 |
| :---: | :---: | :--- |
| `container` | `object` | ビヘイビアのコンテナオブジェクト |
| `text` | `string` | フィールドに入力された完全な文字列 |

OKボタンが押されたときにキーボードがこのイベントをバブルします。
