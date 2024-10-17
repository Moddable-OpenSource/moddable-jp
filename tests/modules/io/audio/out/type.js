/*---
description: 
flags: [module]
---*/

import AudioOut from "embedded:io/audioout";

let out = new AudioOut({});
assert.sameValue(typeof out.type, "string");
assert.sameValue(out.type, "LPCM");
out.close();

out = new AudioOut({type: "LPCM"});
assert.sameValue(typeof out.type, "string");
assert.sameValue(out.type, "LPCM");

assert.throws(TypeError, () => out.type = "xyzzy");

out.close();
