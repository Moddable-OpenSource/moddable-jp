# Moddable SDK - ドライバーサンプル

Copyright 2017-2021 Moddable Tech, Inc.<BR>
改訂： 2021年11月3日

これらのサンプルは、Moddable SDKで利用可能なさまざまなセンサー、ディスプレイ、およびタッチドライバーを使用しています。このドキュメントでは、各サンプルとサポートされているコンポーネントの簡単な説明を提供します。各ドライバーのソースコードは[$MODDABLE/modules/drivers](../../modules/drivers)ディレクトリにあります。

***

### [`arducamframeserver`](./arducamframeserver)

[ArduCAM Mini 2MP SPIカメラ](http://www.arducam.com/arducam-mini-released/)からキャプチャされたJPEG画像を返すHTTPサーバー。

***

### [`arducamhttpput`](./arducamhttpput)

[ArduCAM Mini 2MP SPIカメラ](http://www.arducam.com/arducam-mini-released/)からキャプチャされた連続JPEG画像をHTTPサーバーに投稿するHTTPクライアント。

***

### [`arducampreview`](./arducampreview)

[ArduCAM Mini 2MP SPIカメラ](http://www.arducam.com/arducam-mini-released/)からキャプチャされた生の16ビットRGB画像を表示します。

***

### [`arducamstreamserver`](./arducamstreamserver)

HTTPサーバーが[ArduCAM Mini 2MP SPIカメラ](http://www.arducam.com/arducam-mini-released/)からキャプチャしたJPEG画像をマルチパートHTTPレスポンスとして返します。HTTPクライアントはリクエストで画像の幅と高さを指定することもできます。

***

### [`BMP180`](./BMP180)

HTTPサーバーがI<sup>2</sup>C [Bosch BMP180気圧/温度センサー](https://www.adafruit.com/product/1603)からのテキスト読み取りを返します。このアプリケーションはさらにホストをWi-Fiベースステーションとして構成します。

***

### [`bmp280`](./bmp280)

[BMP280温度/湿度センサー](https://www.adafruit.com/product/2651)を読み取ります。

***

### [`dotstar`](./dotstar)

カラー画像の各行を[Adafruit DotStar 144 LEDストリップ](https://www.adafruit.com/product/2242)に順次レンダリングします。[ドライバー](../../modules/drivers/dotstar)はSPIを介してストリップと通信します。

***

### [`ft6206calibrate`](./ft6206calibrate)

Moddable OneおよびModdable TwoのFT6206タッチスクリーンのキャリブレーションアプリケーション。

***

### [`ga1auv100wp`](./ga1auv100wp)
デュアルモードの[Sharp UV Light Sensor](http://www.sharp-world.com/products/device/lineup/selection/opto/receiving_light/opic/ultraviolet/index.html)から環境光とUV光の測定値を読み取ります。アプリケーションはセンサーの動作モードを設定します。[ドライバ](../../modules/drivers/ga1auv100wp)はSMBusプロトコルを使用してI<sup>2</sup>C経由でセンサーと通信します。

***

### [`gp2ap01vt00f`](./gp2ap01vt00f)
[Sharp Time-of-Flight Laser Ranging Sensor](http://www.sunnic.com/upfiles/tw_/sharp/GP2AP01VT00F.pdf)から100ms間隔で距離測定値を読み取ります。[ドライバ](../../modules/drivers/gp2ap01vt00f)はI<sup>2</sup>C経由でセンサーと通信します。

***

### [`HD44780i-i2c`](./HD44780i-i2c)

Hitachi HD44780キャラクターLCDモジュールの使用を示します。

***

### [`HMC5883L`](./HMC5883L)

I<sup>2</sup>C [SparkFun Triple Axis Magnetometer Breakout - HMC5883L](https://www.sparkfun.com/products/retired/10530)から250ms間隔でデジタルコンパスの測定値を読み取ります。
***

### [`hx711`](./hx711)

HX711ロードセルアンプを使用して重量を測定する方法を示します。

***

### [`kaluga-buttons`](./kaluga-buttons)

ESP32-S2搭載のKalugaボードでタッチパッドボタンとプッシュボタンの両方を使用する方法を示します。

> Kalugaボードの詳細については、[このブログ記事](https://blog.moddable.com/blog/espidf42/)を参照してください。

***

### [`lis3dh`](./lis3dh)

[Adafruit LIS3DH三軸加速度計](https://www.adafruit.com/product/2809)を100ms間隔で読み取ります。[ドライバ](../../modules/drivers/lis3dh)はI<sup>2</sup>Cを介して加速度計と通信します。

***

### [`lis3dhball`](./lis3dhball)

[Commodetto](../../documentation/commodetto/commodetto.md)アプリで、[Adafruit LIS3DH三軸加速度計](https://www.adafruit.com/product/2809)のセンサー値をリアルタイムで表示し、XおよびYの読み取り値に基づいてバウンシングボールをアニメーション化します。

***

### [`ls013b4dn04`](./ls013b4dn04)

Silicon Labs Geckoデバイス用の[Commodetto](../../documentation/commodetto/commodetto.md)アプリです。[このSharpミラーディスプレイ](../../documentation/displays/wiring-guide-sharp-memory-1.3-spi.md)とディープスリープモードの使用方法を示します。

***

### [`m5atom-echo`](./m5atom-echo)

M5Atom Echo開発ボードの内蔵ハードウェア機能を示すシンプルなサンプル。

***

### [`m5atom-lite`](./m5atom-lite)

M5Atom Lite開発ボードの内蔵ハードウェア機能を示すシンプルなサンプル。

***

### [`m5atom-matrix`](./m5atom-matrix)

M5Atom Matrix開発ボードの内蔵ハードウェア機能を示すシンプルなサンプル。

***

### [`m5core_ink-clock`](./m5core_ink-clock)

ePaperディスプレイに基本的なデジタル時計を描画し、1分ごとに更新します。バッテリー駆動のため、M5 PaperとM5Core Inkでのみ動作します。特別な電源管理技術を使用しており、M5Core Inkでの描画ちらつきを排除する技術も含まれています。

> このサンプルについての詳細は、[ブログ記事](https://moddable.com/blog/epaper/)「Getting the Most from ePaper Displays」を参照してください。

***

### [`m5stack-imu`](./m5stack-imu)

[Commodetto](../../documentation/commodetto/commodetto.md)アプリは、[M5Stack Fire](https://www.aliexpress.com/item/32847906756.html?spm=2114.12010615.8148356.14.11c127aeBNzJBb)用です。オンボードのInvenSense MPU-6050加速度計/ジャイロとXtrinsic MAG3110磁力計のデータに基づいてデバイスの動きを視覚化します。

***

### [`m5stickc-axp192`](./m5stickc-axp192)

[M5Stick-C](https://www.adafruit.com/product/4290) 用の [Commodetto](../../documentation/commodetto/commodetto.md) アプリ。画面の明るさを変更して電力を節約する方法を示します。

***

### [`m5stickc-imu`](./m5stickc-imu)

[M5Stick-C](https://www.adafruit.com/product/4290) 用の [Commodetto](../../documentation/commodetto/commodetto.md) アプリ。内蔵のSH200Q加速度計/ジャイロからのデータに基づいてデバイスの動きを視覚化します。

***

### [`m5stickc-pedometer`](./m5stickc-pedometer)

[M5Stick-C](https://www.adafruit.com/product/4290) 用の [Commodetto](../../documentation/commodetto/commodetto.md) アプリ。[M5Stick-C](https://www.adafruit.com/product/4290) の内蔵画面と加速度計を組み合わせて、シンプルな歩数計を作成します。

***

### [`m5stickc-rtc`](./m5stickc-rtc)

[M5Stick-C](https://www.adafruit.com/product/4290) 用の [Commodetto](../../documentation/commodetto/commodetto.md) アプリ。ハードウェアのバージョン2でRTC BM8563を使用する方法を示します。

***

### [`mcp23008`](./mcp23008)

単一のI<sup>2</sup>Cデバイスから複数のLEDを制御するために、[MCP23008 GPIOエクスパンダ](https://www.adafruit.com/product/593)を活用します。

***

### [`mcp23017`](./mcp23017)

単一のI<sup>2</sup>Cデバイスから複数のLEDを制御するために、[MCP23017 GPIOエクスパンダ](https://www.adafruit.com/product/732)を活用します。

***

### [`neopixel`](./neopixel)

Moddable SDKのNeoPixel APIの使用例を示します。このAPIは、M5Stack FireやLily Go TAudioなどの統合されたNeoPixelsを持つESP32ボードで動作します。また、NodeMCU ESP32のようなESP32ボードと、AdaFruitやSparkfunのNeoPixelボードを組み合わせることもできます。詳細については、[このブログ記事](https://blog.moddable.com/blog/neopixels/)を参照してください。

***

### [`neostrand`](./neostrand/docExample)

WS2811ストリングライトをアニメーション化するESP32のサンプルです。HTTPサーバーへのリクエストによりエフェクトを切り替えます。このサンプルとプロジェクトにNeostrandsを組み込む方法の詳細については、[このブログ記事](https://blog.moddable.com/blog/a-very-neopixel-christmas/)を参照してください。

***

### [`onewire`](./onewire)

ESP8266またはESP32に接続された1つ以上のDS18X20温度センサーから温度を読み取ります。[DS18B20](https://www.adafruit.com/product/374)および[DS18S20](https://www.mouser.com/ProductDetail/Maxim-Integrated/DS18S20%2BTR?qs=7H2Jq%252byxpJJMRp9%252bZx3PtA%3D%3D&gclid=EAIaIQobChMIwZzuu9OZ5AIViMBkCh0aoQ4tEAAYAiAAEgIZjfD_BwE)デジタル温度センサーでテストされています。これらのセンサーはDallas 1-Wireプロトコルを使用するため、通信には1つのデジタルピンだけが必要です。複数のセンサーを同じピンに接続することができます。

***

### [`peripherals/rtc`](./peripherals/rtc)

リアルタイムクロック（RTC）の使用を示す一連のサンプル。

***

### [`qm1h0p0073`](./qm1h0p0073)

[シャープ温湿度センサー](http://www.sharp-world.com/products/device/lineup/selection/rf/tem_hum/index.html)を100ms間隔で読み取ります。[ドライバー](../../modules/drivers/qm1h0p0073)はI<sup>2</sup>Cを介してセンサーと通信します。

***

### [`qwiictwist`](./qwiictwist)


デジタルRGBロータリーエンコーダである[SparkFun Qwiic Twist](https://www.sparkfun.com/products/15083)の使用方法を示します。エンコーダの回転数と押下回数を表示し、画面上のカラーピッカーを使用して色を変更できます。

***

### [`radiotest`](./radio/radiotest)

[Mighty Gecko](https://www.silabs.com/products/development-tools/wireless/mesh-networking/mighty-gecko-starter-kit)ラジオの使用方法を示します。

***

### [`redandblack`](./redandblack)

[DESTM32S](../../modules/drivers/destm32s) SPI ePaperディスプレイコントローラに対応するePaperディスプレイにビットマップをレンダリングします。例として、[Crystalfontz 128x296 3-Color ePaper Module](https://www.crystalfontz.com/product/cfap128296d00290-128x296-epaper-display)があります。

***

### [`sakuraio`](./sakuraio)

[Sakura IO LTEモジュール](http://python-sakuraio.readthedocs.io/en/latest/index.html)とインターフェースしてパケットの送受信やモジュールのステータスを読み取る方法を示します。[ドライバ](../../modules/drivers/sakuraio)はI<sup>2</sup>Cを介してセンサーと通信します。

***

### [`sensors`](./sensors)

TC53 [IOクラスパターン](../../documentation/io/io.md)で書かれた、以下のセンサーの一連のサンプル。

- `aht10`
- `am2320`
- `bmp180`
- `bmp280`
- `capacitiveMoisture`
- `ccs811`
- `hmc5883`
- `htu21d`
- `l3gd20`
- `lis3dh`
- `lm75`
- `lsm303dlhc`
- `mlx90614`
- `mpu6050`
- `mpu9250`
- `ntcThermistor`
- `sht3x`
- `shtc3`
- `si7020`
- `tmp102`
- `tmp117`
- `urm09`
- `zioQwiicSoilMoisture`

***

### [`si7021`](./si7021)

[SI7021温度/湿度センサー](https://www.adafruit.com/product/3251)を読み取ります。このセンサーは多くのSilicon Labs開発キットに組み込まれています。

***

### [`sonoffb1`](./sonoffb1)

[Sonoff B1電球](https://sonoff.ie/sonoff/54-sonoff-b1.html)の一連のサンプルアプリ。Sonoff B1にModdable SDKアプリケーションをインストールする方法については、[このブログ記事](https://blog.moddable.com/blog/hacking-sonoff-b1/)を参照してください。

***

### [`ssd1306`](./ssd1306)

画面をグレーのシェードで満たす[Commodetto](../../documentation/commodetto/commodetto.md)アプリ。SSD1306互換のディスプレイで動作します。例えば、[SparkFun TeensyView](../../documentation/displays/wiring-guide-sparkFun-teensyview-spi.md)など。

***

### [`TMP102`](./TMP102)
I<sup>2</sup>Cを介して100ms間隔で[Texas Instruments TMP102デジタル温度センサー](https://www.sparkfun.com/products/13314)を読み取ります。

***

### [`xpt2046calibrate`](./xpt2046calibrate)
[Moddable Zero](../../documentation/devices/moddable-zero.md)デバイスで使用される[XPT2046タッチスクリーンコントローラー](../../modules/drivers/xpt2046)をキャリブレーションする[Commodetto](../../documentation/commodetto/commodetto.md)アプリです。このアプリはキャリブレーションデータを永続ストレージに保存します。

***
