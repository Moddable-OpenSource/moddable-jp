/*---
description: 
flags: [async, module]
---*/

import TCP from "embedded:io/socket/tcp";

await $NETWORK.connected;

$TESTMC.timeout(5_000);

let resolve, reject, p = new Promise((a, b) => {
	resolve = a;
	reject = b;
});

let t = new TCP({
	address: await $NETWORK.resolve("www.example.com"),
	port: 80,
	onWritable(bytes) {
		resolve(bytes);
	},
	onError() {
		reject("connection failure")
	}
});
assert.throws(Error, () => t.write(Uint8Array.of(1)), "write before connected");

t.write(new ArrayBuffer(await p));

$DONE();
