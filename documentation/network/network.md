# ネットワーキング
Copyright 2017-2024 Moddable Tech, Inc.<BR>
改訂: 2024年2月23日

## 目次

* [ソケット](#socket)
* [リスナー](#listener)
* HTTP
	* [リクエスト](#http-request)
	* [サーバー](#http-server)
* WebSocket
	* [クライアント](#websocket-client)
	* [サーバー](#websocket-server)
* [ネット](#net)
* [WiFi](#wifi)
* [SNTP](#sntp)
* DNS
	* [定数](#dns)
	* [パーサー](#dns-parser)
	* [シリアライザー](#dns-serializer)
	* [サーバー](#dns-server)
* [MDNS](#mdns)
* [Telnet](#telnet)
* [Ping](#ping)
* [MQTT](#mqtt)

<a id="socket"></a>
## Socketクラス

- **ソースコード:** [socket](../../modules/network/socket)
- **関連するサンプル:** [socket](../../examples/network/socket/socket), [socketreadwrite](../../examples/network/socket/socketreadwrite)

`Socket` クラスは、TCP または UDP ソケットを使用して非ブロッキングのネットワーク接続を実装します。

```js
import {Socket, Listener} from "socket";
```

### `constructor(dictionary)`

`Socket` コンストラクタは、初期化パラメータの辞書を引数として取ります。コンストラクタはリモートホストへの接続を直ちに開始します。

IPアドレスが既知の場合、辞書の `address` プロパティを使用します。

```js
let socket = new Socket({address: "17.172.224.47", port: 80});
```

ホスト名で指定されたリモートサーバーへの接続を開始するには、辞書に `host` と `port` プロパティを含めます。ソケットはホスト名をIPアドレスに解決します。

```js
let host = "www.moddable.tech";
let port = 80;
let socket = new Socket({host, port});
```

デフォルトでは、新しいソケットはTCPを使用します。ソケットの種類は辞書で設定できます：

```js
let tcp = new Socket({host: "moddable.tech", port: 1234, kind: "TCP"});
let udp = new Socket({port: 123, kind: "UDP"});
let raw = new Socket({kind: "RAW", protocol: 1});
```

`Listener` からの新しい接続要求を受け入れるには、辞書に `listener` プロパティを指定します：

```js
let listener = new Listener({port: 80});
let socket = new Socket({listener});
```

TCPソケットの場合、辞書は2つのオプションプロパティをサポートします：

- `noDelay` - Nagleアルゴリズムが有効かどうかを制御するブール値 (`TCP_NODELAY`)。ほとんどのプラットフォームではデフォルトで有効になっています。状況によっては、これを無効にすることで書き込みパフォーマンスが向上する場合があります。

```js
	{...., noDelay: true}
```

- `keepalive` - ソケットのキープアライブ動作を制御するオブジェクトです。`idle` と `interval` プロパティはミリ秒単位です。例えば:

```js
	{...., keepalive: {idle: 60 * 1000, interval: 30 * 1000, count: 4}}
```

***

### `close()`

`close` 関数はソケットを即座に終了し、ソケットに関連するすべてのリソースを解放します。

```js
socket.close();
```

***

### `read(type [, until])`

`read` 関数はソケットからデータを受信します。データは `data` メッセージを受信したときにのみコールバック関数内で読み取ることができ、それ以外の時にデータを読み取ろうとすると失敗します。

すべての利用可能なデータを `String` に読み取るには:

```js
let string = this.read(String);
```

すべての利用可能なデータを `ArrayBuffer` に読み取るには:

```js
let buffer = this.read(ArrayBuffer);
```

1バイトを `Number` に読み取るには:

```js
let byte = this.read(Number);
```

12バイトを `String` または `ArrayBuffer` に読み取るには:

```js
let string = this.read(String, 12);
let buffer = this.read(ArrayBuffer, 12);
```

次のスペース文字までを `String` または `ArrayBuffer` に読み込みます。スペース文字が見つからない場合、利用可能なデータの残りが読み込まれます:

```js
let string = this.read(String, " ");
let buffer = this.read(ArrayBuffer, " ");
```

読み取りバッファ内のデータをスキップするには、`null` を読み込みます:

```js
this.read(null, 5);		// 5バイト先にスキップ
```

次のキャリッジリターン（または見つからない場合はバッファの終わり）までスキップするには:

```js
this.read(null, "\n");
```

`null` に読み込む場合、戻り値はスキップされたバイト数です。

バッファ内に残っている利用可能なバイト数を確認するには、引数なしで `read` を呼び出します:

```js
let bytesAvailable = this.read();
```

***

### `write(data [, data1, ...])`

`write` 関数はソケットにデータを送信します。1つ以上の引数を `write` に渡して送信することができます。

TCPソケットの場合、すべてのパラメータは送信されるデータです。

```js
socket.write("Hello");
socket.write(32);
socket.write("world.", 13);
socket.write(JSON.stringify(obj));
```

`String` と `ArrayBuffer` の値はそのまま送信されます。`Number` の値はバイトとして送信されます。

ソケットにデータを送信するためのバッファスペースが不足している場合、データは一切送信されません。送信可能なバイト数を確認するには、引数なしで `write` を呼び出します:

```js
let bytesToSend = socket.write();
```

UDPソケットの場合、最初の2つのパラメータはパケットを送信するIPアドレスとポートです。3番目のパラメータは `ArrayBuffer` として送信するデータです:

```js
socket.write("1.2.3.4", 1234, packet);
```

RAWソケットの場合、最初のパラメータはパケットを送信するIPアドレスです。2番目のパラメータは `ArrayBuffer` として送信するデータです:

```js
socket.write("1.2.3.4", packet);
```

複数回の `write` 呼び出しを行うよりも、複数のパラメータを持つ単一の `write` 呼び出しを行う方が効率的です。

***

### `get(what)`

`get` メソッドはソケットの状態情報を返します。`what` 引数は要求された状態を示す文字列です。状態が利用できない場合、`get` は `undefined` を返します。

| `what` | 説明 |
| :---: | :--- |
| `"REMOTE_IP"` | リモートエンドポイントのIPアドレスを返します。TCPソケットでのみ利用可能です。 |

***

### `callback(message [, value])`

ソケットのユーザーはコールバック関数を通じてステータス情報を受け取ります。コールバックの最初の引数はメッセージ識別子です。正の `message` 値は通常の操作を示し、負の `message` 値はエラーを示します。メッセージに応じて、追加の引数がある場合があります。

| `message` | 説明 |
| :---: | :--- |
| -2 | **エラー:** エラーが発生しました。ソケットはもう使用できません。 |
| -1 | **切断:** ソケットがリモートホストから切断されました。 |
| 1 | **接続:** ソケットがリモートホストに正常に接続されました。 |
| 2 | **データ受信:** ソケットがデータを受信しました。`value` 引数には読み取り可能なバイト数が含まれます。 |
| 3 | **データ送信:** ソケットが書き込まれたデータの一部または全部を正常に送信しました。`value` 引数には安全に書き込めるバイト数が含まれます。

UDPソケットの場合、`dataReceived` のコールバックにはメッセージ識別子の後に3つの追加引数があります。最初の引数はTCPソケットと同様に読み取り可能なバイト数です。2番目の引数は送信者のIPアドレスを含む文字列です。3番目の引数は送信者のポート番号です。

```js
callback(message, byteLength, remoteIP, remotePort) {}
```

RAWソケットの場合、`dataReceived`のコールバックにはメッセージ識別子の後に2つの追加引数があります。最初の引数はTCPソケットと同様に読み取り可能なバイト数です。2番目の引数は送信者のIPアドレスを含む文字列です。

```js
callback(message, byteLength, remoteIP) {}
```

***

### 例: HTTP GET

以下の例は、`Socket`オブジェクトを使用してシンプルなHTTP GETリクエストを行い、ヘッダーを含むレスポンスをコンソールにトレースする方法を示しています。この例は有用なHTTPクライアントとして意図されたものではありません。

```js
let host = "www.example.com";
let port = 80;
let socket = new Socket({host, port});

socket.callback = function(message, value)
{
	if (1 == message) {
		this.write("GET / HTTP/1.1\r\n");
		this.write("Host: ", host, "\r\n");
		this.write("Connection: close\r\n");
		this.write("\r\n");
	}
	else if (2 == message)
		trace(this.read(String));
}
```

***

<a id="listener"></a>
## Listenerクラス

- **ソースコード:** [socket](../../modules/network/socket)
- **関連するサンプル:** [socketlistener](../../examples/network/socket/socketlistener)

`Listener`クラスは、新しいTCP接続を受け入れるためのネットワークソケットリスナーを実装します。`Listener`クラスは`Socket`クラスと一緒に使用されます。

```js
import {Socket, Listener} from "socket";
```

### `constructor(dictionary)`

`Listener` コンストラクタは、初期化パラメータのオブジェクト辞書を引数として受け取ります。

リッスンするには、`port` プロパティを使用してリッスンするポートを指定します:

```js
let telnet = new Listener({port: 23});
```

***

### `callback()`

リスナーのユーザーは、コールバック関数を通じて通知されます。コールバック関数は接続要求を受け入れ、リスナーインスタンスを使用して Socket コンストラクタを呼び出すことで新しいソケットをインスタンス化します。

```js
telnet.callback = function() {
	let socket = new Socket({listener: this});
	...
}
```

***

### 例: HTTP サーバー

次の例は、`Listener` と `Socket` を使用して簡単な HTTP サーバーを実装しています。このサーバーは非常に簡単で、クライアントのリクエストを解析することさえしません。

```js
let count = 0;
let listener = new Listener({port: 80});
listener.callback = function() {
	let socket = new Socket({listener});
	let message = `Hello, server ${++count}.`;
	socket.write("HTTP/1.1 200 OK\r\n");
	socket.write("Connection: close\r\n");
	socket.write(`Content-Length: ${message.length}\r\n`);
	socket.write("Content-Type: text/plain\r\n");
	socket.write("\r\n");
	socket.write(message);
	socket.close();
}
```

***

<a id="http-request"></a>
## HTTPリクエストクラス

- **ソースコード:** [http](../../modules/network/http)
- **関連するサンプル:** [httpget](../../examples/network/http/httpget), [httppost](../../examples/network/http/httppost), [httpsget](../../examples/network/http/httpsget), および [その他多数](../../examples/network/http/)

HTTP `Request` クラスは、HTTP リクエストを行うためのクライアントを実装します。これは `Socket` クラスに基づいて構築されています。`Socket` クラスと同様に、HTTP `Request` は辞書ベースのコンストラクタと単一のコールバックを使用します。

```js
import {Request} from "http"
```

> **注**: リクエストボディに渡される文字列は、ASCII 範囲の 0 から 127 の文字のみを含むことができます。完全な UTF-8 を使用するには、文字列を `ArrayBuffer.fromString` または `TextEncoder` を使用してバッファに変換してください。

### `constructor(dictionary)`

新しい HTTP `Request` は、プロパティの辞書を使用して構成されます。この辞書は `Socket` 辞書のスーパーセットです。

HTTP `Request` が `Socket` 辞書に追加するプロパティの完全なリストは次のとおりです:

| パラメータ | デフォルト値 | 説明 |
| :---: | :---: | :--- |
| `port` | 80 | リモートポート番号 |
| `path` | `/` | HTTP URL のパス、クエリ、およびフラグメント部分 |
| `method` | `GET` | この HTTP リクエストに使用するメソッド |
| `headers` | デフォルトは空の配列 (例: `[]`) | 追加する HTTP ヘッダーを含む配列。偶数要素はヘッダー名、奇数要素は対応するヘッダー値です。 |
| `body` | `false` | リクエストボディの内容。完全なボディを持つ `String` または `ArrayBuffer` を提供します。コールバックを介して断片的にリクエストボディを提供するには `true` に設定します。 |
| `response` | `undefined` | リクエストが完了したときにコールバックに渡されるレスポンスボディに使用するオブジェクトのタイプ。`String`、`ArrayBuffer`、または `undefined` に設定できます。`undefined` に設定されている場合、レスポンスボディは到着時に断片的にコールバックに渡されます。 |

ポート80でルート "/" リソースを `String` としてリクエストするには:

```js
let request = new Request({host: "www.example.com", response: String});
```

ポート8080から "/info.dat" リソースを `ArrayBuffer` としてリクエストするには:

```js
let request = new Request({host: "www.example.com", path: "/info.dat", port: 8080, response: ArrayBuffer});
```

IPアドレス "192.0.1.15" を持つデバイスから "/weather.json" リソースを `String` オブジェクトとしてリクエストするには:

```js
let request = new Request({address: "192.0.1.15", path: "/weather.json", response: String});
```

DELETEリクエストを発行するには、辞書内の `method` プロパティを設定します:

```js
let request = new Request({address: "192.0.1.15", path: "/resource/to/delete", method: "DELETE"});
```

***

### `close()`

`close` 関数はHTTPリクエストを即座に終了し、ソケットおよびその他の関連メモリを解放します。

```js
request.close();
```

***

### `read(type [, until])`

`read` 関数は `Socket` クラスの read 関数と全く同じように動作します。`read` 関数は、レスポンスボディのフラグメントを提供するコールバック内でのみ呼び出すことができます。

> **注**: HTTP 読み取り関数の実装は、チャンク転送エンコーディングを使用するレスポンスに対して `until` 引数として `String` を渡すことを現在サポートしていません。

***

### `callback(message, val1, val2)`

`Request` オブジェクトのユーザーは、コールバック関数を通じてステータス情報を受け取ります。コールバックはメッセージを受け取り、一部のメッセージには追加のデータ値も含まれます。非負の `message` 値は通常の動作を示し、負の `message` 値はエラーを示します。

| `message` | `Request.` | 説明 |
| :---: | :---: | :--- |
|-2 | `error` |
| 0 | `requestFragment` | リクエストボディのフラグメントを取得します。このコールバックは、辞書の `body` プロパティが `true` に設定されている場合にのみ受信されます。呼び出されたとき、`val1` は送信可能な最大バイト数です。次のリクエストボディのフラグメントを含む `String` または `ArrayBuffer` を返します。フラグメントがもうない場合は `undefined` を返します。
| 1 | `status` | ステータスコード付きのレスポンスステータスを受信します。このコールバックは、HTTP レスポンスステータスラインが正常に受信されたときに呼び出されます。呼び出されたとき、`val1` は HTTP ステータスコード（例: OK の場合は 200）です。
| 2 | `header` | 1つのヘッダーを受信します。レスポンスの各ヘッダーに対してコールバックが呼び出されます。呼び出されたとき、`val1` は小文字のヘッダー名（例: `connection`）で、`val2` はヘッダー値（例: `close`）です。
| 3 | `headersComplete` | すべてのヘッダーを受信します。すべてのヘッダーが受信されたときにコールバックが呼び出されます。
| 4 | `responseFragment` | レスポンスボディのフラグメント。このコールバックは、完全な HTTP レスポンスボディのフラグメントが受信されたときに呼び出されます。`val1` はフラグメント内のバイト数で、`read` 関数を使用して取得できます。このコールバックは、`response` プロパティ値が `undefined` の場合にのみ呼び出されます。
| 5 | `responseComplete` | すべてのレスポンスボディを受信します。このコールバックは、レスポンスボディ全体が受信されたときに呼び出されます。`response` プロパティ値が `undefined` でない場合、`val1` にレスポンスが含まれます。

***

<a id="http-server"></a>
## HTTPサーバークラス

- **ソースコード:** [http](../../modules/network/http)
- **関連するサンプル:** [httpserver](../../examples/network/http/httpserver), [httpserverbmp](../../examples/network/http/httpserverbmp), [httpserverchunked](../../examples/network/http/httpserverchunked), [httpserverputfile](../../examples/network/http/httpserverputfile)

HTTP `Server` クラスは、HTTP リクエストに応答するサーバーを実装します。これは `Socket` クラスを基に構築されています。`Socket` クラスと同様に、HTTP `Server` クラスは辞書ベースのコンストラクタと単一のコールバックを使用します。

```js
import {Server} from "http"
```

> **注意**: レスポンスボディに渡される文字列は、ASCII 範囲の 0 から 127 の文字のみを含むことができます。完全な UTF-8 を使用するには、文字列を `ArrayBuffer.fromString` または `TextEncoder` を使用してバッファに変換してください。

### `constructor(dictionary)`

新しい HTTP サーバーは、プロパティの辞書を使用して構成されます。この辞書は `Socket` 辞書のスーパーセットです。

デフォルトポート (80) で HTTP サーバーを開くには:

```js
let server = new Server({});
```

ポート8080でHTTPサーバーを開くには:

```js
let server = new Server({port: 8080});
```

***

### `close([connections])`

`close`関数はHTTPサーバーを即座に終了し、サーバーリスナーソケットおよびその他の関連メモリを解放します。
`connections`がtrueの場合、サーバーへのすべてのアクティブな接続も閉じます。

```js
server.close();
```

***

### `detach(connection)`

`detach`関数はサーバーインスタンスのアクティブなHTTP接続を受け入れ、それをサーバーから削除し、接続のソケットインスタンスを返します。これは、HTTPサーバーの既存の接続をWebSocketサーバーに引き渡すことにより、HTTPとWebSocketの両方の接続を受け入れるHTTPエンドポイントを実装するのに役立ちます。

```js
server.detach(connection);
```

***

### `callback(message, val1, val2)`

サーバーのユーザーは`callback`関数を通じてステータス情報を受け取ります。`callback`はメッセージを受け取り、一部のメッセージには追加のデータ値も含まれます。正の`message`値は通常の操作を示し、負の`message`値はエラーを示します。

| `message` | `Server.` | 説明 |
| :---: | :---: | :--- |
| -1| `error`  | 切断されました。完全な応答が送信される前にリクエストが切断されました。切断されると、リクエストはサーバーによって閉じられます。 |
| 1 | `connection` | 新しい接続が受信されました。サーバーが新しいリクエストを受け入れました。 |
| 2 | `status` | リクエストのステータス行が受信されました。`val1` 引数にはリクエストパス（例：`index.html`）が含まれ、`val2` にはリクエストメソッド（例：`GET`）が含まれます。 |
| 3 | `header` | 1つのヘッダーが受信されました。1つのHTTPヘッダーが受信され、ヘッダー名は小文字で `val1` に（例：`connection`）、ヘッダー値は `val2` に（例：`close`）含まれます。 |
| 4 | `headersComplete` | すべてのヘッダーが受信されました。すべてのHTTPヘッダーが受信されました。`requestComplete` メッセージの引数として完全なリクエストボディを対応するタイプで受け取るには `String` または `ArrayBuffer` を返します。フラグメントが到着するたびに `requestFragment` を呼び出すには `true` を返します。リクエストボディを無視するには `false` または `undefined` を返します。他の戻り値の動作は未定義です。 |
| 5 | `requestFragment` |
| 6 | `requestComplete` |
| 8 | `prepareResponse` | 応答の準備をします。サーバーは応答を送信する準備ができています。コールバックは、`status` プロパティに応答ステータス（例：200）、`headers` プロパティにHTTPヘッダーの配列、`body` プロパティに応答ボディを持つ辞書を返します。ステータスプロパティが欠落している場合、デフォルト値の `200` が使用されます。ボディが `String` または `ArrayBuffer` の場合、それは完全な応答です。サーバーは `Content-Length` HTTPヘッダーを追加します。ボディプロパティが `true` に設定されている場合、応答は `Transfer-encoding` モード `chunked` を使用して配信され、コールバックは各応答フラグメントを取得するために呼び出されます。 |
| 9 | `responseFragment` | 応答フラグメントを取得します。サーバーは応答の別のフラグメントを送信する準備ができています。`val1` 引数には送信可能なバイト数が含まれます。コールバックは `String` または `ArrayBuffer` のいずれかを返します。リクエストのすべてのデータが返されたとき、コールバックは `undefined` を返します。 |
| 10| `responseComplete`  | リクエストが正常に完了しました。 |

新しいHTTP `Request`は、受信するリクエストごとにインスタンス化されます。コールバックは、そのリクエストのコールバックインスタンスに設定された`this`と共に呼び出されます。コールバック関数は、グローバル変数を使用するのではなく、特定のリクエストの処理に関連するプロパティを`this`にアタッチすることができ、複数のアクティブなリクエストがある場合に状態の衝突がないようにします。

***

### 例: シンプルなHTTPサーバー

次の例は、要求されたパスをエコーすることで全てのリクエストに応答するHTTPサーバーを実装しています。

```js
(new Server({})).callback = function(message, value) {
	switch (message) {
		case 2:		// HTTP status line received
			this.path = value;
			break;

		case 8:		// prepare response body
			return {headers: ["Content-type", "text/plain"], body: this.path};
	}
}
```

サーバーインスタンスには、HTTPリクエストを満たすためのステップに対応するメッセージに応答する単一のコールバック関数があります。各リクエストごとに新しいリクエストインスタンスが作成されるため、コールバックは各リクエストに対して一意の`this`を受け取ります。この例では、新しいリクエストのHTTPステータスラインが受信されたとき（メッセージ2）、コールバックはリクエストのパスを保存します。サーバーがレスポンスの本文を送信する準備ができたとき（メッセージ8）、コールバックはHTTPヘッダーとレスポンス本文（この場合はパス）を返します。サーバーは`Content-Length`ヘッダーを追加します。

***

### 例: チャンクレスポンスを持つHTTPサーバー

次の例は、ランダムな長さのランダムな数列でリクエストに応答するHTTPサーバーを実装しています。

```js
(new Server({})).callback = function(message, value) {
	switch (message) {
		case 8:	// prepare response body
			return {headers: ["Content-type", "text/plain"], body: true};

		case 9:	// provide response body fragment
			let i = Math.round(Math.random() * 20);
			if (0 == i)
				return;
			return i + "\n";
	}
}
```

この例では、サーバーがレスポンスボディを送信する準備ができたとき（メッセージ8）、コールバックはHTTPヘッダーを返し、レスポンスボディがフラグメントで提供されることを示すために`true`を返します。この場合、サーバーは値`chunked`の`Transfer-encoding`ヘッダーを追加します。サーバーがレスポンスの次のチャンクを送信する準備ができたとき、コールバックが呼び出され（メッセージ9）、チャンクを返します。この例では、ランダムな数値を返します。ランダムな数値が0の場合、サーバーはリクエストが完了したことを示すために`undefined`を返します。

***

### 例: JSON PUTを受信するHTTPサーバー

次の例は、JSONリクエストを受信し、レスポンスボディでJSONをエコーバックするHTTPサーバーを実装しています。

```js
(new Server({})).callback = function(message, value) {
	switch (message) {
		case 4:		// request headers received, prepare for request body
			return String;

		case 6:		// request body received
			this.jsonRequest = JSON.parse(value);
			trace(`received JSON: ${value}\n`);
			break;

		case 8:		// prepare response body
			return {headers: ["Content-type", "application/json"],
						body: JSON.stringify(this.jsonRequest)};
	}
}
```

コールバックはリクエストヘッダーが受信されたときに呼び出され（メッセージ4）、リクエストボディをStringオブジェクトとして受信したいことを示すStringを返します。完全なリクエストボディが受信されたとき、コールバックが呼び出されます（メッセージ6）。コールバックはリクエストインスタンスの`jsonRequest`プロパティにJSONオブジェクトへの参照を保持します。レスポンスボディを送信するためにコールバックが呼び出されたとき（メッセージ8）、メッセージボディとして送信するためにJSONオブジェクトを文字列にシリアライズします。

***

### 例: ファイルにストリーミングするHTTPサーバーのPUTリクエスト

次の例は、PUTリクエストを受信し、HTTPリクエストパスをローカルファイルパスとして使用してリクエストボディをファイルにストリーミングするHTTPサーバーを実装しています。

```js
import {File} from "file";
import config from "mc/config";

(new Server({})).callback = function(message, value) {
	switch (message) {
		case Server.status:						// request status received
			const path = config.file.root + value.slice(1);
			File.delete(path);
			this.file = new File(path, true);
			break;

		case Server.headersComplete:			// prepare for request body
			return true;						// provide request body in fragments

		case Server.requestFragment:			// request body fragment
			this.file.write(this.read(ArrayBuffer));
			break;

		case Server.requestComplete				// request body received
			this.file.close();
			break;
	}
}
```

コードを試すには、必要に応じてファイルパスとIPアドレスを置き換えて、次のように`curl`ツールを使用します：

	curl --data-binary "@/users/hoddie/projects/test.txt"  http://192.168.1.37/test.txt

***

<a id="websocket-client"></a>
## WebSocket Clientクラス

- **ソースコード:** [websocket](../../modules/network/websocket)
- **関連するサンプル:** [websocketclient](../../examples/network/websocket/websocketclient)

WebSocket `Client`クラスは、WebSocketサーバーと通信するためのクライアントを実装しています。これは`Socket`クラスを基に構築されています。`Socket`クラスと同様に、WebSocket `Client`は辞書ベースのコンストラクタと単一のコールバックを使用します。

```js
import {Client} from "websocket"
```

WebSocketクライアントの実装は、小さなメッセージの送受信を目的としています。以下の制限があります：

- 各メッセージは単一フレームでなければなりません。断片化されたメッセージはサポートされていません。
- 送信時にメッセージはマスクされません。

### `constructor(dictionary)`

新しいWebSocket `Client`はプロパティの辞書を使用して構成されます。辞書は`Socket`辞書のスーパーセットです。

WebSocket `Client`が`Socket`辞書に追加するプロパティの完全なリストは次のとおりです：

| プロパティ | デフォルト値 | 説明 |
| :---: | :---: | :--- |
| `port` | 80 | リモートポート番号 |
| `path` | `/` | HTTP URLのパス、クエリ、およびフラグメント部分 |

ルートパス "/"のポート80でサーバーに接続するには：

```js
let ws = new Client({host: "echo.websocket.org"});
```

ポート8080でIPアドレスによるサーバーに接続するには：

```js
let ws = new Client({address: "174.129.224.73", port: 8080});
```

***

### `close()`

`close`関数はWebSocket接続を即座に終了し、ソケットおよびその他の関連メモリを解放します。

```js
ws.close();
```

***

### `write(message)`

write関数は単一のWebSocketsメッセージを送信します。メッセージは`String`であり、テキストメッセージとして送信されるか、`ArrayBuffer`であり、バイナリメッセージとして送信されます。

```js
ws.write("hello");
ws.write(JSON.stringify({text: "hello"}));
```

***

### `callback(message, value)`

WebSocketクライアントのユーザーは、コールバック関数を通じてステータス情報を受け取ります。コールバックはメッセージを受け取り、一部のメッセージにはデータ値も含まれます。正の`message`値は正常な動作を示し、負の`message`値はエラーを示します。

| `message` | `Client.` | 説明 |
| :---: | :---: | :--- |
| 1 | `connect` | ソケットが接続されました。クライアントがWebSocketサーバーに接続したときにこのコールバックが受信されます。 |
| 2 | `handshake` | WebSocketハンドシェイクが完了しました。クライアントがHTTP接続からWebSocket接続へのアップグレードをWebSocketサーバーと正常に完了した後にこのコールバックが受信されます。 |
| 3 | `receive` | メッセージが受信されました。サーバーから新しい完全なメッセージが到着したときにこのコールバックが受信されます。`value`引数にはメッセージが含まれます。バイナリメッセージは`ArrayBuffer`で、テキストメッセージは`String`で配信されます。 |
| 4 | `disconnect` | 接続が閉じられました。このコールバックは、サーバーの要求またはネットワークエラーによって接続が閉じられたときに受信されます。`value`にはエラーコードが含まれ、サーバーによって接続が閉じられた場合は0、ネットワークエラーの場合は非ゼロです。 |

***

<a id="websocket-server"></a>
## WebSocket Serverクラス

- **ソースコード:** [websocket](../../modules/network/websocket)
- **関連するサンプル:** [websocketserver](../../examples/network/websocket/websocketserver)

WebSocket `Server` クラスは、WebSocket クライアントと通信するためのサーバーを実装します。これは `Socket` クラスに基づいて構築されています。`Socket` クラスと同様に、WebSocket `Server` は辞書ベースのコンストラクタと単一のコールバックを使用します。

```js
import {Server} from "websocket"
```

WebSocket サーバーの実装は、小さなメッセージの送受信を目的としています。以下の制限があります:

- 各メッセージは単一のフレームでなければなりません。断片化されたメッセージはサポートされていません。

### `constructor(dictionary)`

新しい WebSocket `Server` は、プロパティの辞書を使用して構成されます。この辞書は `Listener` 辞書のスーパーセットです。サーバーはソケットリスナーです。辞書にポートが指定されていない場合、ポート 80 が使用されます。ポートが `null` に設定されている場合、リスナーは作成されません。これは、http サーバーとリスナーを共有する場合に便利です（下記の `attach` を参照）。

この時点では、WebSocket `Server` は辞書に対して追加のプロパティを定義していません。

```js
let ws = new Server({});
```

<!-- to do: WebSocket subprotocol -->

***

### `close()`

`close` 関数は、WebSocket サーバーリスナーを即座に終了し、ソケットおよびその他の関連メモリを解放します。アクティブな接続は開いたままになります。

```js
ws.close();
```

***

### `attach(socket)`

`attach` 関数は、提供されたソケットから新しい着信 WebSockets 接続を作成します。サーバーは `Server.connect` コールバックを発行し、その後 WebSockets ハンドシェイクを実行します。ステータスラインはソケットから読み取られていますが、HTTP ヘッダーはハンドシェイクを完了するために必要なため、読み取られていません。

HTTP と WebSockets サーバー間で単一のリスナーソケットを共有するサンプルについては、[httpserverwithwebsockets](../../examples/network/http/httpserverwithwebsockets/main.js) を参照してください。

***

### `callback(message, value)`

WebSocket サーバーのコールバックは、"Socket connected" (`1` または `Server.connect`) メッセージが追加された点を除いて、WebSocket クライアントのコールバックと同じです。サーバーのソケット接続メッセージは、サーバーが新しい着信接続を受け入れたときに呼び出されます。

`this` の値は、サーバーへの各接続ごとにユニークです。メッセージは、コールバックが WebSocket ハンドシェイク完了メッセージ (`Server.handshake`) を受け取った後でなければ送信できません。

コールバックの `this` インスタンスには、WebSocket クライアントと同じ `write` および `close` メソッドがあります。これらのメソッドは、データの送信および接続の終了に使用されます。

>**注意**: マスクビットが設定されたテキストおよびバイナリメッセージは、コールバックに配信される前にサーバーによってマスク解除されます。

***

<a id="net"></a>
## Netクラス

- **ソースコード:** [net](../../modules/network/net)
- **関連するサンプル:** [net](../../examples/network/net)

`Net` クラスは、アクティブなネットワーク接続に関するステータス情報へのアクセスを提供します。

```js
import Net from "net";
```

### `static get(property [, interface])`

`get` 関数は、アクティブなネットワーク接続のプロパティを返します。

利用可能なプロパティは次のとおりです:

| Property | Description |
| :---: | :--- |
| `IP` | ネットワーク接続の IP アドレスを `String` で返します。例: "10.0.1.4"。これらは IPv4 または IPv6 アドレスである可能性があります。
| `MAC` | デバイスの MAC アドレスを `String` で返します。例: "A4:D1:8C:DB:C0:20"
| `SSID` | 接続されている Wi-Fi アクセスポイントの名前を `String` で返します。例: "Moddable Wi-Fi"
| `BSSID` | 接続されている Wi-Fi アクセスポイントの MAC アドレスを `String` で返します。例: "18:64:72:47:d4:32"
| `RSSI` | Wi-Fi の[受信信号強度](https://en.wikipedia.org/wiki/Received_signal_strength_indication)を `Number` で返します。
| `CHANNEL` | 現在使用中の Wi-Fi チャンネルを `Number` で返します。
| `DNS` | `Net.resolve` によってドメイン名を IP アドレスに解決するために使用される DNS サーバーを、IP アドレス文字列の `Array` として返します。これらは IPv4 または IPv6 アドレスである可能性があります。

```js
trace(`Connected to Wi-Fi access point: ${Net.get("SSID")}\n`);
```

デバイスがWi-Fiステーション（クライアント）とWi-Fiアクセスポイントの両方として動作する場合、静的な `get` メソッドは、リクエストがステーションインターフェースかアクセスポイントインターフェースかを示すためのオプションの第2引数を受け入れます。インターフェースは `"station"` と `"ap"` の値を受け入れます。これは `IP` および `MAC` プロパティに使用されます。

ESP32では、オプションの第2引数を使用して、値 `"ethernet"` を提供することでイーサネットインターフェースに関する情報を明示的に要求することもできます。

```
trace(`IP default ${Net.get("IP")}\n`);
trace(`IP station ${Net.get("IP", "station")}\n`);
trace(`IP AP ${Net.get("IP", "ap")}\n`);
```
ステーションモードでは、インターフェースのデフォルト値は `"station"` です。アクセスポイントモードでは `"ap"` です。ステーションとアクセスポイントの結合モードでは、デフォルト値はありません（曖昧であるため）。このモードで `IP` または `MAC` プロパティを要求すると、`undefined` が返されます。

***

### `static resolve(host, callback)`

`resolve` 関数は、指定された `host` の非同期 [DNS](https://en.wikipedia.org/wiki/Domain_Name_System) ルックアップを実行し、結果を配信するために `callback` を呼び出します。

```js
Net.resolve("moddable.tech", (name, address) => trace(`${name} IP address is ${address}\n`));
```

IP アドレスはドット区切りの IP アドレス表記の `String` として提供されます。`host` を解決できない場合、`address` パラメータは `undefined` です。

lwIP の DNS 実装は、同時に実行できる DNS ルックアップの数が限られています。この数は特定のプラットフォームの展開に依存します。ESP8266 では 4 です。DNS 解決キューが満杯の場合、resolve は例外をスローします。

***

<a id="wifi"></a>
##  WiFiクラス

- **ソースコード:** [wifi](../../modules/network/wifi)
- **関連するサンプル:** [wifiaccesspoint](../../examples/network/wifi/wifiaccesspoint), [wifiscan](../../examples/network/wifi/wifiscan)

`WiFi` クラスは、ホストデバイスの Wi-Fi 機能を使用および構成するためのアクセスを提供します。

```js
import WiFi from "wifi";
```

### `constructor(dictionary, callback)`

`WiFi` コンストラクタは、初期化パラメータの辞書を引数として受け取ります。コンストラクタは接続の確立プロセスを開始します。

辞書には、接続するベースステーションの名前を持つ必須の `ssid` プロパティが常に含まれています。ベースステーションがパスワードを必要とする場合は、オプションの `password` プロパティが含まれます。オプションの `bssid` プロパティが含まれている場合、対応するデバイスターゲットでのWi-Fi接続が高速化されることがあります。

接続プロセスは非同期であり、コールバック関数を使用して監視することができます。

次の例は、Wi-Fiアクセスポイントへの接続プロセスを開始し、デバイスにIPアドレスが割り当てられるまで接続が成功するのを待ちます。

```js
let monitor = new WiFi({ssid: "My Wi-Fi", password: "secret"}, msg => {
	switch (msg) {
		case WiFi.connected:
			break; // still waiting for IP address
		case WiFi.gotIP:
			trace(`IP address ${Net.get("IP")}\n`);
			break;
		case WiFi.disconnected:
			break;  // connection lost
	}
});
```

次の例は、パスワードなしでWi-Fiアクセスポイントへの接続を開始します。接続進行状況を監視するコールバック関数がないため、接続が準備完了になる時期を判断するにはポーリングが必要です。`Net` クラスを使用してデバイスのIPアドレスを取得することでポーリングを行います。接続がない場合、結果は `undefined` です。

```js
let monitor = new WiFi({ssid: "Open Wi-Fi"});
```

### `close()`

`close` 関数は、`WiFi` インスタンスとデバイスのネットワーク接続を管理する基礎プロセスとの間の接続を閉じます。言い換えれば、コールバック関数への将来の呼び出しを防ぎますが、ネットワークからの切断は行いません。

```js
monitor.close();
```

### `static scan(dictionary, callback)`

`scan` 静的関数は、利用可能な Wi-Fi アクセスポイントのスキャンを開始します。

dictionary パラメータは、2 つのオプションプロパティをサポートします:

| プロパティ | 説明 |
| :---: | :--- |
| `hidden` | `true` の場合、隠しアクセスポイントがスキャン結果に含まれます。デフォルトは `false` です。 |
| `channel` | スキャンする Wi-Fi チャンネル番号。このプロパティが存在しない場合、すべてのチャンネルがスキャンされます。 |

コールバック関数は、見つかった各アクセスポイントごとに一度呼び出されます。スキャンが完了すると、コールバック関数は `null` 引数で最終的に一度呼び出されます。

```js
WiFi.scan({}, item => {
	if (item)
		trace(`name: ${item.ssid}, password: ${item.authentication != "none"}, rssi: ${item.rssi}, bssid: ${(new Uint8Array(item.bssid)).join(".")}\n`);
	else
		trace("scan complete.\n");
});
```

Wi-Fiスキャンは約2秒間の固定期間実行されます。その間に、すべてのアクセスポイントが見つかるわけではありません。見えるアクセスポイントの完全なリストを作成するために、スキャンを数回呼び出す必要があるかもしれません。

> **注意**: 一度にアクティブにできるスキャンは1つだけです。まだアクティブなスキャンがある状態で新しいスキャンを開始すると、例外がスローされます。

***

### `mode` プロパティ

`mode` プロパティは、ステーションモード（例：デバイスがWi-Fiクライアントとして動作する場合）には `WiFi.Mode.station`、アクセスポイントモード（例：デバイスがWi-Fiベースステーションとして動作する場合）には `WiFi.Mode.accessPoint`、ステーションモードとアクセスポイントモードを同時に動作させるには `WiFi.Mode.station | WiFi.Mode.accessPoint` に設定されます。

`mode` は、ステーションモードとアクセスポイントモードの両方を無効にするために `WiFi.Mode.none` に設定することができます。プラットフォームによっては、`WiFi.Mode.none` に設定されている場合でもWi-Fiが電源オンのままになることがあります。一部のプラットフォームでは、Wi-Fiをシャットダウンするために `WiFi.Mode.off` をサポートしています。実行時に `"off" in WiFi.Mode` を使用して、その機能がサポートされているかどうかを確認してください。

***

### `static connect(dictionary)`

`connect` 関数は接続を確立するプロセスを開始します。接続プロセスは非同期であり、`Net.get("IP")` をポーリングするか、新しい WiFi インスタンスを作成することで監視できます。辞書には接続する基地局を示す `ssid` または `bssid` プロパティと、オプションの `password` が含まれます。

```js
WiFi.connect({ssid: "Moddable", password: "1234"});
```

> **Note**: パラメータなしで `WiFi.connect` を呼び出すと切断されます。ただし、代わりに `WiFi.disconnect` を使用することをお勧めします。

***

### `static disconnect()`

現在の Wi-Fi 基地局から切断します。

```js
WiFi.disconnect();
```
***

### `static accessPoint(dictionary)`

`accessPoint` 関数はデバイスを Wi-Fi アクセスポイントとして構成します。デバイスによっては、ステーションモードを終了する場合があります。

辞書には、アクセスポイントの名前を示す文字列である `ssid` プロパティを含める必要があります。

辞書には、以下のプロパティをオプションで含めることができます:

| プロパティ | デフォルト値 | 説明 |
| :---: | :---: | :--- |
| `password` | なし | アクセスポイントのパスワードを示す文字列。パスワードが提供されない場合、アクセスポイントはオープンになります。 |
| `channel` | 1 | アクセスポイントに使用するチャネルを示す数値。 |
| `hidden` | `false` | チャネルを隠すかどうかを示すブール値。 |
| `interval` | 100 | ビーコン間隔をミリ秒単位で示す数値。 |
| `max` | 4 | 同時接続の最大数を示す数値。 |
| `station` | `false` | アクセスポイントモードと同時にステーションモードを有効にするかどうかを示すブール値。 |

```js
WiFi.accessPoint({
	ssid: "Moddable Zero",
	password: "12345678"
});
```

***

<a id="sntp"></a>
## SNTPクラス

- **ソースコード:** [sntp](../../modules/network/sntp)
- **関連するサンプル:** [sntp](../../examples/network/sntp), [ntpclient](../../examples/network/mdns/ntpclient)

`SNTP` クラスは、リアルタイムクロックの値を取得するための [SNTP](https://en.wikipedia.org/wiki/Network_Time_Protocol#SNTP) クライアント ([RFC 4330](https://tools.ietf.org/html/rfc4330)) を実装します。

```js
import SNTP from "sntp";
```

SNTP クライアントの実装には、障害発生時に追加のサーバーに問い合わせるフェイルオーバーメカニズムが含まれています。

### `constructor(dictionary, callback)`

SNTP コンストラクタは、プロパティの辞書とインスタンスのステータスに関する情報を受け取るコールバック関数を取ります。

辞書には、SNTP サーバーのホスト名または IP アドレスを示す文字列である `host` プロパティを含める必要があります。

コールバックはメッセージを受け取り、一部のメッセージにはデータ値が含まれます。正の `message` 値は通常の操作を示し、負の `message` 値はエラーを示します。

| `message` | 説明 |
| :---: | :--- |
| -1 | 時間を取得できません。`value` パラメータには失敗の理由が `String` として含まれます。コールバック関数は、試すべき別の SNTP サーバーのホスト名または IP アドレスを `String` として返すことがあります。そうでない場合、SNTP クライアントは自動的に閉じられ、追加のリクエストには使用できません。フェイルオーバー処理の実装については、[SNTP のサンプル](https://github.com/Moddable-OpenSource/moddable/blob/public/examples/network/sntp/main.js) を参照してください。 |
| 1 | 時間が取得されました。`value` パラメータは1970年からの秒数で、Date コンストラクタに渡すのに適しています。 |
| 2 | リトライ。時間はまだ取得されておらず、SNTP クライアントが追加のリクエストを行っています。

***

### 例: 時間の取得
次の例では、`pool.ntp.org` の NTP サーバーから現在の時間値を取得します。

```js
new SNTP({host: "pool.ntp.org"}, (message, value) => {
	if (1 === message)
		trace(`time value is ${value}\n`);
});
```

SNTP コンストラクタには、タイムサーバーのホスト名または IP アドレスが必要です。ホスト名が提供された場合、SNTP クライアントはまず `Net.resolve` を使用してそれを IP アドレスに解決します。

***

<a id="dns"></a>
## DNSクラス定数

- **ソースコード:** [dns](../../modules/network/dns)

DNSモジュールには、DNSプロトコルと直接やり取りするコードを実装する際に役立つ定数が含まれています。これは、DNS `Parser`、DNS `Serializer`、およびmDNS実装によって使用されます。

```js
import DNS from "dns";
```

- `DNS.RR` には、`DNS.RR.PTR` などのリソースレコードタイプの定数が含まれています。
- `DNS.OPCODE` には、`DNS.OPCODE.QUERY` および `DNS.OPCODE.UPDATE` の値が含まれています。
- `DNS.CLASS` には、`DNS.CLASS.IN`、`DNS.CLASS.NONE`、および `DNS.CLASS.ANY` の値が含まれています。
- `DNS.SECTION` には、`DNS.QUESTION` および `DNS.ANSWER` を含む値が含まれています。

<a id="dns-parser"></a>
##  DNS パーサークラス

- **ソースコード:** [dnsparser](../../modules/network/dns)

DNS `Parser` クラスは、バイナリDNSレコードからJavaScriptオブジェクトを抽出します。

```js
import Parser from "dns/parser";
```

DNSパーサークラスは、メモリ使用量を最小限に抑えるために、一度に1つのリソースレコードを解析して返します。A、AAAA、PTR、SRV、TXTリソースレコードタイプのリソースデータ用のパーサーを備えています。

> **注**: DNSパーサーは、mDNSなどの高レベルサービスを構築するために使用される低レベルクラスです。

### `constructor(buffer)`
DNS `Parser` コンストラクタは、単一のDNSパケットを含む `ArrayBuffer` で初期化されます。

コンストラクタによる検証は行われません。エラーがある場合は、リソースレコードを抽出する際に報告されます。

***

### `questions(index)`
インデックス引数に対応する質問リソースレコードを返します。インデックスは0から番号が付けられます。インデックスがパケット内の質問レコードの数を超える場合は `null` を返します。

***

### `answers(index)`
インデックス引数に対応する回答リソースレコードを返します。インデックスは0から番号が付けられます。インデックスがパケット内の回答レコードの数を超える場合は `null` を返します。

***

### `authorities(index)`
インデックス引数に対応する権威リソースレコードを返します。インデックスは0から番号が付けられます。インデックスがパケット内の権威レコードの数を超える場合は `null` を返します。

***

### `additionals(index)`
インデックス引数に対応する追加リソースレコードを返します。インデックスは0から番号が付けられます。インデックスがパケット内の追加レコードの数を超える場合は `null` を返します。

***

### 例: DNSパケットの解析
DNSパケットは通常UDPパケットとして受信されます。`Socket`オブジェクトは各DNSパケットを`ArrayBuffer`で提供します。以下の例では、`ArrayBuffer`のためのDNSパーサーインスタンスを作成します:

```js
let packet = new Parser(dnsPacket);
```

`Parser`コンストラクタはパケットを検証しません。パケットが無効な場合、レコードを抽出する際にエラーが報告されます。

***

### 例: ヘッダーフィールドの読み取り
パーサーインスタンスには、DNSパケットの`id`および`flags`フィールドのプロパティがあります:

```js
let id = packet.id;
let flags = packet.flags;
```

***

### 例: レコード数の決定
パーサーインスタンスには、各セクションのリソースレコードの数を提供するプロパティがあります。

```js
let total = packet.questions + packet.answers + packet.authorities + packet.additionals;
```

***

### 例: リソースレコードの抽出
リソースレコードのセクションに対応する関数を呼び出すことで、単一のリソースレコードを含むJavaScriptオブジェクトが取得されます。以下の例では、2番目の質問リソースレコードを取得します（インデックスは0から始まります）:

```js
let rr = packet.question(1);
```

また、`answers`、`authorities`、`additionals` 関数もあります。

***

<a id="dns-serializer"></a>
## DNSシリアライザクラス

- **ソースコード:** [dnsserializer](../../modules/network/dns)

DNS `Serializer` クラスは、DNS レコードのシリアライザを実装します。

```js
import Serializer from "dns/serializer";
```

DNS `Serializer` クラスは、A、NSEC、PTR、SRV、および TXT リソースレコードタイプをシリアライズすることができます。クライアントは他のリソースレコードタイプのシリアライズを独自に行い、その結果を DNS `Serializer` クラスに提供して生成された DNS パケットに含めることができます。

> **注**: DNS `Serializer` は、mDNS などの高レベルサービスを構築するために使用される低レベルクラスです。

### `constructor(dictionary)`
DNS `Serializer` コンストラクタは、作成される DNS パケットを構成するプロパティを持つ辞書を受け取ります。辞書には次のプロパティを含めることができます:

| プロパティ | デフォルト値 | 説明 |
| :---: | :---: | :--- |
| `opcode` | `DNS.OPCODE.QUERY` | `opcode` ヘッダーフィールドの数値
| `query` | `true` | このパケットがクエリまたは応答を含むかどうかを示すブール値
| `authoritative` | `false` | ヘッダーの `authoritative` ビットの値を示すブール値
| `id` | 0 | ID フィールドの数値

***

### `add(section, name, type, clss, ttl, ...)`
`add` 関数は、リソースレコードを追加してDNSパケットにシリアライズします。`add` の最初の5つの引数は、すべてのリソースレコードに共通です。

| 引数 | 説明 |
| :---: | :--- |
| `section` | このリソースレコードを追加するセクション、例: `DNS.SECTION.ANSWER`。
| `name` | DNS QNAME を含む `String`
| `type` | リソースレコードタイプを表す `Number`、例: `DNS.RR.A`
| `clss` | リソースレコードクラスフィールド値を含む `Number`、通常は `DNS.CLASS.IN`
| `ttl` | このリソースレコードの有効期間を秒単位で含む `Number`

オプションの `data` 引数は、リソースレコードのリソースデータ部分を構築するために使用されます。存在しない場合、リソースデータは空です。`ArrayBuffer` の場合、その内容がリソースデータとして使用されます。`data` 引数は次のリソースレコードタイプに解釈されます:

| タイプ | 説明 |
| :---: | :--- |
| `A` | IPアドレスを含む文字列。
| `NSEC` | 2つのキーを持つ辞書。最初のキーは次のホスト名値を含む文字列 `next`。2つ目はビットマスクを含む Uint8Array。
| `PTR` | PTR値を含む文字列。
| `SRV` | 4つのキーを持つ辞書。`priority`、`weight`、`port` フィールドは対応するフィールドの値を持つ数値。`target` プロパティはターゲットの名前を含む文字列。
| `TXT` | TXTレコードのキー/バリューペアの辞書。プロパティ名がキー。この時点では文字列値のみがサポートされています。

***

### `build()`
`build` 関数は、シリアライザーインスタンスへの以前の呼び出しに基づいてDNSパケットを生成します。パケットは `ArrayBuffer` として返されます。

> **注意**: 現在の実装では QNAMES を圧縮しないため、必要以上に大きなDNSパケットになります。

***

### 例: DNSクエリの構築

次の例では、DNSシリアライザーを使用して "example.com" ドメインのAレコードをクエリするDNSパケットを作成します:

```js
let serializer = new Serializer({query: true, opcode: DNS.OPCODE.QUERY});
serializer.add(DNS.SECTION.QUESTION, "example.com", DNS.RR.A, DNS.CLASS.IN);
let buffer = serializer.build();
```

`build` 関数は、`Socket` クラスの `write` 関数を使用して送信するのに適したDNSパケットを返します。

***

<a id="dns-server"></a>
## DNSサーバークラス

- **ソースコード:** [dnsserver](../../modules/network/dns)
- **関連するサンプル:** [dnsserver](../../examples/network/dns/dnsserver)

`DNSServer` クラスは、シンプルなDNSサーバーを実装します。

```js
import DNSServer from "dns/server";
```

サーバーは、キャプティブポータルとして機能したいWi-Fiアクセスポイントモードのデバイスでの使用を想定しています。DNSサーバーは、特定のドメインのルックアップをデバイス（通常はDNSサーバーを実行しているデバイス）のIPアドレスに向けるために使用されます。

### `constructor(callback)`

`DNSServer`コンストラクタは、ルックアップリクエストが受信されたときに呼び出される関数を引数として取ります。コールバックは2つの引数を受け取ります。最初の引数`message`は、ルックアップが実行されたときに1に設定されます。2番目の引数`value`は、ルックアップリクエストが行われたときに解決される名前に設定されます。

```js
let server = new DNSServer((message, value) => {
	...
});
```

***

### `close()`

DNSサーバーが不要になった場合、`close`を呼び出して終了し、リソースを解放します。

```js
server.close();
```

***

### 例: シンプルなDNSサーバー

次の例では、すべてのDNSルックアップをサーバーを実行しているデバイスのIPアドレスにリダイレクトします。

```js
new DNSServer((message, value) => {
	if (1 === message)
		return Net.get("IP", "ap");
})
```

> **Note:**: この例は、アクセスポイントモードのWi-Fi接続で実行されることを想定しています。`Net.get`のインターフェース引数に"ap"を渡して、アクセスポイントのIPアドレスを取得します。

### 例: 単一ホスト名のためのDNSサーバー

次の例では、"example.com" のすべてのDNSルックアップをサーバーを実行しているデバイスのIPアドレスにリダイレクトします。他のすべてのルックアップは無視されます。

```js
new DNSServer((message, value) => {
	if ((1 == message) && ("example.com" == value))
		return Net.get("IP", "ap");
})
```

***

<a id="mdns"></a>
## MDNSクラス

- **ソースコード:** [mdns](../../modules/network/mdns)
- **関連するサンプル:** [discoverhttp](../../examples/network/mdns/discoverhttp), [httpserver](../../examples/network/mdns/httpserver), [ntpclient](../../examples/network/mdns/ntpclient), [ntpservice](../../examples/network/mdns/ntpservice),

`MDNS` クラスは [Multicast DNS](https://tools.ietf.org/html/rfc6762) のディスカバリーとサービスを扱うためのサービスを実装します。これには `.local` 名のクレーム（取得）、mDNS サービス利用可能のアドバータイズ（通知）、および利用可能な mDNS サービスのスキャンが含まれます。

```js
import MDNS from "mdns";
```

### `constructor(dictionary [, callback])`

`MDNS` コンストラクタは、mDNS インスタンスを構成するための辞書と、インスタンスのステータスに関する情報を受け取るためのオプションの `callback` を取ります。

辞書に `hostName` プロパティが含まれている場合、MDNS インスタンスはアクティブなネットワーク接続上の `.local` ドメインで名前をクレームしようとします。利用可能な mDNS サービスをスキャンするために `hostName` は必須ではありません。

コールバックはメッセージを受け取り、一部のメッセージにはデータ値も含まれます。`message` と `value` は、名前取得プロセスに関する情報を提供します。正の `message` 値は通常の動作を示し、負の `message` 値はエラーを示します。

| `message` | 説明 |
| :---: | :--- |
| 1 | **probing:** `value` が空の文字列の場合、名前のクレームが進行中です。プロービングが成功すると、`value` にクレームした名前が含まれます。 |
| 2 | **conflict:** 要求された名前をクレームしようとした際に、既にその名前を使用している別のデバイスが発見されました。コールバック関数の結果に基づいて次の処理が決まります。<BR>- 結果が未定義の場合、新しい名前が自動的に作成され、名前のクレームプロセスが続行されます。<BR>- 文字列が返される場合、その文字列が候補ホスト名として使用され、名前のクレームプロセスが続行されます。true を返すと、名前をクレームせずに名前のクレームプロセスが終了します。
| 任意の負の数 | **error:** 名前のクレームプロセスが終了しました。

以下の例は、ローカルネットワーク上で「mydevice」という名前をクレームする方法を示しています。

```js
const mdns = new MDNS({hostName: "mydevice"});
```

クレームプロセスには少し時間がかかりますが、通常は1秒未満です。名前が既に使用されているため、クレームが成功しない場合があります。オプションのコールバック関数はクレームのステータスを提供します：

```js
const mdns = new MDNS({hostName: "mydevice"}, function(message, value) {
	switch (message) {
		case 1:
			trace(`MDNS - claimed hostname is "${value}"\n`);
			break;
		case 2:
			trace(`MDNS - failed to claim "${value}", try next\n`);
			break;
		default:
			if (message < 0)
				trace("MDNS - failed to claim, give up\n");
			break;
	}
});
```

***

### `monitor(serviceType, callback)`

`monitor`関数は、`serviceType`パラメータで指定されたタイプのmDNSサービスをネットワーク上で継続的にスキャンします。

コールバック関数は、見つかった各ユニークなサービスインスタンスおよびサービスがTXTリソースレコードの変更を通知するたびに呼び出されます。コールバックの最初の引数はサービスのタイプで、例えば「\_http.\_tcp」です。2番目の引数は、サービスを説明する`name`、`protocol`、`port`、および`txt`プロパティを含む辞書です。

次の例は、ローカルネットワーク上で利用可能な`_http._tcp`サービスを継続的にスキャンします：

```js
mdns.monitor("_http._tcp", (service, instance) => {
	trace(`Found ${service}: ${instance.name}\n`);
});
```

***

### `add(service)`

`add` 関数は、アドバータイズするための mDNS サービス記述を登録します。サービスレコードには次のプロパティが含まれます:

| プロパティ | 説明 |
| :---: | :--- |
| `name` | サービスの名前、例: "http" |
| `protocol` | サービスのプロトコル、例: "tcp" または "udp" |
| `port` | サービスのポート |
| `txt` | サービスの TXT リソースレコードを埋めるための名前と値のペアを持つオプションの JavaScript オブジェクト |

`add` は、ホスト名のクレームプロセスが正常に完了した後にのみ呼び出すことができます。

次の例は、現在のホストのポート 80 で `_http._tcp` サービスの利用可能性を通知します。

```js
mdns.add({
	name: "http",
	protocol: "tcp",
	port: 80,
	txt: {
		url: `/index.html`,
	}
});
```

***

### `update(service)`

`update` 関数は、TXT レコードの内容が変更されたことを MDNS 実装に通知します。これにより、新しい TXT レコードがローカルネットワークに通知されます。渡される `service` オブジェクトは、`add` に提供されたものと同じオブジェクトでなければなりません。

```js
let service = mdns.services[0];
service.txt["value"] = 123;
mdns.update(service);
```

***

### `remove(service)` または `remove(serviceType)`

`remove` 関数は、サービスの登録解除とサービスタイプのスキャンのキャンセルの両方に使用されます。

サービスの登録を解除するには、サービスの説明を渡します。これにより、ネットワークに対してそのサービスが利用できなくなったことが通知されます。`service` オブジェクトは、`add` に提供されたものと同じオブジェクトでなければなりません。

```js
mdns.remove(mdns.services[0]);
```

サービスタイプの監視をキャンセルするには、サービスタイプの名前を渡します。

```js
mdns.remove("_http._tcp");
```

***

<a id="telnet"></a>
##  Telnetクラス

- **ソースコード:** [telnet](../../modules/network/telnet)
- **関連するサンプル:** [telnet](../../examples/network/telnet)

`Telnet` クラスは、シンプルな telnet サーバーを実装します。telnet サーバーでサポートされるコマンドは、Console `module` に登録された `CLI` クラスによって決定されます。

### `constructor(dictionary)`

telnet サーバーを開始するには、`Telnet` コンストラクターを呼び出します。

`Telnet` コンストラクターは、1 つの引数である辞書を取ります。辞書には、`port` という単一のプロパティがあり、新しい接続を待ち受けるポートを示します。辞書に `port` が含まれていない場合、デフォルトで 23 に設定されます。

```js
let telnet = new Telnet({port: 2300});
```

***

### `close()`

Telnetサーバーが不要になった場合、`close`を呼び出して終了し、リソースを解放します。

```js
telnet.close();
```

***

<a id="ping"></a>
## Pingクラス

- **ソースコード:** [ping](../../modules/network/ping)
- **関連するサンプル:** [ping](../../examples/network/ping)

`Ping`クラスはpingネットワークユーティリティを実装します。

```js
import Ping from "ping";
```

### `constructor(dictionary, callback)`

`Ping`コンストラクタは2つの引数、辞書とコールバック関数を取ります。

辞書には以下のプロパティが含まれている必要があります:

| プロパティ | 説明 |
| :---: | :--- |
| `host` | pingするホスト
| `id` | pingプロセスの識別子; 各`Ping`インスタンスごとに一意である必要があります

辞書にはオプションで`interval`パラメータを含めることができ、これはpingの間隔をミリ秒単位で設定します。指定されていない場合、デフォルトは5000ミリ秒、つまり5秒です。

ユーザーはコールバック関数を通じてステータス情報を受け取ります。コールバックはメッセージを受け取り、一部のメッセージにはデータ値と追加情報が`etc`パラメータに含まれます。正の`message`値は通常の動作を示し、負の`message`値はエラーを示します。

| `message` | 説明 |
| :---: | :--- |
| -1 | **エラー:** エラーが発生し、ホストがこれ以上pingされません。 |
| 1 | **成功:** ホストがエコーリクエストにエコーリプライで応答しました。 |
| 2 | **タイムアウト:** ホストが応答しませんでした。 |

以下の例では、`example.com`サーバーに1000msごとにpingを送り、結果をコンソールに表示します。

```js
let ping = new Ping({host: "example.com", id: 1, interval: 1000}, (message, value, etc) => {
	if (1 == message)
		trace(`${value} bytes from ${etc.address}: icmp_seq=${etc.icmp_seq}\n`);
}
```

***

### `close()`

ホストへのpingを停止するには、`close`関数を呼び出します。

```js
ping.close();
```

***

<a id="mqtt"></a>
##  MQTTクラス

- **ソースコード:** [mqtt](../../modules/network/mqtt)
- **関連するサンプル:** [mqttbasic](../../examples/network/mqtt/mqttbasic), [mqttsecure](../../examples/network/mqtt/mqttsecure)

MQTT `Client`クラスは、MQTTブローカー（サーバー）に接続するクライアントを実装します。

```js
import Client from "mqtt";
```

### `constructor(dictionary)`

新しいMQTT `Client`は、プロパティの辞書を使用して構成されます。辞書には`host`と`id`プロパティが含まれている必要があります。他のプロパティはオプションです。

| パラメータ | 説明 |
| :---: | :--- |
| `host` | リモートMQTTサーバーのホスト名 |
| `port` | リモートポート番号。TLSを使用する接続には必須で、直接MQTT接続の場合はデフォルトで1883、WebSocket経由のMQTT接続の場合はデフォルトで80です。 |
| `id` | このデバイスの一意のID |
| `user` | ユーザー名 |
| `password` | `ArrayBuffer`としてのパスワード |
| `will` | 接続のWillとして設定される`topic`と`message`プロパティを持つオブジェクト。`message`は文字列または`ArrayBuffer`である可能性があります。 |
| `path` | 接続するエンドポイント。存在する場合、MQTTクライアントは`mqtt`サブプロトコルを使用してWebSocket接続を確立します。 |
| `timeout` | ミリ秒単位のキープアライブタイムアウト間隔。タイムアウトが提供されない場合、MQTTキープアライブ機能は使用されません。 |
| `Socket` | MQTT接続を作成するために使用するソケットコンストラクタ。TLSを使用して安全な接続を確立するには`SecureSocket`を使用します。 |
| `secure` | `SecureSocket`を使用する場合のTLS接続のオプションの辞書 |

```js
let mqtt = new Client({
	host: "test.mosquitto.org",
	id: "iot_" + Net.get("MAC"),
	user: "user name",
	password: ArrayBuffer.fromString("secret")
});
```

***

### `onReady()`

`onReady` コールバックは、サーバーへの接続が正常に確立されたときに呼び出されます。`onReady` が呼び出される前にメッセージを公開したり、サブスクリプションを作成したりすることはできません。

```js
mqtt.onReady = function () {
	trace("connection established\n");
}
```

***

### `subscribe(topic)`

トピックにサブスクライブするには、`subscribe` メソッドを使用します。クライアントは `subscribe` を複数回呼び出すことで、複数のクライアントにサブスクライブできます。

```js
mqtt.subscribe("test/string");
mqtt.subscribe("test/binary");
mqtt.subscribe("test/json");
```

### `unsubscribe(topic)`

トピックのサブスクリプションを解除するには、`unsubscribe` メソッドを使用します。

```js
mqtt.unsubscribe("test/string");
```

### `onMessage(topic, data)`

`onMessage` コールバックは、クライアントがサブスクライブしている任意のトピックにメッセージが受信されたときに呼び出されます。`topic` 引数はトピックの名前であり、`data` 引数は完全なメッセージです。

```js
mqtt.onMessage = function(topic, data) {
	trace(`received message on topic "${topic}"\n`);
}
```


`data` 引数は `ArrayBuffer` です。UTF-8 テキストのみを含むメッセージの場合、`String.fromArrayBuffer` を使用して文字列に変換できます。

```js
mqtt.onMessage = function(topic, data) {
	trace(`received message on topic "${topic}"\n`);
	trace(`data: ${String.fromArrayBuffer(data)}\n`);
}
```

***

### `publish(topic, message)`

トピックにメッセージを送信するには、`publish` メソッドを使用します。`message` 引数は文字列または `ArrayBuffer` のいずれかです。

```js
mqtt.publish("test/string", "hello");
mqtt.publish("test/binary", Uint8Array.of(1, 2, 3).buffer);
```

JSON を公開するには、まずそれを文字列に変換します。

```js
mqtt.publish("test/json", JSON.stringify({
	message: "hello",
	version: 1
}));
```

***

### `onClose()`

`onClose` コールバックは、ネットワークエラーや MQTT ブローカーが接続を閉じたために接続が失われたときに呼び出されます。

```js
mqtt.onClose = function() {
	trace("connection lost\n");
}
```

***
