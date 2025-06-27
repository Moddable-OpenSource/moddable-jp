# XS プラットフォーム
Copyright 2016-2024 Moddable Tech, Inc.<BR>
更新日: 2024年4月22日

## 歴史

プラットフォームは、ハードウェアとシステムソフトウェアの組み合わせです。各プラットフォームに対して、XSはインターフェースファイル`xsPlatform.h`と実装ファイル`xsPlatform.c`を必要とします。

歴史的に、XSは1つのインターフェースファイル`xsPlatform.h`を使用し、実装を`xsPlatform.c`と`xsHost.c`の2つのファイルに分割していました。多くのプラットフォームは、KinomaJSプラットフォーム抽象化か、コマンドラインツール用のアドホックプラットフォーム抽象化のいずれかに基づいて、同じインターフェースと実装ファイルを共有していました。

さらに、XSマシンには、モジュールとプログラムを見つけてロードする多くの方法がありました：JSファイルから、付属のDLLまたはSOファイルありまたはなしのスタンドアロンでコンパイルされたXSBファイルから、付属のDLLまたはSOファイルを持つリンクされたXSAファイルから...XSプラットフォームは、そのようなオプションを提供する責任がありました。

マイクロコントローラでの作業を開始したとき、XSプラットフォームの主なインスピレーションは、最も複雑なバージョンであったコマンドラインツール用のアドホックプラットフォーム抽象化でした。

今日、XSランタイムは、特にマイクロコントローラ上で大幅に合理化されています。XSマシンは常に、XSリンカーによって準備された読み取り専用マシンからクローンされます。XSコンパイラによってバイトコード化されたモジュールのみが存在します。モジュールは、プリロードされるか、実行時にロードおよびアンロードされるように準備されています。

したがって、XSプラットフォームを構築することは現在はるかに簡単になりました。このドキュメントでは、必要なインターフェースと実装ファイルについて説明します。

## xsPlatform.h

### 基本型

XSは、インターフェースファイルで定義する必要があるいくつかの基本型を使用します。

```c
#include <stdint.h>
typedef int8_t txS1;
typedef uint8_t txU1;
typedef int16_t txS2;
typedef uint16_t txU2;
typedef int32_t txS4;
typedef uint32_t txU4;
```

### C定義とインクルード

XSは主にC標準ライブラリの定数と関数に依存し、`C_`または`c_`プレフィックスを持つマクロを介してアクセスします：

```c
#include <math.h>
#define C_NAN NAN
//...

#include <stdlib.h>
#define c_free free
#define c_malloc malloc
//...
```

これらの定義と対応するインクルードは、インターフェースファイルの最も重要な部分です。マクロにより、プラットフォームが独自の定数と関数を提供できます。定義すべきマクロのリストについては、提供されている任意の`xsPlatform.h`を参照してください。

### ESPマクロ

Espressifのマイクロコントローラで最も顕著に使用されるXtensa命令セットとアーキテクチャでは、ROMの特定の定数データを配置し、そのデータを読み取るための特別なマクロが必要です。他のプラットフォームでは、これらのマクロは単純に定義されます：

```c
#define c_read8(POINTER) *((txU1 *)(POINTER))
#define c_read16(POINTER) *((txU2 *)(POINTER))
#define c_read32(POINTER) *((txU4 *)(POINTER))

#define ICACHE_FLASH_ATTR
#define ICACHE_RODATA_ATTR
#define ICACHE_XS6RO_ATTR
#define ICACHE_XS6RO2_ATTR
#define ICACHE_XS6STRING_ATTR
#define mxGetKeySlotID(SLOT) (SLOT)->ID
#define mxGetKeySlotKind(SLOT) (SLOT)->kind
```


###  `mxMachinePlatform`

プラットフォームは、`mxMachinePlatform`マクロを定義することで、マシンレコードにフィールドを追加できます。マシンはXSが呼び出すすべての関数に渡される（ユビキタスな`the`として）ため、プラットフォームがアプリケーションコンテキスト以外に独自のコンテキストを持つ便利な方法です。

例えば、Macでは、`mxMachinePlatform`マクロは**xsbug**との通信用のソケットとランループソース、およびプロミス用の別のランループソースへの参照を追加します。

```c
#include <CoreServices/CoreServices.h>

#define mxMachinePlatfom \
	CFSocketRef connection; \
	CFRunLoopSourceRef connectionSource; \
	CFRunLoopSourceRef promiseSource;
```

Windowsでは、`mxMachinePlatform`マクロは同じ目的で使用されるソケットとメッセージウィンドウハンドルを追加します。

```c
#include <winsock2.h>

#define mxMachinePlatfom \
	SOCKET connection; \
	HWND window;
```

## xsPlatform.c

実装ファイルは最初に`xsAll.h`をインクルードします。これにはすべてのXSマクロと型の定義、およびすべてのXS extern関数の宣言が含まれています。その後、プラットフォームは以下で説明する関数を実装する必要があります。

XSマシンはマルチスレッドをサポートしていませんが、プラットフォームはそれぞれ独自のXSマシンを持つマルチスレッドをサポートできます。ここで説明するすべての呼び出しとコールバックは、マシンを作成またはクローンしたスレッドで実行される必要があります。

関数は意味のあるセクションにグループ化されています。xsPlatform.cファイルは、プラットフォームに不足しているPOSIX関数も提供できます。

--

- `void fxCreateMachinePlatform(txMachine* the)`

`fxCreateMachinePlatform`は、XSマシンの作成とクローン時に呼び出されます。プラットフォームは、その`mxMachinePlatform`マクロで定義されたフィールドを初期化します。デフォルトでは、すべてのフィールドはゼロです。

--

- `void fxDeleteMachinePlatform(txMachine* the)`

`fxDeleteMachinePlatform`は、XSマシンの削除時に呼び出されます。プラットフォームは、その`mxMachinePlatform`マクロで定義された適切なフィールドをここで破棄または解放する必要があります。

--

### デバッグ

このセクションの関数は、XSのデバッグバージョンでのみ必要です。これらは条件付きで定義できます：

```c
#ifdef mxDebug
// debug functions
#endif
```

プラットフォームが**xsbug**との通信をサポートしていない場合、このセクションの関数は空にできますが、`fxIsConnected`と`fxIsReadable`は`0`を返す必要があります。

**xsbug**とXSマシン間の通信は、TCP/IPまたはシリアル接続のいずれかで行うことができます。TCP/IP接続の場合、**xsbug**がサーバーでXSマシンがクライアントです。シリアル接続を使用する場合、**xsbug**はTCP/IPでの通信を継続し、コンピュータ上で動作するブリッジがシリアル接続とTCP接続間でデータを中継します。ESP8266の場合、この中継は**serial2xsbug**によって実行されます。

プラットフォームは、XSマシンがバイトコードを実行中、つまりプラットフォームが`fxRun`関数内にいるときに**xsbug**からメッセージを受信できるように`fxIsReadable`を実装する必要があります。ほとんどの場合、プラットフォームは`fxRun`関数の外にいます。そのため、システムイベントと`fxDebugCommand`を使用して**xsbug**からのメッセージについてXSに通知します。

例えば、Macでは、プラットフォームは`kCFSocketReadCallBack`で`CFSocketCreate`を使用します：

```c
void fxReadableCallback(CFSocketRef socketRef, CFSocketCallBackType cbType, CFDataRef addr, const void* data, void* context)
{
	txMachine* the = context;
	if (fxIsReadable(the))
		fxDebugCommand(the);
}
```

Windowsでは、プラットフォームは`WM_XSBUG`メッセージで`WSAAsyncSelect`を使用します：

```c
LRESULT CALLBACK fxMessageWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)	{
#ifdef mxDebug
	case WM_XSBUG: {
		txMachine* the = (txMachine*)GetWindowLongPtr(window, 0);
		if (fxIsReadable(the))
			fxDebugCommand(the);
	} break;
#endif
	default:
		return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}
```

--

- `void fxConnect(txMachine* the)`

XSは、`the`マシンを**xsbug**に接続するために`fxConnect`を呼び出します。

TCP/IP接続の場合、プラットフォームはソケットを作成し、**xsbug**に接続します。MacとWindowsでは、**xsbug**のアドレスは通常`localhost`で、他のプラットフォームでは通常環境変数で定義されます。**xsbug**のポートは慣例により`5002`がデフォルトです。

マシンは作成後に**xsbug**に接続されます。つまり、`fxConnect`は`fxCreateMachinePlatform`の後に発生します。

--

- `void fxDisconnect(txMachine* the)`

XSは、`the`マシンを**xsbug**から切断するために`fxDisconnect`を呼び出します。

TCP/IP接続の場合、プラットフォームはソケットを閉じます。

マシンは削除前に切断されます。つまり、`fxDisconnect`は`fxDeleteMachinePlatform`の前に発生します。

--

- `txBoolean fxIsConnected(txMachine* the)`

XSは、`the`マシンが**xsbug**に接続されているかどうかを知るために`fxIsConnected`を呼び出します。

--

- `txBoolean fxIsReadable(txMachine* the)`

XSは、`the`マシンが**xsbug**からメッセージを受信したかどうかを知るために`fxIsReadable`を呼び出します。プラットフォームは、読み取り可能なバイトの有無に応じて1または0を返す必要があります。

XSは`LINE`バイトコードごと（つまり、実行されるJavaScriptソースコードの各行）に`fxIsReadable`を呼び出すため、`fxIsReadable`の実装のパフォーマンスは重要です。

--

- `void fxReceive(txMachine* the)`

XSは、**xsbug**からメッセージを受信するために`fxReceive`を呼び出します。実装は`the->debugBuffer`にバイトを読み込み、受信したバイト数を`the->debugOffset`に設定します。

XSは、メッセージ全体が受信されるまで`fxReceive`を繰り返し呼び出します。`fxReceive`で読み取れるバイトの最大数は`sizeof(the->debugBuffer) - 1`です。

--

- `void fxSend(txMachine* the, txBoolean more)`

XSは、**xsbug**にメッセージを送信するために`fxSend`を呼び出します。実装は`the->echoOffset`から送信するバイト数を取得し、`the->echoBuffer`からバイトを書き込みます。

XSは、メッセージ全体が送信されるまで`fxSend`を繰り返し呼び出します。`more`は、メッセージが不完全な間は`1`、メッセージが完了すると`0`になります。

--

### Eval

標準の`eval`関数、`Function`コンストラクタ、`Generator`コンストラクタは、ソースコードをバイトコードとキーに変換する必要があります。

XSは、このような機能がメモリコストに見合うかどうかをプラットフォームに決定させます。

--

- `txScript* fxParseScript(txMachine* the, void* stream, txGetter getter, txUnsigned flags)`

XSは、ソースコードをXSバイトコードとキーに変換するために`fxParseScript`を呼び出します。`stream`と`getter`引数により、パーサーがソースコードにアクセスできます。`flags`引数は、ソースコードの種類をパーサーに伝えます：`mxModuleCode`、`mxProgramCode`、または`mxEvalCode`。

プラットフォームがこのような機能をサポートする場合、`xsScript.h`をインクルードし、以下のように`fxParseScript`を実装する必要があります：

```c
#include "xsScript.h"

txScript* fxParseScript(txMachine* the, void* stream, txGetter getter, txUnsigned flags)
{
	txParser _parser;
	txParser* parser = &_parser;
	txParserJump jump;
	txScript* script = NULL;
	fxInitializeParser(parser, the, 32*1024, 1993);
	parser->firstJump = &jump;
	if (c_setjmp(jump.jmp_buf) == 0) {
		fxParserTree(parser, stream, getter, flags, NULL);
		fxParserHoist(parser);
		fxParserBind(parser);
		script = fxParserCode(parser);
	}
	fxTerminateParser(parser);
	return script;
}
```

プラットフォームは、`xsScript.c`、`xsLexical.c`、`xsSyntaxical.c`、`xsTree.c`、`xsSourceMap.c`、`xsScope.c`、および`xsCode.c`もコンパイルしてリンクする必要があります。

プラットフォームがこのような機能をサポートしない場合、`fxParseScript`は`NULL`を返す必要があり、上記のCファイルをコンパイルしてリンクする必要はありません。

--

### キー

キーは、XSがプロパティを識別するために使用する名前とシンボルです。

--

- `void fxBuildKeys(txMachine* the)`

`fxBuildKeys` is called only when creating an XS machine, in order to initialize the default keys used by the standard ECMAScript host functions.

On most platforms today, XS machines are cloned. The default keys are available and ready to be used in the read-only machine. So `fxBuildKeys` is never called and can be empty.

If the platform supports the creation of XS machines from scratch, `fxBuildKeys` must be implemented as:

```c
void fxBuildKeys(txMachine* the)
{
	int i;
	for (i = 0; i < XS_SYMBOL_ID_COUNT; i++) {
		txID id = the->keyIndex;
		txSlot* description = fxNewSlot(the);
		fxCopyStringC(the, description, gxIDStrings[i]);
		the->keyArray[id] = description;
		the->keyIndex++;
	}
	for (; i < XS_ID_COUNT; i++) {
		fxID(the, gxIDStrings[i]);
	}
}
```

--

### Memory

XS machines use two heaps: the chunks heap and the slots heap.

Chunks are blocks of variable size that the garbage collector can move to compact memory. XS stores strings, buffers, arrays, etc into chunks. On microcontrollers without a dedicated memory management unit, chunks are also useful to store any kind of data. For instance Piu uses chunks to store its containment hierarchy.

Slots are blocks of fixed size (four times the size of a pointer) that never move. XS maintains a list of free slots, slots are allocated from the list and freed into the list by the garbage collector.

--

- `void* fxAllocateChunks(txMachine* the, txSize size)`

XS calls `fxAllocateChunks` to get a system memory block for chunks. Usually implemented as:

	return c_malloc(size);

XS throws an exception if `fxAllocateChunks` returns NULL.

XS checks if the result of `fxAllocateChunks` is contiguous to `the->firstBlock` so microcontrollers can grow the chunks heap without fragmenting system memory.

--

- `txSlot* fxAllocateSlots(txMachine* the, txSize count)`

XS calls `fxAllocateSlots` to get a system memory block for `count` slots. Usually implemented as:

	return (txSlot*)c_malloc(count * sizeof(txSlot));

XS throws an exception if `fxAllocateSlots ` returns NULL.

--

- `void fxFreeChunks(txMachine* the, void* chunks)`

XS calls `fxFreeChunks` to free the `chunks` system memory block. Usually implemented as:

	c_free(chunks);

--

- `void fxFreeSlots(txMachine* the, void* slots)`

XS calls `fxFreeSlots` to free the `slots` system memory block. Usually implemented as:

	c_free(slots);

--

### Modules

On platforms that support several ways to get modules, the implementation of `fxFindModule` and `fxLoadModule` can be complex. On microcontrollers, where all modules are prepared or preloaded, the implementation of `fxFindModule`and `fxLoadModule` can be simple enough, as demonstrated by the code snippets here under.

--

- `txID fxFindModule(txMachine* the, txID moduleID, txSlot* slot)`

XS calls `fxFindModule` to find an imported or required module.

The `moduleID` argument is the importing or requiring module identifier. It is `XS_NO_ID` when the machine itself requires a module.

The `slot` argument is the imported or required module name. It is the module specifier of the `import` syntactical construct or the module parameter of the `require` host function.

If the module is found, `fxFindModule` returns the module identifier, otherwise `fxFindModule` returns `XS_NO_ID`.

A module identifier is a unique `txID`, but the platform defines the format of its corresponding key: it can be a path, a URL, a URI...

The platform defines also how the importing or requiring module identifier and the imported or required module name are merged. The usual convention is based on absolute (`/*`), relative (`./*`, `../*`) or search (*) paths.

Finding modules can involve looking for various kinds of files, using a set of preferred locations, etc.  But on microcontrollers, all modules modules are prepared and ready to be found:

```c
txID fxFindModule(txMachine* the, txID moduleID, txSlot* slot)
{
	txPreparation* preparation = the->archive;
	char name[PATH_MAX];
	char path[PATH_MAX];
	txBoolean absolute = 0, relative = 0, search = 0;
	txInteger dot = 0;
	txSlot *key;
	txString slash;
	txID id;

	fxToStringBuffer(the, slot, name, sizeof(name));
	if (!c_strncmp(name, "/", 1)) {
		absolute = 1;
	}
	else if (!c_strncmp(name, "./", 2)) {
		dot = 1;
		relative = 1;
	}
	else if (!c_strncmp(name, "../", 3)) {
		dot = 2;
		relative = 1;
	}
	else {
		relative = 1;
		search = 1;
	}
	if (absolute) {
		c_strcpy(path, preparation->base);
		c_strcat(path, name + 1);
		if (fxFindScript(the, path, &id))
			return id;
	}
	if (relative && (moduleID != XS_NO_ID)) {
		key = fxGetKey(the, moduleID);
		c_strcpy(path, key->value.key.string);
		slash = c_strrchr(path, '/');
		if (!slash)
			return XS_NO_ID;
		if (dot == 0)
			slash++;
		else if (dot == 2) {
			*slash = 0;
			slash = c_strrchr(path, '/');
			if (!slash)
				return XS_NO_ID;
		}
		if (!c_strncmp(path, preparation->base, preparation->baseLength)) {
			*slash = 0;
			c_strcat(path, name + dot);
			if (fxFindScript(the, path, &id))
				return id;
		}
	}
	if (search) {
		c_strcpy(path, preparation->base);
		c_strcat(path, name);
		if (fxFindScript(the, path, &id))
			return id;
	}
	return XS_NO_ID;
}

txBoolean fxFindScript(txMachine* the, txString path, txID* id)
{
	txPreparation* preparation = the->archive;
	txInteger c = preparation->scriptCount;
	txScript* script = preparation->scripts;
	path += preparation->baseLength;
	c_strcat(path, ".xsb");
	while (c > 0) {
		if (!c_strcmp(path, script->path)) {
			path -= preparation->baseLength;
			*id = fxNewNameC(the, path);
			return 1;
		}
		c--;
		script++;
	}
	*id = XS_NO_ID;
	return 0;
}
```

--

- `void fxLoadModule(txMachine* the, txID moduleID)`

XS calls `fxLoadModule` to tell the platform to prepare the byte codes, keys and host functions of a module. When ready, the platform  must call `fxResolveModule` with a `txScript` structure that references the byte codes, keys and host functions of the module.

Preparing modules can involve reading and mapping files, parsing, scoping and byte coding scripts, loading dynamic libraries, etc. But on microcontrollers, all `txScript` structures are available and ready to be used:

```c
void fxLoadModule(txMachine* the, txID moduleID)
{
	txString path = fxGetKeyName(the, moduleID);
	txScript* script = fxLoadScript(the, path);
	fxResolveModule(the, moduleID, script, C_NULL, C_NULL);
}

txScript* fxLoadScript(txMachine* the, txString path)
{
	txPreparation* preparation = the->archive;
	txInteger c = preparation->scriptCount;
	txScript* script = preparation->scripts;
	path += preparation->baseLength;
	while (c > 0) {
		if (!c_strcmp(path, script->path))
			return script;
		c--;
		script++;
	}
	return C_NULL;
}
```

--

### Promises

Promises are essentially asynchronous. The `then` method of a `Promise` object takes two arguments: a function to call when the promise is fulfilled and a function to call when the promise is rejected. Both functions have to be called by a **Job**:

> *A Job is an abstract operation that initiates an ECMAScript computation when no other ECMAScript computation is currently in progress.* (ECMAScript® 2015 Language Specification, Section 8.4).

XS takes care queuing and running Jobs but relies on platforms for their scheduling.

--

- `void fxQueuePromiseJobs(txMachine* the)`

XS calls `fxQueuePromiseJobs` once when jobs have been queued. Platforms can use any mechanism to defer a call to `fxRunPromiseJobs`.

For instance on Mac the platform uses a run loop source and `CFRunLoopSourceSignal`:

```c
void fxQueuePromiseJobsCallback(void *info)
{
	txMachine* the = info;
	fxRunPromiseJobs(the);
}

void fxQueuePromiseJobs(txMachine* the)
{
	CFRunLoopSourceSignal(the->promiseSource);
}
```

On Windows the platform uses a message window and `PostMessage`:

```c
LRESULT CALLBACK fxMessageWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)	{
	case WM_PROMISE: {
		txMachine* the = (txMachine*)GetWindowLongPtr(window, 0);
		fxRunPromiseJobs(the);
	} break;
	default:
		return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}

void fxQueuePromiseJobs(txMachine* the)
{
	PostMessage(the->window, WM_PROMISE, 0, 0);
}
```

### SharedArrayBuffer & Atomics

From XS point of view, `SharedArrayBuffer` instances are host objects, i.e. instances with an internal host slot. The data of the host slot is a pointer to the data of a **shared chunk**. The destructor of the host slot is `fxReleaseSharedChunk`.

What is a shared chunk is defined by the platform. XS atomically accesses 8-bit, 16-bit or 32-bit signed or unsigned integers inside the data of a shared chunk. XS accesses integers either thru GCC atomics, or between calls to `fxLockSharedChunk` and `fxUnlockSharedChunk`

Since `Atomics.wait` and `Atomics.wake` require to synchonize the **shared cluster** of machines created or cloned by XS, platforms usually need a global synchronization mechanism, and synchronization related fields in every machine record, thru the `mxMachinePlatform` macro explained here above.

#### Shared Cluster

```c
void fxInitializeSharedCluster();
```

Applications that use `Atomics.wait` and `Atomics.wake` must call `xsInitializeSharedCluster` before creating or cloning their first machine. `xsInitializeSharedCluster` is the application programming interface, `fxInitializeSharedCluster` is the platform implementation.

`fxInitializeSharedCluster` allows the platform to setup its global synchronization mechanism.

The thread that calls `fxInitializeSharedChunks` must be the thread that runs the user interface, usually the main thread. `Atomics.wait` fails for all machines running in that thread.

```c
void fxTerminateSharedCluster();
```

Applications that use `Atomics.wait` and `Atomics.wake` must call `xsTerminateSharedCluster` after deleting their last machine. `xsTerminateSharedCluster` is the application programming interface, `fxTerminateSharedCluster` is the platform implementation.

`fxTerminateSharedCluster` allows the platform to cleanup its global synchronization mechanism.

#### Shared Chunk

```c
void* fxCreateSharedChunk(txInteger byteLength);
```

`fxCreateSharedChunk` allocates a shared chunk, `byteLength` is the size of its data, which must be initialised to zero.

Typically platforms use a reference count to track how many machines are referencing the shared chunk. `fxCreateSharedChunk` must initialise the reference count to one.

`fxCreateSharedChunk` returns a pointer to the data.

```c
void fxLockSharedChunk(void* data);
```

`fxLockSharedChunk` locks the shared chunk, `data` is a pointer to the data of the shared chunk.

`fxLockSharedChunk` is never called if the platform supports GCC atomics.

```c
txInteger fxMeasureSharedChunk(void* data);
```

`fxMeasureSharedChunk` returns the size of the data of the chunk, `data` is a pointer to the data of the shared chunk.

	void fxReleaseSharedChunk(void* data);

Machines call `fxReleaseSharedChunk` when they do not reference the shared chunk anymore, `data` is a pointer to the data of the shared chunk.

Typically platforms use an atomic operation to decrement the reference count of the shared chunk and free the shared chunk when the reference count is zero.

`fxReleaseSharedChunk` is the destructor of the host slot.

```c
void* fxRetainSharedChunk(void* data);
```

A machine calls `fxRetainSharedChunk` when marshalling a shared chunk to another machine. `data` is a pointer to the data of the shared chunk.

Typically platforms use an atomic operation to increment the reference count of the shared chunk.

```c
void fxUnlockSharedChunk(void* data);
```

`fxUnlockSharedChunk` unlocks the shared chunk. `data` is a pointer to the data of the shared chunk.

`fxUnlockSharedChunk` is never called if the platform supports GCC atomics.

	txInteger fxWaitSharedChunk(txMachine* the, void* data, txInteger byteOffset, txInteger value, txNumber timeout);

If the application did not call `fxInitializeSharedCluster` or if the current thread is the thread that called `fxInitializeSharedCluster`, `fxWaitSharedChunk` throws a `TypeError` object.

If the 32-bit signed integer at `byteOffset` in `data` is not equal to `value`, `fxWaitSharedChunk` returns `-1` immediately. Else `fxWaitSharedChunk` suspends the current thread.

If a matching call to `fxWakeSharedChunk` resumed the thread, `fxWaitSharedChunk` returns `1`. A matching call is a call with the same `data` and `byteOffset`.

If `timeout` expired, `fxWaitSharedChunk` returns `0`. `timeout` is a number between `Date.now()` and `C_INFINITY`.

	txInteger fxWakeSharedChunk(txMachine* the, void* data, txInteger byteOffset, txInteger count);

If the application did not call `fxInitializeSharedCluster`, `fxWakeSharedChunk` returns `0`.

`fxWakeSharedChunk` resumes at most `count` threads that have been suspended by a matching call to `fxWaitSharedChunk `. A matching call is a call with the same `data` and `byteOffset`.

`fxWakeSharedChunk` returns the number of threads that resumed.

#### Default Implementations

XS provides four default implementations of shared cluster and chunks:

- For systems with Linux futex and GCC atomics
	- define `mxUseLinuxFutex`
	- define `mxUseGCCAtomics`
	- define `mxUseDefaultSharedChunks`
- For systems with POSIX threads, with or without GCC atomics
	- define `mxUsePOSIXThreads`
	- define `mxUseGCCAtomics` if the tool chain supports GCC atomics
	- define `mxUseDefaultSharedChunks`
- For Windows
	- define `mxUseDefaultSharedChunks`
- For systems with a single thread
	- define `mxUseDefaultSharedChunks`

All default implementations use `c_malloc` and `c_free` to create and delete shared chunks.

On systems with POSIX threads and on Windows, to use the default implementation of shared cluster and chunks, the platform must define the `mxMachinePlatform` macro with at least the following fields:

		#define mxMachinePlatform \
			void* waiterCondition; \
			void* waiterData; \
			txMachine* waiterLink;

Obviously, on systems with a single thread, `Atomics.wait` always fails and `Atomics.wake` always returns zero.

### Strings

XS has several options to control the behavior of strings.

#### Internal representation

By default, XS stores strings in UTF-8 encoding. This is convenient in many ways, but results in some subtle differences with standard JavaScript which assumes UTF-16. In particular, the handling of surrogate pairs is different. For environments where stricter conformance is a priority, the internal encoding can be changed to [CESU-8](https://en.wikipedia.org/wiki/CESU-8), a way of encoding UTF-16 in UTF-8. More precisely, XS uses [Modified UTF-8 from Java](https://en.wikipedia.org/wiki/UTF-8#Modified_UTF-8), which is CESU-8 with special handling of NULL characters.

To have XS use CESU-8 encoding as its internal representation, define `mxCESU` when building XS. Note that any native code that interacts with XS strings will need to use CESU-8 as well.

#### Cache

To limit memory use, XS stores minimal information about strings. This is valuable for runtimes where RAM is limited. However, as strings get longer it can result in reduced performance. Platforms working with longer strings generally have more RAM. For this situation, XS has an optional string cache that can be enabled to store additional information about strings. This information is only stored for the most recently used strings, not each string, so it is relatively small.

To enable the string cache, define `mxStringInfoCacheLength` to the number of string cache entries. The recommended default is `4`, which stores additional information about the four mostly recent used strings. This can be increased, but it should generally be a small number as cache searches are linear. The string cache is off by default, as if `mxStringInfoCacheLength` is defined as `0`.

#### Normalize

The JavaScript function [String.prototype.normalize](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/normalize) is useful but obscure: most applications don't use it. The implementation is remarkably large because of required data tables. Consequently, it is disabled by default for most platforms. Platforms that want to support String.prototype.normalize must define `mxStringNormalize` to `1`.

#### Unicode property escapes

Unicode property escapes are supported by XS, but because of the large data tables required to implement the feature, they are not enabled by default on all platforms. An error is thrown if they are used on an unsupported platform. To enable Unicode property escapes, define `mxRegExpUnicodePropertyEscapes` when building XS.
