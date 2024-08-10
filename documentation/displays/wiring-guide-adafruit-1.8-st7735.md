# Adafruit 1.8" ST7735 配線ガイド
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： 2018年12月10日

![Generic SPI Display](images/adafruit-st7735-1.8.jpg)

## 仕様

| | |
| :---: | :--- |
| **部品** | Adafruit 製品ID: 2088
| **サイズ** | 1.8", 128 × 160
| **タイプ** | TFT LCD
| **インターフェース** | SPI
| **ドライバ** | video [ST7735](../../documentation/drivers/st7735/st7735.md), タッチなし
| **入手先** | [Adafruit 1.8" カラー TFT LCD ディスプレイ](https://www.adafruit.com/product/358)<BR>[HiLetgo 1.8" ST7735R](https://www.amazon.com/gp/product/B00LSG51MM/)
| **説明** | カラフルで明るいディスプレイ、4線式SPI。


## Moddableのサンプルコード

このディスプレイをテストするには、[balls](../../examples/piu/balls/) サンプルが適しています。デバッグビルドを実行するには、以下のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/balls
mcconfig -d -m -p esp/adafruit_st7735
```

## ESP8266 ピン配置

| Adafruit 1.8" TFT | ESP8266 | ESP8266 開発ボードラベル
| --- | --- | --- |
| LITE | 3.3v |
| MISO | GPIO 12 | (D6)
| SCK | GPIO 14 | (D5)
| MOSI | GPIO 13 | (D7)
| TFT_CS | GPIO 15| (D8)
| DC | GPIO 2 | (D4)
| RESET | 3.3v  |
| VCC| 3.3v |
| GND | GND |

![汎用2.4"-2.8"配線図](images/adafruit-st7735-1.8-wiring.png)
