## RTCのテストアプリ

`setTime` - 現在の時刻に設定します。

`showTime` - RTCが報告する時刻を表示します。

`test` - さまざまな時刻の値を試します。

### 異なるRTCドライバを試すには:

デバイス用のドライバを含めるために、`main.js`と`manifest.json`を変更する必要があります。

| クラス | モジュール指定子 |
| :---: | :--- |
| `DS1307` | `embedded:RTC/DS1307`
| `DS3231` | `embedded:RTC/DS3231`
| `RV3028` | `embedded:RTC/RV3028`
| `PCF8523` | `embedded:RTC/PCF8523`
| `PCF8563` | `embedded:RTC/PCF8563`
| `MCP7940` | `embedded:RTC/MCP7940`

`main.js`で、デバイスに合わせて`import RTC`を変更します。例えば：

```
import RTC from "embedded:RTC/DS1307";
```

`manifest.json`ファイルで、デバイスに合わせて`include`を変更します。例えば：

```
	"include": [
		"$(MODDABLE)/modules/drivers/peripherals/ds1307/manifest.json"
	]
```
