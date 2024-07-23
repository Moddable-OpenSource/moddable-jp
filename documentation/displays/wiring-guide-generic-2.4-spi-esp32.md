# 汎用 2.4インチ & 2.8インチ ディスプレイ (抵抗膜タッチ) 配線ガイド - ESP32
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： 2018年10月23日

![](./images/spi-touch-display.jpg)

## 仕様

| | |
| :---: | :--- |
| **サイズ** | 2.4インチ & 2.8インチ, 320 x 240
| **タイプ** | TFT LCD
| **インターフェース** | SPI
| **ドライバ** | ビデオ [ILI9341](../../documentation/drivers/ili9341/ili9341.md), タッチ XPT2046
| **入手可能性** | [eBayの汎用SPIディスプレイ](https://www.ebay.com/sch/i.html?_odkw=spi+display+2.4&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR0.TRC0.H0.Xspi+display+2.4+touch.TRS0&_nkw=spi+display+2.4+touch&_sacat=0)
| **説明** | これらの安価なディスプレイはeBayや他のリソースで入手可能です。<BR><BR>注意: タッチバージョンと非タッチバージョンがあり、非常に似ています。


## Moddableのサンプルコード

このディスプレイをテストするには、[drag](../../examples/piu/drag/) サンプルが適しています。デバッグビルドを実行するには、以下のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/drag
mcconfig -d -m -p esp32/moddable_zero
```

## ESP32 ピン配置

| ILI9341 ディスプレイ | ESP32 |
| --- | --- |
| SDO / MISO | GPIO 12  |
| LED | 3.3V |
| SCK | GPIO 14 |
| SDI / MOSI | GPIO 13 |
| CS | GPIO 15 |
| DC | GPIO 2 |
| RESET | 3.3V |
| GND | GND |
| VCC | 3.3V |
| T_DO | GPIO 12 |
| T_DIn | GPIO 13 |
| T_CLK | GPIO 14 |
| T_IRQ | GPIO 23 |
| T_CS | GPIO 18 |

![ESP32 - Generic 2.4"-2.8" wiring](images/ESP32+display-wiring2.png)
