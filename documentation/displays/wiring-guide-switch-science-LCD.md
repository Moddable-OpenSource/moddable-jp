# Switch Science Reflective LCD Display Wiring Guide
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： October 23, 2018

![](./images/switch-science-LCD-relective-display.jpg)

## スペック

| | |
| :---: | :--- |
| **部品** | 2858: JDI - REFLCD - 128
| **サイズ** | 1.28", 176 × 176
| **タイプ** | カラー反射 LCD (バックライトなし)
| **インターフェース** | SPI
| **ドライバ** | video [lpm013m126a](../../documentation/drivers/lpm013m126a/lpm013m126a.md), タッチなし
| **入手先** | [1.28" Switch Science Color reflective LCD](https://translate.google.com/translate?hl=en&sl=ja&tl=en&u=https%3A%2F%2Fwww.switch-science.com%2Fcatalog%2F2874%2F)
| **説明** | Moddable はこのディスプレイを東京で購入しました。[ここに](https://translate.googleusercontent.com/translate_c?depth=1&hl=en&rurl=translate.google.com&sl=ja&sp=nmt4&tl=en&u=https://www.switch-science.com/catalog/2858/&usg=ALkJrhijtlYZnC4qJ2sRkLE3mkVZujVU1w)このディスプレイに関する情報があります。

## Moddable のサンプルコード

このディスプレイをテストするには、[balls](../../examples/piu/balls/) サンプルが適しています。デバッグビルドを実行するには、以下のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/balls
mcconfig -d -m -p esp/switch_science_reflective_lcd
```

## ESP8266 ピン配置

| Switch Science LCD | ESP8266 | ESP8266 開発ボードラベル
| --- | --- | --- |
| 14 - SCLK | GPIO 14 | (D5)
| 13 - SI | GPIO 13 | (D7)
| 15 - SCS | GPIO 15 | (D8)
| DISP | 3.3v |
| GND | GND |
| VIN | 3.3v |

![汎用 2.4"-2.8" 配線図](images/switch-science-esp-wiring.png)

```
