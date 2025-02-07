/*
 * Copyright (c) 2020-2024  Moddable Tech, Inc.
 *
 *   This file is part of the Moddable SDK.
 *
 *   This work is licensed under the
 *       Creative Commons Attribution 4.0 International License.
 *   To view a copy of this license, visit
 *       <http://creativecommons.org/licenses/by/4.0>.
 *   or send a letter to Creative Commons, PO Box 1866,
 *   Mountain View, CA 94042, USA.
 *
 */

import AudioIn from "audioin"
import AudioOut from "pins/audioout"
import Timer from "timer"

const samples = [];

const input = new AudioIn;
const {numChannels, sampleRate, bitsPerSample} = input;
if (16 !== bitsPerSample)
	throw new Error("expects 16 bit samples");

trace("start recording\n");

try {
	while (true) {
		const s = new SharedArrayBuffer(4096);
		input.read((s.byteLength >> 1) / numChannels, s, 0);
		samples.push(s);
		if (samples.length > 40)
			break;
	}
}
catch {
}
trace("recording complete\n");
input.close();

let speaker = new AudioOut({
	streams: 1,
	sampleRate,
	numChannels
});

const playing = [];
function enqueue() {
	speaker.enqueue(0, AudioOut.RawSamples, samples[0]);
	speaker.enqueue(0, AudioOut.Callback, 0);
	playing.push(samples.shift());
}
enqueue();
enqueue();
enqueue();
enqueue();
speaker.callback = function() {
	playing.shift();
	if (!samples.length) {
		if (!playing.length) {
			trace("playback complete\n");
			speaker.stop();
		}
		return;
	}
	enqueue();
}
speaker.start();
trace("start playback\n");
