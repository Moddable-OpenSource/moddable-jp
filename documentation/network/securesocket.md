# TLS (セキュアソケット)
Copyright 2017-2022 Moddable Tech, Inc.<BR>
改訂： 2022年11月11日

## 目次

* [セキュアソケット](#securesocket)
	* [HTTPクライアントでの使用 (`https:`)](#https)
	* [WebSocketクライアントでの使用 (`wss:`)](#websockets)
	* [MQTTクライアントでの使用 (`mqtts:`)](#mqtts)
	* [TLS証明書](#certificates)
		* [組み込み証明書の使用](#certificate-bulitin)
		* [証明書の提供](#certificate-providing)
		* [PEMをDERに変換](#converting-pem)
		* [証明書ストアの更新](#certificate-update)
	* [メモリ](#memory)
* [設定](#configuration)

<a id="securesocket"></a>
## `SecureSocket`クラス

- **ソースコード:** [securesocket](../../modules/crypt/securesocket)
- **関連するサンプル:** [socketsecure](../../examples/network/socket/socketsecure), [httpsget](../../examples/network/http/httpsget), [mqttsecure](../../examples/network/mqtt/mqttsecure)

`SecureSocket`クラスはTLSを実装しています。現時点では、`SecureSocket`にはリスナーはなく、TCP専用のソケットのみが存在します（UDPはありません）。

```js
import SecureSocket from "securesocket";
```

### `constructor(dictionary)`

`SecureSocket` のコンストラクタは、初期化パラメータのオブジェクト辞書を引数として受け取ります。この辞書は `Socket` 辞書のスーパーセットです（詳細は [ネットワークドキュメント](./network.md) の `Socket` クラスのドキュメントを参照してください）。

`SecureSocket` は、TLS接続を構成するために使用される辞書である `secure` という追加のプロパティを持つコンストラクタの辞書を拡張します。

`secure` 辞書には次のプロパティを含めることができます：

| パラメータ | デフォルト値 | 説明 |
| :---: | :---: | :--- |
| `protocolVersion` | `0x302` (TLS 1.1) | 実装する TLS プロトコルの最小バージョン（16進数）。<BR><BR>- `0x303` は TLS 1.2<BR>- `0x302` は TLS 1.1<BR>- `0x301` は TLS 1.0
| `certificate` | 該当なし | DER（バイナリ）形式の証明書を `ArrayBuffer`、`Uint8Array`、またはホストバッファに含む
| `trace` | `false` | `true` の場合、TLS スタックはその活動をログ出力します。これは障害診断に役立ちます。
| `verify` | `true` | `false` の場合、サーバーによって提供される証明書チェーンは検証されません。これは本番システムでは決して行うべきではありませんが、デバッグ時には役立つことがあります。
| `tls_max_fragment_length` | 該当なし | 要求された最大フラグメントサイズを示す数値。残念ながら、多くのサーバーはこのオプションの拡張を無視します。サポートされている場合、メモリ要件を減らすのに役立ちます。
| `applicationLayerProtocolNegotiation` | 該当なし | [RFC 7301](https://datatracker.ietf.org/doc/html/rfc7301) をサポートします。単一のアプリケーション層プロトコルのサポートを示す `String` または `ArrayBuffer` 値、または複数のアプリケーション層プロトコルのサポートを示す一つ以上の `String` および `ArrayBuffer` 値の `Array` を指定します。
| `clientKey` | 該当なし | DER（バイナリ）形式のキーを `ArrayBuffer`、`Uint8Array`、またはホストバッファに含む
| `clientCertificates ` | 該当なし | DER（バイナリ）形式の一つ以上のクライアント証明書を `ArrayBuffer`、`Uint8Array`、またはホストバッファに含む配列

以下の例では、TLSソケットはTLSのバージョン`0x303`（TLS 1.2に対応）をサポートするように作成されています。

```js
let socket = new SecureSocket({
	host: "www.example.com",
	port: 443,
	secure: {
		protocolVersion: 0x303
	}
});
```

<a id="https"></a>
### HTTPクライアントでの使用 (`https:`)

HTTP `Client`クラスは、コンストラクタの辞書にオプションの`Socket`プロパティを受け入れます。このプロパティを`SecureSocket`に設定してHTTPSリクエストを行います。TLS接続を構成するために`secure`プロパティを提供することができます：

```js
let request = new Request({
	host: "www.howsmyssl.com",
	path: "/",
	response: String,
	Socket: SecureSocket,
	port: 443,
	secure: {
		trace: true,
		protocolVersion: 0x303
	}
});
```

<a id="websockets"></a>
### WebSocketクライアントでの使用 (`wss:`)

WebSocket `Client`クラスは、コンストラクタの辞書にオプションの`Socket`プロパティを受け入れます。このプロパティを`SecureSocket`に設定してWSSリクエストを行います。TLS接続を構成するために`secure`プロパティを提供することができます：

```js
let ws = new Client({
	host: "echo.websocket.org",
	port: 443,
	Socket: SecureSocket,
	secure: {
		protocolVersion: 0x302
	}
});
```

<a id="mqtts"></a>
### MQTTクライアントでの使用 (`mqtts:`)

MQTT `Client`クラスは、コンストラクタの辞書にオプションの`Socket`プロパティを受け入れます。このプロパティを`SecureSocket`に設定してセキュアなMQTT接続を確立します。TLS接続を構成するために`secure`プロパティを提供することができます：

```js
const mqtt = new MQTT({
	host: "iot.aws.com",
	id: "unique mqtt client id",
	port: 8883,
	Socket: SecureSocket,
	secure: {
		protocolVersion: 0x303,
		applicationLayerProtocolNegotiation: "mqtt",
		certificate: new Resource("AmazonRootCA1.der"),
		clientKey: new Resource("device.key.der"),
		clientCertificates: [new Resource("device.crt.a.der")]
	},
});
```

<a id="certificates"></a>
### TLS証明書

TLS証明書は、サーバーに送信するデータを暗号化するために使用されます。`SecureSocket`オブジェクトは、DER（バイナリ）形式の証明書を使用します。

<a id="certificate-bulitin"></a>
#### 組み込み証明書の使用
証明書ストアは、Moddable SDKの[`modules/crypt/data`ディレクトリ](../../modules/crypt/data)にあります。すべての証明書がすべてのアプリケーションで使用されるわけではないため、すべての証明書をデフォルトで含めるのは限られたフラッシュメモリの無駄になります。代わりに、証明書はマニフェストの`resources`セクションに明示的に指定します。

```json
"resources": {
    "*": [
        "$(MODULES)/crypt/data/ca9",
    ]
}
```

どの証明書を含める必要があるかわからない場合は、ウェブサイトにアクセスしようとするアプリケーションを実行し、どの証明書が読み込まれないかを確認してください。アプリケーションは次のような例外をスローします：

![リソースが見つからないエラーメッセージ](../assets/network/ssl-resource-not-found.png)

この場合、`ca109.der`を含める必要があるため、マニフェストの`resources`オブジェクトに追加する必要があります。


```json
"resources": {
    "*": [
        "$(MODULES)/crypt/data/ca109"
    ]
}
```

証明書ストアの代替として、アプリケーションに必要な証明書を配置し、`secure`ディクショナリに適切な証明書を渡すことができます。

```js
let request = new Request({
	host: "www.howsmyssl.com",
	path: "/",
	response: String,
	Socket: SecureSocket,
	port: 443,
	secure: {
		certificate: new Resource("ca109.der")
	}
});
```

<a id="certificate-providing"></a>
#### 証明書の提供
Moddable SDKに含まれている証明書を使用する必要はありません。SecureSocketのディクショナリにDER形式の有効な証明書を渡すことができます。

```js
let request = new Request({
    ...
    secure: {
    	certificate: new Resource("mycert.der")
    }
});
```

<a id="converting-pem"></a>
#### PEMをDERに変換する
`SecureSocket`の実装では、証明書をDER（バイナリ）形式で提供する必要があります。PEM（Base64エンコード）形式の証明書を持っている場合は、DERに変換する必要があります。

可能であれば、プロジェクトに追加する前にPEMファイルをDER形式に変換してください。変換を行うツールは多数ありますが、信頼できる選択肢として`openssl`があります。以下のコマンドラインは多くの証明書に対して機能します（PEMファイルのパスを`data.pem`に、出力ファイルのパスを`data.der`に置き換えてください）:


```shell
openssl x509 -inform pem -in data.pem -out data.der -outform der
```


時には、実行時にPEMをDERに変換するしかない場合があります。例えば、プロビジョニング中にサービスからPEM形式の証明書を受け取り、後でその証明書を使用してTLS接続を確立する必要がある場合です。Moddable SDKは、このような状況に対応するために[`pemtoDER`](../crypt/crypt.md#transform-pemToDER)および[`privateKeyToPrivateKeyInfo`](../crypt/crypt.md#transform-privateKeyToPrivateKeyInfo)関数を提供しています。これらの関数はCrypt `Transform`クラスの一部です。


<a id="certificate-update"></a>
#### 証明書ストアの更新
証明書ストアは、`$MODDABLE/modules/crypt/data`に新しい証明書や改訂された証明書を追加することで更新できます。証明書は`ca0.der`から始まる連番で番号が付けられます。証明書を追加または更新した後、証明書ストアインデックス`ca.ski`を再構築する必要があります。インデックスを更新するには、引数なしで`mcprintski`ツールを実行します。`ca.ski`インデックスファイルはその場で更新されます。（このツールは現在macOSでのみ利用可能です。WindowsおよびLinuxは、ツールのmakefileを更新することでサポートされる可能性があります）

<a id="memory"></a>
### メモリ

TLSハンドシェイクにはかなりの量のメモリが必要です。必要な正確な量は、接続するサイトによって異なります。大まかなガイドラインとして、以下のメモリが空いている必要があります：

- スタックに4 KB
- チャンクヒープに10 KB
- スロットヒープに6 KB
- システムヒープに2 KB

ハンドシェイクが完了すると（例えば、セキュア接続が確立されると）、メモリ使用量は大幅に減少します。

しかし、サーバーが大きなフラグメントを送信する場合（例えば、apple.comは16 KBのフラグメントを送信します）、それらをバッファするための十分な空きRAMがマイクロコントローラーにないかもしれません。リクエストは失敗します。IoTデバイスと連携するように設計されたセキュアなウェブサーバーは、デフォルトでより小さなフラグメントを使用するか、`tls_max_fragment_length` 拡張をサポートします。

HTTPSを使用する場合、HTTPクライアントが応答本文全体をメモリにバッファするのではなく、応答本文が到着するたびにストリーミングモードを使用して取得するのが最善です。このサンプルについては、[`httpsgetstreaming` のサンプル](../../examples/network/http/httpsgetstreaming)を参照してください。

<a id="configuration"></a>
## 設定

TLS実装はデフォルトでDHEおよびECDHEを使用する暗号スイートを有効にします。これらのモードは計算が複雑であるため、遅いマイクロコントローラ上で実行するのに長い時間がかかることがあります。これらのスイートの使用はマニフェストの`config`セクションで制御されます。これらのスイートを無効にするには、プロジェクトのマニフェストに以下を含めてください：

```json
"config": {
	"tls": {
		"DHE_RSA": false,
		"ECDHE_RSA": false
	}
}
```
