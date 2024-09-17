# Pins
Copyright 2017-2023 Moddable Tech, Inc.<BR>
改訂： 2023年4月10日

## 目次

* [Digital](#digital)
* [Monitor](#monitor)
* [Analog](#analog)
* [PWM](#pwm)
* [I2C](#i2c)
* [SMBus](#smbus)
* [Servo](#servo)
* [RMT](#rmt)
* [SPI](#spi)

<a id="digital"></a>
## Digital クラス

- **ソースコード:** [digital](../../modules/pins/digital)
- **関連するサンプル:** [blink](../../examples/pins/blink), [button](../../examples/pins/button)

`Digital` クラスはGPIOピンへのアクセスを提供します。

```js
import Digital from "pins/digital";
```

### `constructor(dictionary)`
### `constructor([port], pin, mode)`

Digitalコンストラクタは、指定されたGPIOピンへの接続を確立します。ピンを構成する方法は、辞書を渡す方法と、個別の引数を渡す方法の2つがあります。

辞書を使用する場合、`pin` と `mode` プロパティが必要です。portプロパティはオプションで、デフォルト値は `NULL` です。

ピン番号とポート名はデバイスに依存します。

```js
let pin = new Digital({pin: 4, mode: Digital.Input});
```

引数を個別に指定する場合、`port` と `pin` 引数はピンを指定します。portが指定されていない場合、デフォルト (`NULL`) が使用されます。`mode` パラメータはインスタンスの `mode` 関数に渡され、GPIOハードウェアを構成します。

```js
let pin = new Digital(4, Digital.Input);
```

特定のポートでGPIOピンを開くには、オプションの最初の引数を使用してDigitalコンストラクタを使用します。

```js
let blink = 1;
let led = new Digital("gpioPortName", 5, Digital.Output);

Timer.repeat(id => {
	blink = blink ^ 1;
	led.write(blink);
}, 500);
```

次のモード値が利用可能です。

```js
Digital.Input
Digital.InputPullUp
Digital.InputPullDown
Digital.InputPullUpDown
Digital.Output
Digital.OutputOpenDrain
```

***

### `static read(pin)`

`read` 関数はピンを `Digital.Input` モードに設定し、指定されたピンの値をサンプリングして0または1を返します。デフォルトポートが使用されます。

次の例では、ピン0を入力として構成し、入力に接続されたボタンが押されているかどうかをテストします。ESP8266 NodeMCUボードでは、ピン0は内蔵のユーザーボタンです。

```js
if (Digital.read(0))
	trace("button is pressed\n");
```

***

### `static write(pin)`

`write` 関数はピンを `Digital.Output` モードに設定し、その値を0または1に設定します。デフォルトポートが使用されます。

次の例では、ピン5を出力として設定し、1秒ごとに点滅させます。ESP8266 NodeMCUボードでは、ピン5は内蔵LEDです。

```js
let blink = 1;

Timer.repeat(id => {
	blink = blink ^ 1;
	Digital.write(5, blink);
}, 500);
```

***

### `read()`

ピンの状態をサンプリングし、0または1として返します。

静的な `Digital.read` と `Digital.write` はすべてのピンモードを設定することはできません。完全な設定を行うには、Digitalコンストラクタを使用します。例えば、内部プルアップ抵抗を使用するように入力を設定します。

```js
let button = new Digital(0, Digital.InputPullUp);
trace(`button state is ${button.read()}`);
```

***

### `write(value)`
ピンの現在の値を0または1に設定します。

```js
let led = new Digital(0, Digital.Output);
led.write(1);
```

***

### `mode(mode)`

mode関数はピンのモードを設定します。すべてのピンがすべてのモードをサポートしているわけではないので、ハードウェアのドキュメントを参照してください。

```js
pin.mode(Digital.Input);
```

***

### `wakeEdge and onWake() callback`

デジタル入力トリガーでデバイスをディープスリープからウェイクアップすることをサポートするプラットフォームでは、`Digital` コンストラクタに `onWake` コールバックを提供できます。`onWake` コールバックは、そのピンがリセットとなる理由でウェイクアップした後、最初にピンがインスタンス化されたときに呼び出されます。ウェイクアップエッジイベントトリガーは `wakeEdge` プロパティで設定されます。

次の例では、内部プルアップ抵抗を使用して、入力ピン7が立ち下がりエッジでウェイクアップをトリガーするように設定します。

```js
let digital = new Digital({
	pin: 7,
	mode: Digital.InputPullUp,
	wakeEdge: Digital.WakeOnFall,
	onWake() {
		// take action based on digital wake-up trigger
	}
});
```

次のエッジイベントがウェイクアップトリガーの設定に利用できます。

```js
Digital.WakeOnRise = (1 << 6);
Digital.WakeOnFall = (1 << 7);
```

**注**: 現在、nRF52デバイスのみがデジタル入力トリガーからのディープスリープウェイクアップをサポートしています。

### `close()`

`close` 関数は、`Digital` インスタンスに関連付けられたリソースを解放します。

***

<a id="monitor"></a>
## Monitor クラス

- **ソースコード:** [monitor](../../modules/pins/digital/monitor)
- **関連する例:** [monitor](../../examples/pins/monitor)

`Monitor` クラスは、デジタル入力ピンの状態変化を追跡します。インスタンスは、立ち上がりエッジおよび/または立ち下がりエッジイベントでトリガーするように設定されます。トリガーイベントが発生すると、コールバック関数が呼び出されます。さらに、インスタンスはトリガーされたイベントの総数をカウントします。

```js
import Monitor from "pins/digital/monitor";
```

**注意**: 効率の理由から、実装は各ピンに対して単一のモニターのみをサポートする場合があります。スクリプトは特定のピンに対して単一のモニターのみをインスタンス化する必要があります。

### `constructor(dictionary)`

コンストラクターは、インスタンスを構成するプロパティを含む単一の辞書を取ります。`Monitor`辞書は`Digital`コンストラクターの拡張であり、`pin`、`port`、および`mode`引数を受け入れます。`Monitor`インスタンスには入力モードのみが許可され、インスタンス化後にモードを変更することはできません。

```js
let monitor = new Monitor({pin: 0, port: "B", mode: Digital.Input, edge: Monitor.Rising});
```

***

### `onChanged()` コールバック

スクリプトは、指定されたエッジイベントが発生したときに呼び出される`onChanged`コールバックをインスタンスに設定する必要があります。

```js
monitor.onChanged = function() {
	trace(`Pin value is ${this.read()}\n`);
}
```

`onChanged`関数は、イベントトリガーの直後にできるだけ早く呼び出されます。これは、イベントが発生した後、スクリプトを安全に呼び出せるポイントにディスパッチするのに時間がかかる場合があるため、イベントが発生してからしばらく後に発生する可能性があります。この遅延のため、ピンの値が再び変わっている可能性があります。このため、スクリプトはコールバックが呼び出されたときのピンの値を仮定せず、代わりにインスタンスの`read`関数を呼び出して現在の値を取得する必要があります。

***

### `read()`

`read` 関数はピンの現在の値を返します。

```js
let value = this.read();
```

***

### `wakeEdge と onWake() コールバック`

デジタル入力トリガーでデバイスがディープスリープからウェイクアップすることをサポートするプラットフォームでは、`Monitor` コンストラクタに `onWake` コールバックを提供できます。`onWake` コールバックは、そのピンがリセット理由としてウェイクアップした後、最初にピンがインスタンス化されたときに呼び出されます。ウェイクアップエッジイベントトリガーは `wakeEdge` プロパティによって設定されます。

以下のエッジイベントがウェイクアップトリガーの設定に利用可能です。

```js
Digital.WakeOnRise = (1 << 6);
Digital.WakeOnFall = (1 << 7);
```

次の例では、内部プルアップ抵抗を使用して、入力ピン7が立ち下がりエッジでウェイクアップをトリガーするように設定しています：

```js
let monitor = new Monitor({
	pin: 7,
	mode: Digital.InputPullUp,
	wakeEdge: Digital.WakeOnFall,
	onWake() {
		// take action based on digital monitor wake-up trigger
	}
});
```

**Note**: 現在、nRF52デバイスのみがデジタル入力トリガーからのディープスリープウェイクアップをサポートしています。

### `close()`

`close` 関数は `Monitor` インスタンスに関連付けられたリソースを解放します。

```js
monitor.close();
```

モニターは、その `close` 関数が呼び出されるまでガベージコレクションの対象にはなりません。

***

### `rises` と `falls` プロパティ

`rises` と `falls` プロパティは、インスタンスが作成されてから対応するトリガーイベントの総数を返します。イベントは `onChanged` コールバックが呼び出されるよりも速くトリガーされることがあるため、`onChanged` コールバックを使用してトリガーイベントの正確な数を維持することは不可能です。

```js
let triggers = this.rises + this.falls;
```

***

### 例: 通知を受け取る
次の例は、立ち上がりエッジと立ち下がりエッジイベントでコールバックを受け取る方法を示しています。

```js
let monitor = new Monitor({pin: 4, mode: Digital.InputPullUp, edge: Monitor.Rising | Monitor.Falling});
monitor.onChanged = function() {
	trace(`Pin value is ${this.read()}\n`);
}
```

***

<a id="analog"></a>
## Analog クラス

- **ソースコード:** [analog](../../modules/pins/analog)
- **関連する例:** [simple-analog](../../examples/pins/simpleAnalog/), [analog](../../examples/pins/analog/)

`Analog` クラスはアナログ入力ピンへのアクセスを提供します。

```js
import Analog from "pins/analog";
```

### `constructor(dictionary)`

`Analog` の辞書には `pin` プロパティが必要です。アナログ入力トリガーでディープスリープからのウェイクアップをサポートするプラットフォームでは、アナログ入力をウェイクアップトリガーとして設定するために、`onWake`、`wakeCrossing`、および `wakeValue` プロパティも必要です。詳細については、以下の `onWake() コールバック` セクションを参照してください。

以下は、アナログ入力インスタンスを設定し、アナログ値を読み取る方法を示しています：

```js
let analog = new Analog({ pin: 5 });
trace(`Analog value is ${analog.read()}\n`);
```

**注意**: 現在、nRF52プラットフォームはコンストラクタとインスタンスの読み取り機能のみをサポートしています。他のすべてのプラットフォームでは、Analogクラスは静的関数のみを提供します。

### `static read(pin)`

`read` 関数は指定されたピンの値をサンプリングし、0から1023の値を返します。

ピン番号はデバイスに依存します：
 - ESP8266 NodeMCUボードには単一のアナログ入力ピンがあり、アナログピン番号は0です。
 - ESP32では、関連するGPIO番号ではなくアナログチャネル番号を使用します。ESP32およびESP32-S2のGPIO番号とアナログチャネルのマッピングは [ESP-IDF ADC ドキュメント](https://docs.espressif.com/projects/esp-idf/en/v4.2-beta1/esp32/api-reference/peripherals/adc.html#_CPPv414adc1_channel_t) で確認できます。ESP32ではADC1のみがサポートされています。

次の例では、アナログ温度センサーをサンプリングし、その結果を摂氏度に変換します。

	let temperature = (Analog.read(0) / 1023) * 330 - 50;
	trace(`Temperature is ${temperature} degrees celsius\n`);

この例は、広く利用可能な低コストの[温度センサー](https://learn.adafruit.com/tmp36-temperature-sensor/overview)で動作します。

> 注意: TMP36温度センサーの出力電圧範囲は0.1Vから2.0Vであり、ESP8266のアナログからデジタルコンバーターの入力電圧範囲は0Vから1.0Vです。ESP8266を損傷しないようにするためには、TMP36の出力電圧の大きさを減少させるために電圧分圧器を使用する必要があります。ESP8266 NodeMCUボードにはこの目的のための電圧分圧器が備わっています。他のボードには電圧分圧器がない場合があります。

***

### `onWake() コールバック`

デバイスがアナログ入力トリガーでディープスリープからウェイクアップすることをサポートするプラットフォームでは、`Analog`コンストラクターに`onWake`コールバックを提供できます。`onWake`コールバックは、そのピンがリセット理由としてウェイクアップした後に初めてインスタンス化されたときに呼び出されます。ウェイクアップトリガーは`wakeValue`および`wakeCrossing`プロパティによって構成されます。

次の例では、アナログ入力が512を超えるか下回るときにウェイクアップをトリガーするように、入力ピン5を設定します：

```js
let analog = new Analog({
	pin: 5,
	wakeValue: 512,
	wakeCrossing: Analog.CrossingUpDown,
	onWake() {
		for (let i = 0; i < 10; ++i) {
			// take action based on analog wake-up trigger
		}
	}
});
```

ウェイクアップトリガーを設定するために利用可能なクロッシングモード値は次の通りです。

```js
Analog.CrossingUp = 0;
Analog.CrossingDown = 1;
Analog.CrossingUpDown = 2;
```

**注意:** 現在、nRF52デバイスのみがアナログ入力トリガーからのディープスリープウェイクアップをサポートしています。

<a id="pwm"></a>
## PWM クラス

- **ソースコード:** [pwm](../../modules/pins/pwm)
- **関連する例:** [tricolor-led](../../examples/pins/tricolor-led/)

`PWM`クラスはPWM出力ピンへのアクセスを提供します。

```js
import PWM from "pins/pwm";
```

### `constructor(dictionary)`

PWMコンストラクタは、使用するピン番号を含む辞書を受け取ります。ピン番号はデバイスに依存します。

辞書にはオプションで`port`プロパティを含めることができます。ポートが提供されない場合、デフォルトのポート（`NULL`）が使用されます。

```js
let pwm = new PWM({ pin: 12 });
```

***

### `write(value)`

ピンの現在の値を設定します。値は0から1023の間である必要があります。

```js
pwm.write(512);
```

***

### `close()`

`close`関数は、`PWM`インスタンスに関連付けられたリソースを解放します。

```js
pwm.close();
```

***

<a id="i2c"></a>
## I2C クラス

- **ソースコード:** [i2c](../../modules/pins/i2c)
- **関連する例:** [bmp180](../../examples/drivers/bmp180/), [lis3dh](../../examples/drivers/lis3dh)

`I2C`クラスは、ピンのペアに接続されたI2Cバスへのアクセスを提供します。

```js
import I2C from "pins/i2c";
```

### `constructor(dictionary)`

I2Cコンストラクタは、クロックとデータ（それぞれ`scl`と`sda`）に使用するピン番号、およびターゲットデバイスのI2Cアドレスを含む辞書を受け取ります。ピン番号はデバイスに依存します。

```js
let sensor = new I2C({sda: 5, scl: 4, address: 0x48});
```

コンストラクタの辞書には、このインスタンスを使用する際のI2Cバスの速度を指定するためのオプションの`hz`プロパティがあります。

```js
let sensor = new I2C({sda: 5, scl: 4, address: 0x48, hz: 1000000});
```

コンストラクタ辞書には、I2Cクロックストレッチングのタイムアウトをマイクロ秒（μs）で指定するためのオプションの `timeout` プロパティもあります。このプロパティのサポートはデバイスに依存します。

```js
let sensor = new I2C({sda: 5, scl: 4, address: 0x48, timeout: 600});
```

多くのデバイスにはデフォルトのI2Cバスがあります。これらのデバイスでは、デフォルトのI2Cバスを使用するために辞書から `sda` および `scl` パラメータを省略することができます。

```js
let sensor = new I2C({address: 0x48});
```

***

### `read(count [, buffer])`

`read` 関数はターゲットデバイスから `count` バイトを読み取り、それらを `Uint8Array` で返します。`count` の最大値は34です。

```js
let bytes = sensor.read(4);
```

オプションのバッファパラメータは、結果を格納するために使用される `ArrayBuffer` を提供するために使用されます。複数の読み取りに同じバッファを使用することは、メモリ割り当てを排除し、ガベージコレクタの実行頻度を減らすことで、より効率的になる可能性があります。

```js
let bytes = new UInt8Array(4);
sensor.read(4, bytes.buffer);
sensor.read(2, bytes.buffer);
sensor.read(3, bytes.buffer);
```

***

### `write(...value [, stop])`

`write` 関数はターゲットデバイスに最大40バイトを書き込みます。`write` 関数は複数の引数を受け取り、それらを連結してデバイスに送信します。値は数値（バイトとして送信される）、配列、TypedArrays、および文字列（UTF-8エンコードされたテキストバイトとして送信される）である可能性があります。

`write` 関数は、書き込みが完了したときにストップ条件を送信するかどうかを示すオプションの最後のブール引数 `stop` を受け取ります。最後の引数がブール値でない場合、ストップ条件が送信されます。

```js
let sensor = new I2C({address: 0x48});
sensor.write(0);
```
***

### 例: I2C温度センサーの読み取り

次の例では、アドレス0x48の[温度センサー](https://www.sparkfun.com/products/11931)にピン4と5を接続してI2C接続をインスタンス化します。

```js
let sensor = new I2C({sda: 5, scl: 4, address: 0x48});
sensor.write(0);					// set register address 0
let value = sensor.read(2);	// read two bytes

// convert data to celsius
value = (value[0] << 4) | (value[1] >> 4);
if (value & 0x800) {
	value -= 1;
	value = ~value & 0xFFF;
	value = -value;
}
value *= 0.0625;

trace(`Celsius temperature: ${value}\n`);
```

***

<a id="smbus"></a>
## SMBus クラス

- **ソースコード:** [smbus](../../modules/pins/smbus)
- **関連する例:** [HMC5883L](../../examples/drivers/HMC5883L)

`SMBus` クラスは、I2Cデバイスで一般的に使用される[システム管理バス](https://en.wikipedia.org/wiki/System_Management_Bus)プロトコルのサポートを実装します。`SMBus` クラスは追加の関数を持つ `I2C` クラスを拡張します。

```js
import SMBus from "pins/smbus";
```

### `constructor(dictionary)`

`SMBus` コンストラクタは `I2C` コンストラクタと同じです。

***

### `readByte(register)`

指定されたレジスタから1バイトのデータを読み取ります。

***

### `readWord(register)`

指定されたレジスタから始まる16ビットのデータ値を読み取ります。データはリトルエンディアンのバイト順で送信されると仮定されます。

***

### `readBlock(register, count [, buffer])`
指定されたレジスタから始まるcountバイトのデータを読み取ります。最大34バイトのデータを読み取ることができます。データは `Uint8Array` で返されます。`readBlock` は、`I2C` クラスの `read` 関数のオプションの `buffer` 引数と同じように動作するオプションの `buffer` 引数を受け取ります。

***

### `writeByte(register, value)`

指定されたレジスタに1バイトのデータを書き込みます。

***

### `writeWord(register, value)`

指定されたレジスタから始まる16ビットのデータ値を書き込みます。値はリトルエンディアンのバイト順で送信されます。

***

### `writeBlock(register, ...value)`

指定されたレジスタから始まる提供されたデータ値を書き込みます。値の引数は `I2C` クラスの `write` 関数の引数と同じように処理されます。

***


### 例: 三軸磁力計の初期化

次の例では、[三軸磁力計](https://www.sparkfun.com/products/10530)へのSMBus接続を確立します。接続が確立されると、デバイスIDを確認して、期待されるデバイスモデルであることを確認します。その後、デバイスを連続測定モードに設定します。

```js
let sensor = new SMBus({address: 0x1E});

let id = sensor.readBlock(10, 3);
id = String.fromCharCode(id[0]) + String.fromCharCode(id[1]) + String.fromCharCode(id[2]);
if ("H43" != id)
	throw new Error("unable to verify magnetometer id");

sensor.writeByte(2, 0);	// continuous measurement mode
```

***


<a id="servo"></a>
## クラス Servo

- **ソースコード:** [servo](../../modules/pins/servo)
- **関連する例:** [servo](../../examples/pins/servo), [servo-sweep](../../examples/pins/servo-sweep)

`Servo`クラスはデジタルピンを使用してサーボモーターを制御します。APIはArduinoのServoクラスを基に設計されています。

```js
import Servo from "pins/servo";
```

### `constructor(dictionary)`

Servoコンストラクタは辞書を受け取ります。`pin`プロパティは必須で、使用するデジタルピンを指定します。`min`および`max`プロパティはオプションで、マイクロ秒単位のパルス持続時間の範囲を指定します。

```js
let servo = new Servo({pin: 5, min: 500, max: 2400});
```

***

### `write(degrees)`

`write` 呼び出しは、指定された度数にサーボの位置を変更します。小数点以下の度数もサポートされています。

次の例では、ピン4にサーボをインスタンス化し、45度に回転させます。

```js
let servo = new Servo({pin: 4});
servo.write(45);
```

***

### `writeMicroseconds(us)`

`writeMicroseconds` 呼び出しは、パルス幅の持続時間をマイクロ秒単位で設定します。提供された値は `min` と `max` の値によって固定されます。

サーボの実装は、回転角度に比例したマイクロ秒数のデジタル信号をパルスします。スクリプトは、より高い精度のために度数の代わりに信号パルスのマイクロ秒数を提供することができます。

```js
servo.writeMicroseconds(1000);
```

***

<a id="rmt"></a>
## RMT クラス

- **ソースコード:** [rmt](../../modules/pins/rmt)
- **関連する例:** [write](../../examples/pins/rmt/write), [read](../../examples/pins/rmt/read)

`RMT` クラスは、RMT（リモートコントロール）モジュールへのアクセスを提供します。`RMT` はESP32およびESP32-S2マイクロコントローラーでサポートされています。

```js
import RMT from "pins/rmt";
```

### `constructor(dictionary)`

RMTコンストラクタはRMTモジュールを初期化し、指定されたピンに関連付けます。

RMTコンストラクタは辞書を受け取ります。`pin`プロパティは必須で、RMTモジュールに関連付けるピンを指定します。

```js
let rmt = new RMT({pin: 17});
```

コンストラクタの辞書にはいくつかのオプションプロパティもあります：

- `channel`プロパティは、このインスタンスで使用するRMTチャンネルを指定し、デフォルト値は`0`です。
- `divider`プロパティはRMTティックを生成するために使用されるクロック分周器を指定し、範囲は`1`から`255`で、デフォルト値は`255`です。
- `direction`プロパティは、このRMTを入力として使用するか出力として使用するかを指定します。入力には`"rx"`を、出力には`"tx"`を使用します。デフォルトは出力です。

```js
let rmt = new RMT({pin: 17, channel: 1, divider: 100});
```

RMTを入力として使用する場合、辞書には追加のオプションプロパティがあります：

- `filter`プロパティは、指定されたティック数より短い受信パルスをフィルタリングするようにRMTモジュールを設定し、範囲は`0`から`255`で、デフォルト値は`0`です。
- `timeout`プロパティは、RMTモジュールがアイドルモードに入るトリガーとなるパルスの長さ（ティック単位）を指定し、範囲は`0`から`65_535`で、デフォルト値は`5000`です。
- `ringbufferSize`プロパティは、RMTモジュールの入力バッファのサイズ（バイト単位）を設定し、デフォルト値は`1024`です。

```js
let inputRMT = new RMT({pin: 17, channel: 3, divider: 100, direction: "rx", filter: 100, timeout: 7000, ringbufferSize: 512});
```

***

### `write(firstValue, durations)`

`write` 関数は、RMTモジュールを介して設定されたピンに1と0（つまり高電圧と低電圧）の交互のパルスを送信します。`firstValue` パラメータは、シーケンス内の最初のパルスが `1` か `0` かを指定します。`durations` パラメータは、ピンに書き込むシーケンス内の各パルスの持続時間（RMTモジュールのティック単位）を指定する整数の配列でなければなりません。

次の例では、ピンを2000ティックの間高にし、その後5000ティックの間低にし、これを合計3回繰り返し、最後に10000ティックの高パルスで終了します。

```js
rmt.write(1, [2000, 5000, 2000, 5000, 2000, 5000, 10000]);
```

書き込みは非同期で実行されるため、`write` は即座に戻ります。書き込みが完了すると、`onWriteable` コールバックが呼び出され、別の `write` を発行することができます。

***

### `onWriteable()` コールバック

スクリプトは、RMTモジュールがデータを書き込む準備ができたときに呼び出される `onWriteable` コールバックをインスタンスに設定することができます。

```js
rmt.onWritable = function() {
	rmt.write(1, [2000, 5000, 2000, 5000, 2000, 5000, 10000]);
}
```

`onWriteable` 関数は、RMTモジュールが初期化され、最初の `write` 呼び出しを受け取る準備ができたときに一度呼び出されます。その後、書き込みが完了し、RMTモジュールが新しいシーケンスを送信する準備ができるたびに呼び出されます。

***

### `read(buffer)`

`read` 関数は、`buffer` 引数で提供されたArrayBufferにRMTデータを返します。RMTモジュールのリングバッファから `buffer.byteLength` バイトまでがArrayBufferに返されます。

`read` は3つのプロパティを持つオブジェクトを返します：

- `buffer` プロパティは、RMTモジュールによって読み取られた受信パルス持続時間シーケンスで満たされた、引数として `read` に提供されたArrayBufferです。
- `count` プロパティは、シーケンス内で受信されたパルスの総数です。
- `phase` プロパティは、シーケンス内の最初の項目の論理レベル (`0` または `1`) です — `buffer` 内の後続のパルスは論理レベルが交互に変わります。

```js
const data = new Uint16Array(512);

Timer.repeat(() => {
	let result = inputRMT.read(data.buffer);
	if (result.count) {
		let value = result.phase;
		for (let i = 0; i < result.count; i++) {
			trace(`Pulse of value ${value} for duration ${data[i]}\n`);
			value ^= 1;
		}
	}
}, 20);
```

***

### `close()`

`close` 関数は、`RMT` インスタンスに関連付けられたリソースを解放します。

```js
rmt.close();
```

***

<a id="spi"></a>
## SPI クラス

現在、SPIにアクセスするためのJavaScript APIはありません。
