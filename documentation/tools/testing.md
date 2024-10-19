# Testing the Moddable SDK
Copyright 2020-2024 Moddable Tech, Inc.<BR>
改訂： 2024年9月16日

## 目次

* [はじめに](#intro)
* [テスト](#tests)
	* [JavaScript言語テスト (TEST262)](#javascript-tests)
	* [Moddable SDKテスト](#moddable-tests)
* [test262テストアプリ](#test262-app)
* [testmcテストアプリ](#testmc-app)
* [シミュレーターでのテスト実行](#testing-on-simulator)
* [Moddable SDKのためのテストの書き方](#writing-tests)
	* [テストの構造](#writing-structure)
	* [テストするモジュールのインポート](#writing-importing)
	* [合格か不合格か？](#writing-pass-fail)
	* [非同期テスト](#writing-async)
	* [アサーション](#writing-assert)
	* [テストフィクスチャ](#writing-fixtures)
	* [テストを小さく保つ](#writing-small)
	* [環境についての仮定を最小限にする](#writing-assumptions)
	* [ネットワークテスト](#writing-network)
	* [グラフィックステスト](#writing-graphics)
	* [Piuタッチインタラクションテスト](#writing-touch)
* [テストの書き方に関する参考資料](#test-writing-reference)

<a id="intro"></a>
## はじめに
このドキュメントでは、Moddable SDKで実装されたJavaScriptのユニットテストを実行する方法について説明します。テストを実行するには3つの部分があります：

- **テスト**. ユニットテストは成功条件と失敗条件のテストを行います。これらはJavaScriptで実装され、ソースコードは `.js` ファイルに保存されています。
- **テストアプリ**. テストアプリはユニットテストを実行する責任があります。テスト対象のデバイス上で実行されます。
- **テストランナー**. テストランナーは実行するテストを選択し、テストアプリと協調して各テストを実行し、テスト結果を収集する責任があります。xsbugデバッガーはModdable SDKのテストランナーです。

テスト、テストアプリ、テストランナーの分離により、リソースが制約された組み込みデバイス上で大規模なテストスイートを実行できます。

> **注意**: テストランナーはxsbug通信チャネルを使用してテストアプリとテスト実行を調整します。その結果、リリースビルドはテスト実行に使用できません。

<a id="tests"></a>
## テスト

Moddable SDKは、JavaScript言語用とModdable SDKランタイム用の2つの別々のテストスイートを使用しています。以下の2セクションでは、これらのテストスイートとそのインストール方法について紹介します。

<a id="javascript-tests"></a>
### JavaScript言語テスト（TEST262）
TEST262は、ECMAScript（JavaScript）言語委員会であるTC39からの公式な適合性テストスイートです。これは[Ecma-414](https://www.ecma-international.org/publications-and-standards/standards/ecma-414/)標準で定義されています。これらのテストは、すべてのJavaScriptエンジンによって実行され、仕様に対する互換性、相互運用性、および適合性を保証するために行われます。

TEST262テストはGitHubのリポジトリにあります。

```
git clone https://github.com/tc39/test262
```

> 注：`xst`ツールは、コマンドラインからローカルコンピュータで[test262テストを実行する](../xs/xst.md#test262)ために使用できます。

<a id="moddable-tests"></a>
### Moddable SDKテスト
Moddable SDKテストは、Moddableによって作成されたテストスイートで、異なるデバイスターゲット間の実装の正確性と一貫性を検証します。テストはハードウェアI/O、ネットワーキング、グラフィックス、データなど、幅広い機能をカバーしています。各ポートでサポートされているハードウェアの機能と特徴の違いにより、すべての環境ですべてのテストが合格するわけではありません。

Moddable SDKのテストは、`$MODDABLE/tests`にあるModdable SDKリポジトリに位置しています。

Moddable SDKのテストは、test262の`test`ディレクトリにコピーする必要があります。

```
cp -r $MODDABLE/tests <path-to-test262-repository>/test/moddable
```

> **注**: Moddable SDKのテストを実行するには、TEST262のテストをインストールする必要があります。TEST262のテストを実行する予定がない場合でもです。

<a id="test262-app"></a>
## test262 テストアプリ

test262テストアプリは、test262のテストを実行するModdableアプリです。これには、そのテストに必要なTEST262テストハーネスの実装が含まれています。test262アプリは`$MODDABLE/tools/test262`に位置しています。

> **注**: test262テストアプリは、実行するために約128 KBの空きメモリが必要です。ESP8266のような空きメモリが少ないデバイスでは実行できません。

### Build `test262` テストアプリ
他のModdableアプリと同様に、`-d`デバッグオプションを使用して`test262`アプリをビルドします。

```
cd $MODDABLE/tools/test262
mcconfig -d -m -p esp32
```

> 注: これらの指示は、ターゲットデバイスのビルド環境が設定されていることを前提としています。

> 注意: このドキュメントの例は `esp32` プラットフォーム用です。他のプラットフォーム用にビルドする場合は、対応するプラットフォーム識別子を使用してください。

### xsbug でテストを設定する

`xsbug` で、`Preferences` メニューを選択します。

`On Exceptions` スライダーをオフにし、`Test` タブをオンにします。下の画像で囲まれた項目を参照してください：

<img src="../assets/tools/testing/test-xsbug-prefs.png" width=794>

ウィンドウの上部にTESTタブが表示されます。

> 注意: 多くのテストが意図的に例外を引き起こすため、「Break: On exception」を無効にすることが必要です。

### テストの位置を特定する

TEST262を初めて実行するとき、xsbugでテストリポジトリの場所を設定する必要があります。

<img src="../assets/tools/testing/test-xsbug-locate.png" width=340>

`REPOSITORY...` 項目を選択し、ファイル選択ダイアログが表示されます。GitHubからクローンしたリポジトリの `test262` ディレクトリを選択します。

`REPOSITORY...` 項目はテストカテゴリのリストに置き換えられます。

<img src="../assets/tools/testing/test-categories.png" width=335>

### テストの選択

カテゴリまたは個々のテストの左側をクリックすると、そのテストまたはカテゴリが実行されるように選択されます。黒丸は実行されるテストを示します。

<img src="../assets/tools/testing/test-select.png" width=344>

また、`SELECT` テキストフィールドを使用して、実行したいテストの名前を入力することもできます。

### テストの実行

"Play" ボタンをクリックしてテストを開始します：

<img src="../assets/tools/testing/test-run.png" width=344>

"Play" ボタンは "Stop" ボタンに変わり、テストの実行数と結果が更新されます。

<img src="../assets/tools/testing/test-running.png" width=340>

"Stop" ボタンを押してテストを停止します。

テストが実行されている間、`REPORT` 項目には現在実行中のテストが表示されます。

> **注記**: ほとんどの test262 テストは非常に迅速に実行されますが、一部は非常に時間がかかることがあります。

### 結果の表示

テストの実行が完了すると、"Stop" ボタンは再び "Play" ボタンに戻ります。

<img src="../assets/tools/testing/test-results.png" width=1179>

"Play" ボタンの隣には、失敗、成功、スキップしたテストの数を示すステータスラインがあります。

`REPORT` アイテムを開くと、失敗が表示されます。`LOG` パネルにはエラーや例外を含む出力が含まれています。

> **注意**: テストは意図的に失敗条件を作り出し、ログに表示される例外が発生します。例外が必ずしもテストの失敗を示すわけではありません。テストは有効な入力と無効な入力の両方を検証して、成功条件と失敗条件を確認します。

`REPORT` アイテムの上にマウスポインタを置くと、アイコンが表示されます。このアイコンをクリックすると、結果をテキストファイルに出力して保存できます。

<img src="../assets/tools/testing/test-report.png" width=372>

<a id="testmc-app"></a>
## testmc テストアプリ

`testmc` テストアプリは、Moddable SDKに特有のテストを実行するModdableアプリです。`test262` アプリと似ており、同じように動作します。ただし、`testmc` はJavaScript言語ではなく、Moddable SDKモジュールの検証用に設定されています。

`testmc` アプリは `$MODDABLE/tools/testmc` に位置しています。

> **重要**: TEST262テストを実行するには `test262` アプリを使用する必要があります。Moddable SDKテストを実行するには `testmc` アプリを使用する必要があります。

### `testmc` テストアプリのビルド

他のModdableアプリと同様に、`testmc`アプリをビルドします。

```
cd $MODDABLE/tools/testmc
mcconfig -d -m -p esp32
```

ネットワークテストを実行するには、`mcconfig`行に`ssid`と`password`を含める必要があります：

```
cd $MODDABLE/tools/testmc
mcconfig -d -m -p esp32 ssid=<ssid> password=<password>
```
> 注意: 画面が必要なテストを実行する場合は、ビルド時にサブプラットフォーム識別子を含めてください（例：`esp32/moddable_two`、`esp32`だけではなく）。これにより、スクリーンドライバが含まれます。

### テストの実行

テストはテストリストの`moddable`カテゴリに表示されます。

<img src="../assets/tools/testing/test-moddable-category.png" width=365>

testmcテストアプリが実行され、テストが配置されると、test262テストと同じ方法でテストの選択、実行、結果の表示が行われます。

<a id="testing-on-simulator"></a>
## シミュレータでのテストの実行
test262およびtestmcテストアプリは、Moddable SDKのシミュレータであるmcsimでも実行できます。これは、シミュレータがターゲットデバイスと一致した結果を提供していることを確認し、テストの開発に役立ちます。もちろん、ハードウェアI/Oなど、シミュレータがサポートしていない機能に依存するテストは失敗します。

シミュレータでの実行プロセスはxsbugで同じです。シミュレータが起動すると、シミュレータオプションに変更を加える必要があります。「Reload on Abort」スイッチを「on」に設定します。これが設定されていない場合、最初のテストが実行された後にテストが停止します。

<img src="../assets/tools/testing/test-simulator.png" width=300>

<a id="writing-tests"></a>
## Moddable SDKのテストの作成
Moddable SDKのテストはTEST262テストと同じ方法で書かれます。まだTEST262のテストの書き方に慣れていない場合、以下のセクションが参考になります。

<a id="writing-structure"></a>
### テストの構造
各テストファイルには2つの部分があります：frontmatterとテストスクリプトです。frontmatterは[YAML](https://en.wikipedia.org/wiki/YAML)マークアップ形式を使用します。frontmatterは最初に現れ、テストに関するメタデータを含んでいます。テストスクリプトはfrontmatterの後に続きます。

こちらは非常に基本的なテストです：

```js
/*---
flags: [module]
---*/
assert.sameValue("Java" + "Script", "JavaScript");
```

`module`フラグは、テストがプログラムではなくECMAScriptモジュールとしてロードされるべきであることをテストランナーに伝えます。`module`フラグが存在しない場合、テストはプログラムとして実行されます。Moddable SDK用に書かれたすべてのテストには`module`フラグが含まれるべきです。なぜなら、Moddable SDKのほとんどのスクリプトはモジュールとして実行されるからです。

フラグに加えて、frontmatterにはテストの短い説明や、より長い情報セクションが含まれることがあります：

```
/*---
description: concatenation of two strings
info: |
  Two strings are concatenated together using
  the plus operator to verify that the JavaScript
  engine is correctly concatenating them
  together.
flags: [module]
---*/
```

frontmatterは必須です。存在しない場合、テストスクリプトは実行されません。frontmatterが正しくない場合、テストスクリプトが正しく実行されない可能性があります。

慣例により、著作権表示はfrontmatterの直前または直後に配置されますが、テストスクリプトの前に配置されます。

```
/*---js
description: https://github.com/Moddable-OpenSource/moddable/issues/452
flags: [module]
---*/

// Copyright 2022 Moddable Tech, Inc. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided.

assert.sameValue("Java" + "Script", "JavaScript");
```

以下で紹介される`async`フィールドは、非同期で実行されるテストを作成するために導入されます。frontmatterフィールドの[リファレンスガイド](https://github.com/tc39/test262/blob/main/INTERPRETING.md#metadata)については、TEST262のドキュメントを参照してください。

<a id="writing-importing"></a>
### モジュールのインポート
Moddable SDKには多くのモジュールが含まれています。これらのモジュールの正しい動作を検証するためのテストを書くことも、テストの一部としてこれらのモジュールを使用することもできます。テストにModdable SDKからモジュールを取り入れるには、単にそれをインポートします。

```js
/*---
flags: [module]
---*/
import structuredClone from "structuredClone";
...
```

`import` ステートメントは、frontmatterに `module` フラグが設定されているため使用できます。

Moddable SDKからインポートできるモジュールは、testmcアプリケーションによって決定されます。その [manifest.json](https://github.com/Moddable-OpenSource/moddable/blob/public/tools/testmc/manifest.json#L29) ファイルを確認して、含まれているモジュールを確認できます。テストに他のモジュールが必要な場合は、testmcマニフェストに追加することができます。

<a id="writing-pass-fail"></a>
### 合格か不合格か？
テストは、未処理の例外を生成せずに最後まで実行された場合、合格とみなされます。未処理の例外で終了した場合、テストは不合格とみなされます。先の例で、`"Java"` と `"Script"` の連結が `"JavaScript"` であることを確認したテストは、未処理の例外を投げないため合格です。次のテストは常に失敗します。なぜなら `undefined` はオブジェクトではないからです。

```js
/*---
flags: [module]
---*/
undefined.toString();
```

時には、テストが失敗することを確認したい場合もあります。前述の例は `TypeError` で失敗することが期待されています。これを行う方法はいくつかあります。便利な方法の1つはfrontmatterを使用することです。frontmatterにはオプションの `negative` 項目があり、これによりテストランナーはエラーなしで終了した場合にテストを失敗とみなし、期待されるタイプの未処理例外でのみ終了した場合に合格とみなします。この修正されたテストは、JavaScript言語によって要求されるように `TypeError` が投げられた場合に合格します。

```js
/*---
flags: [module]
negative:
  type: TypeError
---*/
undefined.toString();
```

<a id="writing-async"></a>
### 非同期テスト {/*examples*/}
一部のテストは非同期で実行する必要があります。これには、ネットワーク操作、タイマー、およびPromiseを使用するテストが含まれます。非同期テストの場合、テストスクリプトの終わりに到達した時点でテストの合格または不合格がわからないため、テストが合格したか失敗したかを判断するデフォルトの方法は機能しません。非同期操作が完了して初めてテストが合格したかどうかがわかります。非同期で実行するテストを書くことは、テストの書き方を少し変更することで可能です。まず、フロントマターに`async`フラグを追加します。

```js
/*---
flags: [module,async]
---*/
```

テストは、テストによって`$DONE`関数が呼び出されるまで終了とはみなされません。`$DONE`が引数なしで呼び出された場合、テストは合格とみなされます。引数付きで`$DONE`が呼び出された場合、テストは失敗とみなされます。`$DONE`が呼び出されるまで、テストは実行を続けます。以下のテストは例を示しています：

```js
/*---
flags: [module,async]
---*/
(Promise.resolve(123)).then(value => {
	if (123 === value)
		$DONE();
	else
		$DONE("unexpected value");
});
```

上記では、`assert.sameValue` 関数を使用して `value` が期待される `123` と等しいかをテストしていませんでした。それは、テストがテストの結果を `$DONE` に渡さなければならないからです。非同期テストで一般的なアサートを使用できるようにするために、`testmc` は `$DO` 関数を提供しています。`$DO` を使用すると、テストは次のように書き直すことができます：

```js
/*---
flags: [module,async]
---*/
(Promise.resolve(123)).then($DO(value => {
	assert.sameValue(value, 123);
}));
```

上記のテストは、Promiseが解決された場合に機能します。しかし、Promiseが拒否された場合はどうでしょうか？その場合、拒否ハンドラがないため、テストは無期限に実行されます。非同期テストが長時間実行されていることを簡単に検出するために、`testmc` は `$TESTMC.timeout` 関数を提供しており、これにより、テストが予想されるミリ秒数よりも長く実行されると、非同期テストが失敗します。

```js
/*---
flags: [module,async]
---*/
(Promise.resolve(123)).then($DO(value => {
	assert.sameValue(value, 123);
}));
$TESTMC.timeout(500, "timeout on promise resolve");
```

非同期テストは同期テストよりも書くのが難しいですし、非同期コードも同期コードよりも書くのが難しいです。始めるときは注意を払い、すでに機能している既存のテストのパターンに従うようにしてください。

<a id="writing-assert"></a>
### アサーション {#examples}
TEST262ランタイムには、基本的なアサーションをいくつか持つ`assert`グローバル変数が含まれています。可能な限り、テストではこれらを使用して一貫性と可読性を保つべきです。すべてのアサーション関数は、期待される条件が満たされない場合に例外を投げます。

最も基本的なアサーションは`assert`関数で、偽の値が渡された場合に例外を投げます：

```js
assert(false);					// throws
assert(true);						// does not throw
assert(1 === 3);				// throws
assert(3 === 3);				// does not throw
assert(Array.isArray(5), "expected Array");	// throws
```

`assert`は、失敗を説明するメッセージを含むオプショナルな第二パラメータを受け入れることに注意してください。これらのメッセージは、テスト結果をレビューする際に、どこで失敗が発生したかを理解するのに役立ちます。

`assert.sameValue`と`assert.notSameValue`は、通常、期待される値と実際の値を比較するために使用されます。`assert`と同様に、テストを説明するメッセージのためのオプショナルな引数があります。

```js
assert.sameValue(result, 12, "expected result of 12");
assert.notSameValue(result, 12, "expected result different from 12");
```

慣例により、`assert.sameValue`と`assert.notSameValue`への最初の引数はテストによって生成された値です。2番目の値はテストの期待される結果です。

`assert.throws` 関数は、操作が特定のエラーを投げることを確認するために使用されます。同じ結果は `try`/`catch` ブロックを使用して達成できますが、`assert.throws` 関数はより簡潔で読みやすいです。`assert.throws` の最初の引数は生成される予定のエラーのコンストラクタです。2番目の引数は実行する関数で、この関数は最初の引数で渡されたエラーコンストラクタのインスタンスを投げることが期待されます。オプションの最終引数は、他のアサート関数と同様、失敗を説明するメッセージです。

```js
assert.throws(SyntaxError, () => Timer.delay(), "one argument required")
```

もし `Timer.delay` が `SyntaxError` を投げる場合、`assert.throws` は例外をキャッチして返し、テストは合格します。もし `Timer.delay` が期待された `SyntaxError` を投げない場合、`assert.throws` は例外を投げ、そのためテストは失敗します。

<a id="writing-fixtures"></a>
### テストフィクスチャ
特定のモジュールのテストを書くとき、いくつかのテストが複数のファイルに分割されている間、共有したいデータやコードがしばしばあります。この共有されたデータやコードはフィクスチャとして参照されます。testmcのテストを書くとき、テストフィクスチャは単に追加のモジュールです。これらのモジュールはテストで `import` ステートメントを使用してインポートできます。

温度センサーから値を読み取るテストを考えてみましょう。連続して取得したセンサーの読み取り値が安定していることを確認するために、整数部分のみを比較し、小数部分は無視します。テストフィクスチャは、2つの値を整数として比較する関数を追加することができます。こちらがそのテストフィクスチャで、"integerFixture.js"というファイル名でテストを使用する同じディレクトリに保存されています。

```js
assert.sameInteger = function (actual, expected, reason) {
	actual = Math.round(actual);
	expected = Math.round(expected);
	if (actual !== expected)
		throw new Error(reason ?? `sameInteger got ${actual}, but expected ${expected}`);
}
```

このテストフィクスチャは、グローバルの`assert`に`sameInteger`関数を追加します。その後、通常通り`assert.sameInteger`を使用できます：

```js
/*---
flags: [module]
---*/
import "./integerFixture.js"
const sensor = device.sensor.temperature;
assert.sameInteger(sensor.sample().temperature, sensor.sample().temperature);
```

テストフィクスチャはデータもエクスポートすることがあります。例えば、テストフィクスチャはJSONシリアライゼーションのテスト用のオブジェクトを提供するかもしれません：

```js
export default {
	a: 1,
	b: "2",
	c: {three: 3}
}
```

テストフィクスチャモジュールはテストではないため、テストを識別するために使用されるYAML frontmatterを持つべきではありません。もちろん、JavaScriptコメントを使用して説明や著作権通知を含むことはできます。

<a id="writing-small"></a>
### テストを小さく保つ
`testmc`の主な目的は、リソースが制限されたマイクロコントローラーでテストを実行することです。これを実現するために、テスト自体は小さく保たれる必要があります。テストモジュールと任意のテストフィクスチャのソースコードはマイクロコントローラーにダウンロードされ、そこでXS JavaScriptエンジンによってバイトコードにコンパイルされます。ソースコードはRAMを使用し、コンパイルされたバイトコードも同様です。もちろん、テストの実行にもメモリが必要です。ほとんどの場合、これは問題になりません。大きな既存のテストをマイクロコントローラーではなくコンピューターで実行するように設計した場合に、メモリ不足の例外が発生したことがあります。通常、問題は複数の異なるソースコードファイルにテストを分割することで解決できます。これにより、テスト間で共有されるコードとデータのためのテストフィクスチャを使用します。

テストスクリプトのソースコードは100行以下に保ち、フィクスチャを含む全体の行数は200行以下にするというのが一般的なルールです。一部のマイクロコントローラはかなりのメモリを持っているため、もっと大きなファイルでも動作します。しかし、テストができるだけ多くの環境で実行できるようにするために、これらのガイドラインに従って書かれるべきです。

TEST262自体のテストは、しばしば数行のコードだけです。テストを小さく保つことは、問題が発生したときのデバッグにも便利です。テストファイルに数百のテストが含まれている場合、テストの失敗を特定してデバッグすることがより困難になります。特定の領域を検証することに焦点を当てたテストファイルは、テストログでの結果をより容易に理解できます。

<a id="writing-assumptions"></a>
### 環境についての仮定を最小限にする
各テストは新しい仮想マシンで実行されます。これにより、各テストがそれ以前に実行されたテストから独立して実行されるため、一貫したテスト結果が保証されます。しかし、以前に実行されたテストからどれだけテストを隔離できるかには限界があります。

ファイルシステムのテストでは、ファイルの作成、削除、変更が行われます。このようなテストは、一時ファイルを削除することで後始末を行うべきです。しかし、これは完璧ではありません。テストが失敗した場合、後始末の機会を得られないことがあります。したがって、一般的には、以前のテストが適切に後始末を行っていなくても機能するようにテストを記述するべきです。

各テストは別々の仮想マシンで実行されますが、テストの間にマイクロコントローラ自体が再起動することはありません。再起動すると、テストスイートの実行が大幅に遅くなるためです。仮想マシンが閉じられると、それが所有するすべてのリソースが解放されます。これにより、すべてのメモリが解放され、すべてのファイルが閉じられ、すべてのネットワーク接続が終了します。しかし、いくつかの状態は残ります。例えば、ネットワーク接続の状態は変わりません：マイクロコントローラがWi-Fiに接続したままの場合です。これは便利ですが、各テストがWi-Fi接続を確立する必要がないため、一連のネットワークテストを迅速に実行できます。しかし、Wi-Fiが切断された状態から開始することを前提としたテストは失敗する可能性があります。そのようなテストは、最初に明示的にWi-Fiから切断するべきです。

<a id="writing-network"></a>
### ネットワークテスト
ネットワークを使用するテストを書く際の最初のステップは、マイクロコントローラがネットワークに接続されていることを確認することです。testmcランタイムにはネットワークテストを支援するための`$NETWORK`グローバルが含まれており、ネットワーク接続が利用可能になるのを待つために使用できる`connected`プロパティが含まれています。

```js
/*---
flags: [module,aync]
---*/
import {Client} from "http";
await $NETWORK.connected;
new Client(...);
...
```

ネットワーク接続が確立できない場合、`$NETWORK.connected`によって返されるPromiseは拒否され、例外が投げられ、テストが失敗します。

ネットワークテストの追加サポートについては、以下の`$NETWORK`グローバルに関するリファレンスセクションを参照してください。

<a id="writing-graphics"></a>
### グラフィックステスト
グラフィックスレンダリング操作の正しい動作を検証するテストを書く際の一般的な課題は、画像が正しく描画されているかを判断することです。一般的な解決策は、期待される結果を含む画像を保存して、レンダリングされた画像と比較することです。このアプローチは、ストレージスペースが限られているマイクロコントローラでのテストでは実用的ではありません。代わりに、testmcはレンダリングされた画像のチェックサムを生成する機能を提供します。チェックサムはほんの数バイトであり、テストスクリプト自体に簡単に組み込むことができます。さらに、チェックサムは画像が出力デバイスに送信されるときに生成されるため、期待される画像をバッファするための追加メモリは必要ありません。

testmcを実行するとき、標準の`screen`グローバル変数はチェックサムをサポートするように拡張されます。表示が更新されるたびに、描画された画像のチェックサムが自動的に生成されます。以下のテストでは、Pocoを使用して青色の100ピクセルの正方形の長方形を描画し、期待されるチェックサムが生成されることを確認します。

```js
/*---
flags: [module]
---*/
import Poco from "commodetto/Poco";
import Bitmap from "commodetto/Bitmap";

assert.sameValue(Bitmap.RGB565LE, screen.pixelFormat, "requires RGB565LE output");

const render = new Poco(screen);
const blue = render.makeColor(255, 0, 0);
render.begin(0, 0, 100, 100);
	render.fillRectangle(blue, 0, 0, render.width, render.height);
render.end();

assert.sameValue("bdae73e6e02019bdbc080589058c0135", screen.checksum, "100x100 blue square");
```

テストを描画する前に、出力として使用されるスクリーンが16ビットRGB565リトルエンディアンピクセルをレンダリングすることを確認します。これは、生成されるチェックサムがピクセル形式に依存するためです。スクリーンが異なるピクセル形式を使用している場合、チェックサムは一致しません。テストは、各ピクセル形式に対する期待されるチェックサムを含めることで、いくつかの異なるピクセル形式で動作するように書くことができます。

青色を`255, 0, 0`から`128, 0, 0`に変更するとチェックサムは一致しません。しかし、青色を`254, 0, 1`に変更すると、チェックサムは一致します。これは、565ピクセルが色の低ビットを無視するためです。画像が24ビットまたは32ビットのピクセルでレンダリングされる場合、すべての色ビットが使用されるため、不一致が生じます。

画像のチェックサムは、Piuユーザーインターフェースフレームワークによって生成された画面でも機能します。ただし、Piuは即座にレンダリングしないため、少し異なる技術が必要です。testmcアプリケーションは、Piuを使用してレンダリングされた画像を検証するための`screen.checkImage`関数を提供します。この関数は、Piuが更新を保留してレンダリングすることを許可し、その後、Piuがレンダリングした画像が期待されるチェックサムを持っていることを確認します。

```js
/*---
description:
flags: [module]
---*/
import Bitmap from "commodetto/Bitmap";

assert.sameValue(Bitmap.RGB565LE, screen.pixelFormat, "requires RGB565LE output");
assert((240 === screen.width) && (320 === screen.height), "unexpected screen");

const redSkin = new Skin({ fill: "red" });
new Application(null, {
	skin: new Skin({
		fill: "white"
	}),
	contents: new Content(null, {
		height: 100, width: 100,
		skin: new Skin({ fill: "red" })
	})
});
screen.checkImage("b2342b9d128b17b544c8a1e7c4ff652d");
```

<a id="writing-touch"></a>
### Piu タッチインタラクションテスト {/*examples*/}
インタラクティブなユーザーインターフェースを持つアプリケーションの正しい操作をテストするには、シミュレートされたタッチイベントをアプリケーションに送信します。testmcは、タッチ開始、タッチ移動、タッチ終了イベントを送信する関数で`screen`オブジェクトを拡張します。これらのイベントはPiuにルーティングされ、通常どおり処理されます。タッチイベントの配信は非同期であるため、テスト用のタッチイベントを送信するために使用される関数も非同期です。

次のテストでは、タッチ開始イベントを送信します。この動作は、期待されるタッチIDと座標が受信されることを確認します。もしタッチ開始イベントが半秒以内に動作によって受信されない場合、テストはタイムアウトで失敗します。

```js
/*---
description:
flags: [async, module]
---*/
class TouchTestBehavior extends $TESTMC.Behavior {
	onTouchBegan(content, id, x, y) {
		assert.sameValue(id, 0);
		assert.sameValue(x, 10);
		assert.sameValue(y, 20);
		$DONE();
	}
}

new Application(null, {
	active: true, Behavior: TouchTestBehavior
});

$TESTMC.timeout(500, "timeout for doTouchBegan");
await screen.doTouchBegan(0, 10, 20);
```

テストにPiuの振る舞いを取り入れる場合、振る舞いのクラスは`Behavior`ではなく`$TESTMC.Behavior`を継承するべきです。これにより、すべてのメソッドが例外を投げてテストを失敗させるように自動的にラップされ、結果として`assert`関数がそのまま使用できるようになります。

<a id="test-writing-reference"></a>
## テスト作成のための参考資料
testmcアプリケーションは、テストスクリプトをサポートするためにランタイム環境に機能を追加します。これは、TEST262テストハーネスによって追加された`$TEST262`グローバル変数と同じアイデアです。以下のセクションでは、テストで使用できるtestmcのランタイム機能を紹介します。

### `$TESTMC` グローバル

`$TESTMC` グローバルは、さまざまな目的のためのプロパティを持つオブジェクトです。

#### `timeout(ms [, message])`
`timeout` 関数は、指定された時間内にテストが完了しない場合にテストを失敗させるために使用されます。`timeout` 関数は非同期テスト（frontmatterで `async` とマークされているもの）でのみ機能します。

```js
$TESTMC.timeout(5000, "dns lookup timeout");
```

#### `HostObject`, `HostObjectChunk`, `HostBuffer`
これらのコンストラクタは、テスト中の関数に引数として渡すためのホストオブジェクトを作成するために使用されます。

```js
new $TESTMC.HostObject // host object with pointer (-1) for storage
new $TESTMC.HostObjectChunk // host object with 16 byte chunk for storage
new $TESTMC.HostBuffer(count) // host buffer of count bytes
```
ホストオブジェクトに関する追加情報については、[XS in C](../xs/XS%20in%20C.md)のドキュメントを参照してください。

#### `config`
`config` プロパティは `mc/config` モジュールのデフォルトエクスポートへのショートカットです。これにはいくつかのテストのための設定プロパティが含まれています（詳細は以下）。これは、モジュールとしてではなくプログラムとして実行されるテストスクリプトにとっても便利です。

```js
let ssid = $TESTMC.config.ssid;
```

#### `wifiInvalidConnectionTimeout`, `wifiConnectionTimeout`, `wifiScanTimeout`
これらの定数はWi-Fiテストのタイムアウトを設定するために使用されます。ターゲットデバイスやネットワーク条件（信号強度など）によって最適な値が異なるため、`$TESTMC` に値として提供されています。

```js
$TESTMC.timeout($TESTMC.wifiScanTimeout, "wi-fi scan timeout");
```

#### `Behavior`
`$TESTMC.Behavior` コンストラクタは、テスト用のPiu `Behavior` クラスを拡張します。各ハンドラーを `try`/`catch` ブロックでラップします。ラッパーが例外をキャッチした場合、テストは失敗として終了します。これはPiuイベントのテストを書く際に便利です。

```js
class SampleBehavior extends $TESTMC.Behavior {
	onTouchEnded(content, id, x, y, ticks) {
		assert.sameValue(x, 10);
		assert.sameValue(y, 20);
	}
}
```

### `$NETWORK` グローバル
`$NETWORK` グローバルは、ネットワークを使用する操作のテストに役立つ関数や値を提供します。

#### `connected`
`connected` プロパティは、ネットワーク接続が利用可能になったときに解決されるPromiseを返します。ネットワークがすでに接続されている場合（シミュレーターでテストを実行している場合など）、Promiseはすぐに解決されます。

Promiseの解決は非同期であるため、`connected` プロパティは `async` とマークされたテストでのみ使用するべきです。

```js
await $NETWORK.connected;
```

#### `invalidDomain`
`invalidDomain` プロパティは、解決に失敗することが保証されているDNSホスト名を含む文字列です。これはネットワークコードのエラーハンドリングをテストするのに役立ちます。

#### `resolve(hostname)`
The `resolve` function returns a promise which resolves to the IP address corresponding to the DNS hostname provided.

```js
const address = await $NETWORK.resolve("www.example.com");
```

#### `wifi()`
`wifi` 関数は、テスト用のWi-Fi接続を確立するオプションオブジェクトを解決するPromiseを返します。

```js
let options = await $NETWORK.wifi();
new WiFi(options);
```

ほとんどのネットワークテストでは `$NETWORK.connected` を使用して接続を確立できます。`$NETWORK.wifi` 関数はWi-Fi接続を確立するために使用されるAPIをテストするために提供されています。

### `screen` グローバル
`screen` グローバルは、ディスプレイを持つデバイスのModdable SDKランタイム環境の標準的な部分です。これはPocoとPiuによってディスプレイドライバにアクセスするために使用されます。`testmc` テストアプリは、デバイスの `screen` グローバルをラップして追加の機能を提供します。

##### `checksum`
`checksum` プロパティは、画面に描画された最後の更新のハッシュです。値は32文字の16進数文字列です。

```js
render.begin();
	render.fillRectangle(black, 0, 0, render.width, render.height);
	render.blendRectangle(white, 32, 10, 10, 20, 20);
render.end();

assert.sameValue("e5386bfa56b0ab7128c75199547c2178", screen.checksum, "mismatch");
```

チェックサムは、最後の更新によってディスプレイドライバに送信されたピクセルのものです。すべてのピクセルが単一の更新で描画された場合にのみ、画面全体のチェックサムになります。ピクセルフォーマットやソフトウェアの回転が変更されると、チェックサムも変更されます。

#### `checkImage(checksum)` {/*examples*/}
`checkImage` メソッドは `screen.checksum` を使用して、Piuによるレンダリングの正確性を検証します。チェックサムが一致しない場合、テストは失敗として終了します。

```js
one.height = 50;
one.width = 50;
screen.checkImage("f1dd85eb20df5f900845e1ae8b79aa25");

one.height = 150;
one.width = 150;
screen.checkImage("4e008cd76aa8c80e480d966a8aa91228");
```

#### `doTouchBegan()`, `doTouchMoved()`, `doTouchEnded()` {/*examples*/}
これらの関数は、タッチスクリーンドライバによって生成されたかのようにタッチ入力イベントを送信します。各関数への引数は `id, x, y, ticks` です。これらの `async` 関数は、タッチイベントが配信された後に解決されます。

```js
await screen.doTouchBegan(0, 100, 100, Time.ticks);
```

#### `mc/config` でのテスト設定
testmcマニフェストには、ハードウェアピン番号やI/Oポートなど、デバイス上でのテストに使用される設定値が含まれています。これらの値はテストに使用され、テストがテスト対象のデバイスに依存しないように記述されます。新しいデバイスでtestmcを実行する場合、デバイスの設定値をマニフェストに追加する必要があります。

- `config.digital[]`

ECMA-419デジタルピン指定子の配列です。

- `config.i2c`

デフォルトのI²Cバスに接続されたデバイスの`hz`と`address`のプロパティを持つオブジェクトです。また、デバイスが接続されていないことが保証されている`unusedAddress`も含まれています。

- `config.pwm`

PWMテストのためのプロパティを持つオブジェクトです。`from`は、コンストラクターオプションオブジェクトの`from`プロパティをホスト実装がサポートしている場合に`true`です。`pins`はPWMに使用可能なECMA-419ピン指定の配列です。`ports`はPWMピンに使用可能なECMA-419ポート指定の配列です。`invalidPorts`は無効なECMA-419ポート指定の配列です。`resolutions`はサポートされているPWM解像度の配列です。`hzs`はサポートされているPWM周波数の配列です。

- `config.spi`

SPIテストのためのプロパティを持つオブジェクトです。`select`は選択ピンのECMA-419ピン指定です。`ports`はSPIのためのECMA-419ポート指定の配列です。`invalidPorts`は無効なSPIポート指定の配列です。`hzs`はサポートされているSPI周波数の配列です。

- `config.invalidPins`

ECMA-419ピン指定の配列で、ターゲットデバイスに対して無効です。

- `config.flashPartition`

テストに使用するフラッシュパーティションの名前です。このパーティションの内容はテストによって破壊されます。
