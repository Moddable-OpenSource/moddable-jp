# FirmataでTC53 IOを探求する
Copyright 2019 Moddable Tech, Inc.<BR>
著者: Peter Hoddie<BR>
更新日: 2019年8月4日

Moddable SDKには、2つのデバイス（多くの場合はコンピュータとマイクロコントローラ）間でハードウェア制御情報を通信するためのFirmataプロトコルの実装が含まれています。この実装では、[Ecma TC53](https://www.ecma-international.org/memento/tc53.htm)への積極的な提案に基づく新しいIOクラスセットを使用しています。IOクラスは、現代のJavaScriptを実行できる最も制約の厳しいハードウェア構成での効率的な実装を可能にしながら、様々なIOタイプにわたって一貫した動作を提供するシンプルなAPIを通じてハードウェアリソースへのアクセスを提供するように設計された共通のIOパターンに基づいています。

実装の主な目標は、新しいIOクラスの使用を探求することです。Firmataプロトコルは、多くの異なる種類のIOを活用するため、興味深いテストケースです。Firmataプロトコルはこの点で非常に貴重でした。結果として得られたFirmata実装は、より広く有用である可能性もあります。TC53 IOクラスが他のハードウェアプラットフォームで利用可能になるにつれて、このFirmata実装はそこでほぼそのまま動作し、追加のハードウェアホストでFirmataサポートを提供する可能性があります。

> **注意**: IOクラス実装は、リソース制約のあるデバイスの代表例であり、広く利用可能で、Moddableでよく理解されており、安価であるため初期ターゲットとして選択されたESP8266マイクロコントローラでのみ利用可能です。

Firmataプロトコルは本質的にクライアント/サーバープロトコルであり、マイクロコントローラがサーバーの役割を、デバイスコントローラ（多くの場合はコンピュータ）がクライアントの役割を担います。Firmataエコシステムでは、[Firmata.js](https://github.com/firmata/firmata.js#firmatajs)が人気のクライアントで、FirmataサーバーとやりとりするためのNode.js環境での[JavaScript API](https://github.com/firmata/firmata.js/tree/master/packages/firmata.js#firmata-prototype-api)を提供しています。様々な言語と環境で利用可能な[多くの他の](https://github.com/firmata/arduino#firmata-client-libraries) Firmataクライアントがあります。Firmataサーバーの実装ははるかに少なく、[Standard Firmata Arduino実装](https://github.com/firmata/arduino#firmata)が最も一般的に使用されています。

Moddable SDKでのFirmataの実装には、クライアントとサーバーの両方が含まれています。それぞれは、提案されているTC53 IOクラスAPIを使用してJavaScriptで実装されています。クライアントの場合、FirmataをTC53 IOクラスプロバイダーとして提示します。

クライアントとサーバーの両方を実装することで、IOクラスのより多くの使用方法を探求することができました。サーバーの実装はFirmata.jsクライアントに対して開発され、クライアントの実装はサーバーに対して開発されました。プロトコルドキュメントの情報を補完するために、Firmata.jsとArduino Firmataサーバーのコードが参照されました。

> **注意:** このドキュメントの例をビルドして実行するには、すでにModdable SDKをインストールし、[入門ドキュメント](../Moddable%20SDK%20-%20Getting%20Started.md)からESP8266ターゲット用のビルドツールをセットアップする手順に従っている必要があります。

## 目次

* [Firmataの背景](#firmata-background)
* [Firmataサーバー](#firmata-server)
	* [サンプルコード](#server-example-code)
	* [TCP経由でのFirmataサーバーとの通信](#server-tcp)
	* [実装ノート](#server-implementation-notes)
* [Firmataクライアント](#firmata-client)
	* [サンプルコード](#client-example-code)
	* [実装ノート](#client-implementation-notes)
* [Pocoを使用したFirmataグラフィックス](#firmata-graphics-using-poco)
	* [サンプルコード](#poco-example-code)
	* [実装ノート](#poco-implementation-notes)
* [結論](#conclusion)

<a id="firmata-background"></a>
## Firmataの背景
Firmataは、低レベルのハードウェアリソースとやりとりするための[通信プロトコル](https://github.com/firmata/protocol#firmata-protocol-documentation)です。これらのリソースには、デジタル入力、デジタル出力、I2Cペリフェラル、シリアルポート、アナログ入力などが含まれます。シリアルは、例えばArduinoとコンピュータの間など、Firmataで最も一般的なトランスポートですが、プロトコルはTCPネットワーク接続などの双方向接続を介して実行することもできます。

シリアル経由でのFirmataのデフォルトボーレートは57600で、これは比較的低速です。比較として、Moddable SDKはESP8266で921600ボー（16倍高速）でシリアル経由で`xsbug`デバッグプロトコルを実行します。FirmataプロトコルはMIDIプロトコルから派生しているため、多くのメッセージが数バイトのみを必要とするコンパクトです。したがって、ボーレートは通常制限要因ではありません。

Firmataプロトコルには、実装を形作る暗黙の前提があります：

- **信頼性のあるトランスポート**: プロトコルにチェックサム、確認応答、または再送信の規定がないため、信頼性のあるトランスポートを想定しています。Firmataプロトコルの基本形式はMIDIプロトコルから派生していますが、メッセージの意味は異なります。

- **準リアルタイム**: プロトコルにタイムスタンプがないため、接続が準リアルタイムであることを想定しています。シリアルなどの直接物理接続で動作する場合、遅延は小さく一定です。しかし、Wi-Fi経由で通信する場合、遅延はより大きく予測不可能です。

- **単一クライアント**: プロトコルにピンリソースがすでに使用中であるために利用できないことを報告する規定がないため、サーバーが1つ以下のクライアントに接続されることを想定しています。

限られたテストのため、見落としや誤解に基づく実装エラーがある可能性があります。Firmataを実装する前に、ModdableチームはFirmataでの作業経験がありませんでした。

<a id="firmata-server"></a>
## Firmataサーバー
Firmataサーバーをビルドして実行するには、以下のコマンドを実行します：

	cd $MODDABLE/examples/io/firmata/server
	mcconfig -m -p esp

これはFirmataサーバーをビルドしてESP8266にデプロイします。これはデバッグビルドではなくリリースビルドであることに注意してください。これは、Firmataが通信にシリアルポートを使用し、xsbugデバッガーがESP8266との通信に使用することを妨げるためです。

ここから、Firmata.jsサーバーはFirmataクライアントで通常通り使用できます。Firmataサーバーはシリアル経由で自分自身をアナウンスしないため、接続が完全に確立される前に、Firmata.jsがプローブリクエストを発行するまで約5秒待つ必要があることに注意してください。

Firmataサーバー実装は、以下の標準ピンタイプをサポートしています：

- デジタル入力
- デジタル入力プルアップ
- I2C
- アナログ入力
- シリアル

利用可能なピンは構成に依存します。例えば、実装では単一のシリアルポートのみが利用可能です。そのポートがFirmataトランスポートに使用されている場合、Firmataクライアントで使用することはできません。

Firmataサーバーは主に、ESP8266を高品質IPSディスプレイと静電容量式タッチスクリーンと組み合わせた[Moddable One](https://www.moddable.com/moddable-one.php)でテストされました。

<a id="server-example-code"></a>
### サンプルコード

以下のFirmata.jsコードフラグメントは、Moddable Oneハードウェアを扱う際に有用です。Firmata.jsリポジトリに含まれている`repl`は、探求を始めるのに便利な場所です。

#### 内蔵LEDのオン・オフ

````js
board.pinMode(2, 1)
board.digitalWrite(2, 1)	// off
board.digitalWrite(2, 0)	// on
````
#### フラッシュボタンの監視

````js
board.pinMode(0, 0)
board.digitalRead(0, v => console.log(v))
````

#### フラッシュボタン監視の無効化

````js
board.reportDigitalPin(0, 0);
````

#### アナログ入力の監視

````js
board.analogRead(0, v => console.log(v))
````

#### アナログ入力監視の無効化

````js
board.reportAnalogPin(0, 0)
````

#### シリアルポートへのバイト書き込み

````js
board.serialConfig({portId: 0, baud: 921600})
board.serialWrite(0, [64, 65, 66, 67]);
````

#### シリアルポートの受信バイト監視

````js
board.serialConfig({portId: 0, baud: 921600})
board.serialRead(0, bytes =>
	console.log(bytes.map(c => String.fromCharCode(c)).join("")))
````

#### シリアルポート監視の無効化

````js
board.serialStop(0)
````

#### タッチ画面のタッチポイント数監視（I2C）

````js
void board.i2cConfig()
void board.i2cRead(0x38, 2, 1, v => console.log(v))
````

#### I2C監視の無効化

````js
void board.i2cStop(0x38)
````

<a id="server-tcp"></a>
### TCP経由でのFirmataサーバーとの通信
Firmataサーバーは、シリアルに加えてTCPネットワーク接続での通信を実装しています。FirmataサーバーはTCPで2つの異なる方法で動作できます。1つの方法は、FirmataクライアントへのTCP接続を開始することで、この場合（混乱を招くことに）FirmataサーバーがTCPクライアントとして動作し、FirmataクライアントがTCPサーバーとして動作します。これがより一般的な方法です。または、FirmataクライアントからのTCPリクエストを待ち受けることもでき、この場合FirmataサーバーがTCPサーバーであり、FirmataクライアントがTCPクライアントになります。

#### `FirmataTCPClient`の使用
FirmataサーバーがFirmataクライアントへのTCP接続を開始するように設定するには、以下を行います：

1. Firmataサーバーファイルの`main.js`のソースコードを変更して、Firmataクライアントに接続します。

	````js
	// new FirmataSerial;
	new FirmataTCPClient({address: "192.168.1.19"});
	````

	TCP経由のFirmataはデフォルトでポート3030を使用します。異なるポートに接続するには、`FirmataTCPClient`を呼び出す際に`port`プロパティを含めます：

	````js
	new FirmataTCPClient({address: "192.168.1.19", port: 3029});
	````

2. Firmataサーバーをビルドします。Wi-Fiアクセスポイントの認証情報を必ず提供してください：

	```
	mcconfig -d -m -p esp ssid="Moddable" password="secret"
	```

Firmata.jsクライアントは[Etherport](https://github.com/rwaldron/etherport#etherport)モジュールを使用したTCP通信をサポートしています。Firmata.js replはEtherportをサポートしていませんが、[簡単に追加できます](https://gist.github.com/phoddie/17601031d83602f688c20c98292c622e)。そのバージョンの`repl`を使用している場合は、シリアルポートIDの代わりに`etherport`と入力するだけで、接続待ちのTCPリスナーでFirmata.jsを起動できます。

#### `FirmataTCPServer`の使用
FirmataサーバーがFirmataクライアントからの接続を待ち受けるように設定するには、以下を行います：

1. Firmataサーバーファイルのmain.jsのソースコードを変更して、Firmataクライアントに接続します。

	````js
	// new FirmataSerial;
	new FirmataTCPServer;
	````

	TCP経由のFirmataはデフォルトでポート3030を使用します。異なるポートで待ち受けるには、`FirmataTCPServer`を呼び出す際に`port`プロパティを含めます：

	````js
	new FirmataTCPServer({port: 3029});
	````

2. Firmataサーバーをビルドします。Wi-Fiアクセスポイントの認証情報を必ず提供してください：

	```
	mcconfig -d -m -p esp ssid="Moddable" password="secret"
	```

`FirmataTCPServer`クラスは一度に単一の接続のみを許可します。アクティブな接続がある場合、新しい接続要求は拒否されます。アクティブな接続が閉じられると、次の接続要求が受け入れられます。

`FirmataTCPServer`は`FirmataTCPClient`を使用してテストされました。Firmata.jsはまだ接続待ちのFirmataサーバーへの接続をサポートしていないためです。

<a id="server-implementation-notes"></a>

### 実装ノート
ModdableのFirmataサーバー実装は以下を報告します：

- **プロトコル メジャーバージョン**: 2
- **プロトコル マイナーバージョン**: 6
- **ファームウェア メジャーバージョン**: 2
- **ファームウェア マイナーバージョン**: 10
- **ファームウェア名**: moddable

デジタルレポートは、ポーリングではなく、デジタル入力の変化を検出する割り込みによって駆動されます。

ESP8266には、他のすべてのピンとは異なるハードウェアユニットに接続されているデジタルピンが1つあります（GPIO 16）。この違いは、このピンがディープスリープからマイクロコントローラを起こすための特別な用途を持っているからです。サーバー実装は、このピンでのデジタルレポートをサポートしていません。

Moddable OneのI2Cバスは、データにピン5、クロックにピン4を使用します。

ESP8266のプライマリシリアル接続は、TXにピン1、RXにピン3を使用します。

Moddable Firmataサーバー実装は、汎用FirmataサーバーとESP2866ピン構成の特定の知識を組み合わせています。ESP8266の知識は大部分が分離されており、最終的には追加のマイクロコントローラのサポートを容易にするために別のファイルに移行すべきです。

Firmataサーバーは、サーバー実装がクライアントに短いテキスト文字列を送信できる`doSendString`関数でオプションの`STRING_DATA`メッセージを実装しています。これらのメッセージはプロトコルで定義された意味を持ちません。これらは、xsbugデバッグ接続を利用できなくするシリアル経由でFirmataと通信する際には不可能な、サーバーからクライアントへの簡単なコンソールトレースを生成するデバッグに有用であることが証明されました。Firmata.jsを使用してこれらのメッセージをコンソールに出力するには、この行を追加します：

```js
board.on("string", msg => console.log(`Board message: ${msg}`));
```

<a id="firmata-client"></a>
## Firmataクライアント
Firmataクライアントの例は、スタートアップ時にFirmataサーバーへの接続を確立することで動作します（FirmataクライアントはTCPクライアントとして動作）。サーバーのIPアドレスはコードで定義されています。使用するサーバーのIPアドレスに合わせて、クライアント例の`main.js`ファイルの以下の行を変更してください。

```js
const ServerAddress = "10.0.1.36";
```

Firmataクライアントをビルドして実行するには、以下のコマンドを実行します：

	cd $MODDABLE/examples/io/firmata/client
	mcconfig -d -m -p esp ssid="Moddable" password="secret"

これはFirmataクライアントをビルドしてESP8266にデプロイします。サーバーとは異なり、これはデバッグビルドであることに注意してください。例ではシリアルではなくTCP経由でのみFirmataを使用するためです。

クライアントAPIは、提案されているTC53 IOプロバイダークラスです。IOプロバイダーは、1つ以上の種類のIOへのアクセスを提供するオブジェクトです。アクセスを提供するIOは、ここで説明するFirmataの使用のようにリモートの場合もあれば、例えばI2C経由で接続されたGPIOエキスパンダーのようにローカルの場合もあります。

Firmataプロバイダーを使用するには、最初にクラスをインポートします。この例ではTCPクライアントを使用するため、`FirmataClientTCP`をインポートします。

```js
import {FirmataClientTCP} from "firmataclient";
```

次に`FirmataClientTCP`プロバイダーをインスタンス化します。インスタンス化には2つの部分があります：設定とコールバックです。設定は、プロバイダーにハードウェアリソースへの接続方法を伝えます。この場合、設定はIPアドレスです。

```js
const firmata = new FirmataClientTCP({
	address: ServerAddress,
	onReady() {
		...
	},
}
```

コンストラクタ引数は、デフォルトポート3030を使用しない場合に接続するリモートポートを示す`port`を含む、オプションのプロパティを受け入れます。FirmataがI2Cとアナログピンデータを報告するために使用するポーリング間隔（ミリ秒）は、オプションの`interval`プロパティで設定されます。

```js
const firmata = new FirmataClientTCP({
	address: ServerAddress,
	port: 3029,
	interval: 100,
	onReady() {
		...
	},
}
```

プロバイダーの設定とコールバックは、コンストラクタが戻る際に設定され、その後変更することはできません。`FirmataClientTCP`には単一のコールバック`onReady`があり、接続が確立されたときに呼び出されます。接続が切断されたときに呼び出す`onError`コールバックの追加は今後の作業項目です。

プロバイダー実装は、リモートデバイスとの接続を確立するまで、どのIOが利用可能かを知らない場合があります。これはFirmataプロトコルの場合で、クライアントがサーバーの機能を学習するために初期のメッセージセットが交換されます（例：`CAPABILITY_QUERY`、`ANALOG_MAPPING_QUERY`、`REPORT_FIRMWARE`など）。`onReady`が呼び出されると、このプロセスは完了します。プロバイダーインスタンスには、スクリプトがプロバイダーのハードウェアリソースにアクセスするために使用する1つ以上のコンストラクタが含まれています。

<a id="client-example-code"></a>
### サンプルコード

以下のコードサンプルは、`FirmataClientTCP`クライアントが提供するIOコンストラクタの使用方法を示しています。これらは`onReady`コールバック内またはその後で実行する必要があります。その前に実行すると、コンストラクタがまだ利用できないため、例は失敗します。これらの例では、プロバイダーがESP8266に接続されていることを前提としています。I2Cの例の場合は、Moddable Oneを前提としています。

#### リモートLEDの点滅

```js
let led = new firmata.Digital({
	pin: 2,
	mode: firmata.Digital.Output,
});

let value = 0;
System.setInterval(() => {
	led.write(value);
	value ^= 1;
}, 500);
```

#### リモートフラッシュボタンの監視

```js
let remoteButton = new firmata.Digital({
	pin: 0,
	mode: firmata.Digital.Input,
	onReadable() {
		trace(`Remote Button: ${this.read()}\n`);
	}
});
```

この例では`onReadable`コールバックを持つことはオプションです。`read`呼び出しは常にプロバイダーが利用できる最新の値を返すためです。例えば、すでにポーリングループを持つスクリプトは、`onReadable`コールバックを使用するのではなく、その時点で値を直接読み取ることを選択する場合があります。

#### リモートアナログピンの監視

```js
let analog = new firmata.Analog({
	pin: 17,
	onReadable() {
		const value = this.read();
		trace(`Analog: ${value}\n`);
		if (value >= 1023)
			this.close();
	}
});
```

この例では、ピーク読み取り値を受信すると、アナログモニターを閉じます。Moddable Oneでこのコードをテストするには、アナログピントレースの近くまたは上に指を置いて、値の範囲を確認してください。

#### リモートデジタルバンクの監視
Firmataでは、ピンは8つのピンのグループであるポートに編成されています。TC53 IOクラス提案では、Firmataが`port`と呼ぶものに対して`bank`という用語を使用しています。この例では、バンク1の上位4つのピンであるデジタルピン12、13、14、15用の単一モニターを作成します。

```js
let remoteBank = new firmata.DigitalBank({
	pins: 0xF0,
	bank: 1,
	mode: firmata.Digital.Input,
	onReadable() {
		trace(`Change: ${this.read().toString(2)}\n`);
	}
});
```

#### I2C
I2Cは、トランザクションベースのハードウェアプロトコルであるため、アナログおよびデジタルピンよりも少し複雑です：バイトの読み書きを行うためにハードウェアにリクエストが行われます。プロバイダーは完全に非同期です。Firmataプロトコルは信頼性の高い配信を前提としているため、書き込み要求は最終的に書き込みピンに配信されるので、書き込み操作には問題ありません。

```js
let i2c = new firmata.I2C({
	address: 0x38,
})
i2c.write(Uint8Array.of(3, 11, 5));
```

I2C読み取りでは、読み取り呼び出しを行う際に読み取るバイト数を示す必要があります。IO Class API内でこの状況に対処するため、読み取りは要求されたバイト数で通常通り発行されます。読み取られたバイトが利用可能になると`onReadable`コールバックが呼び出されます。パラメータなしで`read`呼び出しを実行すると、読み取りの結果が返されます。複数の`read`呼び出しが発行された場合、それらの結果は要求されたのと同じ順序で返されます。

以下の例は、Moddable Oneの静電容量式タッチコントローラーで動作します。タッチセンサーによって現在検出されている指が0、1、または2本であるかを示すタッチカウントレジスタを読み取ります。

```js
let touchController = new firmata.I2C({
	address: 0x38,
	onReadable() {
		const result = new Uint8Array(this.read());
		trace(`${result[0]} touch points\n`);
	}
});
touchController.write(Uint8Array.of(2));
touchController.read(1);
```

<a id="client-implementation-notes"></a>
### 実装ノート
Firmataクライアントは`FirmataClientSerial`クラスの開始を提供します。これはテストされていません。Moddable Firmataサーバーと動作するために初期ハンドシェイクを管理するコードの追加が必要です。

実装はまだシリアルをサポートしていません。

<a id="firmata-graphics-using-poco"></a>
## Pocoを使用したFirmataグラフィックス
Moddable Oneハードウェアには統合タッチスクリーンがあります。Firmataクライアントコードがディスプレイに描画できるようにするため、シンプルなグラフィックスプロトコルがFirmataプロトコルに追加されました。グラフィックス実装は、Moddable SDKの[Commodettoグラフィックスライブラリ](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/commodetto.md)の[Pocoグラフィックスエンジン](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/commodetto/poco.md)を使用しています。統合は全機能の小さなサブセットで、多かれ少なかれ矩形を塗りつぶす機能です。ただし、このアプローチは将来的に追加の描画機能をサポートするように設計されています。

理論的には、FirmataでSPIピンを使用してModdable Oneディスプレイにレンダリングすることは可能です。しかし、最適化されたネイティブディスプレイドライバーを、Firmata経由の比較的遅いデータ転送で置き換える良い理由はありません。さらに、レンダリングされたグラフィックスを配信するために必要な大量のデータは、Firmataが最も得意とするものではありません。代わりに、実装はFirmataプロトコル経由でPoco描画コマンドをマーシャリングし、レンダリングはFirmataサーバー上で効率的に行われます。

<a id="poco-example-code"></a>
### サンプルコード
以下の例は、FirmataサーバーとやりとりするときにFirmataクライアントでPocoグラフィックスを使用する方法を示しています。Pocoを使用するには、Firmataサーバーがディスプレイドライバーサポートでビルドされている必要があります。Moddable Oneの場合は、`esp/moddable_one`ターゲットでFirmataサーバーをビルドします：

	cd $MODDABLE/examples/io/firmata/server
	mcconfig -m -p esp/moddable_one ssid="Moddable" password="secret"

#### 接続と画面消去
以下の例は、Moddable Oneに接続するFirmataプロバイダーを作成します。接続が確立されると、リモートディスプレイが青色で消去されます。

````js
new FirmataClientTCP({
	address: "10.0.1.22",
	onReady() {
		const poco = new this.Poco;
		const blue = poco.makeColor(0, 0, 255);
		poco.begin();
			poco.fillRectangle(blue, 0, 0, poco.width, poco.height);
		poco.end();
	}
})
````

`this.Poco`コンストラクタの呼び出しによって作成されるインスタンスは、完全なPoco JavaScript APIのサブセットを提示します。Moddable Oneでローカルディスプレイを消去するコードは、コンストラクタ以外は同じです：

````js
const poco = new Poco(screen);
const blue = poco.makeColor(0, 0, 255);
poco.begin();
	poco.fillRectangle(blue, 0, 0, poco.width, poco.height);
poco.end();
````

#### ディスプレイ付きリモートボタン
以下の例は、リモートボタンを読み取り、ボタンが押されたときに画面の矩形を赤色に、そうでなければ灰色に更新します。

```js
new FirmataClientTCP({
	address: "10.0.1.22",
	onReady() {
		const poco = new this.Poco;
		let gray = poco.makeColor(128, 128, 128);
		let red = poco.makeColor(255, 0, 0);
		let remoteButton = new this.Digital({
			pin: 0,
			mode: this.Digital.Input,
			onReadable() {
				const size = 80;
				poco.begin((poco.width - size) >> 1,
								(poco.height >> 2) - (size >> 1), size, size);
					poco.fillRectangle(this.read() ? gray : red,
								0, 0, poco.width, poco.height);
				poco.end();
			}
		});
	}
})
```

<a id="poco-implementation-notes"></a>
### 実装ノート
FirmataのPocoサポートは以下の呼び出しを実装しています：

- `constructor`
- `begin`
- `end`
- `makeColor`
- `fillRectangle`
- `drawPixel`
- `clip`
- `origin`

色は、チャネルあたり7ビット（R、G、Bはそれぞれ7ビット）を使用してクライアントからサーバーに送信されます。これは、Firmataが7ビットデータを運ぶように設計されており、多くの組み込みディスプレイは色チャネル情報の5または6ビットのみを使用するためです。

Firmataサーバーは、`CAPABILITY_QUERY`メッセージへの応答に続いて値`hasPoco`の`STRING_DATA`メッセージを送信することで、Pocoプロトコル拡張がサポートされていることをクライアントに通知します。この機能を知らないクライアントはメッセージを無視します。これは一時的なアドホックアプローチです。

すべてのPocoメッセージは、Firmataの既存または提案された拡張との競合を避けるために、Sysexメッセージ`User Command 1`内に含まれています。

<a id="conclusion"></a>
## 結論
このプロジェクトは、提案されているTC53 IOクラスのESP8266実装を動作させるための小さなFirmataサーバーを実装する実験として始まりました。この目標は成功し、実装のバグや設計で見落とされた詳細を振り落とすための優れたテストベッドを提供しました。実装は効率的です。そのサイズと複雑さは、実行するタスクに適切に見えます。コードは、所望の動作を得るためにIOクラスと格闘するのではなく、FirmataとIOクラスAPI間の変換に焦点を当てています。Firmataが要求する機能とIOクラスが提供するAPIの間には、合理的に強い対応関係があります。

Firmataクライアント実装は、`FirmataTCPServer`をテストする方法として始まりました。その基本が動作すると、TC53 IOクラスプロバイダーモデルを使用してそれを構築することが理にかなっていました。最初の非同期プロバイダー実装として、IOクラス設計がこれらのシナリオにどのように適応すべきかについてのアイデアの一部を形式化するのに役立ちました。

FirmataサーバーとクライアントをJavaScriptで実装することは、言語自体に組み込まれた多くの機能のため、比較的簡単です。結果として得られる実装は、まだ非常に新しいコードにとってはかなり安定しています。

FirmataへのPoco拡張は実験であり、サーバーとクライアントが連携して動作するようになったことで可能になりました。矩形を描画することはとても有用になるほどの十分な機能ではありませんが、いくつかの関数を追加することで、Firmata経由でリモートグラフィックスを使用することは簡単です。今のところ、いくつかの視覚的なFirmataの例を構築し、ディスプレイを含むインタラクティブなシナリオを探求する素晴らしい方法です。

全体的に、このFirmata実験は、提案されているIOクラスが非常に異なるIOモデルを実装するためにかなり使用可能であることを示しました。これは重要なことです。TC53提案の主要な設計ポイントは、特定の用途や市場セグメントに焦点を当てた高レベルのフレームワークを構築するための基盤として使用できる低レベルのIOクラスを定義することだからです。ここでFirmataによって具体化されたそのようなフレームワークが、IOクラスなどのTC53 APIサポートでローンチする新しいハードウェアリリースによって、箱から出してサポートされる世界を垣間見ることは刺激的です。
