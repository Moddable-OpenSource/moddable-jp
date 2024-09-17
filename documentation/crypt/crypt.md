# Crypt
Copyright 2017-2022 Moddable Tech, Inc.<BR>
改訂： 2022年6月14日

<a id="digest"></a>
## Digest (Crypt)クラス

Digestクラスは、さまざまなアルゴリズムを使用して暗号ハッシュを作成します。


```js
import {Digest} from "crypt";
```

### MD5ハッシュ


```js
let digest = new Digest("MD5");
digest.write("hello, world");
trace(`MD5 Hash: ${digest.close()}\n`);
```

### SHA1ハッシュ

```js
let digest = new Digest("SHA1");
digest.write("hello,");
digest.write(" world");
trace(`SHA1 Hash: ${digest.close()}\n`);
```

### new Digest(type)

`Digest`コンストラクタは、計算するハッシュのタイプのみを引数として受け取ります。

```js
let digest = new Digest("SHA1");
```

次のハッシュ関数がサポートされています：

* MD5
* SHA1
* SHA224
* SHA256
* SHA384
* SHA512

### write(message)

`write`関数は、計算中のハッシュにメッセージを追加します。メッセージの長さに制限はありません。`write`のメッセージ引数は`String`または`ArrayBuffer`である可能性があります。`write`関数は、特定のダイジェスト計算に対して複数回呼び出すことができます。

```js
digest.write("123");
digest.write("456");
```

### close()

`close`関数は計算されたハッシュを返します。ハッシュは`ArrayBuffer`として返されます。`close`関数は一度しか呼び出すことができません。なぜなら、ハッシュ計算に関連するすべてのリソースを解放するからです。

<!-- 11/7/2017 BSF
ここでリセット関数を例とともに文書化する必要があるかもしれません。
-->

<!-- 11/7/2017 BSF
ブロックサイズと出力サイズのアクセサ/ゲッタ関数に加えて、プロセスと更新のヘルパー関数もあります。これらもここで文書化する必要がありますか？
-->

<!-- 11/7/2017 BSF
BlockCipher、StreamCipher、およびModeクラスを文書化する必要があります。これらはcryptblockcipherの例アプリで使用されています。
-->

<a id="transform"></a>
## Transform クラス

`Transform`クラスは、証明書データの一般的な変換を行う静的メソッドを含んでいます。

```js
import Transform from "crypt/transform";
```

可能な限り、データの変換は実行時に行うべきではありません。なぜなら、追加の時間とメモリを使用するからです。代わりに、データはターゲットデバイスに最適な形式で保存されるべきです。これらの変換関数は、デバイスプロビジョニングフローなど、実行時に変換を行う必要がある状況のために提供されています。

プロジェクトで使用するために、マニフェストに `Transform` モジュールを追加します。

```json
"modules": {
	"crypt/transform": "$(MODULES)/crypt/etc/transform"
}
```

<a id="transform-pemToDER"></a>
### static pemToDER(data)

`pemToDER` 関数は、PEM形式（Base64エンコードされたASCII）の証明書を [DER](https://en.wikipedia.org/wiki/X.690#DER_encoding) 形式（バイナリデータ）に変換します。入力 `data` は `String`、`ArrayBuffer`、またはホストバッファである可能性があります。戻り値は `ArrayBuffer` です。

複数の証明書を含むPEMファイルの場合、`pemToDER` は最初の証明書のみを変換します。

この関数は、以下の `openssl` コマンドラインに似ています：

```
openssl x509 -inform pem -in data.pem -out data.der -outform der
```

`pemToDER` は、デリミタとして `-----BEGIN CERTIFICATE-----` および `-----BEGIN RSA PRIVATE KEY-----` の両方を探します。

<a id="transform-privateKeyToPrivateKeyInfo"></a>
### static privateKeyToPrivateKeyInfo(data[, oid])

`privateKeyToPrivateKeyInfo` 関数は、秘密鍵を秘密鍵情報に変換します（どちらもバイナリDER形式）。入力 `data` は `ArrayBuffer` またはホストバッファです。オプションの `oid` パラメータは、`Array` としてのキーアルゴリズムのオブジェクトIDです。指定されていない場合、デフォルトで `PKCS#1` のOID `[1, 2, 840, 113549, 1, 1, 1]` が使用されます。戻り値は `ArrayBuffer` です。


`pemToDER` と `privateKeyToPrivateKeyInfo` を一緒に使用することは、次の `openssl` コマンドラインに似ています：

```
openssl pkcs8 -topk8 -in private_key.pem -inform pem -out private_key.pk8.der -outform der -nocrypt
```
