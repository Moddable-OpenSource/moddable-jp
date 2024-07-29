# Moddable 開発者ガイド
Copyright 2024 Moddable Tech, Inc.<BR>
改訂： 2024年1月9日

このドキュメントは、Moddable Display製品ファミリーに関する情報を提供します。

## 目次

- [Moddable Displayについて](#about-moddable-display)
- [Moddable Display 1](#display-1)
- [Moddable Display 2](#display-2)
- [Moddable Display 3](#display-3)
- [Moddable Display 4](#display-4)

<a id="about-moddable-display"></a>
## Moddable Displayについて

<img src="../assets/devices/moddable-display.jpg" width=600>

Moddable Displayは、最新のプロジェクトを作業台から現実世界に持ち込むための美しい方法です。各Moddable Displayは、強力な開発ボードの1つをエレガントなスタンドで包み込んでいます。洗練されたデザインは、家庭やオフィスにぴったりです。興味をそそる、メーカーフレンドリーなオープンバックは、プロジェクトに必要な追加のハードウェアコンポーネントを簡単に組み込むことができます。

Moddable Displayは4種類あり、それぞれのModdable開発ボードに対応しています。例えば、Moddable Display 1はModdable One開発ボードを基に構築されています。

<a id="display-1"></a>
## Moddable Display 1

Moddable Display 1は、Moddable One開発ボードを中心に構築されています。Moddable Display 1を使用して開発するための詳細については、[Moddable One Developer Guide](./moddable-one.md)を参照してください。

### ビルド

Moddable Display 1用にビルドするには、`esp/moddable_display_1`ビルドターゲットを使用します。例えば：

```
mcconfig -d -m -p esp/moddable_display_1
```

### ディスプレイの回転

Moddable Display 1には、縦向きのときに180度回転するModdable One開発ボードが含まれています。デフォルトのビルドでは、自動的に回転が180度に設定されます。横向きで実行するには、回転を90または270に設定します。タッチオリエンテーションで実行するには、回転を0に設定します。

回転はコマンドラインで設定できます：

```
mcconfig -d -m -p esp/moddable_display_1 driverRotation=90
```

またはプロジェクトのマニフェストで：

```json
	"config": {
		"driverRotation": 90
	}
```

またはJavaScriptで実行時に：

```js
screen.rotation = 90;
```

JavaScriptから回転を変更しても、画面はすぐには更新されません。プロジェクトは画面を再描画する必要があります。

**注意**: Piuユーザーインターフェースフレームワークを使用する場合、Moddable Display 1のタッチ入力は自動的に回転します。Commodettoを使用する場合、プロジェクトコードでディスプレイの回転を適用する必要があります。

<a id="display-2"></a>
## Moddable Display 2

Moddable Display 2はModdable Two開発ボードを中心に構築されています。Moddable Display 2での開発に関する詳細は、[Moddable Two Developer Guide](./moddable-two.md)を参照してください。

### ビルド

Moddable Display 2用にビルドするには、`esp32/moddable_display_2`ビルドターゲットを使用します。例えば：

```
mcconfig -d -m -p esp32/moddable_display_2
```

### ディスプレイの回転

Moddable Display 2には、ポートレート（縦向き）時に回転していない（0度）Moddable Two開発ボードが含まれています。ランドスケープ（横向き）で実行するには、回転を90度または270度に設定します。タッチオリエンテーションで実行するには、回転を180度に設定します。

回転はコマンドラインで設定できます：

```
mcconfig -d -m -p esp32/moddable_display_2 driverRotation=180
```

または、プロジェクトのマニフェストで：

```json
	"config": {
		"driverRotation": 180
	}
```

または、JavaScriptで実行時に：

```js
screen.rotation = 180;
```

JavaScriptから回転を変更しても、画面はすぐには更新されません。プロジェクトは画面を再描画する必要があります。

**注意**: Piuユーザーインターフェースフレームワークを使用する場合、Moddable Display 2のタッチ入力は自動的に回転します。Commodettoを使用する場合、プロジェクトコードはディスプレイの回転を適用する必要があります。

<a id="display-3"></a>
## Moddable Display 3

Moddable Display 3はModdable Three開発ボードを中心に構築されています。Moddable Display 3を使用した開発の詳細については、[Moddable Three Developer Guide](./moddable-three.md)を参照してください。

### ビルド

Moddable Display 3用にビルドするには、`esp/moddable_display_3`ビルドターゲットを使用します。例えば：

```
mcconfig -d -m -p esp/moddable_display_3
```

### ディスプレイの回転

Moddable Display 3には、横向きのときに180度回転するModdable Three開発ボードが含まれています。縦向きで実行するには、回転を90または270に設定します。

回転はコマンドラインで設定できます：

```
mcconfig -d -m -p esp/moddable_display_3 rotation=90
```

または、プロジェクトのマニフェストで設定できます：

```json
	"config": {
		"rotation": 90
	}
```

<a id="display-4"></a>
## Moddable Display 4

Moddable Display 4はModdable Four開発ボードを中心に構築されています。Moddable Display 4を使用した開発の詳細については、[Moddable Four Developer Guide](./moddable-four.md)を参照してください。

### ビルド

Moddable Display 4用にビルドするには、`nrf52/moddable_display_4`ビルドターゲットを使用します。例えば：

```
mcconfig -d -m -p nrf52/moddable_display_4
```

### ディスプレイの回転

Moddable Display 4には、縦向きのときに180度回転するModdable Four開発ボードが含まれています。デフォルトのビルドでは、自動的に回転が180度に設定されます。回転を0度に設定して実行することもできます。

回転はコマンドラインで設定できます：

```
mcconfig -d -m -p nrf52/moddable_display_4 driverRotation=0
```

または、プロジェクトのマニフェストで設定できます：

```json
	"config": {
		"driverRotation": 0
	}
```

実行時にJavaScriptで：

```js
screen.rotation = 0;
```

JavaScriptから回転を変更しても、画面はすぐには更新されません。プロジェクトは画面を再描画する必要があります。
