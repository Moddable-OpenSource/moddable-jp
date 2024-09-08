# DotStarディスプレイドライバ
Copyright 2018 Moddable Tech, Inc.<BR>
改訂：2018年1月3日

DotStarディスプレイコントローラーは、カラーLEDライトのストリングを動作します。ディスプレイは、さまざまな長さと構成で[Adafruit](https://learn.adafruit.com/adafruit-dotstar-leds/overview)から入手できます。

これらのLEDは厳密にはディスプレイではありませんが、ディスプレイの1行として見ることができます。長方形のグリッドに整理された状態で購入するオプションもあり、これはディスプレイのようなものです。

[dotstar](../../../examples/drivers/dotstar)のサンプルは、144個のLEDを持つDotStarストリングで動作します。画像を1行ずつスキャンして、DotStarストリング上のピクセルを更新します。

### DotStarをプロジェクトに追加する
プロジェクトにDotStarドライバを追加するには、そのマニフェストをインクルードします：

```
"include": [
	/* other includes here */
	"$(MODULES)/drivers/dotstar/manifest.json"
],
```

### ピクセルフォーマット
DotStarドライバは、16ビットカラー（`rgb565le`）ピクセルを必要とします。

	mcconfig -m -p esp -f rgb565le

### 定義
`defines`オブジェクトでは、オプションの`brightness`プロパティを設定できます。255が最も明るく、0がオフです。

```json
"defines": {
	"dotstar": {
		"brightness": 64
	}
}
```

### SPIの設定
`defines`オブジェクトには`spi_port`が含まれている必要があります。

```
"defines": {
	"dotstar": {
		/* other properties here */
		"spi_port": "#HSPI"
	}
}
```

`hz`プロパティは、存在する場合、SPIバスの速度を指定します。デフォルト値は20,000,000 Hzで、これはコントローラがサポートする最大SPI速度に近い値です。
