# イーサネット
Copyright 2021 Moddable Tech, Inc.<BR>
改訂： 2024年9月10日

## 概要

Moddable SDKはESP32のためのイーサネットをサポートしています。イーサネットサポートは、Wi-FiやHTTP/HTTPS、MQTT、WebSockets、DNS、mDNSなどのプロトコルをサポートする[ネットワークサポート](./network.md)の拡張です。Moddable SDKのほとんどの[ネットワーキングのサンプル](../../examples/network)はデフォルトでWi-Fiを有効にしていますが、Wi-Fiで動作するサンプルは簡単にイーサネットを使用するように変更できます。

このドキュメントでは、アプリケーションでイーサネットを有効にする方法、イーサネット接続を確立および監視するためのJavaScript APIの詳細、および互換性のあるイーサネットブレイクアウトボードの配線手順について説明します。

## 目次

* [配線](#wiring)
* [アプリケーションでのアプリケーションでのイーサネットの有効化](#enabling-ethernet)
* [イーサネットクラス](#class-ethernet)

<a id="wiring"></a>
## 配線

Moddable SDKは、ハードウェアに配線し、ESP-IDFと統合できる任意のイーサネットモジュールをサポートします。さまざまなENC28J60イーサネットモジュールブレイクアウトボード（下の写真）で作業し、良好な結果を得ました。このセクションには、ENC28J60モジュールの配線情報が含まれています。

<img src="https://www.olimex.com/Products/Modules/Ethernet/ENC28J60-H/images/thumbs/310x230/ENC28J60-H-01.jpg" height=100>
<img src="https://www.atomsindustries.com/assets/images/items/1077/1077.jpg" height=100>

ESP32はSPIを介してENC28J60と通信します。

| ENC28J60 | ESP32 |
| :---: | :---: |
| INT | GPIO 33 |
| CS | GPIO 27 |
| MISO | GPIO 35 |
| MOSI | GPIO 26 |
| SCK | GPIO 0 |
| PWR | PWR |
| GND | GND |

以下の図は、Moddable Two（ESP32ベースのハードウェアモジュール）がHanRun ENC28J60モジュールに配線されている様子を示しています。

![](../assets/network/enc28j60-wiring-moddable-two.png)

他のESP32ベースの開発ボードも使用できます。多くの開発者は、以下の図に示されているNodeMCU ESP32ボードを使用しています。これらのボードでは、GPIO 0のピン配置がないため（GPIO 0はBOOTボタンによって使用されるため）、GPIO 0パッドにハンダ付けする必要があります。

![](../assets/network/enc28j60-wiring-nodemcu.png)

<a id="enabling-ethernet"></a>
## アプリケーションでのイーサネットの有効化

Moddable SDKの[ネットワーキングのサンプル](../../examples/network)のほとんどは、デフォルトでWi-Fiを有効にしています。これらは、`manifest_net.json`マニフェストを含めることで実現しています。

```jsonc
"include": [
	/* other included manifests here */
	"$(MODDABLE)/examples/manifest_net.json"
],
```

これらのサンプルでイーサネットを使用したい場合は、単に `manifest_net.json` を `manifest_net_ethernet.json` に置き換えるだけです。`manifest_net.json` と `manifest_net_ethernet.json` の両方を含めるべきではないことに注意してください。どちらか一方のみを含めてください。

```jsonc
"include": [
	/* other included manifests here */,
	"$(MODDABLE)/modules/network/ethernet/manifest_net_ethernet.json"
],
```

`manifest_net_ethernet.json` マニフェストには、イーサネットへの接続を自動的に設定する `setup/network` モジュールが含まれています。接続はアプリケーションの残りの部分が実行される前に設定されます。つまり、デバイスはイーサネットに接続し、その後アプリケーションの `main` モジュールが実行されます。デバイスがイーサネットに接続できない場合、`main` モジュールは実行されません。

独自のアプリケーションから `setup/network` モジュールを削除し、代わりに以下の [イーサネットクラス](#class-ethernet) セクションで説明されているJavaScript APIを使用してアプリケーションコード内でイーサネット接続を設定および監視することもできます。

### ESP-IDF ENC28J60 ドライバマルチキャストパケット問題

ESP-IDFのENC28J60ドライバには、マルチキャストパケットがハードウェアでフィルタリングされるバグが含まれています。Moddableは、将来のESP-IDFリリースに含まれる[ESP-IDFプルリクエストを通じてこのバグを修正しました](https://github.com/espressif/esp-idf/commit/3e9cdbdedfd47813a55454ff3b9541fb5c9f9a61)。それまでの間、プロジェクトでマルチキャストが必要な場合は、次の小さな修正をファイル `$IDF_PATH/examples/ethernet/enc28j60/main/esp_eth_mac_enc28j60.c` の551-552行目に適用できます：

```diff
-    // set up default filter mode: (unicast OR broadcast) AND crc valid
-    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN) == ESP_OK,
+    // set up default filter mode: (unicast OR broadcast OR multicast) AND crc valid
+    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN | ERXFCON_MCEN) == ESP_OK,
```

<a id="class-ethernet"></a>
## イーサネットクラス

- **ソースコード:** [ethernet](../../modules/network/ethernet)
- **関連するサンプル:** [`ethernet-test`](../../examples/network/ethernet/ethernet-test), [`ethernet-test-graphic`](../../examples/network/ethernet/ethernet-test-graphic), [`ethernet-monitor`](../../examples/network/ethernet/ethernet-monitor)

`Ethernet` クラスは、イーサネットモジュールのイーサネット機能を使用および構成するためのアクセスを提供します。

```js
import Ethernet from "ethernet";
```

イーサネット用のソフトウェアは、接続の確立を除いてWi-Fiとほぼ同じです。イーサネットはWi-Fiと同じAPIを実装しているため、Moddable SDKのサンプルや自分のアプリケーションでWi-Fiの代わりにイーサネットをほぼそのまま使用することができます。

### `static start()`

`start`メソッドは、デバイスのネットワーク接続を管理するための基礎的なプロセスを開始します。

```js
Ethernet.start();
```

### `constructor(callback)`

`Ethernet`コンストラクタは、イーサネットのステータスを監視するためのモニターを作成します。ステータスメッセージを受け取るためのコールバック関数を取ります。メッセージはイーサネット接続の状態を表す文字列です。`Ethernet`クラスには、開発者の便宜のために各文字列に対応するプロパティがあります。

| プロパティ | 説明 |
| :---: | :--- |
| `Ethernet.connected` | 物理リンクが確立された
| `Ethernet.disconnected` | 物理リンクが失われた
| `Ethernet.gotIP` | DHCPによってIPアドレスが割り当てられ、ネットワーク接続が使用可能になった
| `Ethernet.lostIP` | IPアドレスが失われ、ネットワーク接続が使用できなくなった

> **注:** アプリケーションは、イーサネット接続接続を開始するために`Ethernet`クラスの静的メソッド`start`を呼び出す必要があります。`start`を呼び出さないと、`Ethernet`コンストラクタに渡されたコールバックは決して呼び出されません。

```js
Ethernet.start();
let monitor = new Ethernet((msg) => {
	switch (msg) {
		case Ethernet.disconnected:
			// Ethernet disconnected
			break;
		case Ethernet.gotIP:
			// Got IP address
			break;
	}
});
```

### `close()`

`close`メソッドは、`Ethernet`インスタンスとデバイスのネットワーク接続を管理する基礎プロセスとの間の接続を閉じます。言い換えれば、将来のコールバック関数の呼び出しを防ぎますが、ネットワークからの切断は行いません。

```js
let monitor = new Ethernet((msg) => {
	trace(`Ethernet msg: ${msg}\n`);
});

monitor.close();
```

### 例: イーサネット IPアドレスの取得

次の例は、イーサネット接続への接続プロセスを開始し、接続が成功してイーサネット接続デバイスにIPアドレスが割り当てられたときにコンソールにログ出力します。

`Net.get`メソッドは、Wi-Fiと同様にイーサネット接続インターフェースのIPアドレスとMACアドレスを取得するために使用できます。`Net.get`のオプションの第二引数は、どのインターフェースをクエリするかを指定します： `"ethernet"`、`"ap"`、または`"station"`。第二引数が提供されない場合、`Net.get`はアクティブなネットワークインターフェースをデフォルトとします（例えば、唯一のネットワーク接続がイーサネット接続の場合、イーサネット接続インターフェースがデフォルトになります）。

```js
Ethernet.start();
let monitor = new Ethernet((msg) => {
	switch (msg) {
		case Ethernet.connected:
			trace(`Physical link established. Waiting for IP address.\n`);
			break;

		case Ethernet.gotIP:
			let ip = Net.get("IP", "ethernet");
        	trace(`Ethernet connected. IP address ${ip}\n`);
        	break;

		case Ethernet.disconnected:
			trace(`Ethernet connection lost.\n`);
			break;
    }
});
```

### 例: イーサネットデバイスのMACアドレスを取得する

次の例では、イーサネットデバイスのMACアドレスを取得し、それをコンソールにログ出力します。

`Net.get`メソッドは[ネットワークドキュメント](./network.md)の**Net**セクションに記載されています。この例では、`Net.get`関数に渡される第2引数として文字列`"ethernet"`があることに注意してください。これは、ESP32のWi-FiデバイスのMACアドレスではなく、イーサネットデバイスのMACアドレスを取得したいことを指定しています。

```js
let mac = Net.get("MAC", "ethernet");
trace(`Ethernet MAC address is ${mac}\n`);
```
