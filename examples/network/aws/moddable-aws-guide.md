# Moddable SDKを使用してAWS IoTに接続する
​Copyright 2023 Moddable Tech, Inc.<BR>
改訂日： 2023年3月28日

このガイドでは、Moddable SDKを使用してHTTPおよびMQTTプロトコルを使用してAWS IoTに安全に接続する方法を学びます。
​
このガイドで説明されている技術を使う2つのサンプルがあります。[http-aws](./http-aws)と[mqqt-aws](./mqtt-aws)です。
​
## AWS Thingのセットアップ
​
まず、AWS IoTをAWSの「Thing」でセットアップし、AWS IoTに接続するための正しい権限を持つキーをダウンロードできるようにします。サンプルでは、接続のセキュリティのためにTLSを使用します。詳細は[Moddable SDKのセキュアソケット](../../../documentation/network/securesocket.md)を参照してください。
​
必要なキーは次のとおりです：
​
* デバイス証明書
* 秘密鍵
* Amazon Trust Servicesエンドポイントキー
  * 以降「CAキー」と呼びます
  * RSAとECCの両方が使用可能です
​

これらのキーの作成とダウンロード方法については、[Amazonのドキュメント](https://docs.aws.amazon.com/iot/latest/developerguide/create-iot-resources.html)を参照してください。
​
## キーの変換
​
キーを取得したら、それらをPEM（Base64エンコードされたASCII）形式からDER（二進）形式に変換する必要があります。次のコマンドを使用して、Moddable SDKで使用される形式にキーを変換します。変換されたキーのファイル名は、サンプルで使用されている名前に合わせて`device.der`、`CA1.key`、`private-key.der`とします。
​
1. デバイス証明書を変換します：
​
```
openssl x509 -inform pem -in <path/to/device-cert.key> -out device.der -outform der
```
​
2. CAキーを変換します：
​
```
openssl x509 -inform pem -in <path/to/ca-key.key> -out CA1-key.der -outform der
```
​
3. 秘密鍵を変換します：
​
```
openssl pkcs8 -topk8 -in </path/to/private_key.key> -inform pem -out private-key.der -outform der -nocrypt
```
​
4. 変換された新しいキーを、実行しているサンプルのディレクトリ`$MODDABLE/examples/network/aws/mqtt-aws`に配置します。
​
> **注**: サンプルと異なる証明書名を使用する場合は、`main.js`および`manifest.json`のソースコードも更新することを忘れないでください。
​
キーの変換に関する詳細は、[暗号化ドキュメント](../../..//documentation/crypt/crypt.md#class-transform)を参照してください。
​
## AWSでMQTTテストクライアントを準備する
​
[AWS MQTTテストクライアント](https://us-east-1.console.aws.amazon.com/iot/home?region=us-east-1#/test)は、デバイスからのメッセージがAWSによって受信されることを確認するための便利なツールです。MQTT、WebSocket経由のMQTT、およびHTTPSを使用して送信されたMQTTメッセージをサポートします。これを`mqtt-aws`および`http-aws`の両方のサンプルで使用します。
​
1. [AWS MQTTテストクライアント](https://us-east-1.console.aws.amazon.com/iot/home?region=us-east-1#/test)ページに移動します。ここから、購読するトピックを作成します。このサンプルでは、トピックは`moddable/with/aws/#`になります。これを変更する場合は、サンプルのコードもそれに応じて変更してください。
2. クライアントの`エンドポイント`も取得する必要があります。これはページ上部の「接続の詳細」ドロップダウンにあります。次のステップのためにエンドポイントをメモし、このタブを開いたままにしておきます。
​
AWS MQTTテストクライアントは、セットアップが正しく機能していることを確認するための簡単な方法を提供するだけでなく、プロジェクトの要件に応じてクライアントからのネットワークトラフィックをAWSの他のサービスに接続することも可能にします。
​
## エンドポイントの設定
​
キーが正しくフォーマットされ、正しいディレクトリに配置され、AWSがメッセージを受信する準備が整ったら、AWS IoTと通信するためにこのディレクトリのサンプルを実行する準備がほぼ整いました。
​
最初に、前述のAWSエンドポイントを使用するようにサンプルを更新する必要があります。選択したサンプルの`main.js`で、`awsEndpoint`変数を次の形式に変更します：`<endpoint>`.iot.`<region>`.amazonaws.com（例：`a2vxxxxxxxxxxx-ats.iot.us-east-1.amazonaws.com`）
​
> **注**: `https://`のようなプロトコル識別子を含めないでください
​
## 接続のテスト
​
これらすべてのセットアップが完了したら、選択したプロジェクトを実行できます！
​
このコマンドは、シミュレータを使用してコンピュータ上でサンプルを実行します：
​
```
mcconfig -m -d
```
​
`mqtt-aws`の場合、xsbugコンソールに1秒ごとに日付とランダムな数値が記録されるはずです。また、これらのメッセージがAWS MQTTテストクライアントに表示されるはずです。
​
`http-aws`の場合、xsbugで「OK」というメッセージ応答が表示され、「Moddable: 123」というメッセージがAWS MQTTテストクライアントに表示されるはずです。
​
<!-- これらのモジュールの動作についての詳細は、[MQTTモジュール](../../..//modules/network/mqtt/mqtt.js)および[HTTPモジュール](../../../modules/network/http/http.js)を参照してください。 -->
​
## トラブルシューティング
​
テストクライアントにメッセージが表示されない場合は、証明書のパスが正しいか、名前がコード内で正しくフォーマットされているか、エンドポイントURLが正しくフォーマットされているか（`https://`なし、例：`a2vxxxxxxxxxxx-ats.iot.us-east-1.amazonaws.com`）を確認してください。
​
### 簡単な接続テスト
​
AWSからダウンロードしたキー（.der形式に変換する前）を使用して、キー、エンドポイント、およびAWS IoTが正しく設定されていることを確認するために、コンピュータのターミナルから次のコマンドを実行できます。これにより、HTTPSを使用してAWS MQTTテストクライアントに「Hello, World」メッセージが送信されます（クライアントで`moddable/with/aws/#`を購読していることを確認してください）：
​
```
curl --tlsv1.2 --cacert <path/to/CA-key.pem> --cert <path/to/device.pem.crt> --key <path/to/private.pem.key> --request POST --data "{ \"message\": \"Hello, world\" }" "https://<endpoint_url>:8443/topics/moddable/with/aws/test?qos=1"
```
​
接続が成功した場合、ターミナルに次のようなものが表示されるはずです：
​
```json
{"message":"OK","traceId":"xxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxx"}
```
​
注：
​
* `<path/to/CA-key.pem>`, `<path/to/device.pem.crt>`, `<path/to/private.pem.key>`を、それぞれAWSから最初にダウンロードしたCAキー、デバイスキー、および秘密鍵に置き換えてください。
* `<endpoint_url>`をAWS MQTTテストクライアントのエンドポイントに変更します。これは`main.js`の`awsEndpoint`変数と同じ値である必要があります（例：`a2vxxxxxxxxxxx-ats.iot.us-east-1.amazonaws.com`）
* マシンに`curl`がインストールされていない場合は、[こちら](https://everything.curl.dev/get)から入手できます。
​
### これでModdable SDKを使用してAWSと通信できるようになりました！
