/*---
description: 
flags: [module]
---*/

import AudioOut from "embedded:io/audioout";

let out = new AudioOut({});
let numChannels = out.numChannels;
assert.sameValue(typeof numChannels, "number");
assert((1 <= numChannels) && (numChannels <= 2), "numChannels out of range");
assert.throws(TypeError, () => out.numChannels = numChannels, "numChannels is read-only");
out.close();
