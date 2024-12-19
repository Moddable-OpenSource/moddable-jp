/*
 * Copyright (c) 2016-2024  Moddable Tech, Inc.
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

import Bitmap from "commodetto/Bitmap";

function initialize() @ "xs_JPEG_initialize";

export default class JPEG @ "xs_JPEG_destructor" {
	constructor(buffer, options) {
		initialize.call(this, buffer, options, Bitmap)
	}
	close() @ "xs_JPEG_close"
	push() {throw new Error("ESP32 JPEG decoder doesn't spool");}
	read() @ "xs_JPEG_read"
	get ready() @ "xs_JPEG_get_ready"
}
