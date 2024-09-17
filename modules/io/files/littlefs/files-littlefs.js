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

function fstatus() @ "xs_filelittlefs_status"

class File @ "xs_filelittlefs_destructor"{
	constructor(options) @ "xs_filelittlefs"
	close() @ "xs_filelittlefs_close"

	read(buffer /* or count */, posiiton) @ "xs_filelittlefs_read"
	write(buffer, position) @ "xs_filelittlefs_write"

	status() {
		return fstatus.call(this, new Status);
	}

	setSize(length) @ "xs_filelittlefs_setSize"

	flush() @ "xs_filelittlefs_flush"
}

class Scan @ "xs_scanlittlefs_destructor" {
	constructor(options) @ "xs_scanlittlefs"
	close() @ "xs_scanlittlefs_close"

	read() @ "xs_scanlittlefs_read"
}

function openFile(options) @ "xs_direectorylittlefs_openFile"
function openDirectory(options) @ "xs_direectorylittlefs_openDirectory"
function scan(path /* OR OPTIONS */) @ "xs_direectorylittlefs_scan"
function status(path) @ "xs_direectorylittlefs_status"

class Directory @ "xs_direectorylittlefs_destructor" {
	constructor(options) @ "xs_direectorylittlefs"
	close() @ "xs_direectorylittlefs_close"

	openFile(options) {
		return openFile.call(this, options, File.prototype);
	}
	openDirectory(options) {
		return openDirectory.call(this, options, Directory.prototype);
	}

	delete(path) @ "xs_direectorylittlefs_delete"

	move(from, to) @ "xs_direectorylittlefs_move"

	status(path) {
		return status.call(this, path, new Status);
	}

	createDirectory(options) @ "xs_direectorylittlefs_createDirectory"
	createLink(path, target) @ "xs_direectorylittlefs_link"

	readLink(path) @ "xs_direectorylittlefs_link"

	scan(path) {
		return scan.call(this, path, Scan.prototype);
	}
}

export {Directory}
