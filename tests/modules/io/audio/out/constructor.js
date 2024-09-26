/*---
description: 
flags: [module]
---*/

import AudioOut from "embedded:io/audioout";

[8000, 22050, 24000, 32000, 44100, 48000].forEach(sampleRate => {
	let out = new AudioOut({
		sampleRate
	});
	assert.sameValue(out.sampleRate, sampleRate, "sample rate option " + sampleRate);
	out.close();
});

[1, 2].forEach(numChannels => {
	let out = new AudioOut({
		numChannels
	});
	assert.sameValue(out.numChannels, numChannels, "numChannels option " + numChannels);
	out.close();
});


[8, 16].forEach(bitsPerSample => {
	let out = new AudioOut({
		bitsPerSample
	});
	assert.sameValue(out.bitsPerSample, bitsPerSample, "bitsPerSample option " + bitsPerSample);
	out.close();
});

let out = new AudioOut({
	bitsPerSample: "16",
	numChannels: "2",
	sampleRate: "44100"
});
assert.sameValue(out.bitsPerSample, 16, "bitsPerSample option 16");
assert.sameValue(out.numChannels, 2, "numChannels option 2");
assert.sameValue(out.sampleRate, 44100, "sampleRate option 44100");

assert.throws(RangeError, () => new AudioOut({bitsPerSample: -1}), "bitsPerSample -1");
assert.throws(RangeError, () => new AudioOut({bitsPerSample: 0}), "bitsPerSample 0");
assert.throws(RangeError, () => new AudioOut({bitsPerSample: 17}), "bitsPerSample 17");
assert.throws(RangeError, () => new AudioOut({bitsPerSample: 65536 + 8}), "bitsPerSample 65536 + 8");
assert.throws(TypeError, () => new AudioOut({bitsPerSample: Symbol()}), "bitsPerSample symbol");

assert.throws(RangeError, () => new AudioOut({numChannels: -1}), "numChannels -1");
assert.throws(RangeError, () => new AudioOut({numChannels: 0}), "numChannels 0");
assert.throws(RangeError, () => new AudioOut({numChannels: 3}), "numChannels 3");
assert.throws(RangeError, () => new AudioOut({numChannels: 65537}), "numChannels 65537");
assert.throws(TypeError, () => new AudioOut({numChannels: Symbol()}), "numChannels symbol");

assert.throws(RangeError, () => new AudioOut({sampleRate: -1}), "sampleRate -1");
assert.throws(RangeError, () => new AudioOut({sampleRate: 0}), "sampleRate 0");
assert.throws(RangeError, () => new AudioOut({sampleRate: 100_000}), "sampleRate 100_000");
assert.throws(TypeError, () => new AudioOut({sampleRate: Symbol()}), "sampleRate symbol");
