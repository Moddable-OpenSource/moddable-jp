# BLE
Copyright 2017-2024 Moddable Tech, Inc.<BR>
改訂： 2024年9月10日

## このドキュメントについて
このドキュメントは、Moddable SDKのBluetooth Low Energy (BLE) モジュールについて説明します。Espressif ESP32、Silicon Labs Blue Gecko、Qualcomm QCA4020、およびNordic nRF52デバイスで、クライアント（マスター）とサーバー（スレーブ）の両方の役割がサポートされています。

> **注:** BLEサーバー/スレーブは、一般的にBLEペリフェラルとも呼ばれます。このドキュメントでは、*サーバー*、*スレーブ*、および*ペリフェラル*という用語は同義に使用されています。

## 目次
* [プロジェクトにBLEを追加する](#addingble)
* [BLEの使用](#usingble)
* [BLEクライアント](#bleclient)
	* [BLEClientクラス](#classbleclient)
	* [Clientクラス](#classclient)
	* [Serviceクラス](#classservice)
	* [Characteristicクラス](#classcharacteristic)
	* [Descriptorクラス](#classdescriptor)
	* [Advertisementクラス](#classadvertisement)
	* [Bytesクラス](#classbytes)
* [BLEサーバー](#bleserver)
	* [BLEServerクラス](#classbleserver)
	* [GATTサービス](#gattservices)
* [BLEセキュリティ](#blesecurity)
	* [SMクラス](#classsm)
* [BLEホワイトリスト](#blewhitelisting)
	* [GAPWhitelistクラス](#classgapwhitelist)
* [ESP32プラットフォームでのBLEアプリ](#esp32platform)
* [Blue GeckoプラットフォームでのBLEアプリ](#geckoplatform)
* [BLE例示アプリ](#exampleapps)

<a id="addingble"></a>
## プロジェクトにBLEを追加する
BLEクライアントとサーバーは、組み込みデバイスで利用可能な限られたメモリとストレージに対応するために、別々のモジュールとして実装されています。BLEアプリケーションは、BLEクライアントまたはBLEサーバーのいずれかをインスタンス化しますが、両方を同時に使用することはありません。

BLEクライアントとBLEサーバー用の事前作成されたマニフェストが利用可能です。これらをプロジェクトで使用するには、アプリケーションのマニフェストの`include`配列に追加してください。

プロジェクトにBLEクライアントを追加するには：

```jsonc
"include": [
	/* other includes here */
	"$(MODDABLE)/modules/network/ble/manifest_client.json"
],
```

プロジェクトにBLEサーバーを追加するには：

```jsonc
"include": [
	/* other includes here */
	"$(MODDABLE)/modules/network/ble/manifest_server.json"
],
```

<a id="usingble"></a>
## BLEの使用

BLEアプリケーションは通常、`BLEClient`または`BLEServer`をサブクラス化し、その後にデバイスやアプリケーション固有のコードを追加します。次のBLEクライアントの例では、`BLEClient`をサブクラス化して、周辺機器をスキャンして発見します：

```javascript
import BLEClient from "bleclient";

class Scanner extends BLEClient {
	onReady() {
		this.startScanning();
	}
	onDiscovered(device) {
		let scanResponse = device.scanResponse;
		let completeName = scanResponse.completeName;
		if (completeName)
			trace(completeName + "\n");
	}
}

let scanner = new Scanner;
```

次のBLEサーバーの例では、`BLEServer`をサブクラス化して、デバイスをBLE健康体温計周辺機器としてアドバタイズします：

```javascript
import BLEServer from "bleserver";
import {uuid} from "btutils";

class HealthThermometerService extends BLEServer {
	onReady() {
		this.startAdvertising({
			advertisingData: {flags: 6, completeName: "Moddable HTM", completeUUID16List: [uuid`1809`, uuid`180F`]}
		});
	}
}

let htm = new HealthThermometerService;
```

<a id="bleclient"></a>
## BLE クライアント
BLEクライアントは、1つ以上のBLEペリフェラルに接続できます。 同時接続の最大数はビルド時に定義され、最終的には基盤となる組み込みプラットフォームがサポートするものによって制限されます。 デフォルトの最大接続数である2を上書きするには、マニフェストの `defines` セクションで `max_connections` 値を上書きします：

```json
"defines": {
	"ble": {
		"max_connections": 3
	}
},
```

BLEクライアントは通常、BLEペリフェラルから通知を受け取るために次の手順を実行します：

1. ペリフェラルのスキャンを開始する
2. スキャン応答でアドバタイズされた完全な名前またはUUIDを一致させて、目的のペリフェラルを見つける
3. ペリフェラルとの接続を確立する
4. 目的のプライマリサービスを発見する
5. 目的のサービス内のキャラクタリスティックを発見する
6. 目的のキャラクタリスティックに対して通知を有効にする

[powermate](../../../examples/network/ble/powermate) サンプルアプリの次のコードは、典型的なクライアントフローを示しています：

```javascript
const DEVICE_NAME = 'PowerMate Bluetooth';
const SERVICE_UUID = uuid`25598CF7-4240-40A6-9910-080F19F91EBC`;
const CHARACTERISTIC_UUID = uuid`9CF53570-DDD9-47F3-BA63-09ACEFC60415`;

class PowerMate extends BLEClient {
	onReady() {
		this.startScanning();
	}
	onDiscovered(device) {
		if (DEVICE_NAME == device.scanResponse.completeName) {
			this.stopScanning();
			this.connect(device);
		}
	}
	onConnected(device) {
		device.discoverPrimaryService(SERVICE_UUID);
	}
	onServices(services) {
		let service = services.find(service => service.uuid.equals(SERVICE_UUID));
		if (service)
			service.discoverCharacteristic(CHARACTERISTIC_UUID);
	}
	onCharacteristics(characteristics) {
		let characteristic = characteristics.find(characteristic => characteristic.uuid.equals(CHARACTERISTIC_UUID));
		if (characteristic)
			characteristic.enableNotifications();
	}
	onCharacteristicNotification(characteristic, buffer) {
		let value = new Uint8Array(buffer)[0];
		trace(`value: ${value}\n`);
	}
}
```

BLEクライアントは基本的に非同期であり、結果は常に`BLEClient`クラスのコールバックメソッド（例：上記の`onReady`、`onConnected`など）に配信されます。以下のセクションでは、BLEクライアントクラス、プロパティ、およびコールバックについて説明します。

<a id="classbleclient"></a>
## BLEClientクラス

`BLEClient`クラスはBLEクライアント機能へのアクセスを提供します。

```javascript
import BLEClient from "bleclient";
```

### 関数

#### `onReady()`

アプリケーションは他の`BLEClient`関数を呼び出す前に`onReady`コールバックを待つ必要があります：

```javascript
class Client extends BLEClient {
	onReady() {
		/* stack is ready to use */
	}
}
let client = new BLEClient;
```

***

#### `startScanning([params])`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `params` | `object` | スキャンプロパティを含むオブジェクト。

`params`オブジェクトには以下のプロパティが含まれます：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `active` | `boolean` | アクティブスキャンの場合は`true`、パッシブスキャンの場合は`false`を設定します。デフォルトは`true`です。
| `duplicates` | `boolean` | すべてのアドバタイジングパケットを受信する場合は`true`、同じ周辺機器から受信した複数のアドバタイジングパケットをフィルタリングする場合は`false`を設定します。デフォルトは`true`です。
| `filterPolicy` | `number` | スキャンに適用されるフィルタポリシー。デフォルトは`GAP.ScanFilterPolicy.NONE`（フィルタリングなし）です。詳細は[BLEホワイトリスト](#blewhitelisting)セクションを参照してください。
| `interval` | `number` | 0.625 ms単位のスキャン間隔値。デフォルトは`0x50`です。
| `window` | `number` | 0.625 ms単位のスキャンウィンドウ値。デフォルトは`0x30`です。

`filterPolicy` パラメータは以下のいずれかになります：

| 名前 | 説明 |
| --- | :--- |
| `GAP.ScanFilterPolicy.NONE` | フィルタリングなし。
| `GAP.ScanFilterPolicy.WHITELIST` | ホワイトリストデバイスからのみアドバタイズを受信。
| `GAP.ScanFilterPolicy.NOT_RESOLVED_DIRECTED` | すべての非指向アドバタイズと、イニシエータアドレスが解決可能なプライベートアドレスであるすべての指向アドバタイズを受信。
| `GAP.ScanFilterPolicy.WHITELIST_NOT_RESOLVED_DIRECTED` | ホワイトリストデバイスからのみアドバタイズを受信し、イニシエータアドレスが解決可能なプライベートアドレスであるすべての指向アドバタイズを受信。

`startScanning` 関数は、近くのペリフェラルをスキャンするために使用されます。

`params` 引数が提供されない場合、デフォルトのスキャンプロパティが使用されます：

```javascript
class Scanner extends BLEClient {
	onReady() {
		this.startScanning();
	}
	onDiscovered(device) {
		trace("device discovered\n");
	}
}
```

`params` オブジェクトを渡すことでデフォルトを上書きします。この例では、70msの間隔でパッシブスキャンを使用しています：

```javascript
class Scanner extends BLEClient {
	onReady() {
		this.startScanning({ active:false, interval:0x70 });
	}
	onDiscovered(device) {
		trace("device discovered\n");
	}
}
```
***

#### `onDiscovered(device)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `device` | `object` | `device` オブジェクト。詳細は [Clientクラス](#classclient) セクションを参照してください。 |

`onDiscovered` コールバック関数は、発見された各周辺機器に対して1回以上呼び出されます。

`onDiscovered` コールバックから「Brian」という名前のデバイスに接続するには：

```javascript
onDiscovered(device) {
	if ("Brian" == device.scanResponse.completeName) {
		this.connect(device);
	}
}
```

***

#### `stopScanning()`
`stopScanning` 関数は、近くの周辺機器のスキャンを無効にします。

***

#### `connect(device)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `device` | `object` | `device` オブジェクト。詳細は [Clientクラス](#classclient) セクションを参照してください。 |

`connect` 関数は、`BLEClient` とターゲット周辺 `device` 間の接続要求を開始します。

```javascript
onDiscovered(device) {
	this.connect(device);
}
onConnected(device) {
	trace("Connected to device\n");
}
```

***

#### `onConnected(device)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `device` | `object` | `device` オブジェクト。詳細は [Clientクラス](#classclient) セクションを参照してください。 |

`onConnected` コールバック関数は、クライアントがターゲット周辺 `device` に接続したときに呼び出されます。

***

#### `onDisconnected(device)`


| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `device` | `object` | `device` オブジェクト。詳細は [Clientクラス](#classclient) セクションを参照してください。 |

`onDisconnected` コールバックは接続が閉じられたときに呼び出されます。

***

#### `securityParameters`

BLEクライアントは、デバイス間で転送されるデータを保護するために、リンク層のセキュリティをオプションで要求できます。`securityParameters` プロパティアクセサ関数は、デバイスのセキュリティ要件とI/O機能を設定するために使用されます。

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `encryption` | `boolean` | 暗号化を有効にするためのオプションプロパティ。デフォルトは `true` |
| `bonding` | `boolean` | ボンディングを有効にするためのオプションプロパティ。デフォルトは `false` |
| `mitm` | `boolean` | 中間者攻撃 (MITM) 保護を有効にするためのオプションプロパティ。デフォルトは `false` |
| `ioCapability` | `object` | デバイスの I/O 機能を設定するためのオプションの `IOCapability` インスタンス。デフォルトは `IOCapability.NoInputNoOutput`。詳細は [Class SM](#classsm) セクションを参照してください。

ディスプレイのみのデバイスに対して、暗号化によるMITM保護を要求します。デバイスは要求されたときにパスキーを表示します：

```javascript
import {IOCapability} from "sm";

onReady() {
	this.securityParameters = { mitm:true, ioCapability:IOCapability.DisplayOnly };
}
```

ディスプレイとキーボードの両方を備えたデバイスに対して、暗号化とボンディングによるMITM保護を要求します。デバイスはパスキーの入力を要求し、暗号化キーが保存されます：

```javascript
import {IOCapability} from "sm";

onReady() {
	this.securityParameters = { mitm:true, bonding:true, ioCapability:IOCapability.KeyboardDisplay };
}
```

***

> **注:** `BLEClient` オブジェクトは、`BLEClient` で使用されるクラスのすべてのコールバックをホストします。

<a id="classclient"></a>
## Clientクラス
`Client` クラスのインスタンスは `BLEClient` によってインスタンス化され、`BLEClient` の `onDiscovered` および `onConnected` コールバックでホストアプリに提供されます。アプリケーションは `Client` クラスのインスタンスを直接インスタンス化することはありませんが、`Client` クラスの関数を呼び出してGATTサービス/キャラクタリスティックのディスカバリーを行い、より高いMTUをネゴシエートし、ペリフェラル接続を閉じます。

### プロパティ

| 名前 | 型 | 説明 |
| --- | --- | :--- |
| `connection` | `number` | 接続識別子。 |
| `address` | `object` | Bluetooth アドレスバイトを含む [Bytes](#classbytes) クラスのインスタンス。 |
| `addressType` | `number` | Bluetooth アドレスタイプ。詳細は `GAP.AddressType` を参照してください。 |
| `scanResponse` | `object` | アドバタイズおよびスキャン応答パケットの値を含む [Advertisement](#classadvertisement) クラスのインスタンス。 |
| `rssi` | `number` | 発見されたデバイスの信号強度。 |

### 関数

#### `exchangeMTU(mtu)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `mtu` | `number` | 要求されたMTU値 |

`exchangeMTU` 関数を使用して、ペリフェラル接続が確立された後により高いMTUを要求します。
***

#### `onMTUExchanged(device, mtu)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `device` | `object` | `device` オブジェクト。詳細は [Clientクラス](#classclient) セクションを参照してください。 |
| `mtu` | `number` | 交換されたMTU値 |

`onMTUExchanged` コールバック関数は、MTU交換手続きが完了したときに呼び出されます。

MTUサイズを250に増やすよう要求するには：

```javascript
onConnected(device) {
	device.exchangeMTU(250);
}
onMTUExchanged(device, mtu) {
	trace(`MTU size is now ${mtu}\n`);
	device.discoverAllPrimaryServices();
}
```

***

#### `readRSSI()`
`readRSSI` 関数を使用して、接続されたペリフェラルの信号強度を読み取ります。

***

#### `onRSSI(device, rssi)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `device` | `object` | `device` オブジェクト。詳細は [Clientクラス](#classclient) セクションを参照してください。 |
| `rssi` | `number` | 受信信号強度 |

`onRSSI` コールバック関数は、ペリフェラルの信号強度が読み取られたときに呼び出されます。

接続されたデバイスの信号強度を読み取るには：

```javascript
onConnected(device) {
	device.readRSSI();
}
onRSSI(device, rssi) {
	trace(`signal strength is ${rssi}\n`);
}
```

***

#### `discoverAllPrimaryServices()`
`discoverAllPrimaryServices` 関数を使用して、ペリフェラルのすべてのGATTプライマリサービスを発見します。発見されたサービスは `onServices` コールバックに返されます。

***

#### `discoverPrimaryService(uuid)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `uuid` | `object` | 発見する UUID を含む [Bytes](#classbytes) オブジェクト |

`discoverPrimaryService` 関数を使用して、UUIDによって単一のGATTプライマリサービスを発見します。
***

#### `findServiceByUUID(uuid)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `uuid` | `object` | 見つけるサービス UUID を含む [Bytes](#classbytes) オブジェクト |

`findServiceByUUID` 関数は、`uuid` によって識別されるサービスを見つけて返します。この関数は `services` プロパティ配列を検索します。

***

#### `onServices(services)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `services` | `array` | `service` オブジェクトの配列、またはサービスが見つからなかった場合は空の配列です。詳細は [Serviceクラス](#classservice) のセクションを参照してください。 |

`onServices` コールバック関数は、サービスの検出が完了したときに呼び出されます。`findServiceByUUID` が単一のサービスを見つけるために呼び出された場合、`services` 配列には見つかった単一のサービスが含まれます。

すべてのプライマリサービスを検出するには：

```javascript
onConnected(device) {
	device.discoverAllPrimaryServices();
}
onServices(services) {
	trace(`${services.length} services found\n`);
}
```

単一のプライマリサービスを検出するには：

```javascript
const SERVICE_UUID = uuid`0xFF00`;

onConnected(device) {
	device.discoverPrimaryService(SERVICE_UUID);
}
onServices(services) {
	if (services.length)
		trace("found service\n");
}
```
> **注:** `uuid` タグ付きテンプレートは、アプリケーションが UUID 値を文字列として表現することを可能にします。詳細は [Bytes](#classbytes) クラスのセクションを参照してください。

***

#### `close()`
`close` 関数は、ペリフェラル接続を終了します。

```javascript
onConnected(device) {
	trace("connected to device\n");
	device.close();
}
onDisconnected() {
	trace("connection closed\n");
}
```

<a id="classservice"></a>
## Serviceクラス
`Service` クラスは、単一のペリフェラルGATTサービスへのアクセスを提供します。

### プロパティ


| 名前 | タイプ | 説明 |
| --- | --- | :--- |
| `connection` | `number` | 接続識別子。
| `uuid` | `object` | サービスUUIDを含む[Bytes](#classbytes)クラスのインスタンス。
| `start` | `number` | 含まれるキャラクタリスティックの開始ハンドル。
| `end` | `number` | 含まれるキャラクタリスティックの終了ハンドル。
| `characteristics` | `array` | 発見されたサービスキャラクタリスティックの配列。

### 関数

#### `discoverAllCharacteristics()`
`discoverAllCharacteristics()`関数を使用して、すべてのサービスキャラクタリスティックを発見します。発見されたキャラクタリスティックは`onCharacteristics`コールバックに返されます。

***

#### `discoverCharacteristic(uuid)`

| 引数 | タイプ | 説明 |
| --- | --- | :--- |
| `uuid` | `object` | 発見するキャラクタリスティックUUIDを含む[Bytes](#classbytes)オブジェクト |

`discoverCharacteristic`関数を使用して、UUIDで単一のサービスキャラクタリスティックを発見します。

***

#### `findCharacteristicByUUID(uuid)`

| 引数 | タイプ | 説明 |
| --- | --- | :--- |
| `uuid` | `object` | 見つけるキャラクタリスティックUUIDを含む[Bytes](#classbytes)オブジェクト |

`findCharacteristicByUUID` 関数は、`uuid` で識別されるキャラクタリスティックを見つけて返します。この関数は `characteristics` プロパティ配列を検索します。

***

#### `onCharacteristics(characteristics)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristics` | `array` | `characteristic` オブジェクトの配列、またはキャラクタリスティックが発見されなかった場合は空の配列です。詳細は [Characteristicクラス](#classcharacteristic) のセクションを参照してください。 |

`onCharacteristics` コールバック関数は、キャラクタリスティックの発見が完了したときに呼び出されます。`findCharacteristicByUUID` が単一のキャラクタリスティックを見つけるために呼び出された場合、`characteristics` 配列には見つかった単一のキャラクタリスティックが含まれます。

***

[Device Information](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.device_information.xml) サービス内のすべてのキャラクタリスティックを発見するには：

```javascript
const DEVICE_INFORMATION_SERVICE_UUID = uuid`180A`;

onServices(services) {
	let service = services.find(service => service.uuid.equals(DEVICE_INFORMATION_SERVICE_UUID));
	if (service)
		service.discoverAllCharacteristics();
}
onCharacteristics(characteristics) {
	trace(`${characteristics.length} characteristics found\n`);
}
```

[Heart Rate](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.heart_rate.xml) サービス内の [Heart Rate Measurement](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.heart_rate_measurement.xml) キャラクタリスティックを見つけるには：

```javascript
const HEART_RATE_SERVICE_UUID = uuid`180A`;
const HEART_RATE_MEASUREMENT_UUID = uuid`2A37`;

onConnected(device) {
	device.discoverPrimaryService(HEART_RATE_SERVICE_UUID);
}
onServices(services) {
	if (services.length)
		services[0].discoverCharacteristic(HEART_RATE_MEASUREMENT_UUID);
}
onCharacteristics(characteristics) {
	if (characteristics.length)
		trace(`found heart rate measurement characteristic\n`);
}
```

***

<a id="classcharacteristic"></a>
## Characteristicクラス
`Characteristic` クラスは、単一のサービスキャラクタリスティックへのアクセスを提供します。

### プロパティ

| 名前 | 型 | 説明 |
| --- | --- | :--- |
| `connection` | `number` | 接続識別子。 |
| `uuid` | `object` | キャラクタリスティックUUIDを含む [Bytes](#classbytes) クラスのインスタンス。 |
| `service` | `object` | キャラクタリスティックを含む `Service` オブジェクト。 |
| `handle` | `number` | キャラクタリスティックハンドル。 |
| `name` | `string` | オプションのサービスJSONで定義されたキャラクタリスティック名。キャラクタリスティックがサービスJSONで定義されていない場合、このプロパティは `undefined` です。 |
| `type` | `string` | オプションのサービスJSONで定義されたキャラクタリスティックタイプ。キャラクタリスティックがサービスJSONで定義されていない場合、このプロパティは `undefined` です。 |
| `descriptors` | `array` | 発見されたキャラクタリスティックディスクリプタの配列。 |

### 関数

#### `discoverAllDescriptors()`
`discoverAllDescriptors` 関数を使用して、すべてのキャラクタリスティックのディスクリプタを発見します。発見されたディスクリプタは `onDescriptors` コールバックに返されます。

***

#### `onDescriptors(descriptors)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `descriptors` | `array` | `descriptor` オブジェクトの配列、またはディスカバリーされたディスクリプタがない場合は空の配列です。詳細は [Class Descriptor](#classdescriptor) のセクションを参照してください。 |

`onDescriptors` コールバック関数は、ディスクリプタのディスカバリーが完了したときに呼び出されます。

UUID 0xFF00のキャラクタリスティックに対する [Characteristic Presentation Format Descriptor](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Descriptors/org.bluetooth.descriptor.gatt.characteristic_presentation_format.xml) をディスカバーするには：

```javascript
const CHARACTERISTIC_UUID = uuid`FF00`;
const PRESENTATION_FORMAT_UUID = uuid`2904`;

onCharacteristics(characteristics) {
	let characteristic = characteristics.find(characteristic => characteristic.uuid.equals(CHARACTERISTIC_UUID));
	if (characteristic)
		characteristic.discoverAllDescriptors();
}
onDescriptors(descriptors) {
	let descriptor = descriptors.find(descriptor => descriptor.uuid.equals(PRESENTATION_FORMAT_UUID));
	if (descriptor)
		trace("found characteristic presentation format descriptor\n");
}
```

***

#### `enableNotifications()`
`enableNotifications` 関数を使用して、キャラクタリスティックの値の変更通知を有効にします。

***

#### `onCharacteristicNotificationEnabled(characteristic)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristic` | `object` | `characteristic` オブジェクト。 |

`onCharacteristicNotificationEnabled` コールバック関数は、`characteristic` の通知が有効になったときに呼び出されます。

***

#### `disableNotifications()`
`disableNotifications` 関数を使用して、キャラクタリスティックの値変更通知を無効にします。

***

#### `onCharacteristicNotificationDisabled(characteristic)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristic` | `object` | `characteristic` オブジェクト。 |

`onCharacteristicNotificationDisabled` コールバック関数は、`characteristic` の通知が無効になったときに呼び出されます。

***

#### `onCharacteristicNotification(characteristic, value)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristic` | `object` | `characteristic` オブジェクト。 |
| `value` | `varies` | `characteristic` の値。利用可能な場合、値はサービス JSON で定義された `type` に自動的に変換されます。それ以外の場合、`value` は `ArrayBuffer` です。 |

`onCharacteristicNotification` コールバック関数は、通知が有効になっており、ペリフェラルがキャラクタリスティックの値を通知したときに呼び出されます。

[Battery Level](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.battery_level.xml) キャラクタリスティックの値変更通知を有効にして受信するには、[Battery Service](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.battery_service.xml) を使用します。

```javascript
const BATTERY_SERVICE_UUID = uuid`180F`;
const BATTERY_LEVEL_UUID = uuid`2A19`;

onConnected(device) {
	device.discoverPrimaryService(BATTERY_SERVICE_UUID);
}
onServices(services) {
	if (services.length)
		services[0].discoverCharacteristic(BATTERY_LEVEL_UUID);
}
onCharacteristics(characteristics) {
	if (characteristics.length)
		characteristics[0].enableNotifications();
}
onCharacteristicNotification(characteristic, value) {
	let level = new Uint8Array(value)[0];
	trace(`battery level: ${level}%\n`);
}
```

***

#### `readValue([auth])`
`readValue` 関数を使用して、必要に応じてキャラクタリスティックの値を読み取ります。

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `auth` | `number` | 読み取り要求に適用されるオプションの `SM.Authorization`。 |

`Authorization` オブジェクトには次のプロパティが含まれています：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `None` | `number` | 認証なし |
| `NoMITM` | `number` | 認証されていない暗号化 |
| `MITM` | `number` | 認証された暗号化 |
| `SignedNoMITM` | `number` | 署名された認証されていない暗号化 |
| `SignedMITM` | `number` | 署名された認証された暗号化 |

***

#### `onCharacteristicValue(characteristic, value)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristic` | `object` | `characteristic` オブジェクト。 |
| `value` | `varies` | 読み取られた `characteristic` 値。値の `type` は、利用可能な場合はサービス JSON によって定義されます。それ以外の場合、`value` は `ArrayBuffer` です。 |

`onCharacteristicValue` コールバック関数は、`readValue` 関数によってキャラクタリスティックが読み取られたときに呼び出されます。

[Generic Access Service](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.generic_access.xml) から [Device Name](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.gap.device_name.xml) を読み取るには：

```javascript
const GENERIC_ACCESS_SERVICE_UUID = uuid`1800`;
const DEVICE_NAME_UUID = uuid`2A00`;

onConnected(device) {
	device.discoverPrimaryService(GENERIC_ACCESS_SERVICE_UUID);
}
onServices(services) {
	if (services.length)
		services[0].discoverCharacteristic(DEVICE_NAME_UUID);
}
onCharacteristics(characteristics) {
	if (characteristics.length)
		characteristics[0].readValue();
}
onCharacteristicValue(characteristic, value) {
	let name = String.fromArrayBuffer(value);
	trace(`device name: ${name}\n`);
}
```

***

#### `writeWithoutResponse(value)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `value` | `varies` | 書き込む `characteristic` の値。`type` は、利用可能な場合はサービス JSON によって定義されます。それ以外の場合、`value` は `ArrayBuffer` または `String` です。 |

`writeWithoutResponse` 関数を使用して、要求に応じてキャラクタリスティックの値を書き込みます。

[HTTP Proxy](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.http_proxy.xml) サービスで [URI](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.uri.xml) キャラクタリスティックの値を書き込むには：

```javascript
const HTTP_PROXY_SERVICE_UUID = uuid`1823`;
const URI_UUID = uuid`2AB6`;

onConnected(device) {
	device.discoverPrimaryService(HTTP_PROXY_SERVICE_UUID);
}
onServices(services) {
	if (services.length)
		services[0].discoverCharacteristic(URI_UUID);
}
onCharacteristics(characteristics) {
	if (characteristics.length)
		characteristics[0].writeWithoutResponse(ArrayBuffer.fromString("http://moddable.tech"));
}
```

***

<a id="classdescriptor"></a>
## Descriptorクラス
`Descriptor` クラスは、単一のキャラクタリスティックディスクリプタへのアクセスを提供します。

### プロパティ

| 名前 | 型 | 説明 |
| --- | --- | :--- |
| `connection` | `number` | 接続識別子。 |
| `uuid` | `string` | ディスクリプタUUIDを含む [Bytes](#classbytes) クラスのインスタンス。 |
| `characteristic` | `object` | ディスクリプタを含む `Characteristic` オブジェクト。 |
| `handle` | `number` | ディスクリプタハンドル。 |
| `name` | `string` | オプションのサービスJSONで定義されたディスクリプタ名。サービスJSONでディスクリプタが定義されていない場合、このプロパティは `undefined` です。 |
| `type` | `string` | オプションのサービスJSONで定義されたディスクリプタタイプ。サービスJSONでディスクリプタが定義されていない場合、このプロパティは `undefined` です。 |

### 関数

#### `readValue([auth])`
`readValue` 関数を使用して、必要に応じてディスクリプタの値を読み取ります。

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `auth` | `number` | 読み取り要求に適用されるオプションの `SM.Authorization`。 |

`Authorization` オブジェクトには以下のプロパティがあります：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `None` | `number` | 認証なし |
| `NoMITM` | `number` | 認証されていない暗号化 |
| `MITM` | `number` | 認証された暗号化 |
| `SignedNoMITM` | `number` | 署名された認証されていない暗号化 |
| `SignedMITM` | `number` | 署名された認証された暗号化 |

***

#### `onDescriptorValue(descriptor, value)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `descriptor ` | `object` | `descriptor` オブジェクト。 |
| `value` | `varies` | 読み取られた `descriptor` の値。値の `type` は、利用可能な場合はサービス JSON によって定義されます。それ以外の場合、`value` は `ArrayBuffer` です。 |

`onDescriptorValue` コールバック関数は、`readValue` 関数によってディスクリプタが読み取られたときに呼び出されます。

認証されていない暗号化でディスクリプタを読み取るには：

```javascript
import {SM, IOCapability, Authorization} from "sm";

const REPORT_REFERENCE_DESCRIPTOR_UUID = uuid`2908`;

onDescriptors(descriptors) {
	let descriptor = descriptors.find(descriptor => descriptor.uuid.equals(REPORT_REFERENCE_DESCRIPTOR_UUID));
	if (descriptor)
		descriptor.readValue(Authorization.NoMITM);
}
onDescriptorValue(descriptor, value) {
	let report = new Uint8Array(value);
}
```
#### `writeValue(value)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `value` | `varies` | 書き込む `descriptor` の値。値の `type` は、利用可能な場合はサービス JSON によって定義されます。それ以外の場合、`value` は `ArrayBuffer` または `String` です。 |

`writeValue` 関数を使用して、ディスクリプタの値を書き込みます。

[クライアントキャラクタリスティック設定](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Descriptors/org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml)ディスクリプタに0x0001を書き込むことで、キャラクタリスティックの値の変更通知を有効にします。

```javascript
const CCCD_UUID = uuid`2902`;

onDescriptors(descriptors) {
	let descriptor = descriptors.find(descriptor => descriptor.uuid.equals(CCCD_UUID));
	if (descriptor) {
		let value = Uint8Array.of(0x01, 0x00);	// little endian
		descriptor.writeValue(value.buffer);
	}
}
```

> **注:** `Characteristic` の `enableNotifications` 便利関数は通常この目的で使用されますが、CCCDディスクリプタを直接書き込むことでも同じ結果が得られます。

<a id="classadvertisement"></a>
## Advertisementクラス

`Advertisement` クラスは、一般的なアドバタイズとスキャン応答データ型をJavaScriptプロパティとして読み取るためのアクセサ関数を提供します。このクラスは主に、`onDiscovered` コールバック関数の `device` パラメータを介してBLEクライアントに提供される `scanResponseData` を解析するために使用されます。アクセサ関数は、関連するデータ型が `scanResponseData` に存在しない場合は `undefined` を返します。

### プロパティ

| 名前 | 型 | 説明 |
| --- | --- | :--- |
| `buffer` | `object` | 生のアドバタイズデータバイトを含む `ArrayBuffer`。 |
| `completeName` | `string` | アドバタイズされた完全なローカル名。 |
| `shortName` | `string` | アドバタイズされた短縮されたローカル名。 |
| `manufacturerSpecific` | `object` | アドバタイズされたメーカー固有のデータを含むオブジェクト。 |
| `flags` | `number` | アドバタイズされたフラグの値。 |
| `completeUUID16List` | `array` | アドバタイズされた完全な16ビットUUIDリスト。 |
| `incompleteUUID16List` | `array` | アドバタイズされた不完全な16ビットUUIDリスト。 |

### 関数

#### `findIndex(type [,index])`
`findIndex` 関数を使用して、生のアドバタイズデータバイト内の特定のアドバタイズデータタイプのインデックスを見つけます。

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `type` | `number` | 検索する `GAP.ADType`。 |
| `index` | `number` | 検索を開始するオプションの開始インデックス。デフォルトは0。 |

### 例

完全な名前が "Zip" のアドバタイズデバイスを識別するには：

```javascript
onDiscovered(device) {
	let completeName = device.scanResponse.completeName;
	if (completeName == "ZIP") {
		trace("Found ZIP device\n");
	}
}
```

[Blue Maestro Environment Monitor](https://www.bluemaestro.com/product/tempo-environment-monitor/)を識別し、メーカー固有データから温度を読み取るには：

```javascript
onDiscovered(device) {
	let manufacturerSpecific = device.scanResponse.manufacturerSpecific;
	const TempoManufacturerID = 307;

	// If this is a Tempo device...
	if (manufacturerSpecific && (TempoManufacturerID == manufacturerSpecific.identifier)) {
		let data = manufacturerSpecific.data;
		if (data[0] == 0 || data[0] == 1) {	// ...and product model T30 or THP
			let temperature = (data[3] | (data[4] << 8)) / 10;
			trace(`Temperature: ${temperature} ˚C\n`);
		}
	}
}
```

スキャン応答データ内の「TX Power Level」アドバタイズデータタイプを検索するには：

```javascript
onDiscovered(device) {
	let scanResponse = device.scanResponse;
	let index = scanResponse.findIndex(GAP.ADType.TX_POWER_LEVEL);
	if (-1 !== index) {
		trace(`Found advertisement tx power level data at index ${index}\n`);
		const bytes = new Uint8Array(scanResponse.buffer);
		const txPowerLevel = bytes[index + 2];
	}
}
```

<a id="classbytes"></a>
## Bytesクラス

プライベートな`Bytes`クラスは`ArrayBuffer`を拡張します。アプリケーションは通常、提供された`uuid`および`address`タグ付きテンプレートを使用して、16進文字列から`Bytes`インスタンスを作成します。

#### コンストラクタの説明

#### `Bytes(bytes [,littleEndian])`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `bytes` | `object` または `string` | 設定するArrayBufferの`bytes`。バイトは文字列またはArrayBufferで指定できます。 |
| `littleEndian` | `boolean` | `true`に設定すると、ArrayBufferの内容が逆順に格納されます。デフォルトは`false`です。|
### 関数

#### `address`\``string``

`address` タグ付きテンプレートは、16進文字列で表現されたBluetoothアドレスを `Bytes` インスタンスに変換するために使用されます。

```javascript
import {address} from "btutils";

const DEVICE_ADDRESS = address`66:55:44:33:22:11`;
```

#### `uuid`\``string``

`uuid` タグ付きテンプレートは、16進文字列で表現されたBluetooth UUIDを `Bytes` インスタンスに変換するために使用されます。

```javascript
import {uuid} from "btutils";

const HTM_SERVICE = uuid`1809`;
const CHARACTERISTIC_RX_UUID = uuid`6E400002-B5A3-F393-E0A9-E50E24DCCA9E`;
```

#### `equals(buffer)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `buffer` | `object` | 比較する `Bytes` インスタンス。 |

`equals` 関数は、インスタンスのArrayBufferデータが `buffer` に含まれるデータと等しい場合に `true` を返します。

```javascript
import {uuid} from "btutils";

const HTM_SERVICE = uuid`1809`;
onServices(services) {
	let found = services[0].uuid.equals(HTM_SERVICE);
	if (found)
		trace("found HTM service\n");
}
```

#### `toString()`

`toString` ヘルパー関数は、`Bytes` の内容を印刷可能な16進文字列として返します。文字列はビッグエンディアン順でセパレータ付きでフォーマットされます。

```javascript
onDiscovered(device) {
	trace(`Found device with address ${device.address}\n`);
}
onServices(services) {
	if (services.length) {
		trace(`Found service with UUID ${services[0].uuid}\n`);
	}
}
```

<a id="bleserver"></a>
## BLE サーバー
BLEサーバー/ペリフェラルは、1つのBLEクライアントに接続でき、通常、BLEクライアントに通知を送信するために次の手順を実行します。

1. クライアントがペリフェラルを発見できるようにアドバタイズを開始する
2. クライアントとの接続を確立する
3. キャラクタリスティックの値変更通知要求を受け入れる
4. キャラクタリスティックの値の変更を通知する

以下の[heart-rate-server](../../../examples/network/ble/heart-rate-server)サンプルアプリからの省略されたコードは、典型的なサーバーフローを示しています：

```javascript
onReady() {
	this.bpm = [0, 60]; // flags, beats per minute
	this.startAdvertising({
		advertisingData: {flags: 6, completeName: "Moddable HRS", completeUUID16List: [uuid`180D`, uuid`180F`]}
	});
}
onConnected() {
	this.stopAdvertising();
}
onCharacteristicNotifyEnabled(characteristic) {
	this.startMeasurements(characteristic);
}
startMeasurements(characteristic) {
	this.bump = +1;
	this.timer = Timer.repeat(id => {
		this.notifyValue(characteristic, this.bpm);
		this.bpm[1] += this.bump;
		if (this.bpm[1] == 65) {
			this.bump = -1;
			this.bpm[1] == 64;
		}
		else if (this.bpm[1] == 55) {
			this.bump = +1;
			this.bpm[1] == 56;
		}
	}, 1000);
}
```

GATTサービスは、BLEサーバーが起動されると自動的に展開されます。BLEサーバーは基本的に非同期であり、結果は常に`BLEServer`クラスのコールバックメソッド（例：上記の`onReady`、`onConnected`など）に配信されます。以下のセクションでは、`BLEServer`クラス、プロパティ、およびコールバックについて説明します。

<a id="classbleserver"></a>
## BLEServerクラス

`BLEServer`クラスは、BLEサーバーの機能にアクセスするためのものです。

```javascript
import BLEServer from "bleserver";
```

### `constructor([dictionary])`

`BLEServer`コンストラクタは、初期化パラメータのオプションの辞書を1つの引数として受け取ります。

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `deployServices` | `boolean` | [GATT Services](#gattservices)をデプロイするためのオプションプロパティ。デフォルトは`true`です。GATTサービスをデプロイしない場合は`false`に設定します。

### 関数

#### `onReady()`
アプリケーションは、他の`BLEServer`関数を呼び出す前に`onReady`コールバックを待つ必要があります。

```javascript
class Server extends BLEServer {
	onReady() {
		/* stack is ready to use */
	}
}
let server = new BLEServer;
```

***

#### `deviceName`
`deviceName`プロパティアクセサ関数は、Bluetoothペリフェラルデバイスの名前を設定/取得するために使用されます。

```javascript
class Advertiser extends BLEServer {
	onReady() {
		this.deviceName = "My BLE Device";
		trace(`My device is named ${this.deviceName}\n`);
	}
}
let advertiser = new BLEServer;
```

***

#### `localAddress`
読み取り専用の`localAddress`プロパティアクセサ関数は、Bluetoothペリフェラルのローカルアドレスを[Bytes](#classbytes)オブジェクトとして返します。

```javascript
class Advertiser extends BLEServer {
	onReady() {
		trace(`device address: ${(new Uint8Array(this.localAddress)).toHex()}\n`);
	}
}
```

***

#### `securityParameters`

BLEサーバーは、デバイス間で転送されるデータを保護するためにリンク層のセキュリティをオプションで要求できます。`securityParameters`プロパティアクセサ関数は、デバイスのセキュリティ要件とI/O機能を設定するために使用されます。

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `encryption` | `boolean` | 暗号化を有効にするためのオプションプロパティ。デフォルトは `true` |
| `bonding` | `boolean` | ボンディングを有効にするためのオプションプロパティ。デフォルトは `false` |
| `mitm` | `boolean` | 中間者攻撃 (MITM) 保護を有効にするためのオプションプロパティ。デフォルトは `false` |
| `ioCapability` | `object` | デバイスのI/O機能を設定するためのオプションの `IOCapability` インスタンス。デフォルトは `IOCapability.NoInputNoOutput`。詳細は [Class SM](#classsm) のセクションを参照してください。

暗号化を伴うMITM保護をディスプレイのみのデバイスで要求するには。デバイスは要求されたときにパスキーを表示します：

```javascript
import {IOCapability} from "sm";

onReady() {
	this.securityParameters = { mitm:true, ioCapability:IOCapability.DisplayOnly };
}
```

ディスプレイとキーボードの両方を備えたデバイスで、暗号化とボンディングを伴うMITM保護を要求するには。デバイスはパスキーの入力を要求し、暗号化キーが保存されます：

```javascript
import {IOCapability} from "sm";

onReady() {
	this.securityParameters = { mitm:true, bonding:true, ioCapability:IOCapability.KeyboardDisplay };
}
```

***

#### `startAdvertising(params)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `params` | `object` | アドバタイズプロパティを含むオブジェクト。

`params` オブジェクトには以下のプロパティが含まれています：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `advertisingData` | `object` | アドバタイズデータプロパティを含むオブジェクト。 |
| `connectable` | `boolean` | 接続可能モードを指定するためのオプションプロパティ。`true` に設定すると一方向接続可能モードを指定し、`false` に設定すると非接続可能モードを指定します。デフォルトは `true`。 |
| `discoverable` | `boolean` | 発見可能モードを指定するためのオプションプロパティ。`true` に設定すると一般的な発見手順を使用し、`false` に設定すると非発見可能を指定します。デフォルトは `true`。 |
| `fast` | `boolean` | GAP アドバタイズ間隔を指定するためのオプションプロパティ。`true` に設定すると TGAP(adv\_fast\_interval1) を指定し、`false` に設定すると TGAP(adv\_slow\_interval) を指定します。デフォルトは `true`。 |
| `filterPolicy` | `number` | フィルターポリシーを適用するためのオプションプロパティ。デフォルトは `GAP.AdvFilterPolicy.NONE` (フィルタリングなし)。詳細は [BLE ホワイトリスト](#blewhitelisting) セクションを参照してください。 |
| `notify` | `boolean` | 各アドバタイズデータパケット送信後にアプリケーションに通知するためのオプションプロパティ。デフォルトは `false`。 |
| `scanResponseData` | `object` | スキャン応答データプロパティを含むオプションオブジェクト。 |

> 注: `notify` プロパティは nRF52 デバイスでのみ実装されています。

`filterPolicy` プロパティは以下のいずれかです：

| 名前 | 説明 |
| --- | :--- |
| `GAP.AdvFilterPolicy.NONE` | フィルタリングなし。
| `GAP.AdvFilterPolicy.WHITELIST_SCANS` | すべての接続要求を処理しますが、ホワイトリストにあるデバイスからのスキャンのみを処理します。
| `GAP.AdvFilterPolicy.WHITELIST_CONNECTIONS` | すべてのスキャン要求を処理しますが、ホワイトリストにあるデバイスからの接続要求のみを処理します。
| `GAP.AdvFilterPolicy.WHITELIST_SCANS_CONNECTIONS` | ピアデバイスがホワイトリストにない限り、すべてのスキャンおよび接続要求を無視します。

`startAdvertising` 関数は、アドバタイズおよびスキャン応答パケットのブロードキャストを開始します。この関数は、発見可能モードおよび接続可能モードを設定するためにも使用されます。

`advertisingData` および `scanResponseData` は、[Bluetooth Advertisement Data Types](https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile) に対応する1つ以上のプロパティを含みます：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `incompleteUUID16List` | `array` | *不完全な16ビットサービスUUIDのリスト*に対応するUUIDオブジェクトの配列。
| `completeUUID16List` | `array ` | *完全な16ビットサービスUUIDのリスト*に対応するUUIDオブジェクトの配列。
| `incompleteUUID128List` | `array` | *不完全な128ビットサービスUUIDのリスト*に対応するUUIDオブジェクトの配列。
| `completeUUID128List` | `array` | *完全な128ビットサービスUUIDのリスト*に対応するUUIDオブジェクトの配列。
| `shortName` | `string` | *短縮ローカル名*に対応する文字列。
| `completeName` | `string` | *完全なローカル名*に対応する文字列。
| `manufacturerSpecific` | `object` | *メーカー固有データ*に対応するオブジェクト。`identifier` プロパティは *会社識別子コード* に対応する数値です。`data` プロパティは追加のメーカー固有データに対応する `Uint8Array` の数値です。
| `txPowerLevel` | `number` | *TXパワーレベル*に対応する数値。
| `connectionInterval` | `object` | *スレーブ接続間隔範囲*に対応するオブジェクト。`intervalMin` プロパティは最小接続間隔値に対応する数値です。`intervalMax` プロパティは最大接続間隔値に対応する数値です。
| `solicitationUUID16List` | `array` | *16ビットサービス勧誘UUIDのリスト*に対応するUUIDオブジェクトの配列。
| `solicitationUUID128List ` | `array` | *128ビットサービス勧誘UUIDのリスト*に対応するUUIDオブジェクトの配列。
| `serviceDataUUID16` | `object` | *サービスデータ - 16ビットUUID*に対応するオブジェクト。`uuid` プロパティは16ビットサービスUUIDに対応するオブジェクトです。`data` プロパティは追加のサービスデータに対応する数値の配列です。
| `serviceDataUUID128` | `object` | *サービスデータ - 128ビットUUID*に対応するオブジェクト。`uuid` プロパティは128ビットサービスUUIDに対応するオブジェクトです。`data` プロパティは追加のサービスデータに対応する数値の配列です。
| `appearance` | `number` | *外観*に対応する数値。
| `publicAddress` | `object` | *パブリックターゲットアドレス*に対応するアドレスオブジェクト。
| `randomAddress` | `object` | *ランダムターゲットアドレス*に対応するアドレスオブジェクト。
| `advertisingInterval` | `number` | *アドバタイズ間隔*に対応する数値。
| `role` | `number` | *LEロール*に対応する数値。
| `uri` | `string` | *統一リソース識別子*に対応する文字列。

[Health Thermometer Sensor](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.health_thermometer.xml) BLE専用接続可能デバイスを、完全なローカル名「Thermometer」とUUID 0x1809の1つのサービスでアドバタイズするには：

```javascript
import {uuid} from "btutils";

this.startAdvertising({
	advertisingData: {flags: 6, completeName: "Thermometer", completeUUID16List: [uuid`1809`]}
});
```

[Heart Rate Sensor](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.heart_rate.xml) BLE専用接続可能デバイスを、完全なローカル名「Moddable HRS」とUUID 0x180Dおよび0x180Fの2つのサービスでアドバタイズするには：

```javascript
import {uuid} from "btutils";

this.startAdvertising({
	advertisingData: {flags: 6, completeName: "Moddable HRS", completeUUID16List: completeUUID16List: [uuid`180D`, uuid`180F`]}
});
```

BLE専用非接続可能かつ限定的に発見可能なデバイスを、完全なローカル名「Moddable HRS」、短縮名「Moddable」、およびスキャン応答で提供されるランダムターゲットアドレス00:11:22:33:44:55でアドバタイズするには：

```javascript
import {address} from "btutils";

this.startAdvertising({
	connectable: false,
	advertisingData: {flags: 5, completeName: "Moddable HRS"},
	scanResponseData: {shortName: "Moddable", randomAddress: address`00:11:22:33:44:55`}
});
```

各アドバタイズデータパケット送信後に通知コールバックを受け取るには：

```javascript
onReady() {
	this.startAdvertising({
		notify: true,
		advertisingData: {flags: GAP.ADFlag.NO_BR_EDR, completeName: "Advertiser"}
	});
}
onAdvertisementSent() {
	trace("advertisement sent\n");
}
```

***

#### `stopAdvertising()`
`stopAdvertising` 関数を呼び出して、Bluetoothアドバタイズの送信を停止します。

```javascript
import {uuid} from "btutils";

onReady() {
	this.startAdvertising({
		advertisingData: {flags: 6, completeName: "Thermometer Example", completeUUID16List: [uuid`1809`]}
	});
}
onConnected(device) {
	this.stopAdvertising();
}
```

#### `notifyValue(characteristic, value)`
| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristic` | `object` | 通知する `characteristic` オブジェクト。 |
| `value` | `varies` | `characteristic` の通知値。値はサービス JSON で定義された `type` から自動的に変換されます。 |

`notifyValue` 関数を呼び出して、接続されたクライアントにcharacteristic値の変更通知を送信します。

250 msごとに文字列 "hello" をcharacteristicに通知するには：

```javascript
onCharacteristicNotifyEnabled(characteristic) {
	this.startNotifications(characteristic);
}
startNotifications(characteristic) {
	this.timer = Timer.repeat(id => {
		this.notifyValue(characteristic, ArrayBuffer.fromString("hello"));
	}, 250);
}
```

***

#### `onAdvertisementSent()`

`onAdvertisementSent` コールバック関数は、`startAdvertising()` 関数からアドバタイズ通知が有効になっているサーバーで、各アドバタイズデータパケット送信後に呼び出されます。

> 注: `onAdvertisementSent` コールバックは nRF52 デバイスでのみ実装されています。

***

#### `onCharacteristicNotifyEnabled(characteristic)`
| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristic` | `object` | 通知が有効になった `characteristic` オブジェクト。

`onCharacteristicNotifyEnabled` コールバック関数は、クライアントが `characteristic` の通知を有効にしたときに呼び出されます。

***

#### `onCharacteristicWritten(characteristic, value)`
| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristic` | `object` | 書き込まれた `characteristic` オブジェクト。
| `value` | `varies` | 書き込まれた値。値はサービスJSONで定義された `type` に自動的に変換されます。

`characteristic` オブジェクトには以下のプロパティがあります：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `uuid` | `object` | Characteristic UUIDを含む [Bytes](#classbytes) クラスのインスタンス。
| `name` | `string` | サービスJSONで定義されたCharacteristic名。
| `type` | `string` | サービスJSONで定義されたCharacteristicタイプ。
| `handle` | `number` | Characteristicハンドル。

`onCharacteristicWritten` コールバックは、クライアントがサービスのキャラクタリスティックの値を書き込む際に呼び出されます。`BLEServer` アプリケーションは、書き込み要求を処理する責任があります。

以下の [wifi-connection-server](../../../examples/network/ble/wifi-connection-server) 例からの省略された例は、キャラクタリスティックの書き込み要求がどのように処理されるかを示しています。`SSID` と `password` のキャラクタリスティックは文字列であり、`control` のキャラクタリスティックは数値です。BLEクライアントが `control` キャラクタリスティックに値1を書き込むと、アプリはクライアント接続を閉じ、WiFi接続を開始します。

```javascript
onCharacteristicWritten(characteristic, value) {
	if ("SSID" == characteristic.name)
		this.ssid = value;
	else if ("password" == characteristic.name)
		this.password = value;
	else if ("control" == characteristic.name) {
		if ((1 == value) && this.ssid) {
			this.close();
			this.connectToWiFiNetwork(this.ssid, this.password);
		}
	}
}
```

***

#### `onCharacteristicRead(characteristic)`
| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `characteristic ` | `object` | 読み取られているキャラクタリスティックオブジェクト。

`characteristic` オブジェクトには以下のプロパティが含まれています。

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `uuid` | `string` | キャラクタリスティック UUID を含む [Bytes](#classbytes) クラスのインスタンス。
| `name` | `string` | サービス JSON で定義されたキャラクタリスティック名。
| `type` | `string` | サービス JSON で定義されたキャラクタリスティックタイプ。
| `handle` | `number` | キャラクタリスティックハンドル。

`onCharacteristicRead` コールバックは、クライアントがサービスのキャラクタリスティックの値を要求に応じて読み取るときに呼び出されます。`BLEServer` アプリケーションは、読み取り要求の処理を担当します。

数値の "status" キャラクタリスティックに対応する読み取り要求に応答するには、次のようにします。

```javascript
onReady() {
	this.status = 10;
}
onCharacteristicRead(characteristic) {
	if ("status" == characteristic.name)
		return this.status;
}
```
***

#### `disconnect()`
`disconnect` 関数を使用して、BLEクライアント接続を終了します。

```javascript
import {address} from "btutils";

onReady() {
	this.allowed = address`11:22:33:44:55:66`;
}
onConnected(device) {
	this.stopAdvertising();
	if (!device.address.equals(this.allowed))
		this.disconnect();
}
onDisconnected(device) {
	trace("device disconnected\n");
}
```

***

#### `onConnected(device)`
| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `device` | `object` | `device` オブジェクト。詳細は [Class Client](#classclient) セクションを参照してください。 |

`onConnected` コールバック関数は、クライアントが `BLEServer` に接続したときに呼び出されます。

***

#### `onDisconnected(device)`
| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `device` | `object` | `device` オブジェクト。詳細は [Class Client](#classclient) セクションを参照してください。 |

`onDisconnected` コールバック関数は、クライアント接続が閉じられたときに呼び出されます。

***

#### `close()`
`close` 関数を使用して、BLEクライアント接続を終了し、すべてのBLEリソースを解放します。

```javascript
onDisconnected(device) {
	this.close();
}
```
***

<a id="gattservices"></a>
## GATT サービス
GATTサービスは、BLEサーバーまたはクライアントプロジェクトの `bleservices` ディレクトリにあるJSONファイルによって定義されます。JSONファイルはBLEクライアントにとってオプションです。各JSONファイルは、1つ以上のキャラクタリスティックを持つ単一のサービスを定義します。JSONはModdableの `mcconfig` コマンドラインツールによってプラットフォーム固有のネイティブコードに自動的に変換され、コンパイルされたオブジェクトコードはアプリにリンクされます。

サービス用のJSONファイルは、単一の `service` プロパティを持つオブジェクトで構成されています。`service` プロパティは、以下のトップレベルプロパティを含むオブジェクトです：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `uuid` | `string` | サービス UUID。
| `characteristics` | `object` | サービスキャラクタリスティックに関する詳細を持つオブジェクト。各キーはキャラクタリスティック名に対応します。

`characteristics` オブジェクトの各項目には、以下のプロパティが含まれています。GATTサービスJSONファイルを含むBLEクライアントアプリケーションでは、`uuid` と `type` プロパティのみが必須であることに注意してください。

| プロパティ | 型 | 説明 | BLE クライアントで必須 |
| --- | --- | --- | :--- |
| `uuid` | `string` | キャラクタリスティック UUID。 | Y
| `maxBytes` | `number` | キャラクタリスティックの値を格納するために必要な最大バイト数。
| `type` | `string` | オプションの JavaScript データ値の型。サポートされている型には `Array`、`ArrayBuffer`、`String`、`Uint8`、`Uint8Array`、`Int8Array`、`Int16Array`、`Uint16`、`Uint16Array`、および `Uint32` が含まれます。`type` プロパティが存在しない場合、データ値の型はデフォルトで `ArrayBuffer` になります。`BLEServer` および `BLEClient` クラスは、基盤となる BLE 実装によってバッファで提供されるキャラクタリスティックの値を要求された `type` に自動的に変換します。 | Y
| `permissions` | `string` | キャラクタリスティックの権限。サポートされている権限には `read`、`readEncrypted`、`write`、および `writeEncrypted` が含まれます。複数の権限はカンマで区切って指定できますが、各キャラクタリスティックに対して read/readEncrypted と write/writeEncrypted のいずれか一方のみを指定できます。
| `properties` | `string` | キャラクタリスティックのプロパティ。サポートされているプロパティには `read`、`write`、`writeNoResponse`、`notify`、`indicate`、および `extended` が含まれます。複数のプロパティはカンマで区切って指定できます。
| `value` | `array`, `string`, または `number` | オプションのキャラクタリスティックの値。`BLEServer` クラスは、ここで指定された値を `type` プロパティで指定された型に自動的に変換します。

`value` プロパティを含むキャラクタリスティックは静的と見なされます。`BLEServer` クラスは、静的キャラクタリスティックの値の読み取り要求に自動的に応答し、GATTサービスをホストするために必要なスクリプトコードをさらに削減します。

以下は、Bluetooth [Battery Service](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.battery_service.xml) に対応するJSONの例です：

	{
		"service": {
			"uuid": "180F",
			"characteristics": {
				"battery": {
					"uuid": "2A19",
					"maxBytes": 1,
					"type": "Uint8",
					"permissions": "read",
					"properties": "read",
					"value": 85
				},
			}
		}
	}

サービスは次のように定義されます：

* サービスUUIDは0x180Fです
* サービスには「battery」という名前の単一のキャラクタリスティックが含まれています
	* キャラクタリスティックUUIDは0x2A19です
	* キャラクタリスティックは符号なし8ビット値を表します
	* キャラクタリスティックの値は85で、クライアントはキャラクタリスティックを読み取る権限のみを持ちます

<a id="blesecurity"></a>
## BLE セキュリティ

BLEクライアントとサーバーは、`securityParameters`プロパティアクセサを使用して、デバイス間で転送されるデータを保護するためにリンク層のセキュリティをオプションで要求します。パスキーのペアリングでは、接続を確立する前に両方のデバイスがコードを交換して確認する必要があります。ペアリングプロセスが完了すると、接続と転送されるデータは暗号化されます。BLEセキュリティマネージャー（SM）は、ペアリングとキー配布のための方法とプロトコルを定義します。

<a id="classsm"></a>
## SMクラス

`SM`クラスは、BLEクライアントとサーバーのセキュリティ要件とデバイスの機能を設定するために使用されるオブジェクトを提供します。`SM`クラスは、`BLEClient`クラスと`BLEServer`クラスの両方で利用可能です。ここで定義されているセキュリティコールバック関数は、`BLEClient`クラスと`BLEServer`クラスによってホストされます。

`IOCapability`オブジェクトには、次のプロパティが含まれています：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `NoInputNoOutput` | `number` | デバイスには入力または出力機能がありません
| `DisplayOnly` | `number` | デバイスには出力機能のみがあります
| `KeyboardOnly` | `number` | デバイスには入力機能のみがあります
| `KeyboardDisplay` | `number` | デバイスには入力および出力機能があります
| `DisplayYesNo` | `number` | デバイスには出力機能と、YesまたはNoの応答のためのボタンフィードバックがあります

キーボードデバイスで暗号化によるMITM保護を要求するには：

```javascript
import {IOCapability} from "sm";

onReady() {
	this.securityParameters = { mitm:true, ioCapability:IOCapability.KeyboardOnly };
}
```

### 関数

#### `deleteBonding(address, addressType)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `address` | `object` | ピアデバイスのBluetoothアドレスを含む`ArrayBuffer` |
| `addressType` | `number` | ピアデバイスのBluetoothアドレスタイプ。サポートされているアドレスタイプについては`GAP.AddressType`を参照してください。 |

`deleteBonding`関数を呼び出して、指定された`address`と`addressType`を持つピアデバイスの保存されたボンディング情報を削除します。ボンドが削除されると、`onBondingDeleted`コールバックが呼び出されます。`onBondingDeleted`コールバックは、`BLEClient`クラスと`BLEServer`クラスの両方でサポートされています。

ピアデバイスから切断した後にボンディング情報を削除するには：

```javascript
onDisconnected(device) {
	SM.deleteBonding(device.address, device.addressType);
}
onBondingDeleted(params) {
	trace(`device ${params.address} bond deleted\n`);
}
```

***

#### `deleteAllBondings()`

`deleteAllBondings`関数を使用して、永続ストレージからすべてのボンディング情報と暗号化キーを削除します。

```javascript
onReady() {
	SM.deleteAllBondings();
}
```

***

#### `passkeyInput(address, value)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `address` | `object` | ピアデバイスのBluetoothアドレスを含む`ArrayBuffer`
| `value` | `number` | 入力されたパスキーの値

`onPasskeyInput`コールバック関数から`passkeyInput`関数を呼び出して、入力パスキーの値を提供します：

```javascript
onPasskeyInput(params) {
	let passkeyValue = 123456;
	this.passkeyInput(params.address, passkeyValue);
}
```

***

#### `passkeyReply(address, result)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `address` | `object` | ピアデバイスのBluetoothアドレスを含む`ArrayBuffer`
| `result` | `boolean` | パスキーの値を確認するには`true`を設定し、そうでない場合は`false`を設定

`onPasskeyConfirm`コールバック関数から`passkeyReply`関数を呼び出して、ピアデバイスによって表示されたパスキーを確認します：

```javascript
onPasskeyConfirm(params) {
	// passkey is valid
	this.passkeyReply(params.address, true);
}
```

***

#### `onSecurityParameters(params)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `params` | `object` | 適用されたデバイスのセキュリティプロパティ。

`onSecurityParameters` コールバックは、デバイスのセキュリティ要件とI/O機能が設定された後に呼び出されます。

```javascript
import {IOCapability} from "sm";

onReady() {
	this.securityParameters = { mitm:true, ioCapability:IOCapability.NoInputNoOutput };
}
onSecurityParameters() {
	this.startScanning();
}
```

***

#### `onAuthenticated(params)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `params` | `object` | 認証手続きに関連するプロパティ。

`params` オブジェクトには次のプロパティが含まれています：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `bonded` | `boolean` | デバイスがピアとボンディングしている場合は `true` に設定。

`onAuthenticated` コールバックは、認証手続きが正常に完了したとき、つまりデバイスのペアリングまたはボンディングが成功した後に呼び出されます。`onAuthenticated` コールバックは、`BLEClient` および `BLEServer` クラスでサポートされています。

```javascript
onAuthenticated(params) {
	trace(`authentication success, bonded = ${params.bonded}\n`);
}
```

***

#### `onPasskeyConfirm(params)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `params` | `object` | パスキー確認に関連するプロパティ。

`params` オブジェクトには以下のプロパティが含まれています：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `address` | `object` | ピアデバイスのBluetoothアドレスを含む`ArrayBuffer`
| `passkey` | `number` | 確認するパスキー

`onPasskeyConfirm` コールバックは、ユーザーがピアデバイスに表示されたパスキー値を確認する必要があるときに呼び出されます。コールバックは `passkeyReply` を呼び出し、パスキー値を確認するために `true` または `false` を渡します。

```javascript
onPasskeyConfirm(params) {
	trace(`confirm passkey: ${params.passkey}\n`);

	// passkey is valid
	this.passkeyReply(params.address, true);
}
```
***

#### `onPasskeyDisplay(params)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `params` | `object` | パスキー表示に関連するプロパティ。

`params` オブジェクトには以下のプロパティが含まれています：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `address` | `object` | ピアデバイスのBluetoothアドレスを含む`ArrayBuffer`
| `passkey` | `number` | 表示するパスキー

`onPasskeyDisplay` コールバックは、デバイスがパスキーを表示する必要があるときに呼び出されます。

```javascript
onPasskeyDisplay(params) {
	// display passkey on screen
	trace(`display passkey: ${params.passkey}\n`);
}
```

***

#### `onPasskeyInput(params)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `params` | `object` | パスキー入力に関連するプロパティ。

`params` オブジェクトには次のプロパティが含まれています：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `address` | `object` | ピアデバイスのBluetoothアドレスを含む `ArrayBuffer`

`onPasskeyInput` コールバックは、デバイスがピアデバイスに表示されたパスキーを入力する必要があるときに呼び出されます。`inputPasskey` 関数は入力されたパスキー値を返すために呼び出されます。

```javascript
onPasskeyInput(params) {
	// display keyboard to enter passkey displayed by peer
	//let passkey = 123456;
	this.passkeyInput(params.address, passkey);
}
```

***

#### `onPasskeyRequested(params)`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `params` | `object` | 要求するパスキーに関連するプロパティ。

`params` オブジェクトには次のプロパティが含まれています：

| プロパティ | 型 | 説明 |
| --- | --- | :--- |
| `address` | `object` | ピアデバイスのBluetoothアドレスを含む `ArrayBuffer`

`onPasskeyRequested` コールバックは、デバイスがパスキー入力を要求する必要があるときに呼び出されます。コールバックは0から999,999の間の数値のパスキー値を返します。

```javascript
onPasskeyRequested(params) {
	// display keyboard to enter passkey
	let passkey = Math.round(Math.random() * 999999);
	return passkey;
}
```
> **注:** パスキーの値は整数ですが、常に6桁を含める必要があります。ホストアプリケーションは、表示のために先頭にゼロを追加する責任があります。

<a id="blewhitelisting"></a>
## BLEホワイトリスティング

ホワイトリスティングは、Bluetoothアドレスによるピアデバイスのフィルタリングを提供します。BLEクライアントはホワイトリストを使用して、ホワイトリスト内のデバイスへの周辺機器スキャン応答とその後の接続のみを受信できます。BLE周辺機器はホワイトリストを使用して、BLEクライアントのスキャンと接続をフィルタリングできます。ホワイトリストフィルタリングは、既知の信頼できるBLEデバイスとの通信に限定することで、セキュリティを強化します。

BLEクライアントとサーバーの両方で使用されるBLEホワイトリストは1つです。許可されるホワイトリストエントリの最大数は、プラットフォームに依存します。

> **注:** ホワイトリスティングは現在、Blue Geckoプラットフォームではサポートされていません。

<a id="classgapwhitelist"></a>
## GAPWhitelistクラス

`GAPWhitelist` クラスは、BLEホワイトリストを操作するための関数を提供します。

```javascript
import GAPWhitelist from "gapwhitelist";
```

### 関数

#### `add(address [,addressType])`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `address` | `string` または `object` | ホワイトリストに追加するピアアドレス。 |
| `addressType` | `number` | オプションのピアアドレスタイプ。デフォルトは `GAP.AddressType.PUBLIC`。 |

`add` 関数を使用して、ピアデバイスをホワイトリストに追加します。

Bluetoothアドレスをホワイトリストに追加し、ホワイトリスト内のデバイスからのみスキャン応答を受け取るには：

```javascript
onReady() {
	GAPWhitelist.add("B4:99:4C:34:D7:A7");
	this.startScanning({ filterPolicy:GAP.ScanFilterPolicy.WHITELIST });
}
```

Bluetoothアドレスをホワイトリストに追加し、フィルターポリシーを使用してスキャン要求と接続をホワイトリスト内のデバイスに制限するには：

```javascript
onReady() {
	this.deviceName = "Moddable HRM";
	GAPWhitelist.add("8C:10:21:79:C9:F3");
	this.onDisconnected();
}
onDisconnected() {
	this.stopMeasurements();
	this.startAdvertising({
		filterPolicy: GAP.AdvFilterPolicy.WHITELIST_SCANS_CONNECTIONS,
		advertisingData: {flags: 6, completeName: this.deviceName, completeUUID16List: [HEART_RATE_SERVIE_UUID, BATTERY_SERVICE_UUID]
	});
}
```

***

#### `remove(address [,addressType])`

| 引数 | 型 | 説明 |
| --- | --- | :--- |
| `address` | `string` または `object` | ホワイトリストから削除するピアアドレス。 |
| `addressType` | `number` | オプションのピアアドレスタイプ。デフォルトは `GAP.AddressType.PUBLIC`。 |

`remove` 関数を使用して、ピアデバイスをホワイトリストから削除します。

#### `clear()`

`clear` 関数を使用して、ホワイトリストからすべてのピアデバイスを削除します。

<a id="esp32platform"></a>
## ESP32プラットフォームでのBLEアプリ
`mcconfig` コマンドラインツールは、ホストアプリに必要なESP-IDFの[sdkconfig.defaults](https://github.com/Moddable-OpenSource/moddable/blob/public/build/devices/esp32/xsProj-esp32/sdkconfig.defaults) BLEオプションを**自動的に**設定します。ESP-IDFは、Apache [NimBLE](http://mynewt.apache.org/latest/network/index.html#) Bluetooth LE [5.1認証済み](https://launchstudio.bluetooth.com/ListingDetails/97856) オープンソースホストとデュアルモードの[Bluedroid](https://www.espressif.com/sites/default/files/documentation/esp32_bluetooth_architecture_en.pdf) スタックの両方をサポートしています。NimBLEは、Bluedroidに比べて、フラッシュ/RAMのフットプリントが小さい、バッファコピーが少ない、ビルドが速いなどの[いくつかの利点](https://blog.moddable.com/blog/moddable-sdk-improvements-for-esp32-projects/)を提供します。NimBLEは、ESP32ビルドでModdable SDKによってデフォルトで有効化されています。

>**注:** BLEオプションは、必要に応じてホストアプリによってさらにカスタマイズできます。これは、アプリケーションマニフェスト内でカスタムsdkconfigデフォルトエントリを含むディレクトリへのパス名を提供することによって行います。詳細については、[マニフェスト](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/tools/manifest.md)のドキュメントを参照してください。例えば、`CONFIG_BT_NIMBLE_MAX_CONNECTIONS`の値を増やして、複数のBLEクライアント接続をサポートすることができます。この値はアプリケーションマニフェストの`max_connections`値と一致する必要があります。

レガシーBluedroid実装を使用してBLEアプリをビルドするには、`ESP32_BLUEDROID`ビルド環境変数を`1`に設定します。この環境変数は、利便性のために`mcconfig`コマンドラインで設定できます：

	cd $MODDABLE/examples/network/ble/scanner
	ESP32_BLUEDROID=1 mcconfig -d -m -p esp32


<a id="geckoplatform"></a>
## Blue GeckoプラットフォームでのBLEアプリ
Blue GeckoでのBLEアプリのビルドとデプロイは、[Gecko開発者ドキュメント](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/devices/gecko/GeckoBuild.md)で説明されているワークフローに従います。BLEアプリの場合、`soc-ibeacon` Simplicity Studioのサンプルプロジェクトから始めることをお勧めします。

[make.blue.mk](../../../build/devices/gecko/targets/blue/make.blue.mk) makefileには、Geckoターゲットプラットフォーム、キット、およびパートを定義する変数が含まれています。デフォルトでは、このmakefileは、[BRD4104A](https://www.silabs.com/documents/login/reference-manuals/brd4104a-rm.pdf) 2.4 GHz 10 dBmラジオボードに搭載されたBlue Gecko [EFR32BG13P632F512GM48](https://www.silabs.com/products/wireless/bluetooth/blue-gecko-bluetooth-low-energy-socs/device.efr32bg13p632f512gm48) Bluetooth低エネルギーチップ用のアプリをビルドするように設定されています。別のBlue Geckoターゲット用にビルドを設定するには、makefileの `GECKO_BOARD`、`GECKO_PART`、`HWKIT`、および `HWINC` 変数を適切に変更してください。

Blue Gecko用の [scanner](../../../examples/network/ble/scanner) BLEアプリ `xs_gecko.a` アーカイブをビルドするには、以下のコマンドを実行します。

	cd $MODDABLE/examples/network/ble/scanner
	mcconfig -d -m -p gecko/blue

BLEアーカイブをビルドした後、そのアーカイブはSimplicity Studioにホストされているネイティブアプリにリンクされます。基本的なBLEクライアント/サーバーアプリの `main()` ネイティブコード関数は、次のように簡略化できます。

```c
	void main(void)
	{
		initMcu();
		initBoard();
		initApp();

		xs_setup();

		while (1) {
			xs_loop();
		}
	}
```

BLEアプリケーションは追加のスタックとヒープを必要とします。以下の値を使用して、Moddable BLEのサンプルアプリケーションを実行することができました：

	__STACK_SIZE=0x2000
	__HEAP_SIZE=0xA000

Simplicity Studioには、BLEサービスを定義するための**BLE GATT Configurator**が含まれています。ModdableアプリケーションはJSONファイルでBLEサービスを定義するため、コンフィギュレータツールによって生成される`gatt_db.c`と`gatt_db.h`ファイルを使用しません。これらの2つのファイルはSimplicity Studioプロジェクトから削除する必要があります。

<a id="exampleapps"></a>
## BLEサンプルアプリケーション
Moddable SDKには、多くのBLEクライアントおよびサーバーのサンプルアプリケーションが含まれており、それを基に構築することができます。一般的な使用例を実装する方法を示しているため、サンプルアプリケーションから始めることをお勧めします：

### クライアントアプリケーション

| 名前 | 説明 |
| :---: | :--- |
| [colorific](../../../examples/network/ble/colorific) | BLE電球の色を100ミリ秒ごとにランダムに変更します。
| [discovery](../../../examples/network/ble/discovery) | 特定のGATTサービスとキャラクタリスティックを発見する方法を示します。
| [heart-rate-client](../../../examples/network/ble/heart-rate-client) | [心拍数サービス](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.heart_rate.xml)クライアントを実装する方法を示します。
| [hid-keyboard](../../../examples/network/ble/hid-keyboard) | HID over GATTプロファイルを実装するBLEキーボードに接続する方法を示します。
| [hid-mouse](../../../examples/network/ble/hid-mouse) | HID over GATTプロファイルを実装するBLEマウスに接続する方法を示します。
| [ios-media-sync](../../../examples/network/ble/ios-media-sync) | [Apple Media Service](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleMediaService_Reference/Specification/Specification.html#//apple_ref/doc/uid/TP40014716-CH1-SW48)クライアントを実装する方法を示す[Commodetto](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/commodetto.md)アプリ。
| [ios-time-sync](../../../examples/network/ble/ios-time-sync) | iPhoneの[現在時刻サービス](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.current_time.xml)に接続してデバイスの時計を設定する方法を示します。
| [powermate](../../../examples/network/ble/powermate) | [Griffin BLEマルチメディアコントロールノブ](https://griffintechnology.com/us/powermate-bluetooth)からボタンの回転と押下通知を受信します。
| [scanner](../../../examples/network/ble/scanner) | 周辺機器のアドバタイズ名をスキャンして表示します。
| [scanner-whitelist](../../../examples/network/ble/scanner-whitelist) | ホワイトリストに登録された周辺機器のアドバタイズ名をスキャンして表示します。
| [security-client](../../../examples/network/ble/security-client) | SMPを使用してセキュアな健康温度計BLEクライアントを実装する方法を示します。`security-client`は[security-server](../../../examples/network/ble/security-server)アプリに接続できます。
| [sensortag](../../../examples/network/ble/sensortag) | [TI CC2541 SensorTag](http://www.ti.com/tool/CC2541DK-SENSOR#technicaldocuments)のオンボードセンサーからセンサー通知を受信します。
| [tempo](../../../examples/network/ble/tempo) | [Blue Maestro環境モニター](https://www.bluemaestro.com/product/tempo-environment-monitor/)ビーコンから温度、湿度、気圧を読み取ります。
| [uart-client](../../../examples/network/ble/uart-client) | Nordic UARTサービスクライアントを実装する方法を示します。`uart-client`は[uart-server](../../../examples/network/ble/uart-server)アプリに接続できます。

### サーバーアプリケーション

| 名前 | 説明 |
| :---: | :--- |
| [advertiser](../../../examples/network/ble/advertiser) | BLEクライアントが接続するまでアドバタイズをブロードキャストします。
| [advertiser-notify](../../../examples/network/ble/advertiser-whitelist) | 送信された各アドバタイズデータパケットに対して通知コールバックを受け取るようにアドバタイズ主を設定する方法を示します。
| [advertiser-whitelist](../../../examples/network/ble/advertiser-whitelist) | ホワイトリストに登録されたBLEクライアントが接続するまでアドバタイズをブロードキャストします。
| [health-thermometer-server](../../../examples/network/ble/health-thermometer-server) | Bluetooth [Health Thermometer Service](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.health_thermometer.xml) を実装します。
| [health-thermometer-server-gui](../../../examples/network/ble/health-thermometer-server-gui) | ESP32用の[Piu](../../../documentation/piu/piu.md)アプリで、Bluetooth [Health Thermometer Service](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.health_thermometer.xml) を実装します。
| [heart-rate-server](../../../examples/network/ble/heart-rate-server) | Bluetooth [Heart Rate Service](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.heart_rate.xml) を実装します。
| [security-server](../../../examples/network/ble/security-server) | SMPを使用してセキュアな健康温度計BLEサーバーを実装する方法を示します。`security-server`は[security-client](../../../examples/network/ble/security-client)アプリに接続できます。
| [uri-beacon](../../../examples/network/ble/uri-beacon) | Google's [Physical Web](https://github.com/google/physical-web) 発見サービスと互換性のある[UriBeacon](https://github.com/google/uribeacon/tree/uribeacon-final/specification)を実装します。
| [wifi-connection-server](../../../examples/network/ble/wifi-connection-server) | ESP32上でBLE WiFi接続サービスを展開します。この接続サービスにより、BLEクライアントはSSIDとパスワードのキャラクタリスティックを書き込むことでBLEデバイスをWiFiアクセスポイントに接続できます。
| [uart-server](../../../examples/network/ble/uart-server) | Nordic UART Serviceサーバーを実装する方法を示します。`uart-server`は[uart-client](../../../examples/network/ble/uart-client)アプリに接続できます。
