# DotStar 配線ガイド
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： 2018年12月11日

![Generic SPI Display](images/dotstar.jpg)

## 仕様

| | |
| :---: | :--- |
| **サイズ** | 144 LED ストリップ (5mm x 5mm LEDs)
| **タイプ** | ADA102 LED
| **インターフェース** | SPI
| **ドライバ** | [dotstar](../../documentation/drivers/dotstar/dotstar.md)
| **入手可能性** | [Adafruit DotStar](https://www.adafruit.com/product/2241)
| **説明** | Adafruit は DotStar デジタル LED に ADA102 LED を使用しています。これにより、2 線式 SPI で個々の LED を制御できます。これらの LED は厳密にはディスプレイではありませんが、ディスプレイの 1 行として見ることができます。ディスプレイのように長方形のグリッドに整理されたものを購入するオプションもあります。

## Moddable のサンプルコード

[dotstar](../../examples/drivers/dotstar/) サンプルは144 LEDストリップで動作します。画像を1行ずつスキャンしてDotStarストリングのピクセルを更新します。

```
cd $MODDABLE/examples/drivers/dotstar
mcconfig -d -m -p esp
```

## ESP8266 ピン配置

**注意:** Dotstarは5Vのデータ信号を必要とするため、信号線をロジックコンバータを通して5Vにブーストする必要があります。[74AHCT125](https://www.adafruit.com/product/1787) または同様のシフターが使用できます。

| Dotstar Display | ESP8266 | ESP8266 Devboardラベル |
| --- | --- | --- |
| 5V | N/A (5V供給に接続) |
| GND | GND |
| CI | GPIO 14/SCLK | D5 |
| DI | GPIO 13/MOSI | D7 |

![ESP32 - 汎用 2.4"-2.8" 配線図](images/dotstar-wiring.png)
