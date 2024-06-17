/*---
description: 
flags: [module]
includes: [compareArray.js]
---*/

import files from "./files-fixture.js";

const s = files.scan("");
s.close();
s.close();
assert.throws(SyntaxError, () => s.read());
