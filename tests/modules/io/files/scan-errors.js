/*---
description: 
flags: [module]
includes: [compareArray.js]
---*/

import files from "./files-fixture.js";

assert.throws(Error, () => files.scan());
assert.throws(Error, () => files.scan(".."));

assert.throws(SyntaxError, () => files.scan.call(new $TESTMC.HostObject, ""));

let s = files.scan("");

assert.throws(SyntaxError, () => s.read.call(new $TESTMC.HostObject));

s.close();
