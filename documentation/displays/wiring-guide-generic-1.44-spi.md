# 汎用 1.44インチ ディスプレイ配線ガイド
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： 2018年12月10日

![Generic SPI Display](images/generic-1.44-display.jpg)

## 仕様

| | |
| :---: | :--- |
| **サイズ** | 1.44インチ, 128 x 128
| **タイプ** | TFT LCD
| **インターフェース** | SPI
| **ドライバ** | video [ST7735](../../documentation/drivers/st7735/st7735.md), タッチなし
| **入手先** | [eBayでの汎用1.44インチSPIディスプレイ](https://www.ebay.com/sch/i.html?_odkw=spi+display&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR0.TRC0.H0.Xspi+display+1.44%22.TRS1&_nkw=spi+display+1.44%22&_sacat=0)
| **説明** | これらの安価なディスプレイはeBayやその他のリソースで入手可能です。

## Moddableのサンプルコード

このディスプレイをテストするには、[balls](../../examples/piu/balls/) サンプルが適しています。デバッグビルドを実行するには、以下のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/balls
mcconfig -d -m -p esp/generic_square
```

## ESP8266 ピン配置

| 1.44インチ ディスプレイ | ESP8266 | ESP8266 開発ボードラベル
| --- | --- | --- |
| VCC | 3.3V |
| GND | GND |
| CS | GPIO 15 | (D8)
| RESET | 3.3V |
| AO | GPIO 2 | (D4)
| SDA | GPIO 13 | (D7)
| SCK | GPIO 14 | (D5)
| LED | 3.3V |

![汎用 2.4"-2.8" 配線図](images/esp-generic-1.44-display.jpg)
