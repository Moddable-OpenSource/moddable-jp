#  BuyDisplay 2.8インチ CTP ディスプレイ配線ガイド - ESP8266
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： December 6, 2018

![Generic SPI Display](images/spi_serial_2.8_inch_320x240_tft_lcd_display_module_ili9341_arduino_stm32_1.jpg)


## スペック

| | |
| :---: | :--- |
| **部品** | ER-TFTM028-4 LCD w/Capacitive Touch Screen Module ([datasheet](http://www.buydisplay.com/download/manual/ER-TFTM028-4_Datasheet.pdf))
| **サイズ** | 2.8", 320 x 240
| **タイプ** | TFT LCD
| **インターフェース** | SPI
| **ドライバ** | video [ILI9341](../../documentation/drivers/ili9341/ili9341.md), touch FT6206
| **入手先** | [2.8" BuyDisplay CTP display](http://www.buydisplay.com/default/2-8-inch-tft-touch-shield-for-arduino-w-capacitive-touch-screen-module)
| **説明** | このBuyDisplayスクリーンは高度に設定可能で、さまざまなタッチ・モジュール、主電源電圧、接続タイプで注文できます。このサンプルは3.3V CTPバージョンです。

## 設定

このディスプレイは多くの設定に対応しています。この例では、ModdableはディスプレイのIMO設定を4線式SPIをサポートするように構成しました。ER-TFTM028-4の [datasheet](http://www.buydisplay.com/download/manual/ER-TFTM028-4_Datasheet.pdf)をご覧ください。以下のジャンパー設定は、PCB上のドラッグ半田パッドが開いているか閉じているかです。

| IMO Mode | ジャンパー設定 |
| --- | --- |
| 4線式SPIインターフェース | J2, J3, J4, J5 ショート<BR>J1, J6, J7, J8 オープン<BR>R1-R10=0R<BR>R19=0R<BR>R21-R28=0R<BR>R17, R18, R20 未はんだ付け |

**ディスプレイ入力電圧**
TFTM028-4は、はんだジャンパーを介して5vまたは3.3vで電源を供給するように設定できます。詳細は[データシート](http://www.buydisplay.com/download/manual/ER-TFTM028-4_Datasheet.pdf)の表4.3を参照してください。このガイドでは、ディスプレイは3.3v入力で動作するように設定されています。ディスプレイが5vに設定されている場合、ディスプレイヘッダーのピン2には5v入力が必要です。

## Moddable サンプルコード

このディスプレイをテストするには、[drag](../../examples/piu/drag/)のサンプルが適しています。デバッグビルドを実行するには、次のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/drag
mcconfig -d -m -p esp/buydisplay_ctp
```

## ESP8266 ピン配置

| BuyDisplay CTP ディスプレイ | ESP8266 | ESP8266 開発ボードラベル |
| --- | --- | --- |
| 1 - VSS | GND |
| 2 - VDD | 3.3V |
| 21 - リセット | 3.3V |
| 23 - CS | GPIO 15 | (D8) |
| 24 - SCK | GPIO 14 | (D5) |
| 25 - DC | GPIO 2 | (D4) |
| 27 - SDI | GPIO 13 | (D7) |
| 28 - SDO | GPIO 12 | (D6) |
| 30 - CPT SCL | GPIO 4 | (D2) |
| 31 - CPT SDA | GPIO 5 | (D1) |

![汎用 2.4"-2.8" 配線図](images/buydisplay+esp-wiring.png)
