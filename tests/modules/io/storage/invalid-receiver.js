/*---
description: 
flags: [module]
---*/

import storage from "./storage-fixture.js";

function callWithInvalidReceivers(obj, functionName, ...args)
{
	assert.throws(SyntaxError, () => obj[functionName].apply(new $TESTMC.HostObjectChunk, args), `${functionName.toString()} with HostObjectChunk`);
	assert.throws(SyntaxError, () => obj[functionName].apply(new $TESTMC.HostObject, args), `${functionName.toString()} with HostObject`);
	assert.throws(SyntaxError, () => obj[functionName].apply("a string", args), `${functionName.toString()} with string`);
	assert.throws(SyntaxError, () => obj[functionName].apply([], args), `${functionName.toString()} with array`);
}

const path = "test";

callWithInvalidReceivers(storage, "open", {path});
let store = storage.open({path}); 

callWithInvalidReceivers(store, "write", "key", Uint8Array.of(1));
callWithInvalidReceivers(store, "read", "key");
callWithInvalidReceivers(store, "delete", "key");
callWithInvalidReceivers(store, Symbol.iterator);
callWithInvalidReceivers(store, "close");

store.close();
