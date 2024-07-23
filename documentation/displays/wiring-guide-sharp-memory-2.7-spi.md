# Sharp Memory ディスプレイ配線ガイド
Copyright 2018 Moddable Tech, Inc.<BR>
改訂： October 23, 2018

![](./images/Sharp_Memory_LCD_LS027B7DH01.jpg)

## スペック

| | |
| :---: | :--- |
| **部品** | LS027B7DH01A
| **サイズ** | 2.7", 240 x 400
| **タイプ** | シャープメモリTFT-LCD、モノクロ
| **インターフェース** | SPI
| **ドライバ** | video [ls013b4dn04](../../documentation/drivers/ls013b4dn04/ls013b4dn04.md), タッチなし
| **入手可能性** | [Digi-Keyで入手可能なシャープメモリ2.7" LCD](https://www.digikey.com/product-detail/en/sharp-microelectronics/LS027B7DH01A/425-2908-ND/5054067?utm_adgroup=Optoelectronics&gclid=Cj0KCQiAvrfSBRC2ARIsAFumcm-L2iz88RlcYf9Z1MU0J1ZW97VgAa0oPoDBgqYSIIRUyZnhGNURyY4aAjIgEALw_wcB)
| **説明** | シャープメモリディスプレイは、eInk（電子ペーパー）ディスプレイとLCDの融合です。eInkの非常に低い消費電力とLCDの高速リフレッシュレートを兼ね備えています。<BR><BR>Moddableは、ディスプレイFFCとインターフェースし、ESP8266の電力をディスプレイ駆動に必要な5Vにブーストするために、[Kuzyatechシャープメモリディスプレイブレイクアウトボード](https://www.tindie.com/products/kuzyatech/sharp-memory-lcd-breakout-a2/)を使用しています。

## Moddable サンプルコード

[balls](../../examples/piu/balls/)のサンプルは、このディスプレイをテストするのに適しています。デバッグビルドを実行するには、次のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/balls
mcconfig -d -m -p esp/sharp_memory
```

## ESP8266 ピン配置

| Sharp Memory Display | ESP8266 | ESP8266 開発ボードラベル |
| --- | --- | --- |
| VIN | 3.3V |
| GND | GND |
| EXTMODE |  |
| DISP | 3.3V |
| EXTCOMM |  |
| SCS | GPIO 4 | (D2) |
| SI | GPIO 13 | (D7) |
| SCK | GPIO 14 | (D5) |

![Generic SPI Display](images/wiring-Kuzyatech-sharp-2.7.jpg)
