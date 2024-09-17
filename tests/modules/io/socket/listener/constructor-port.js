/*---
description: 
flags: [async, module]
---*/

import Listener from "embedded:io/socket/listener";

await $NETWORK.connected;

$TESTMC.timeout(5_000);

assert.throws(Error, () => {new Listener({
		port: undefined
	});
}, "port of undefined");

assert.throws(RangeError, () => {new Listener({
		port: -1
	});
}, "port of -1");

assert.throws(RangeError, () => {new Listener({
		port: 65536
	});
}, "port of 65536");

assert.throws(TypeError, () => {new Listener({
		port: Symbol.match
	});
}, "port of Symbol.match");

let l = new Listener({
	port: "0"
})
l.close();

$DONE();
