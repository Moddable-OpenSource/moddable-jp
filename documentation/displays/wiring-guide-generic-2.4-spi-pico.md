# 汎用 2.4インチ & 2.8インチ ディスプレイ (抵抗膜タッチ) 配線ガイド - Pico
Copyright 2021 Moddable Tech, Inc.<BR>
Revised: March 4, 2022

![](./images/spi-touch-display.jpg)

## スペック

| | |
| :---: | :--- |
| **サイズ** | 2.4" & 2.8", 320 x 240
| **タイプ** | TFT LCD
| **インターフェース** | SPI
| **ドライバ** | video [ILI9341](../../documentation/drivers/ili9341/ili9341.md), touch XPT2046 **(not supported yet)**
| **入手先** | [Generic SPI Displays on eBay](https://www.ebay.com/sch/i.html?_odkw=spi+display+2.4&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR0.TRC0.H0.Xspi+display+2.4+touch.TRS0&_nkw=spi+display+2.4+touch&_sacat=0)
| **説明** | これらの安価なディスプレイはeBayや他のリソースで入手可能です。<BR><BR>注意: タッチバージョンと非タッチバージョンがあり、非常に似ています。

> 現時点のModdableのサンプルコードではSDカードのサポートは含まれていません。

## Moddableのサンプルコード

このディスプレイをテストするには、[drag](../../examples/piu/drag/) サンプルが適しています。デバッグビルドを実行するには、以下のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/balls
mcconfig -d -m -p pico/ili9341
```

## ili9341 モジュールのピン配置

これはili9341モジュールの典型的なレイアウトです。

![](./images/ili9341-pinout.png)

## Raspberry Pi Pico のピン配置

| ILI9341 ディスプレイ | Pico
| --- | --- |
| VCC | 3.3V
| GND | GND
| CS | GPIO 9
| RESET | GPIO 12
| DC | GPIO 7
| SDI / MOSI | GPIO 11
| SCK | GPIO 10
| LED | 3.3V
<!--
| SDO / MISO | GPIO 8
| T_CLK | GPIO 10
| T_CS | GPIO 13
| T_DIn | GPIO 11
| T_DO | GPIO 8
| T_IRQ | GPIO 14
-->
```
