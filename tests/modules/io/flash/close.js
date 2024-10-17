/*---
description: 
flags: [module]
---*/

import flash from "./flash-fixture.js";
import {path} from "./flash-fixture.js";

let f = flash.open({path});
f.close();
f.close();
assert.throws(SyntaxError, () => f.read(12, 0));
assert.throws(SyntaxError, () => f.write(new ArrayBuffer(12)));
assert.throws(SyntaxError, () => f.eraseBlock(0));
assert.throws(SyntaxError, () => f.status());
