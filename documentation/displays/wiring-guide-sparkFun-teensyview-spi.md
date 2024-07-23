# SparkFun TeensyView 配線ガイド
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： 2018年10月23日

![TeensyView](images/teensyview.jpg)

## 仕様

| | |
| :---: | :--- |
| **部品** | Sparkfun - LCD-14048
| **サイズ** | 128 x 32
| **タイプ** | OLED、モノクロ
|**インターフェース** | SPI
|**ドライバ** | video [SSD1306](../../documentation/drivers/ssd1306/ssd1306.md)、タッチなし
|**入手可能性** | [SparkfunのTeensyView](https://www.sparkfun.com/products/14048)
|**説明** | 非常に小型のモノクロOLEDディスプレイ。Moddableは[標準](https://learn.sparkfun.com/tutorials/teensyview-hookup-guide)モードで設定されたTeensyViewを使用します。

## Moddableのサンプルコード

このディスプレイをテストするには、[balls](../../examples/piu/balls/)のサンプルが適しています。デバッグビルドを実行するには、以下のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/balls
mcconfig -d -m -p esp/teensyview
```

## ESP8266 ピン配置

| TeensyView ディスプレイ | ESP8266 | ESP8266 開発ボードラベル
| --- | --- | --- |
| GND | GND |
| 5 | GPIO 2 | (D4)
| 10 | GPIO 4 | (D2)
| 11 | GPIO 13 | (D7)
| 13 | GPIO 14 | (D5)
| 15 | GPIO 0 | (D3)
| 3v | 3.3V |

![汎用SPIディスプレイ](images/teensyview-wiring.jpg)
