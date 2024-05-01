# Moddable SDK – ドキュメント概要
Copyright 2019-2023 Moddable Tech, Inc.<BR>
改訂： 2023年4月6日

このディレクトリにはModdable SDKのドキュメントが含まれています。

### 入門

[入門ガイド](./Moddable%20SDK%20-%20Getting%20Started.md)では、macOS、Linux、Windows用のModdable SDKのインストール、設定、ビルドの手順をステップバイステップで提供しています。また、時間の経過と共にModdable SDKツールとビルド環境を最新の状態に保つための指示も提供しています。

Moddable SDKがサポートする特定のマイクロコントローラーを扱うガイドは、[devices](./devices)ディレクトリにあります。これにはModdable One、Moddable Two、Moddable Three、Moddable Fourなどの特定の開発ボードの詳細へのリンクが含まれています。

- [ESP32](./devices/esp32.md) by Espressif
- [ESP8266](./devices/esp8266.md) by Espressif
- [Gecko](./devices/gecko/GeckoBuild.md) by Silicon Labs
- [QCA4020](./devices/qca4020/README.md) by Qualcomm
- [Pico](./devices/pico.md) by Raspberry Pi
- [nRF52](./devices/nrf52.md) by Nordic Semiconductor

### モジュールのAPIドキュメント

Moddable SDKのモジュールに対するJavaScript APIは、以下のファイルでドキュメント化されています：

- [**Base**](./base/base.md): time、timer、debug、instrumentation、UUIDを含む基本的なランタイム機能
  - [**Setup**](./base/setup.md): 他のモジュールが実行される前にホストを設定するための`setup`モジュールの使用
  - [**Worker**](./base/worker.md): Web WorkersとShared Workersの使用
- [**Commodetto**](./commodetto/commodetto.md): BMP、JPEG、PNG画像およびBMFontファイルの解析とレンダリング、ビットマップ操作のクラス、ピクセル形式の変換を含むビットマップグラフィックライブラリ
  - [**Poco**](./commodetto/poco.md): PocoレンダラーのJavaScriptおよびC APIの使用例とリファレンス
- [**Crypt**](./crypt/crypt.md): 暗号プリミティブ
- [**Data**](./data/data.md): Base64およびhexエンコーディングとデコーディング
 - [**Files**](./files/files.md): ファイル、フラッシュ、設定、リソース、ZIPを含むストレージ機能
- [**Network**](./network/network.md): ソケットとソケットに基づくプロトコル（HTTP、WebSockets、MQTT、mDMS、DNS、SNTP、telnet、ping）およびWi-Fi API
  - [**Secure socket**](./network/securesocket.md): TLS/SSLの使用と証明書の管理
  - [**BLE**](./network/ble/ble.md): Bluetooth LEクライアントおよびサーバーの作成、サンプルへのガイドを含む
- [**Pins**](./pins/pins.md): ハードウェアプロトコル（デジタル（GPIO）、アナログ、PWM、I2C、SMBus、サーボ）
  - [**Audio out**](./pins/audioout.md): オーディオの再生とプロジェクトへのオーディオデータの追加
- [**Piu**](./piu/piu.md): ユーザーインターフェースフレームワーク
  - [**Die cut**](./piu/die-cut.md): Piuで複雑なクリッピング形状を使用する
  - [**Expanding keyboard**](./piu/expanding-keyboard.md): Piuプロジェクトにアニメーション拡張キーボードを追加する
  - [**Keyboard**](./piu/keyboard.md): Piuプロジェクトにタッチキーボードを追加する
  - [**Localization**](./piu/localization.md): JSONデータを効率的に使用してPiuアプリケーションをローカライズする

### XS JavaScript エンジン

[xs](./xs) ディレクトリには、XS JavaScriptエンジンに関するドキュメントが含まれています。

- [**ROM Colors**](./xs/ROM%20Colors.md): XSリンカーによって適用される最適化で、ROMに格納されたオブジェクトのプロパティのルックアップ速度を向上させます
- [**XS Conformance**](./xs/XS%20Conformance.md): test262言語テストスイートからのXSの詳細なテスト結果
- [**XS Differences**](./xs/XS%20Differences.md): リソースが制限されたターゲットに対してJavaScriptを実装するための設計上の考慮事項についての議論
- [**XS Platforms**](./xs/XS%20Platforms.md): 新しいホストにXSを移植する
- [**XS in C**](./xs/XS%20in%20C.md): CとJavaScriptコードの間の橋渡しをするAPI
- [**XS linker warnings**](./xs/XS%20linker%20warnings.md): ROMに格納するコードを準備する際にXSリンカーが出す警告についての議論
- [**Handle**](./xs/handle.md): JavaScriptクラスのネイティブ実装がメモリ断片化をどのように減少させるか
- [**Mods**](./xs/mods.md): コンパイル済みJavaScriptモジュール（mods）のアーカイブを扱う方法
- [**Preload**](./xs/preload.md): JavaScriptモジュールのメモリ使用量と起動時間を削減する
- [**xsbug**](./xs/xsbug.md): XSのソースレベルデバッガーであるxsbugの使用方法
- [**xst**](./xs/xst.md): `xst`、test262適合性テストを実行するコマンドラインツールの使用方法

### 開発者ツール

[ツール](./tools) ディレクトリには開発者ツールのドキュメントが含まれています。

- [**ツール**](./tools/tools.md): コマンドラインビルドツールには `mcconfig`、`mcrez`、`png2bmp`、`xsc`、`xsl`、デスクトップシミュレーターが含まれます
- [**マニフェスト**](./tools/manifest.md): プロジェクトのビルドを設定するために使用されるJSONマニフェストファイルの説明
- [**定義**](./tools/defines.md): プロジェクトマニフェストを使用してネイティブコードオプションを設定する方法
