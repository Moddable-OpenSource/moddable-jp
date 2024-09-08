# DESTM32S ディスプレイドライバ
Copyright 2017 Moddable Tech, Inc.<BR>
改訂： 2017年12月30日

DESTM32Sディスプレイコントローラは、3種類のePaperディスプレイを動作します。これらのディスプレイは、Crystalfontzを含むさまざまなサイトから入手可能で、私たちのテストユニットもここから購入しました。これらは小さなアダプターボード（[good-display.com](http://www.good-display.com/products_detail/productId=327.html)から）を使用しますが、ディスプレイコントローラはePaperディスプレイ自体の一部です（いわゆるチップオンガラス）。

- 122 x 250白黒（[こちら](https://www.crystalfontz.com/product/cfap122250a00213-epaper-display-122x250-eink)で入手可能）
- 128 x 296白黒および赤（[こちら](https://www.crystalfontz.com/product/cfap128296d00290-128x296-epaper-display)で入手可能）
- 104 x 212白黒、グレーおよび赤（[こちら](https://www.crystalfontz.com/product/cfap104212b00213-epaper-104x212-eink)で入手可能）

白黒ディスプレイは、赤ピクセルをサポートする2つのディスプレイとは異なるコントローラを使用します。白黒ディスプレイは約4フレーム毎秒で更新されますが、赤ピクセルを含むディスプレイは1フレームあたり約20秒（つまり1分あたり3フレーム）かかります。

Moddable SDKのePaperディスプレイドライバは、LCDディスプレイと同じAPIを使用します。しかし、比較的遅いリフレッシュレートやその他の更新要件のため、通常はディスプレイの特性に合わせて設計された専用アプリケーションで使用されます。

[love-e-ink](../../../examples/piu/love-e-ink)のサンプルは、白黒のePaperディスプレイで動作するように設計されています。これは、長い全画面リフレッシュを必要とせずにディスプレイの一部を継続的に更新する方法を示しています。[redandblack](../../../examples/drivers/redandblack)のサンプルは、赤いピクセルを持つePaperディスプレイで動作するように設計されています。これは、黒、白、赤のピクセルを含む画像のスライドショーを循環させます。

### プロジェクトにDESTM32を追加する
SSD1351ドライバをプロジェクトに追加するには、そのマニフェストをインクルードします：

```
"include": [
	/* other includes here */
	"$(MODULES)/drivers/destm32s/manifest.json"
],
```

CommodettoまたはPiuを使用する場合、マニフェストの`config`オブジェクトの`screen`プロパティを`destm32s`に設定して、SSD1351をデフォルトのディスプレイドライバにします。タッチ入力がないため、タッチドライバ名を空の文字列に設定して無効にします。

```json
"config": {
	"screen": "destm32s",
	"touch": ""
},
```

### ピクセルフォーマット
DESTM32Sドライバは、入力として8ビットのグレーまたは、ディスプレイがカラーをサポートしている場合（例：赤）、8ビットのカラーピクセルを必要とします。`mcconfig`を使用してビルドする際、コマンドラインでピクセルフォーマットを`gray256`または`rgb332`に設定します：

	mcconfig -m -p esp -r 90 -f gray256
	mcconfig -m -p esp -r 90 -f rgb332

### 定義
`defines`オブジェクト内でピクセルの`width`と`height`を宣言します。マニフェストで指定されたディスプレイの寸法は、使用されるディスプレイコントローラを選択するため、正しく設定することが重要です。

122 x 250の白黒の場合：

```json
"defines": {
	"destm32s": {
		"width": 122,
		"height": 250
	}
}
```

128 x 296の黒、白、赤の場合：

```json
"defines": {
	"destm32s": {
		"width": 128,
		"height": 296
	}
}
```

104 x 212の黒、白、グレー、赤の場合：

```json
"defines": {
	"destm32s": {
		"width": 104,
		"height": 212
	}
}
```

### SPIの設定
`defines`オブジェクトには、`spi_port`、および`DC`、`CS`、`BUSY`のピン番号を含める必要があります。`RST`ピンが提供されている場合、コンストラクタが呼び出されたときにデバイスがリセットされます。`cs_port`、`dc_port`、`rst_port`、または`busy_port`プロパティが提供されていない場合、それらはデフォルトでNULLになります。


```
"defines": {
	"ssd1351": {
		/* other properties here */
		"cs_pin": 4,
		"dc_pin": 2,
		"rst_pin": 0,
		"busy_pin": 5,
		"spi_port": "#HSPI"
	}
}
```

`hz` プロパティが存在する場合、SPIバスの速度を指定します。デフォルト値は500,000 Hzで、これはコントローラーがサポートする最大SPI速度に近い値です。
