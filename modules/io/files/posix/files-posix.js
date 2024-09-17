/*
 * Copyright (c) 2024  Moddable Tech, Inc.
 *
 *   This file is part of the Moddable SDK Runtime.
 * 
 *   The Moddable SDK Runtime is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   The Moddable SDK Runtime is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 * 
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with the Moddable SDK Runtime.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
class Status {
	isFile() @ "xs_stat_isFile"
	isDirectory() @ "xs_stat_isDirectory"
	isSymbolicLink() @ "xs_stat_isSymbolicLink"
}

function fstatus() @ "xs_fileposix_status"

class File @ "xs_fileposix_destructor"{
	constructor(options) @ "xs_fileposix"
	close() @ "xs_fileposix_close"

	read(buffer /* or count */, posiiton) @ "xs_fileposix_read"
	write(buffer, position) @ "xs_fileposix_write"

	status() {
		return fstatus.call(this, new Status);
	}

	setSize(length) @ "xs_fileposix_setSize"

	flush() @ "xs_fileposix_flush"
}


class Scan @ "xs_scanposix_destructor" {
	constructor(options) @ "xs_scanposix"
	close() @ "xs_scanposix_close"

	read() @ "xs_scanposix_read"
}

function openFile(options) @ "xs_direectoryposix_openFile"
function openDirectory(options) @ "xs_direectoryposix_openDirectory"
function scan(path /* OR OPTIONS */) @ "xs_direectoryposix_scan"
function status(path) @ "xs_direectoryposix_status"

class Directory @ "xs_direectoryposix_destructor" {
	constructor(options) @ "xs_direectoryposix"
	close() @ "xs_direectoryposix_close"

	openFile(options) {
		return openFile.call(this, options, File.prototype);
	}
	openDirectory(options) {
		return openDirectory.call(this, options, Directory.prototype);
	}

	delete(path) @ "xs_direectoryposix_delete"

	move(from, to) @ "xs_direectoryposix_move"

	status(path, options) {
		return status.call(this, path, options, new Status);
	}

	createDirectory(options) @ "xs_direectoryposix_createDirectory"
	createLink(path, target) @ "xs_direectoryposix_createLink"

	readLink(path) @ "xs_direectoryposix_readLink"

	scan(path) {
		return scan.call(this, path, Scan.prototype);
	}
}

export {Directory}
