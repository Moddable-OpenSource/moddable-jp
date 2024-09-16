# Bosch BME68x ビジュアライザー
更新日： 2024年1月24日

このディレクトリには、Bosch BME68xセンサー用の4つの異なるビジュアライザーが含まれています。このセンサーは、温度、湿度、および気圧を提供します。

## 要件
このプロジェクトはESP32ファミリーの製品で実行することを意図しています。また、Raspberry Pi Picoでも動作します。画面が必要です。ビジュアライザーは直径240ピクセルの円形画面を想定しています。四角形や長方形の画面でも動作しますが、円の外側に描画のノイズが見える場合があります。

プロジェクトはデバイスのデフォルトI2Cバスに接続されたBME68xセンサーを期待しています。接続されていない場合、起動時に例外がスローされます。プロジェクトにはBME68xのデフォルト値が含まれているため、これらの例外でデバッガーの「Go」を押すと、選択されたビジュアライザーが表示されます。シミュレータにはシミュレータ値を調整するためのコントロールが含まれています。

BME68xセンサードライバは[ECMA-419](https://419.ecma-international.org)に準拠しています。これは、ターゲットデバイスがECMA-419 IOサポートを含む必要があることを意味します。議論のために、これらのノートでは`esp32/moddable_two_io`ターゲットを使用します。

## ビルド
このプロジェクトは、最初にインストールされるホストで構成されています。これには、JavaScriptエンジン、Piuユーザーインターフェースフレームワーク、アウトライングラフィックスレンダリング、ECMA-419 IO、およびBME68xセンサードライバが含まれます。

ビジュアライザーは、ビルドの一部としてフォントを準備するために `fontbm` コマンドラインツールを使用します。Moddable SDKをインストールおよび更新するためにxs-devを使用する場合、`fontbm` をインストールするには次のコマンドを使用します：

```
xs-dev setup --tool fontbm
```

それ以外の場合は、"[Moddable SDKを使用してアプリケーション用のフォントを作成する](https://www.moddable.com/documentation/commodetto/Creating%20fonts%20for%20Moddable%20applications)"の「
fontbmの使い方」を参照してください。

### シミュレーター

シミュレーターにホストをビルドしてインストールするには：

```
cd $MODDABLE/examples/piu/round-bme68x
mcconfig -d -m
```

**重要**: ホストをインストールした後、シミュレーターを実行したままにしてください。これは次のステップで必要です。

インストールが完了すると、ホストはmodの準備ができたことを示すメッセージを表示します。ビジュアライザーはmodです。ビジュアライザーをインストールして実行するには、次のコマンドのいずれかを使用します：

```
mcrun -d -m ./mods/plain/manifest.json
mcrun -d -m ./mods/bankers/manifest.json
mcrun -d -m ./mods/gauges/manifest.json
mcrun -d -m ./mods/histograms/manifest.json
```

### デバイス
デバイスにホストをビルドしてインストールするには、以下のコマンドラインを使用します。これはModdable Twoデバイスを使用しています。別のデバイスを使用している場合は、`esp32/moddable_two_io`をそのデバイスのプラットフォームターゲットに置き換えてください。

```
cd $MODDABLE/examples/piu/round-bme68x
mcconfig -d -m -p esp32/moddable_two_io
```

インストールが完了すると、ホストはmodの準備ができたことを示すメッセージを表示します。ビジュアライザーはmodです。ビジュアライザーをインストールして実行するには、以下のコマンドのいずれかを使用します：

```
mcrun -d -m -p esp32/moddable_two_io ./mods/plain/manifest.json
mcrun -d -m -p esp32/moddable_two_io ./mods/bankers/manifest.json
mcrun -d -m -p esp32/moddable_two_io ./mods/gauges/manifest.json
mcrun -d -m -p esp32/moddable_two_io ./mods/histograms/manifest.json
```

ESP32の「フラッシュ」ボタンを押して、ビジュアライザーの外観を変更します。

> **Note**: 一度modがインストールされると、別のmodをインストールすることでそれを置き換えることができます。modをインストールするたびにホストを再インストールする必要はありません。

