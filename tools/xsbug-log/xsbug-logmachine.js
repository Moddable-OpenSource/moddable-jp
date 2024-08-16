/*
 * Copyright (c) 2022-2024  Moddable Tech, Inc.
 *
 *   This file is part of the Moddable SDK Tools.
 * 
 *   The Moddable SDK Tools is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   The Moddable SDK Tools is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with the Moddable SDK Tools.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

const { openSync, writeSync } = require('node:fs');
const { Buffer } = require('node:buffer');

const { Machine } = require('./xsbug-machine.js');

class LogMachine extends Machine {
	view = {};
	log = "";
	binaryOutput = null;

	onTitleChanged(title, tag) {
		super.onTitleChanged(title, tag);
		
		if (title && (title !== "mcsim"))
			console.log(`#xsbug-log connected to "${title}"`);

		this.doSetAllBreakpoint([], false, true);		// break on exceptions
	}
	onLogged(path, line, data) {
		this.log += data;
		if (this.log.endsWith("\n")) {
			if (this.log.startsWith("|:") && this.log.endsWith(":|\n")) {
				if (!this.binaryOutput)
					this.binaryOutput = openSync(process.env.MODDABLE + "/build/tmp/log.bin", "w+");
				writeSync(this.binaryOutput, Buffer.from(this.log.slice(2, this.log.length - 4), "base64"), 0);
			}
			else
				console.log(this.log.slice(0, this.log.length - 1));
			this.log = "";
		}
	}
	onBroken(path, line, text) {
		this.view.frames.forEach((frame, index) => {
			let line = "  #" + index + ": " + frame.name;
			if (frame.path)
				line += " " + frame.path + ":" + frame.line
			console.log(line);
		});

		super.onBroken(path, line, text);
	}
	onViewChanged(name, items) {
		this.view[name] = items;
	}
}

exports.LogMachine = LogMachine;
