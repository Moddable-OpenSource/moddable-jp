# Die Cut
Copyright 2016 Moddable Tech, Inc.<BR>
更新日: 2016年11月22日

## はじめに

シリアルインターフェース（SPI）でマイクロコントローラに接続された画面でアニメーションを実現するには、フレーム間で変化するピクセル数を最小化することが重要です。

> 参考として、外観やレイアウトの変更によって画面の無効化と更新が行われる際に、各フレームで何が起こっているかを以下に示します：
>
> - ダーティ領域（描画に変化のある領域）が再描画が必要な範囲を蓄積します。
> - オブジェクト階層が一度走査され、画面を更新するコマンドリストが構築されます。
> - ダーティ領域とコマンドリストが矩形とディスプレイリストに分解されます。
> - 矩形とディスプレイリストが処理され、ピクセルのブロックが画面に送信されます。

`die`オブジェクトは、アニメーションとトランジションが領域を使ってコンテンツを「型抜き」することを可能にする`layout`オブジェクトです。領域操作のおかげで、`die`オブジェクトは再描画と更新する領域を最小化します。実際に変化するピクセルは少ないながら、フルスクリーンアニメーションの錯覚を与えることができます。

## 例

静的な例から始めましょう：

	let TestContainer = Container.template($ => ({
		left:0, right:0, top:0, bottom:0, skin:blueSkin,
		contents: [
			Die($, {
				left:0, right:0, top:0, bottom:0,
				Behavior: class extends Behavior {
					onDisplaying(die) {
						let w = die.width, h = die.height;
						die.empty()
							.or(10, 10, 40, 40)
							.or(w - 50, 10, 40, 40)
							.or(w - 50, h - 50, 40, 40)
							.or(10, h - 50, 40, 40)
							.xor(30, 30, w - 60, h - 60)
							.sub(70, 70, w - 140, h - 140)
							.cut();
					}
				},
				contents: [
					Content($, { left:0, right:0, top:0, bottom:0, skin:whiteSkin,
				]
			}),
		]
	}));

`empty`、`or`、`xor`、`sub`メソッドは、領域を構築するためのチェーン可能な操作です。`cut`メソッドは領域を変更します。

`TestContainer`をアプリケーションに追加すると、次のような表示になります：

![](./../assets/die-cut/die-cut.png)

しかし、もちろん`die`オブジェクトは主にアニメーションとトランジションを構築するために興味深いものです。Piuライブラリには例があります：WipeTransitionとCombTransition。

「ベネチアンブラインド」トランジションを構築してみましょう：

```js
class VenitianBlindTransition extends Transition {
	constructor(duration) {
		super(duration);
	}
	onBegin(container, former, current) {
		container.add(current);
		this.container = container;
		this.die = new Die(null, {});
		this.die.attach(current);
	}
	onEnd(container, former, current) {
		this.die.detach();
		container.remove(former);
	}
	onStep(fraction) {
		let die = this.die;
		die.empty();
		let width = die.width;
		let height = die.height;
		let y = 0;
		let step = height >> 3;
		let delta = Math.round(fraction * step);
		for (let i = 0; i < 8; i++) {
			die.or(0, y, width, delta);
			y += step;
		}
		die.cut();
	}
}
```

`attach`と`detach`メソッドは、一時的に`die`オブジェクトをオブジェクト階層に挿入することを可能にします。トランジションの各ステップで、領域が変化して「ベネチアンブラインド」を段階的に閉じます。

## リファレンス

`die`オブジェクトは、領域を使ってそのコンテンツを「型抜き」することを可能にする`layout`オブジェクトです。`die`オブジェクトは2つの領域を維持します：

- 利用可能な操作が構築する作業領域
- `die`オブジェクトのコンテンツをクリップするクリップ領域

両方の領域は最初は空です。

#### プロトタイプの説明

プロトタイプは`Layout.prototype`から継承します。

##### `Die.prototype.and(x, y, width, height)`

> `x, y, width, height` ローカル矩形、ピクセル単位
>
> 矩形を作業領域と交差させます。thisを返します。

##### `Die.prototype.attach(content)`

> `content ` アタッチする`content`オブジェクト
>
> 指定された`content`オブジェクトをそのコンテナ内でこの`die`オブジェクトに置き換え、その`content`オブジェクトをこの`die`オブジェクトに追加することで、`die`オブジェクトをオブジェクト階層にバインドします。

##### `Die.prototype.cut()`

>  作業領域を現在の領域にコピーします。作業領域とクリップ領域の差分のみを再描画対象とします。

##### `Die.prototype.empty()`

> 作業領域を空にします。`this`を返します。

##### `Die.prototype.detach()`

> この`die`オブジェクトから最初の`content`オブジェクトを削除し、この`die`オブジェクトをそのコンテナ内で削除された`content`オブジェクトに置き換えることで、この`die`オブジェクトをオブジェクト階層からアンバインドします。

##### `Die.prototype.fill()`

> 作業領域をこの`die`オブジェクトの境界に設定します。`this`を返します。

##### `Die.prototype.or(x, y, width, height)`

> `x, y, width, height` ローカル矩形、ピクセル単位
>
> 矩形を作業領域と包含的に結合します。`this`を返します。

##### `Die.prototype.set(x, y, width, height)`

> `x, y, width, height` ローカル矩形、ピクセル単位
>
> 作業領域を矩形に設定します。`this`を返します。

##### `Die.prototype.sub(x, y, width, height)`

> `x, y, width, height` ローカル矩形、ピクセル単位
>
> 作業領域から矩形を減算します。`this`を返します。

##### `Die.prototype.xor(x, y, width, height)`

> `x, y, width, height` ローカル矩形、ピクセル単位
>
> 作業領域と矩形を排他的に結合します。`this`を返します。



