# 汎用 2.4インチ & 2.8インチ ディスプレイ (抵抗膜タッチ) 配線ガイド - ESP8266
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： 2018年10月23日

![](./images/spi-touch-display.jpg)

## 仕様

| | |
| :---: | :--- |
| **サイズ** | 2.4" & 2.8", 320 x 240
| **タイプ** | TFT LCD
| **インターフェース** | SPI
| **ドライバー** | video [ILI9341](../../documentation/drivers/ili9341/ili9341.md), タッチ XPT2046
| **入手可能性** | [eBayの汎用SPIディスプレイ](https://www.ebay.com/sch/i.html?_odkw=spi+display+2.4&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR0.TRC0.H0.Xspi+display+2.4+touch.TRS0&_nkw=spi+display+2.4+touch&_sacat=0)
| **説明** | これらの安価なディスプレイはeBayや他のリソースで入手可能です。 <BR><BR>注意: タッチバージョンと非タッチバージョンがあり、非常に似ています。

> 現時点では、ModdableのサンプルコードにはディスプレイのSDカードサポートが含まれていません。

## Moddableのサンプルコード

このディスプレイをテストするには、[drag](../../examples/piu/drag/) サンプルが適しています。デバッグビルドを実行するには、次のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/drag
mcconfig -d -m -p esp/moddable_zero
```

## ESP8266 ピン配置

| ILI9341 ディスプレイ | ESP8266 | ESP8266 開発ボードラベル |
| --- | --- | --- |
| SDO / MISO | GPIO 12 | (D6) |
| LED | 3.3V |
| SCK | GPIO 14 | (D5) |
| SDI / MOSI | GPIO 13 | (D7) |
| CS | GPIO 15 | (D8) |
| DC | GPIO 2 | (D4) |
| RESET | 3.3V |
| GND | GND |
| VCC | 3.3V |
| T_DO | GPIO 12 | (D6) |
| T_DIn | GPIO 13 | (D7) |
| T_CLK | GPIO 14 | (D5) |
| T_IRQ | GPIO 16 | (D0) |
| T_CS | GPIO 0 | (D3) |

![汎用 2.4"-2.8" 配線図](images/esp-generic-2.4-display.png)
```
