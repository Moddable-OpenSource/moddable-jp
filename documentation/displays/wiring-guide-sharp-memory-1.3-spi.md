# シャープメモリスクリーン 1.3" 配線ガイド
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： 2018年12月10日

![シャープメモリディスプレイ 1.3"](images/sharp-memory-display-1.3.jpg)

## 仕様

| | |
| :---: | :--- |
| **部品** | LS013B4DN04 LCDモジュール ([データシート](https://cdn-shop.adafruit.com/datasheets/LS013B4DN04-3V_FPC-204284.pdf))
| **サイズ** | 1.3", 96 x 96, 168 x 144
| **タイプ** | LCD, モノクローム
| **インターフェース** | SPI
| **ドライバー** | ビデオ [ls013b4dn04](../../documentation/drivers/ls013b4dn04/ls013b4dn04.md), タッチなし
| **入手可能性** |  [Adafruitのシャープメモリモノクロームディスプレイ](https://www.adafruit.com/product/3502)<BR>[Digi-Keyのシャープメモリ反射ディスプレイ](https://www.digikey.com/product-detail/en/1393/1528-1183-ND/5353643)<BR>[Sparkfunのシャープメモリ反射ディスプレイ](https://www.sparkfun.com/products/retired/13192)
| **説明** | これらの興味深いディスプレイは、eInkの超低消費電力とLCDの高速リフレッシュレートを兼ね備えています。[古い96 x 96バージョン](https://www.adafruit.com/product/1393)は、アクティブピクセルが反射する鏡のようなディスプレイを持っています。

## Moddable サンプルコード

[transitions](../../examples/piu/transitions/)のサンプルは、このディスプレイのテストに適しています。デバッグビルドを実行するには、次のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/transitions
mcconfig -d -m -p esp/sharp_memory_square
```

## ESP8266 ピン配置

| 1.3" Memory Display | ESP8266 | ESP8266 開発ボードラベル
| --- | --- | --- |
| 3v3 | 3.3V |
| GND | GND |
| CLK | GPIO 14 | (D5)
| DI | GPIO 13 | (D7)
| CS | GPIO 15 | (D8)

![Generic SPI Display](images/sharp-memory-1.3-wiring.jpg)
