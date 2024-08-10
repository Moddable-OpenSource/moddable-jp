# マニフェストでのdefineの使用
Copyright 2017 Moddable Tech, Inc.<BR>
改訂： 2017年12月11日

## はじめに

マニフェストのdefinesブロックは、C言語の#defineプリプロセッサステートメントのセットを作成します。definesブロックは、ハードウェアドライバーのC言語実装を設定するために設計されています。definesを使用すると、実行時ではなくビルド時に設定が行われます。ビルド時の設定は、通常、使用されていないネイティブコードをC言語の条件付きコンパイルとリンカーのデッドストリッピングによって削除することができるため、コードが小さくなり、実行が速くなります。

この静的な設定アプローチは、実行時にすべてのパラメータを設定する動的なメカニズムの代わりに使用されます。このようなアプローチはLinuxシステムで一般的ですが、コードと時間のオーバーヘッドが高くなります。マイクロコントローラのデプロイメントでは、静的な設定が最適であり、静的な設定（ディスプレイのピクセル形式と回転、JavaScriptメモリヒープ、利用可能なモジュールなど）を使用するModdable SDKと一致しています。

### ILI9341ディスプレイドライバーのための#define
以下は、ILI9341ディスプレイドライバーのためのdefinesブロックの使用を示しています。

```
{
	"build": {
		"BUILD": "$(MODDABLE)/build",
		...
	},
	"creation": {
		"static": 32768,
		...
	},
	...
	"defines": {
		"ili9341": {
			"width": 240,
			"height": 320,
			"cs": {
				"port": null,
				"pin": 4
			},
			"dc": {
				"port": null,
				"pin": 2
			},
			"spi": {
				"port": "#HSPI"
			}
		}
	},
	...
}
```

JSONはドライバーの設定パラメーター（ここでは幅と高さ）と接続（CS、DC、SPI）を定義します。生成されるCの#defineステートメントは以下の通りです：

```c
#define MODDEF_ILI9341_WIDTH (240)
#define MODDEF_ILI9341_HEIGHT (320)
#define MODDEF_ILI9341_CS_PORT NULL
#define MODDEF_ILI9341_CS_PIN (4)
#define MODDEF_ILI9341_DC_PORT NULL
#define MODDEF_ILI9341_DC_PIN (2)
#define MODDEF_ILI9341_SPI_PORT "HSPI"
```

GPIOピン接続（CS、DC）にはポート名とピン番号の両方が含まれます。この例では、ホストターゲットデバイスはポート名を使用しないため、nullとして残されています。SPIポートは名前で定義されており、ここではESP8266の「ハードウェア」SPIバスのための文字列"HSPI"です。文字列に"#"プレフィックスが付いているのは、#defineステートメントの値が引用符で囲まれた文字列であるべきことを示しています。

### オプションの定義
デバイスドライバーはオプションの#defineをサポートすることがあります。例えば、ILI9341では、SPIバスの速度を設定したり、水平および垂直方向の反転を有効にしたり、リセットピンをサポートすることができます。

```
"defines": {
	"ili9341": {
		"width": 240,
		"height": 320,
		"hz": 10000000,
		"flipX": true,
		"flipY": false,
		...
		"rst": {
			"port": null,
			"pin": 0
		},
		...
```

オプションの#defineをサポートするために、ドライバー実装はデフォルトの値と動作を提供します。ILI9341のCコードは、hz、flipX、flipYの定義について以下のようにデフォルト値を実装しています：

```c
#ifndef MODDEF_ILI9341_HZ
	#define MODDEF_ILI9341_HZ (40000000)
#endif
#ifndef MODDEF_ILI9341_FLIPX
	#define MODDEF_ILI9341_FLIPX (false)
#endif
#ifndef MODDEF_ILI9341_FLIPY
	#define MODDEF_ILI9341_FLIPY (false)
#endif
```

多くのデプロイメントでは、電源投入時の自動リセットが十分であるため、ILI9341ディスプレイを明示的にリセットする必要はありません。リセットピンは、マニフェストでリセットピンが定義されている場合にのみ動作するように実装されています。

```c
#ifdef MODDEF_ILI9341_RST_PIN
	SCREEN_RST_INIT;
	SCREEN_RST_ACTIVE;
	modDelayMilliseconds(10);
	SCREEN_RST_DEACTIVE;
	modDelayMilliseconds(1);
#endif
```

flipXとflipYの実装は、#defineを直接使用しており、どちらかの値がfalseの場合、コンパイラが対応するコードを削除できます。

```js
data[0] = 0x48;
if (MODDEF_ILI9341_FLIPX)
	data[0] ^= 0x40;
if (MODDEF_ILI9341_FLIPY)
	data[0] ^= 0x80;
ili9341Command(sd, 0x36, data, 1);
```

ESP8266プラットフォームはGPIOピンのポート名を使用しないため、ILI9341は指定されていない場合、ポートをNULLに設定します：

```c
#ifndef MODDEF_ILI9341_CS_PORT
	#define MODDEF_ILI9341_CS_PORT NULL
#endif
#ifndef MODDEF_ILI9341_DC_PORT
	#define MODDEF_ILI9341_DC_PORT NULL
#endif
```

これにより、ピン接続の記述がより簡潔になります：

```
"defines": {
	"ili9341": {
		...
		"cs": {
			"pin": 4
		},
		"dc": {
			"pin": 2
		},
		...
	}
},
```

または単純に：

```
"defines": {
	"ili9341": {
		...
		"cs_pin": 4,
		"dc_pin": 2,
		...
	}
},
```

### プラットフォームのオーバーライド
各ドライバーにおいて、設定（例：width、height、flipX、flipY、hz）は通常、全てのターゲットプラットフォームにわたって一貫しています。しかし、接続はほぼ常に異なります。#definesブロックは、プラットフォーム固有のブロックを追加し、他の値を上書きすることを許可するmcconfigプラットフォームブロックのパターンに従います。

全デバイスに共通のILI9341設定の一部は以下のようになります：

```json
"defines": {
	"ili9341": {
		"width": 240,
		"height": 320,
		"flipX": true,
		"flipY": true,
	}
},
```

これらはESP8266、Gecko、Zephyrプラットフォームのプラットフォーム定義ブロックです：

```
"platforms": {
	"esp": {
		"modules": {
			...
		},
		...
		"defines": {
			"cs": {
				"pin": 4
			},
			"dc": {
				"pin": 2
			},
			"spi": {
				"port": "#HSPI"
			}
		},
	},
	"gecko": {
		"modules": {
			...
		},
		...
		"defines": {
			"cs": {
				"port": "#gpioPortD",
				"pin": 3
			},
			"dc": {
				"port": "#gpioPortD",
				"pin": 5
			},
			"spi": {
				"port": "#gpioPortD"
			}
		},
	},
	"zephyr": {
		"modules": {
			...
		},
		...
		"defines": {
			"dc": {
				"port": "#GPIO_2",
				"pin": 12
			},
			"spi": {
				"port": "#SPI_0"
			}
		}
	}
}
```

### アプリケーションのオーバーライド
アプリケーションは、特定のドライバーの定義パラメータをオーバーライドすることができます。例えば、特定のデバイス設定でディスプレイへのSPI接続速度を遅くする必要がある場合、それはアプリケーションのマニフェストで指定できます：

```json
{
	"include": "../all.json",
	"modules": {
		"*": "./main"
	},
	"defines": {
		"ili9341": {
			"hz": 500000
		}
	}
}
```

### 設定データ
definesブロックは、数値、ブール値、文字列を定義するために最もよく使用されます。これは、デバイスレジスタのような複雑な設定のために、数値の配列を定義するためにも使用できます。

```json
"defines": {
	"ili9341": {
		"width": 240,
		"height": 320,
		"registers": [0, 54, 32, 99, 255, 255, 255, 0 65]
	}
},
```

registersプロパティは静的に初期化されたC言語の配列として出力されます：

```c
	#define MODDEF_ILI9341_REGISTERS {0, 54, 32, 99, 255, 255, 255, 0, 65}
```

これは次のように使用できます：

```c
static const uint8_t gRegisters = MODDEF_ILI9341_REGISTERS;
int i;

for (i = 0; i < sizeof(gRegisters); i++)
	;	// gRegisters[i]
```

JSONは10進数の値のみを許可するため、16進数および2進数の値は10進数に変換する必要があります。代替案として、レジスタのプロパティをC言語の構文で文字列として定義することができます：

		"registers": "{0x00, 0x38, 0x20, 0x63, 0xFF, 0xFF, 0xFF, 0x00 0x41}"

JSONは文字列リテラル内で改行を許可しません。複数行の文字列リテラルを許可するために、文字列の配列が複数行の`#define`に変換されます。配列の各要素は別々の行として出力されます。以下のJSON、

```json
"registers": [
	"0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,",
	"0xCF, 3, 0x00, 0xC1, 0X30,",
	"0xE8, 3, 0x85, 0x00, 0x78"
]
```

はこの`#define`を生成します：

```c
#define MODDEF_ILI9341_REGISTERS \
0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02, \
0xCF, 3, 0x00, 0xC1, 0X30, \
0xE8, 3, 0x85, 0x00, 0x78
```

以下のように使用できます：

```c
static const uint8_t gRegisters[] = {
	MODDEF_ILI9341_REGISTERS
};
``


### 複数のデバイス
マニフェストには複数のデバイスのための#defineデータを含むことができます：

```json
"defines": {
	"ili9341": {
		"width": 240,
		"height": 320
	},
	"xpt2046": {
		"width": 240,
		"height": 320,
		"hz": 1000000
	}
},
```

## 元のアイデア
マニフェストに#defineデータを含めるアイデアと構文はShotaro Uchida氏によって提案されました。
