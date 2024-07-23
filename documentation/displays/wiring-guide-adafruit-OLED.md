# Adafruit OLED ディスプレイ配線ガイド
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： 2018年10月23日

![](./images/adafruit-oled.jpg)

## スペック

| | |
| :---: | :--- |
| **部品** | Adafruit Product ID: 1431
| **サイズ** | 1.5", 128 × 128
| **タイプ** | OLED
| **インターフェース** | SPI
| **ドライバ** | video [SSD1351](../../documentation/drivers/ssd1351/ssd1351.md), タッチなし
| **入手可能性** | [1.5" OLED Breakout Board](https://www.adafruit.com/product/1431)
| **説明** | 優れた色、小型OLED。


## Moddableのサンプルコード

このディスプレイをテストするには、[countdown](../../examples/piu/countdown/) サンプルが適しています。デバッグビルドを実行するには、以下のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/countdown
mcconfig -d -m -p esp/adafruit_oled
```

## ESP8266 ピン配置

| Adafruit OLED | ESP8266 | ESP8266 開発ボード ラベル
| --- | --- | --- |
| GND | GND |
| VIN | 3.3v |
| 3Vo | 3.3v |
| OLEDCS | GPIO 15| (D8)
| RESET | 3.3v  |
| DC | GPIO 2 | (D4)
| SCK | GPIO 14 | (D5)
| MOSI | GPIO 13 | (D7)

![汎用2.4"-2.8"配線図](images/adafruit-oled-wiring2.png)
