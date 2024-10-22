/*
 * Copyright (c) 2016-2023  Moddable Tech, Inc.
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

import Timer from "timer";

const analogInput = new device.io.Analog({
	pin: 0		// change the pin number for your development board
});

Timer.repeat(() => {
	const rawValue = analogInput.read();
	const value = rawValue / ((1 << analogInput.resolution) - 1);
	trace(`read: ${value.toPrecision(4)}\n`);
}, 100);
