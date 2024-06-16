# mods - ユーザーがインストールする拡張機能
Copyright 2020-2023 Moddable Tech, Inc.<BR>
改訂： 2023年10月11日

modsは、新しい機能を追加したり既存の挙動を変更したりするために、ユーザーがIoT製品にインストールできるスクリプトです。modは、画像、オーディオ、証明書などのアセットとともに、1つ以上のJavaScriptモジュールで構成されます。modは、工場でインストールされたファームウェアを変更することなく、製品のソフトウェアを拡張します。安全性、セキュリティ、およびプライバシーのリスクを最小限に抑えるために、製品はSecure ECMAScript（別名Hardened JavaScript）を使用してmodsをサンドボックス化することがあります。modsは、XS JavaScriptエンジンによってサポートされるModdable SDKの新しいコア機能です。

modsの作成は簡単ですが、その背後にある技術は複雑です。この記事では、modsの基本、modsの作成方法、およびプロジェクトでmodsを実行するためのサポートを実装する方法について紹介します。modsはリソースが制約されたマイクロコントローラーで実行されるため、modsにも制約があります。この記事では、これらの制限について説明し、modsの作者とmodホストがプロジェクトに最適なトレードオフを行うのに役立ちます。

## modsの主な特徴

modsは、IoT製品の機能を拡張するためのツールです。現在、IoT製品が拡張をサポートしていることは稀であり、サポートしている場合でもネイティブコードを通じて行われます。modsの基盤としてJavaScriptを使用することは、多くの製品で拡張性をサポートすることを実現可能にし、より多くのユーザーがそれを行うことを可能にします。

- **modsはポータブルです** - JavaScript言語は、最初からホストのオペレーティングシステムやホストハードウェアに依存しないように設計されています。Moddable SDKは、デバイス間で一貫したAPIを提供することでこれを実現しています。このポータビリティは、開発者が新しい開発ツールや新しい言語、新しいAPIを学ぶ必要なく、多くの異なるメーカーのデバイスで知識と経験を活用できるようにするために重要です。
- **modsは標準です** - IoT製品は、信頼性が高く、安全であり、異なるメーカーの製品と相互運用できるようにするために、標準的な電気コネクタから標準的なWi-Fiまで、標準を使用して構築されます。modsは、プログラミング言語として現代の業界標準であるJavaScript（ECMAScript 2020）を使用することで、この実証済みのパターンに従います。Moddableは、Ecma TC53委員会を通じて、組み込みシステム用のECMAScriptモジュールの標準化を支援しています。
- **modsは安全です** - IoT製品は現実世界の物理デバイスを制御します。modsを実行する際には、それらが安全・安心であることが不可欠です。Moddable SDKは、Secure ECMAScriptからのサンドボックス化方法であるCompartmentsを使用してこれを実現します。Compartmentsは、modホストがmodが実行されるタイミングとmodがアクセスできる機能を制御することを可能にします。これにより、modホストはセキュリティポリシーを設定できます。たとえば、modが特定の時間帯にのみ電球を制御し、特定の信頼できるクラウドサービスとのみ通信することを許可します。Secure ECMAScriptは、modがセキュリティルールから抜け出すことができないことを保証します。
- **modsは軽量です** - IoT製品のメモリとストレージはしばしば限られているため、modsは最小限のリソースで有用な作業を行うことができなければなりません。modsは事前にコンパイルされたバイトコードとしてインストールされるため、すぐに実行可能であり、そのバイトコードはメモリにロードされることなくフラッシュストレージからその場で実行されます。多くの有用なmodsは、有用な作業を行うためにわずか数KBのコードを必要とします。
- **modsは価値を加えます** - IoT製品のユーザーは、製品メーカーが実装する意志がない、または実装できない機能を必然的に要求します。modsは、ユーザーやサードパーティのソフトウェア開発者が製品の組み込みソフトウェアを拡張して新機能を追加し、追加のクラウド接続をサポートし、新しいデバイス統合を追加し、カスタムユーザーインターフェースを提供することを可能にすることで、この問題を解決します。
- **modsは製品を簡素化します** - IoT製品はさまざまな顧客セグメントのニーズに応えるために、より多くの機能をサポートし続けています。この機能の増加は、製品の使用を難しくし、実装を困難にし、出荷前にテストおよび検証をはるかに困難にします。modsは、メーカーがコア機能のみを組み込むことを可能にし、ユーザーが使用する機能のmodsをインストールすることで製品を簡素化します。

## modの構造
modは、JavaScriptソースコード、必要なデータアセット、そしてそれを構築する方法を記述するマニフェストの3つの部分で構成されています。これらの3つの部分は、Moddable SDKで完全なプロジェクトを構築するときとほぼ同じ方法で機能します。

### JavaScriptソースコード
modには複数のJavaScriptソースコードファイルを含むことができます。各ソースコードファイルは標準的なJavaScriptモジュールです。これらのモジュールは、modホストやmod自体によってインポートされることがあります。

modはシステムが起動すると自動的に実行されるわけではありません。`main.js`ファイルが存在しないこともあります。代わりに、modホストがどのモジュールをいつ実行するかを決定します。例えば、スクリーンセーバーを表示するためにmodを使用するmodホストは、システムがアイドル状態のときにのみmodの`screensaver.js`モジュールをロードして実行するかもしれません。

modは、利用可能なJavaScript言語の任意の機能を使用することができます。ストレージの制約のため、modホストは操作に不可欠でないと判断したJavaScript言語の特定の機能を省略することを選択するかもしれません。もしmodが利用できない言語機能を使用しようとすると、XSは「dead strip」エラーを投げます。modホストは、多くの一般的な低コストマイクロコントローラ（ESP32など）で完全なJavaScriptをサポートできるはずです。制約が必要なのは、フラッシュストレージが限られているマイクロコントローラ（Silicon LabsのGeckoパーツなど）やフラッシュアドレス空間が限られているマイクロコントローラ（通常は4 MBのフラッシュチップを持つが、アクセス可能なアドレス空間は1 MBのみのESP8266など）です。

### データとアセット
modにはコードだけでなくデータも含まれることがあります。ユーザーインターフェースを表示するmodには、必要な画像、フォント、音声が含まれている場合があります。ネットワークサービスに接続するmodには、データとしてTLS証明書を含むことができます。また、modには、キャリブレーションデータなどのテキストまたはバイナリデータも含まれることがあります。

modのデータは、`Resource` コンストラクタを使用してアクセスされ、メモリにロードすることなく直接フラッシュストレージから使用できます。

### マニフェスト
modのマニフェストは、modで使用されるファイルを記述するJSONファイルです。マニフェストは、Moddable SDKで使用される[マニフェスト形式](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/tools/manifest.md)の完全なサブセットであり、`include`、`module`、`data`、`resource`、`config` セクションのみを含みます。`build` および `platform` セクションもサポートされていますが、あまり使用されません。

modのマニフェストには通常、ビルドおよびデプロイに必要な設定を含む [`$MODDABLE/examples/manifest_mod.json`](../../examples/manifest_mod.json) が含まれており、デバイスへのmodの転送に使用する接続速度などが含まれています。

### modの例
このセクションでは、シンプルなmodのモジュール、データ、マニフェストを示します。

モジュールは1つだけで、リソースからデータをインポートし、その内容とmodのバージョン番号をコンソールに出力します。

```js
import Resource from "Resource";
import config from "mod/config";

let message = new Resource("message.txt");
trace(String.fromArrayBuffer(message), "\n");
trace(`Software version ${config.version}\n`);
```

このmodには、短いメッセージが書かれたテキストファイルのデータファイルが1つ含まれています。

```
Hello, mod.
```

マニフェストは、これら2つのファイルをビルドシステムに含めるように指示します。

```json
{
	"include": "$(MODDABLE)/examples/manifest_mod.json",
	"config": {
		"version": 1.1
	},
	"modules": {
		"*": [
			"./mod"
		]
	},
	"data": {
		"*": [
			"./message"
		]
	},
}
```

> **注**: マニフェストまたはコマンドラインに `"config"` 値がない場合、"mod/config" モジュールは作成されません。

## modのビルド、実行、デバッグ
modは、Moddable SDKの `mcrun` コマンドラインツールを使用してビルドおよび実行されます。`mcrun` ツールは、ネイティブコードを含む完全なホストをビルドする `mcconfig` と非常に似ています。

上記の例のmodをビルドして実行するには、modのディレクトリを現在の作業ディレクトリに設定し、次のように `mcrun` を実行します：

```
mcrun -d -m
```

成功したビルドの出力は次のようになります：

```
> mcrun -d -m
# copy message.txt
# xsc mod.xsb
# xsc check.xsb
# xsc mod/config.xsb
# xsl mc.xsa
```

デスクトップ用にビルドする場合、modは自動的にシミュレータで開かれます。しかし、modは単独では実行できません。これはmodホストが必要だからです。modホストは最初に `mcconfig` を使用して起動され、その後 `mcrun` を使用してmodが起動されます。以下に簡単なmodホストを紹介します。

`mcrun` を使用して、`-p` オプションでターゲットプラットフォームを指定することにより、マイクロコントローラにmodをビルドしてインストールすることができます。

```
mcrun -d -m -p esp32/moddable_two
```

`mcrun` がmodをインストールするためには、まずデバイスにmodホストのデバッグビルドがインストールされている必要があります。`mcrun` はxsbugデバッガープロトコルを使用してmodをインストールするため、modホストはデバッグビルドでなければなりません。modはmodホストのリリースビルドでも実行できますが、別の方法でインストールする必要があります（この方法は[この議論](https://github.com/Moddable-OpenSource/moddable/discussions/1105)でESP32シリコンファミリーについて説明されています）。

modがインストールされると、`mcrun` は自動的にマイクロコントローラを再起動し、xsbugに接続してmodのデバッグを行います。

## mod のホスティング
プロジェクトにmodのサポートを追加することは簡単です。追加の設定とmodを呼び出すコードの追加が必要です。それ以外に、デフォルト設定の調整が必要な場合もあります。このセクションでは、modのホスティングの基本的な手順と高度な設定オプションについて説明します。

### シンプルな mod ホスト
このセクションでは、modホストを作成するための基本的な手順を示します。

#### マニフェスト
Moddable SDKで一般的に使用されるマニフェスト、例えば [`manifest_base.json`](https://github.com/Moddable-OpenSource/moddable/blob/public/examples/manifest_base.json) は、modのサポートには追加のコードが必要なため、modのサポートを有効にしていません。実際にmodを使用するプロジェクトのみがそのコードを含めるべきです。modを有効にするには、プロジェクトのマニフェストの `defines` セクションに `XS_MODS` を追加します。

```
"defines": {
	"XS_MODS": 1
},
```

#### インストールされた mod の確認
modホストは、modをロードする前にmodがインストールされているかどうかを確認することが望ましいかもしれません。それを行うには、modを扱うためのユーティリティモジュールである `Modules` モジュールを使用します。まず、`Modules` モジュールをインポートします：

```js
import Modules from "modules";
```

プロジェクトマニフェストに `Modules` モジュールのマニフェストを追加します：

```
"include": [
	$(MODULES)/base/modules/manifest.json
]
```

次に、静的な `has` メソッドを使用して、特定のモジュールが利用可能かどうかを確認します。以下のコードは、上記の例のモジュール "mod" がインストールされているかどうかを判定します：

```js
if (Modules.has("mod"))
	trace("mod installed\n");
```

`has` 関数は、modをロードまたは実行しません。その存在のみを確認します。

#### modの実行
`Modules` モジュールは、mod内でモジュールをロードして実行することができます。

```js
if (Modules.has("mod"))
	Modules.importNow("mod");
```

`importNow` 関数はモジュールをロードし、モジュールの本体を実行します。これは動的な `import` を呼び出すのと似ていますが、`importNow` は同期的です。

`importNow` 関数はモジュールのデフォルトエクスポートを返します。以下のmodがデフォルトエクスポートを通じていくつかの関数をエクスポートすることを考えてみましょう。

```js
export default {
	onLaunch() {
	},
	onLightOn() {
	},
	onLightOff() {
	}
};
```
modホストは起動時にmodをロードし、modが自身を初期化するために `onLaunch` を呼び出します。

```js
const mod = Modules.importNow("mod");
mod.onLaunch();
```

modホストは適切なタイミングで他の関数を呼び出します。`onLightOn`はライトが点灯したとき、`onLightOff`は消灯したときです。

### 実行時の互換性の確認
各modには、現在のホストとの互換性を検証する関数を含む`check`という名前のモジュールがあります。デフォルトでは、`check`モジュールは`mcrun`によって自動的に作成され、modのグラフィックスが現在のホストと互換性があるかどうかを検証します。存在する場合、他のどのモジュールよりも先に`check`モジュールを呼び出すことが良い習慣です。`check`モジュールは、非互換性を見つけた場合に例外を投げる関数をエクスポートします。

```js
if (Modules.has("check")) {
	try {
		const checkFunction = Modules.importNow("check");
		checkFunction();
	}
	catch (e) {
		trace(`Incompatible mod: ${e}\n`);
	}
}
```

ピクセルフォーマットが`"x"`に設定されている場合、`check`モジュールは`mcrun`によって生成されません。ピクセルフォーマットを設定するには、`mcrun`を呼び出す際のコマンドラインで`-f x`を使用するか、modのマニフェストの`"config"`セクションで`"format"`を設定します。

### キー
JavaScript言語の動的な性質は、JavaScriptエンジンが仮想マシンで使用されるすべてのプロパティ名を追跡する必要があります。XSはこれをキーアレイを使用して行います。マイクロコントローラー用のホストを構築するために使用される[`preload`](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/xs/preload.md)機能は、ホストのすべてのプロパティ名をフラッシュストレージのキーアレイに保存します。

起動時に、XSはランタイムで作成されたキーを格納するための第二の配列を作成します。この配列は初期割り当てサイズを持ち、必要に応じて増分で拡大します。

しかし、modsはホストに現れない多くのプロパティ名を含むコードを含むことができるため、より多くのシンボルを作成する可能性があります。これに対応するために、プロジェクトはマニフェストの`"creation"`セクションでランタイムキーアレイの初期長を調整することができます。

```
"creation": {
	"keys": {
		"initial": 128,
		"incremental": 32
	}
}
```

キーアレイの各エントリはポインタであり、32ビットマイクロコントローラ上では4バイトです。上記の128キーテーブルはメモリの512バイトを使用します。

xsbugのインストゥルメントペインは、"Keys Used"セクションでランタイムキーアレイに割り当てられたキーの数を表示します。この値を監視することで、ランタイムキーアレイの最適な初期長を選択するのに役立ちます。

### JavaScript言語機能
Moddable SDKビルドツールのデフォルトの動作は、ホストモジュールによって使用されていないJavaScript言語機能を削除することです。このプロセスはエンジンのサイズを削減し、フラッシュストレージのスペースを節約し、インストール時間を速めます。

XSリンカーは、プロジェクトのモジュールのバイトコードを分析することによって、使用されていない言語機能を安全に削除することができます。ただし、リンカーはホストが実行された後にのみインストールされるmodsのバイトコードを分析することはできません。

modホストは、XSリンカーに自動削除を行わせる代わりに、マニフェストを使用して利用可能な言語機能を制御することができます。最も簡単なアプローチは、すべての言語機能を保持することです。フラッシュストレージの逼迫がないプロジェクトにとっては、これは実行可能なオプションです。プロジェクトマニフェストに空のstrip配列を持つ`strip`オブジェクトを追加します：

```
"strip": []
```

modホストは、選択した機能を明示的に削除することによって、フラッシュストレージのサイズを減らすことができます。これは、削除するコンストラクタと関数を命名することによって行われます。次のstrip配列は、`eval`、MapとSet、Atomics、SharedArrayBuffer、RegExp、およびProxyなど、制約のあるマイクロコントローラでめったに使用されない機能を削除します。

```
"strip": [
	"Atomics",
	"eval",
	"Function",
	"Generator",
	"Map",
	"Proxy",
	"Reflect",
	"RegExp",
	"Set",
	"SharedArrayBuffer",
	"WeakMap",
	"WeakSet"
]
```

言語機能の削除の詳細については、マニフェストドキュメントの[Strip](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/tools/manifest.md#strip)セクションを参照してください。

modが削除されたJavaScript言語機能を使用すると、「dead strip」例外が生成され、仮想マシンは実行を終了します。

### modの保護
modは、ホストと同じモジュールやグローバル変数にアクセスできるため、強力です。これは、modがホストと同じ操作を行う可能性が高いことを意味します。modを実行するための環境を準備するだけのシンプルなホストの場合、これは問題ではありません。しかし、一部のホストは、ホストの操作の完全性を確保するために、modができることを制限したいと考えています。これらのIoT製品のホストは、ホストが許可する操作のみをmodが実行できるようにしたいと考えています。

スクリプトが特定の機能へのアクセスを制限することは一般的です。すべてのWebブラウザは、多くの方法でスクリプトを制限することにより、ユーザーのコンピュータの完全性を保証します。たとえば、スクリプトは通常、ユーザーのファイルシステムにアクセスすることはできません。

Secure ECMAScriptを使用すると、modホストはブラウザがホストするスクリプトに対して行うのと同じように、modが利用できる機能を制限できます。この制限は、Secure ECMAScript（SES）を使用して行われます。SESは、提案されているJavaScript言語の拡張であり、すでにXSで実装されています。SESは、現在のJavaScript仮想マシン内の軽量サンドボックスである`Compartment`にmodのモジュールをロードすることにより、制限を行います。

以下のセクションでは、SESがmodホストにmodがアクセスできる機能を制限する能力をどのように与えるか、またSESが特定の攻撃を排除する方法についての簡単な例を示しています。

#### モジュールアクセスの制限
次のコードはCompartmentを作成し、そのCompartmentにmodの "mod" モジュールをロードします。

```js
let c = new Compartment(
	{},
	{
		"mod": "mod"
	}
});
let exports = c.importNow("mod");
```

`Compartment` コンストラクターへの最初の引数はCompartment内のグローバル変数に関連しており、次のセクションで説明されます。2番目の引数はCompartmentのモジュールマップです。これは2つの機能を果たします。まず、Compartment内で実行されるスクリプトによってインポート可能なモジュールを決定します。次に、Compartment内でモジュールがアクセスされる方法を変更するためにモジュール指定子をリマッピングします。

上記のモジュールマップは、Compartment内で "mod" という名前のモジュールのみをインポートすることを許可します。モジュール指定子のリマッピングはありません。左側のプロパティ名（"mod"）はCompartment内のモジュール指定子であり、右側の値（同じく "mod"）はホストで指定されたモジュールです。

`importNow` メソッドの呼び出しは、Compartment `c` 内でモジュール "mod" をロードします。モジュールマップのため、modはホストのモジュールをインポートすることができません。もし試みた場合、モジュールが存在しないかのようにインポートは失敗します。

以下のCompartmentマップは、リマッピングを使用するように変更されています。

```js
let c = new Compartment(
	{},
	{
		"modExample": "mod"
	}
});
let exports = c.importNow("modExample");
```

リマッピングは、既存のモジュールの限定版をmodにアクセスさせるために役立ちます。例えば、ホストはmodにHTTPクライアントモジュールへのアクセスを許可したいが、HTTPクライアントが接続できるドメインを制限したい場合があります。ホストは制限のないHTTPモジュール ("http") と制限されたバージョン ("httpRestricted") を持っています。ホストは自身のリクエストに制限のないバージョンを使用します。そして、Compartment内でモジュール指定子を "http" にリマッピングすることにより、modがHTTPクライアントモジュールを "http" をインポートすることによって使用できるように制限版を提供します。

#### 別々のグローバル変数
プロジェクトはしばしば重要なデータやオブジェクトを便利なアクセスのためにグローバル変数に保存します。これらのグローバル変数は、能力やプライベート情報へのアクセスを提供するかもしれませんが、modホストはmodからこれらを隠したいと考えています。SESがなければ、グローバル変数はmodに利用可能です。SESが新しいCompartmentを作成すると、そのCompartmentはmodホストのグローバル変数とは別の自身のグローバル変数セットを受け取ります。Compartmentのグローバル変数には、JavaScript言語によって定義されたグローバル変数のみが含まれます。

次のコードでは、グローバル変数「secret」はmodに利用できません。

```js
globalThis.secret = new Secret;
let c = new Compartment(
	{},
	{
		"modExample": "mod"
	}
});
let exports = c.importNow("modExample");
```

Compartmentコンストラクターへの最初の引数は、作成時にCompartmentに追加する追加のグローバルのマップです。次のコードでは、`secret` グローバルが `sharedSecret` という名前のグローバル変数としてmodに利用可能になります：

```js
globalThis.secret = new Secret;
let c = new Compartment(
	{
		sharedSecret: globalThis.secret
	},
	{
		"modExample": "mod"
	}
});
let exports = c.importNow("modExample");
```

Compartmentインスタンス `c` は `globalThis` プロパティを持っており、modホストはCompartmentのグローバルにアクセスするために使用できます。前の例は、Compartmentの `globalThis` プロパティを使用して書き直すことができます。

```js
globalThis.secret = new Secret;
let c = new Compartment(
	{
		"modExample": "mod"
	}
});
c.globalThis.sharedSecret = globalThis.secret;
let exports = c.importNow("modExample");
```

modホストは、いつでもCompartmentのグローバルにアクセスして更新することができます。これは強力な機能であり、modの実行環境を予測不可能にしないように注意して使用する必要があります。

#### modはmodホストの呼び出しを傍受できません
JavaScriptの経験がある人なら、`Object`、`Function`、`Array` などの組み込み原始オブジェクトの巧妙なパッチを通じてmodがmodホストの操作に干渉できると期待するかもしれません。例えば、次の例は `Array` インスタンスへのすべてのpush呼び出しを傍受します。

```js
const originalPush = Array.prototype.push;
Array.prototype.push = function(...elements) {
	// code here now has access to both the array and arguments to push
	// allowing it to inspect and modify the parameters
	if (elements[0] > 0)
		elements[0] *= -1;
	return originalPush.call(this, ...elements);
}
```

この種の組み込みオブジェクトへのグローバルパッチは[モンキーパッチ](https://en.wikipedia.org/wiki/Monkey_patch)と呼ばれています。これは、あるスクリプトが別のスクリプトを攻撃することを可能にする技術です。SESでは、原始的なオブジェクトがフリーズされているため、既存のプロパティを変更することは不可能で、この種の攻撃は不可能です。Compartment内で実行されているスクリプトが`push`関数を置き換えようとすると、例外が投げられます。

## 舞台裏
modsは使いやすいため、それらを動作させるために関与する多くの複雑な詳細を見落としやすくなります。このセクションでは、modsを扱う際に重要になるかもしれない実装の詳細について説明します。

### modsのビルド
`mcrun`は、modのJavaScriptソースコードとデータを単一のバイナリファイル、XSアーカイブ（.xsa拡張子）に変換します。アーカイブ形式は、フラッシュストレージから直接JavaScriptバイトコードが実行されるように設計されており、メモリにコピーする必要がありません。

JavaScriptモジュールはXSバイトコードに事前コンパイルされるため、ソースコードはターゲットデバイスにデプロイされません。これにより、デバイスはJavaScriptソースコードを解析する必要なく、modの実行を開始できます。

マニフェスト内の画像、フォント、およびオーディオリソースは、マニフェストの[リソースルール](https://github.com/Moddable-OpenSource/moddable/blob/public/documentation/tools/manifest.md#resources)に従って、対象デバイスと互換性のある形式に変換されます。

modのソースコードはプラットフォームに依存しないものの、modのバイナリ形式はプラットフォームに依存します。画像のピクセル形式や回転は特定のホストに最適化されており、生成されるXSバイトコードは特定のXSエンジンのバージョン用です等。アーカイブはmodの一般的な配布形式ではありません。

`mcrun` ツールは、modホストのシンボルテーブルを知らずにmodをビルドします。XSバイトコードは16ビットIDでシンボルを参照します。modがmodホストによって実行されると、XSは自動的にmodのバイトコード内のシンボルID値を更新して、ホストのIDと一致させます。このプロセスは非常に高速です。modのシンボルIDはホストが変更されたときのみ更新され、毎回の実行時に更新されるわけではありません。

### Preloadとの相互作用
modホストがモジュールをロードするとき、ホストの前にmodがモジュールを検索します。これにより、modはホストのモジュールをオーバーライドすることができます。

ただし、ホスト内のプリロードされたモジュールは、ビルド時にすでにそのインポートが完了しています。したがって、プリロードされたモジュールによってインポートされたモジュールは、modのモジュールによって上書きされることはできません。

これは一般的に、modホストへの拡張としてmodを使用する際に問題を引き起こすことはありません。ただし、ホスト内のモジュールにパッチを配布するためにmodを使用することはできません。

### modの保存場所
modはフラッシュメモリに保存されますが、modの保存領域の場所とサイズはホストによって異なります。

利用可能なスペースよりも大きなmodをインストールしようとすると失敗するため、予約されたスペースの外に誤って書き込むことはありません。

#### ESP32
ESP32のESP-IDFはパーティションマップをサポートしており、Moddable SDKはmod用にパーティションを予約するためにそれを使用します。デフォルトの[`partitions.csv`](https://github.com/Moddable-OpenSource/moddable/blob/6729c9482b9186f3654d8b158f095451edb74e62/build/devices/esp32/xsProj/partitions.csv#L25)では、modパーティションはここで定義されています：

```
xs,       0x40, 1,       0x3A0000, 0x040000,
```

この設定は、modを保存するために256 KBのスペースを予約します。ESP32用のModdable SDKプロジェクトは、独自のマニフェストを使用してmodの保存領域を増減させたり、modをサポートしていない場合はその領域を削除したりすることができます。

#### ESP8266とnRF52
ESP8266とnRF52でのmodの保存は、ESP32よりも複雑です。その理由は2つあります。まず、正式に定義されたパーティションがないため、Moddable SDKはフラッシュレイアウトを完全に管理する必要があります。次に、modアーカイブはその場で実行を可能にするためにメモリマップされる必要がありますが、ESP8266はフラッシュの最初の1 MBのみをメモリマップでき、nRF52はフラッシュが1 MBしかありません。これにより、modアーカイブはmodホストと同じ1 MBのスペースに保存される必要があります。

Moddable SDKは、この問題を解決するために、modホストイメージに続く最初のフラッシュセクターからmodの保存を開始します。これにより、各ホストで可能な限り大きなスペースをmodに提供します。しかし、これはまた、modのアドレスとmodに利用可能なスペースがホストに依存することを意味します。

### XSアーカイブ形式

XSアーカイブファイル形式は、[ISO Base Media File Format](https://www.loc.gov/preservation/digital/formats/fdd/fdd000079.shtml)のBox（別名アトム）構造メカニズムを使用してデータを構造化します。XSアーカイブファイルのアトム構造は、次の表に示されています：

```
XS_A - signature of the XS archive file
    VERS - XS version number used in byte code
    SIGN - the MD5 checksum of this mod's modules and data (before mapping)
    NAME - the dotted name of the mod (null terminated string)
    SYMB - the XS symbol table for the mods
    IDEN - host identifiers in the order of the symbol table (array of XS ID values)
    MAPS - symbol table indexes in the order of their occurrence in the CODE atoms
    MODS - the modules of this mod
        PATH - the path of this module (null terminated string)
        CODE - the byte code of of this module
        (Additional PATH / CODE pairs follow - one per module]
    RSRC - the data of this mod
        PATH - the path of this module (null terminated string)
        DATA - the data of this resource
        (Additional PATH / DATA pairs follow - one per piece of data]
```

`MODS` アトムの前にあるアトムの順序は、マイクロコントローラの実装がこのレイアウトを期待しているため、示された通りでなければなりません。

アーカイブをロードする際、XSはシンボルテーブルを反復処理して、マップされた識別子の配列を構築します。マップされた識別子の配列が `IDEN` アトムと一致する場合、それ以上の操作は必要ありません。そうでない場合、マップされた識別子テーブルは `IDEN` アトムに書き込まれ、XSは `MAPS` および `CODE` アトムをトラバースして、バイトコード内の識別子をマップし、CODEアトムをそれに応じて更新します。

#### ボックス / アトム構造化メカニズムについて

アトム構造化メカニズムは、バイナリデータを構造化する軽量な方法です。

各アトムは、2つの32ビットのビッグエンディアン値からなる8バイトのヘッダーで始まります。最初の値は、ヘッダーを含むアトムのサイズをバイト単位で示す符号なし整数です。2番目の値は、アトムの内容を識別する[four-character code](https://en.wikipedia.org/wiki/FourCC)で、通常は人間が読めるASCII値で構成されます。例えば、XSアーカイブファイルのルートにある唯一のアトムの場合、最初の値はファイルのバイト単位の長さであり、2番目の値は `XS_A` で、XSアーカイブを含むアトムを示します。

各アトムは、他のデータおよび/または他のアトムを含むことができます。アトムの内容は、その4文字のコードによって定義されます。例えば、上記の`RSRC`アトムは、`PATH`と`DATA`のアトムのペアを0個以上含むように定義されていますが、`NAME`アトムはヌル終端文字列を含むように定義されています。

アトムの4文字のコードを認識しないファイルリーダーは、アトムヘッダーの最初の値を使用してアトムをスキップすることができます。このアプローチにより、新しいアトムを導入しても既存のリーダーが壊れることはありません。
