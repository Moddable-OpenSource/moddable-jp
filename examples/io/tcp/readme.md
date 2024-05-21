# Moddable SDK - TCP IO の例

著作権2022 Moddable Tech, Inc.<BR>
改訂： 2022年12月21日

このディレクトリの例は、Ecma-419によって定義された[TCPソケット](https://419.ecma-international.org/#-10-io-classes-tcp-socket)を使用しています。

- HTTP
	- [simplehttpget](./simplehttpget) – TCPソケットを直接使用してHTTPリクエストを行う基本的な例です。これはTCPソケットIOの使用例として意図されており、HTTPリクエストの良い解決策としては意図されていません。
	- [httpclient](./httpclient) – TCPソケットに基づいたHTTPクライアントAPIです。`HTTPRequest`クラスはEcma-419の第2版の[ドラフト仕様](https://github.com/EcmaTC53/spec/blob/master/docs/proposals/Network%20Classes.md#http-request-class)のほとんどを実装しています。
	- [fetch](./fetch) – 提案されたEcma-419の第2版`HTTPRequest` APIに基づいた標準HTML5の`fetch` [API](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API)の実装です。ストリームを除く`fetch`のほとんどを実装しています（調査中）。2022年12月にHTTPSリクエストも行うように更新されました。
- HTTPS
	- [httpsclient](./httpsclient) – `TLS` TCPソケットと`httpclient`を組み合わせて、google.comへの複数の安全なリクエストを行います。
- WebSocket
	- [websocketclient](./websocketclient) – TCPソケットに基づいたWebSocketクライアントAPIです。`WebSocketClient`クラスはEcma-419の第2版の[ドラフト仕様](https://github.com/EcmaTC53/spec/blob/master/docs/proposals/Network%20Classes.md#websocket-client-class)のほとんどを実装しています。
	- [websocket](./websocket) – 提案されたEcma-419の第2版`WebSocketClient` APIに基づいた標準HTMLの`WebSocket` [API](https://developer.mozilla.org/en-US/docs/Web/API/WebSocket)の実装です。
- MQTT
	- [mqttclient](./mqttclient) – TCPソケットに基づいたMQTTクライアントAPIです。`MQTTClient`クラスはEcma-419の第2版の[ドラフト仕様](https://github.com/EcmaTC53/spec/blob/master/docs/proposals/Network%20Classes.md#network-mqtt-client)のほとんどを実装しています。
	- [MQTT.js](./mqtt) – 提案されたEcma-419の第2版`MQTTClient` APIに基づいた、Node.jsおよびブラウザで広く使用されている[MQTT.jsパッケージ](https://www.npmjs.com/package/mqtt)の実装です。
- サーバー送信イベント
	- [EventSource](./eventsource) – [サーバー送信イベント](https://html.spec.whatwg.org/multipage/server-sent-events.html#parsing-an-event-stream)を受信するための標準HTMLの`EventSource` [API](https://developer.mozilla.org/en-US/docs/Web/API/EventSource)の実装です。HTTPクライアントAPIに基づいて構築されています。

`HTTPRequest`、`WebSocketClient`、および `MQTTClient` クラスは、基本的なプロトコルの機能をすべてサポートしながら、ランタイムリソースを効率的に使用するように設計されています。これらは最も制御が効き、フットプリントが軽いですが、低レベルのAPIとしては使用が便利ではありません。

`fetch`、`WebSocket`、および `mqtt` クラスは、組み込み開発者にウェブプラットフォームから馴染みのある、標準的なAPIへのアクセスを提供します。これらは一般的に使いやすいですが、より多くのリソース（RAM、コードサイズ、CPU）を使用します。

プロジェクトに使用するAPIを選択することは、そのプロジェクトの要件に依存します。多くの状況では、標準の `fetch`、`WebSocket`、および `mqtt` APIがうまく機能します。
