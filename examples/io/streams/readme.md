# Streams
改訂日： 2024年1月24日

このディレクトリには、[web streams](https://streams.spec.whatwg.org) の実験的な実装が含まれています。これは [リファレンス実装](https://github.com/whatwg/streams) に基づいています。

この作業の目的は、リソースが制約された組み込みデバイスでのweb streamsの実現可能性と有用性を評価することです。

リファレンス実装から始めることで、動作がweb streams仕様に準拠していることを確認できます。準拠性を確認するために、この実装はweb streamsのWeb Platform Testsを使用してテストされています（[詳細はこちら](#tests)）。

リファレンス実装には、メモリ使用量とパフォーマンスの両方を最適化する多くの機会があります。これは将来の作業領域です。

<a id="modules"></a>
## `modules` ディレクトリ
`modules` ディレクトリには、web streamsのJavaScript実装が含まれています。

### streams.js

これは [リファレンス実装](https://github.com/whatwg/streams) から派生したweb streamsの実装です。以下はオリジナルのリファレンス実装との違いです：

- ソースコードが複数のファイルに分散されず、単一のファイルにまとめられている
- webプラットフォームへの依存を排除するように変更
- XSでの実行時の効率を向上させるための変更
- 標準のJavaScriptモジュール (ESM) としてパッケージ化

streamsモジュールは以下をエクスポートします：

- AbortController
- AbortSignal
- ByteLengthQueuingStrategy
- CountQueuingStrategy
- DOMException
- ReadableByteStreamController
- ReadableStream
- ReadableStreamBYOBReader
- ReadableStreamBYOBRequest
- ReadableStreamDefaultController
- ReadableStreamDefaultReader
- TransformStream
- TransformStreamDefaultController
- WritableStream
- WritableStreamDefaultController
- WritableStreamDefaultWriter
- TextDecoderStream
- TextEncoderStream

Webでは、これらのエクスポートされたクラスはグローバル変数です。Moddableアプリケーションでwebの例を実行するには、グローバル変数を提供してください：

```js
import * as streams from "streams";
for (let key in streams)
	globalThis[key] = streams[key];
```

Webプラットフォームテストとの互換性のために、現在 `TextDecoderStream` と `TextEncoderStream` クラスはグローバルとして `TextDecoder` と `TextEncoder` クラスにアクセスします。

Moddableアプリケーションで `TextDecoderStream` と `TextEncoderStream` クラスを使用したい場合は、グローバルとして `TextDecoder` と `TextEncoder` クラスを提供してください：

```js
import TextDecoder from "text/decoder";
globalThis.TextDecoder = TextDecoder;
import TextEncoder from "text/encoder";
globalThis.TextEncoder = TextEncoder;
```

### iostreams.js

IO Streamsモジュールは、ストリームを使用したECMA-419 IOをサポートします。このモジュールは、[ECMA-419 IOクラスパターン](https://419.ecma-international.org/#-9-io-class-pattern)に準拠したクラスに基づいて `ReadableStream` と `WritableStream` のサブクラスを作成する2つのミックスインをエクスポートします。`button` の例は、これらのミックスインの使用方法を示しています。

### sensorstreams.js

IO Streamsモジュールは、ストリームを使用したECMA-419センサーをサポートします。このモジュールは、[ECMA-419センサークラスパターン](https://419.ecma-international.org/#-13-sensor-class-pattern)に準拠したクラスに基づいて `ReadableStream` のサブクラスを作成するミックスインをエクスポートします。`touch` の例は、このミックスインの使用方法を示しています。

<a id="examples"></a>
## `examples` ディレクトリ
このディレクトリには、ストリームを使用するいくつかのModdable SDKの例のアプリケーションが含まれています。これらの例は、ESP32マイクロコントローラを中心に構築された開発ボードである [Moddable Two](https://www.moddable.com/moddable-two) で正常に実行されました。他のESP32ベースのデバイスでも動作するはずですが、設定の変更が必要な場合があります。コードサイズとRAMの要件のため、これらの例は能力の低いマイクロコントローラには収まらない場合があります。

### button

この例は、Moddableの [IOボタンの例](https://github.com/Moddable-OpenSource/moddable/tree/public/examples/io/digital/button) を再検討します。ボタンの状態に基づいてLEDをオンおよびオフにします。

`iostreams.js` によってエクスポートされたミックスインを使用すると、`Digital` クラスはボタン用の `ReadableStream` サブクラスとLED用の `WritableStream` サブクラスの両方になります。

値を反転するための `TransformStream` もあります。

ビルドするには：

```
cd /path/to/streams/examples/button
mcconfig -d -m -p esp32/moddable_two_io
```

LEDの状態が変わるときにコンソール出力はありません。状態の変化を見るためには、LED自体を監視する必要があります。

### touch {#examples}

`sensorstream.js`によってエクスポートされるミックスインのおかげで、`embedded:sensor/Touch/FT6x06`の`Touch`クラスは`ReadableStream`のサブクラスになります。

`ReadableStream`は非同期イテレータを提供するため、ポイントのストリームは`for await`ループで読み取られます。

ビルドするには：

```
cd /path/to/streams/examples/touch
mcconfig -d -m -p esp32/moddable_two_io
```

### fetch {#examples}

この例では、Moddable SDKの[ECMA-419 HTTPクライアントクラスパターン](https://419.ecma-international.org/#-20-http-client-class-pattern)と`ReadableStream`クラスを使用して、HTTPリクエストを行うためのWeb標準[`fetch()`関数](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API)を実装しています。

ビルドするには：

```
cd /path/to/streams/examples/fetch
mcconfig -d -m -p esp32/moddable_two_io ssid=<SSID> password=<PASSWORD>
```

fetchの例はシミュレータでも実行できます：

```
mcconfig -d -m
```

<a id="tests"></a>
## `wpt` テストディレクトリ {#tests}

このディレクトリには、[Web Platform Tests Project](https://github.com/web-platform-tests/wpt)から抽出されたWebストリームの単体テストが含まれています。

テストを実行するには、XSテストツールである[xst](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/xs/xst.md)をビルドする必要があります。その後：

```
cd /path/to/streams/wpt
./iterate.sh
```

テストの大部分は合格します - 1203のうち1168（97.1%）。35の失敗のうち、25は`TextDecoder`が組み込みでサポートしていないテキストフォーマットを使用しているためであり、ストリームの実装自体が原因ではありません。それを除けば、99.2%のテストが合格します。
