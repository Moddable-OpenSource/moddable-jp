# AudioOut
Copyright 2021-2023 Moddable Tech, Inc.<BR>
改訂： 2023年4月25日

## class AudioOut
`AudioOut`クラスは、4つのストリームミキサーを使用したオーディオ再生を提供します。

```js
import AudioOut from "pins/i2s";
```

### 音を再生する
次の例では、単一のベル音を再生します。オーディオはリソースに保存されています。リソースの形式はModdable Audioで、これはオーディオエンコーディングを含む小さなヘッダー付きの非圧縮オーディオです。

```js
let bell = new Resource("bell.maud");
let audio = new AudioOut({sampleRate: 11025, bitsPerSample: 16, numChannels: 1, streams: 1});
audio.enqueue(0, AudioOut.Samples, bell);
audio.start();
```

コンストラクタは、出力の`sampleRate`、`bitsPerSample`、および`numChannels`を設定します。サポートされる構成はホストハードウェアに依存します。コンストラクタはまた、このインスタンスがサポートする並列ストリームの最大数を設定します。この例では1つです。

### 音を繰り返し再生する
次の例では、`enqueue`関数のオプションの`repeat`パラメータに4を渡すことで、同じベル音を4回連続で再生します。

```js
let audio = new AudioOut({sampleRate: 11025, bitsPerSample: 16, numChannels: 1, streams: 1});
audio.enqueue(0, AudioOut.Samples, bell, 4);
audio.start();
```

サウンドを無限に繰り返すには、repeatパラメータを `Infinity` に設定します：

```js
audio.enqueue(0, AudioOut.Samples, bell, Infinity);
```

### コールバックを受け取る
バッファの再生が完了した後にコールバックを受け取るには、`Callback` コマンドをエンキューします：

```js
audio.enqueue(0, AudioOut.Samples, bell, 2);
audio.enqueue(0, AudioOut.Callback, 5);
audio.start();
audio.callback = id => trace(`audio callback id ${id}\n`);
```

この例では、ベルの音が2回再生された後にコンソールに値 `5` をトレースします。

### サウンドの一部を再生する
サウンドの一部を再生するには、オプションの `start` および `count` パラメータを `enqueue` 関数に渡します。startとcountパラメータはバイトではなくサンプル数を単位とします。次の例では、ベルの音の2秒目を1回再生し、その後に1秒目を2回再生します。

```js
let audio = new AudioOut({sampleRate: 11025, bitsPerSample: 16, numChannels: 1, streams: 1});
audio.enqueue(0, AudioOut.Samples, bell, 1, 11025, 11025);
audio.enqueue(0, AudioOut.Samples, bell, 2, 0, 11025);
audio.start();
```

### サウンドを並列して再生する
サウンドを並列して再生するには、別々のストリームにエンキューします。次のサンプルでは、ストリーム0でベルの音を1回再生し、ストリーム1でビープ音を4回並行して再生します。

```js
let audio = new AudioOut({sampleRate: 11025, bitsPerSample: 16, numChannels: 1, streams: 2});
audio.enqueue(0, AudioOut.Samples, bell);
audio.enqueue(1, AudioOut.Samples, beep, 4);
audio.start();
```

### エンキューされたオーディオをフラッシュする
特定のストリームにキューされたオーディオは、`Flush` コマンドを使用して `enqueue` を呼び出すことでフラッシュできます。次の例では、ストリーム1をフラッシュします。

```js
audio.enqueue(1, AudioOut.Flush);
```

### 再生を停止する
すべてのストリームの再生を停止するには、`stop` を呼び出します。

```js
audio.stop();
```

`stop` 関数はエンキューされたサンプルをフラッシュしません。`start` を呼び出すと、オーディオが停止した場所から再生が再開されます。

### シンプルなアタック-サステイン-ディケイ
オーディオサンプルは通常、最初のアタック、次に無限にループできるサステインセクション、最後にサンプルの終わりのディケイセクションの3つのセクションで構成されます。`AudioOut` インスタンスを使用して、再生開始時に長さが不明なこれらの種類のオーディオサンプルを再生できます。例えば、ユーザーがボタンを押している間の時間だけサンプルを再生する場合です。

これは、次のサンプルがキューに入るまで、サンプルを無限回数繰り返す機能を使用して行います。サンプルの再生を開始するには、アタックセクションとサステインセクションの両方をエンキューします。

```js
audio.enqueue(0, AudioOut.Samples, aSample, 1, aSample.attackStart, aSample.attackCount);
audio.enqueue(0, AudioOut.Samples, aSample, Infinity, aSample.sustainStart, aSample.sustainCount);
```

サンプルの再生を終了する時が来たら、減衰セクションをキューに入れます。これにより、現在の繰り返しが完了すると、キューに入れたサステインセクションが終了します。

```js
audio.enqueue(0, AudioOut.Samples, aSample, 1, aSample.decayStart, aSample.decayCount);
```

### constructor(dictionary)
コンストラクタは、オーディオ出力を構成するための辞書型引数を受け取ります。

```js
let audio = new AudioOut({sampleRate: 11025, bitsPerSample: 16, numChannels: 2, streams: 3});
```
以下のプロパティが定義されています。

- **sampleRate** -- 再生するオーディオの1秒あたりのサンプル数。8000から48000までのサンプルレートがサポートされていますが、特定のデバイスでは制限される場合があります。
- **bitsPersample** -- サンプルごとのビット数。8または16のいずれかです。
- **numChannels** -- 各オーディオサンプルのチャンネル数。モノラルの場合は1、ステレオの場合は2です。
- **streams** -- このオーディオ出力インスタンスがサポートする同時ストリームの最大数。有効な値は1から4までで、デフォルト値は1です。

特定の`AudioOut`インスタンスで再生されるすべてのオーディオバッファは、そのコンストラクタで初期化されたサンプルレート、サンプルごとのビット数、およびチャンネル数と同じでなければなりません。

オーディオ出力がインスタンス化されると、停止状態になります。オーディオ再生を開始するには `start` を呼び出す必要があります。

### close()
`close` 関数を呼び出して、オーディオ出力に関連するすべてのリソースを解放します。`close` が呼び出された時点でオーディオが再生中の場合、それは直ちに停止されます。

```js
audio.close();
```

### start()
`start` 関数を呼び出してオーディオ再生を開始します。再生のためにオーディオがキューに入っていない場合、無音が再生されます。

```js
audio.start();
```

### stop()
`stop` 関数を呼び出して、オーディオ再生を直ちに中断します。

```js
audio.stop();
```

`stop` を呼び出しても、任意のストリームで再生待ちのオーディオの状態には影響しません。`stop` の後に `start` を呼び出すと、中断されたポイントから再生が再開されます。

### enqueue(stream, command, buffer, repeat, offset, count)
enqueue関数には、再生待ちのオーディオに関連するいくつかの異なる機能があります：

- ストリームで再生するためのオーディオサンプルをキューに入れる
- ストリームの再生の特定のポイントでコールバックをキューに入れる
- ストリームで再生待ちのオーディオをフラッシュする


`enqueue` 関数のすべての呼び出しには、最初のパラメータとしてターゲットストリーム番号が含まれます。この値は、コンストラクタを使用して設定されたストリーム数より1少ない値から0までの範囲です。

`length` 関数は、ストリームの未使用キューエントリの数を返します。

#### オーディオサンプルのエンキュー

再生するオーディオサンプルは、MAUDオーディオリソースとして提供するか、生のオーディオサンプルとして提供することができます。いずれの場合も、サンプルはオーディオ出力と同じ形式でなければなりません（例：同じサンプルレート、サンプルあたりのビット数、チャンネル数）。

Moddable Audioヘッダー (MAUD) を使用してオーディオサンプルを `enqueue` するには、サンプルのバッファを使用してenqueueを呼び出します：

```js
audio.enqueue(0, AudioOut.Samples, buffer);
```

ヘッダーのない非圧縮オーディオサンプルのバッファを `enqueue` するには、サンプルのバッファを使用してenqueueを呼び出します：

```js
audio.enqueue(0, AudioOut.RawSamples, buffer);
```

バッファを複数回再生するには、オプションの `repeat` カウントを指定します。リピートカウントは正の整数または `Infinity` です。

```js
audio.enqueue(0, AudioOut.Samples, bufferOne, 4);
audio.enqueue(0, AudioOut.Samples, bufferTwo, Infinity);
```

リピート回数が `Infinity` の場合、バッファはオーディオアウトインスタンスが閉じられるまで、ストリーミングがフラッシュされるまで、または別のオーディオバッファがキューに追加されるまで繰り返されます。最後の場合、現在再生中のバッファは完了まで再生され、その後に次のバッファが再生されます。

バッファ内のサンプルのサブセットを再生するには、オプションの `offset` および `count` パラメータを使用します。両方のパラメータはバイトではなくサンプル単位です。`offset` パラメータは、バッファの再生を開始するサンプル数を示します。`count` パラメータが提供されていない場合、再生はバッファの終わりまで続きます。`count` パラメータが提供されている場合、指定されたサンプル数だけが再生されます。

#### トーンと無音のキューイング

トーンを `enqueue` するには、周波数とサンプル数を指定します。方形波が生成されます。次の例では、440 HzのAナチュラルの8000サンプルをキューに追加します。サンプル数に `Infinity` を渡すと、`flush`、`stop`、または `close` までトーンが再生されます。

```js
audio.enqueue(0, AudioOut.Tone, 440, 8000);
```
無音を `enqueue` するには、サンプル数を指定します。無音をキューに追加することは、オーディオバッファ間に正確なギャップを追加するのに役立ちます。次の例では、11025サンプルの無音をキューに追加します。

```js
audio.enqueue(0, AudioOut.Silence, 8000);
```

#### コールバックのエンキュー
ストリームの特定のポイントでコールバックをスケジュールするには、バッファ引数に整数値を指定してenqueueを呼び出します。コールバックの前のバッファの再生が完了すると、インスタンスの`callback`関数が呼び出されます。

```js
audio.enqueue(0, AudioOut.Samples, bufferOne);
audio.enqueue(0, AudioOut.Callback, 1);
audio.enqueue(0, AudioOut.Samples, bufferTwo);
audio.enqueue(0, AudioOut.Callback, 2);
audio.callback = id => trace(`finished playing buffer ${id}\n`);
```

すべてのストリームに対して呼び出される単一のコールバック関数の代わりに、各ストリームに対して個別のコールバックを`callbacks`プロパティを使用して提供することができます：

```js
audio.callbacks = [
	id => trace(`finished playing buffer ${id} from stream 0\n`),
	id => trace(`finished playing buffer ${id} from stream 1\n`)
];
```

`callback`プロパティと`callbacks`プロパティの両方が設定されている場合、使用されるのは`callbacks`プロパティのみです。

#### オーディオサンプルのデキュー
指定されたストリームにエンキューされたすべてのサンプルとコールバックは、`stream`パラメータのみを使用して`enqueue`を呼び出すことでデキューできます：

```js
audio.enqueue(3, AudioOut.Flush);
```

#### 音量の変更
音量を変更するには、ストリームに`Volume`コマンドをエンキューします。音量コマンドは、既にエンキューされたサンプルの音量を変更せず、`Volume`コマンドの後にエンキューされたサンプルの音量のみを変更します。
```

```js
audio.enqueue(0, AudioOut.Volume, 128);
```

ボリュームコマンドの値は、無音の場合は0、最大音量の場合は256です。

### length(stream)

`length`関数は、指定されたストリームの未使用のキューエントリの数を返します。この情報を使用して、ストリームが現在追加の`enqueue`呼び出しを受け入れることができるかどうかを判断できます。

```js
if (audio.length(0) >= 2) {
	audio.enqueue(0, AudioOut.Tone, 440, 1000);
	audio.enqueue(0, AudioOut.Tone, 330, 1000);
}
```

### Properties

audioOutインスタンスのすべてのプロパティは読み取り専用です。

#### sampleRate

インスタンスのサンプルレートを数値で示します。

#### bitsPerSample

インスタンスのサンプルあたりのビット数を数値で示します。

#### channelCount

インスタンスが出力するチャンネル数を数値で示します。

#### streams

インスタンスがサポートする同時ストリームの最大数を数値で示します。

## class Mixer

`Mixer`クラスは、`AudioOut`で使用される4チャンネルミキサーおよびオーディオデコンプレッサーへのアクセスを提供します。これは、ネットワークストリーミングなど、他の目的のためにオーディオを処理するのに役立ちます。

```js
import {Mixer} from "pins/i2s";
```


ミキサーは、`enqueue` を含む `AudioOut` と同じAPI基盤を持っています。ミキサーは `start` や `stop` メソッドを実装していませんが、代わりにキューに入れられたサンプルを引き出すための `mix` 関数を提供します。

### mix(sampleCount)
### mix(buffer)
`mix` 関数は2つの方法で呼び出すことができます。まず、ミックスするサンプルの数を整数で渡すと、サンプルを含むホストバッファを返します。次に、バッファ（`ArrayBuffer`、`SharedArrayBuffer`、`Uint8Array`、`Int8Array`、`DataView`）を渡すと、提供されたバッファに直接サンプルをミックスします。

#### 新しいバッファにトーンを出力
次のコードは、440 Hzのトーンを600サンプル新しいバッファにミックスします。

```js
const mixer = new Mixer({streams: 1, sampleRate: 12000, numChannels: 1});
mixer.enqueue(0, Mixer.Tone, 440);
const samples = mixer.mix(600);
```

#### 既存のバッファにトーンを出力
次のコードは、440 Hzのトーンを600サンプル既存のバッファにミックスします。

```js
const samples = new ArrayBuffer(600 * 2);
const mixer = new Mixer({streams: 1, sampleRate: 12000, numChannels: 1});
mixer.enqueue(0, Mixer.Tone, 440);
mixer.mix(samples);
```

## MAUDフォーマット
`maud` フォーマット、「Moddable Audio」は、コンパクトで簡単に解析できるシンプルなオーディオフォーマットです。`AudioOut` クラスの `enqueue` 関数は、`maud` フォーマットのサンプルを受け入れます。Moddable SDKの `wav2maud` ツールは、WAVファイルを `maud` リソースに変換します。

フォーマットには12バイトのヘッダーがあり、その直後にオーディオサンプルが続きます。

- オフセット0 -- ASCII 'm'
- オフセット1 -- ASCII 'a'
- オフセット2 -- バージョン番号 (1)
- オフセット3 -- サンプルあたりのビット数 (8または16)
- オフセット4 -- サンプルレート (8000から48000の範囲内）。2バイト、符号なし、リトルエンディアン
- オフセット6 -- チャンネル数 (1または2)
- オフセット7 -- サンプルフォーマット (0は非圧縮、1はIMA ADPCM、2はSBC)
- オフセット8 -- サンプル数。4バイト、符号なしリトルエンディアン

オーディオサンプルはヘッダーの直後に続きます。チャンネルが2つある場合、チャンネルはインターリーブされます。16ビットサンプルは符号付きリトルエンディアン値です。8ビットサンプルは符号付き値です。

IMA ADPCMは、1992年10月21日の「Recommended Practices for Enhancing Digital Audio Compatibility in Multimedia Systems」リビジョン3.00に記載されたアルゴリズムに基づいています。オーディオ圧縮は16ビットサンプルで約4:1です。単一チャンネルのオーディオのみがサポートされています。各圧縮チャンクには129サンプルが含まれ、68バイトを使用します。チャンクは再生中にオンデマンドで1つずつ解凍され、メモリ内バッファを最小限に抑えます。

## Manifest defines
`audioOut`モジュールはビルド時に設定されます。

### すべてのターゲットの定義
- `MODDEF_AUDIOOUT_STREAMS` -- 同時に再生できる最大オーディオストリーム数。最大は4で、デフォルトも4です。
- `MODDEF_AUDIOOUT_BITSPERSAMPLE` -- サンプルあたりのビット数。macOSは8と16をサポートします。ESP32とESP8266は16のみをサポートします。デフォルトは16です。
- `MODDEF_AUDIOOUT_QUEUELENGTH` -- 単一のストリームにキューイングできるアイテムの最大数。デフォルトは8です。

### ESP32の定義
- `MODDEF_AUDIOOUT_I2S_NUM` -- ESP32のI2Sユニット番号。デフォルトは0です。
- `MODDEF_AUDIOOUT_I2S_BCK_PIN` -- I2Sクロックピン。デフォルトは26です。
- `MODDEF_AUDIOOUT_I2S_LR_PIN` -- I2S LRピン。デフォルトは25です。
- `MODDEF_AUDIOOUT_I2S_DATAOUT_PIN` -- I2Sデータピン。デフォルトは22です。
- `MODDEF_AUDIOOUT_I2S_BITSPERSAMPLE` - I2Sで送信するサンプルあたりのビット数。16または32。デフォルトは16です。
- `MODDEF_AUDIOOUT_I2S_DAC` - 内蔵のデジタル-アナログ変換（DAC）出力を有効にします。DACを有効にするには1に設定します。デフォルトは0です。
- `MODDEF_AUDIOOUT_I2S_DAC_CHANNEL` - DAC出力を制御します - 左（1）、右（2）、または両方（3）。デフォルトは両方です。
- `MODDEF_AUDIOOUT_I2S_PDM` - 内蔵のPDMエンコーダを有効にします。PDMを有効にするには1に設定します。デフォルトは0です。
- `MODDEF_AUDIOOUT_I2S_MIXERBYTES` - ミキシングバッファに割り当てるバイト数。この値はオーディオドライバの2つのDMAバッファのサイズにも使用されます。オーディオストリームが1つだけの場合でも、ミキシングバッファがあります。8ビットオーディオの場合はデフォルトで512バイト、それ以外の場合は768バイトです。小さい値はメモリ使用量を減らしますが、オーディオ処理のオーバーヘッドがわずかに増加します。システム全体でCPU負荷が高い場合、小さいバッファはグリッチが発生しやすくなります。

### ESP8266 の定義
- `MODDEF_AUDIOOUT_I2S_PDM` -- 0の場合、PCMサンプルはI2S経由で送信されます。0以外の場合、サンプルはPDMを使用して送信されます。オーバーサンプリングなしの場合は32、2倍のオーバーサンプリングの場合は64、4倍のオーバーサンプリングの場合は128に設定します。デフォルトは0です。
