# Crystalfontz ePaper ディスプレイ配線ガイド
Copyright 2018 Moddable Tech, Inc.<BR>
Revised: October 23, 2018

![](./images/eink-display.jpeg)

## スペック

| | |
| :---: | :--- |
| **部品** | CFAP128296C0-0290 ([datasheet](https://www.crystalfontz.com/products/document/3660/CFAP128296C0-0290DatasheetReleaseDate2017-08-14.pdf))
| **サイズ**  | 2.9" 128x296
| **タイプ** | EPD (Electronic Paper Displays)
| **インターフェース** | SPI
| **ドライバ** | video [destm32s](../../documentation/drivers/destm32s/destm32s.md), no touch
| **入手可能性** | [128x296 ePaper Display](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)
| **説明** | これは1ビットの白/黒フルディスプレイ機能を持つTFTアクティブマトリックス電気泳動ディスプレイ（ePaper/E-Ink）です。<BR><BR>このディスプレイの利点の一つは非常に低い消費電力です。このePaperモジュールに電力を供給する必要があるのはディスプレイを更新している間だけです。一度画像が表示されると、電源を取り外してもディスプレイは適切に画像を表示し続けます。<BR><BR>ディスプレイとのインターフェースにはdestm32sアダプターボードを使用しました。詳細は[ディスプレイ製品](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)ページのCrystalfontz部品CFAP128296C0-E1-1を参照してください。

## Moddableのサンプルコード

このディスプレイをテストするには、[love-e-ink](../../examples/piu/love-e-ink/) のサンプルが適しています。デバッグビルドを実行するには、次のビルドコマンドを使用します：

```
cd $MODDABLE/examples/piu/love-e-ink/
mcconfig -d -m -p esp/crystalfontz_monochrome_epaper -r 270
```

## ESP8266 pinout

| eInk Display | ESP8266 | ESP8266 開発ボードラベル
| --- | --- | --- |
| 3 - GND | GND |
| 5 - 3.3v | 3.3v |
| 13 - SCK | GPIO 14 | (D5)
| 14 - SDI | GPIO 13 | (D7)
| 15 - DC | GPIO 2 | (D4)
| 16 - CS | GPIO 4 | (D2)
| 17 - BUSY | GPIO 5 | (D1)
| 18 - Reset | 3.3v |
| 19 - BUSSEL | GND |

![汎用 2.4"-2.8" 配線図](images/eink+adaptor+esp-wiring.png)
