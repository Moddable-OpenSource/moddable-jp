/*
 * Copyright (c) 2021-2022  Moddable Tech, Inc.
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

// https://html.spec.whatwg.org/#server-sent-events

import Timer from "timer";
import Headers from "headers";
import URL from "url";

const statusTexts = {
	100: "Continue",
	101: "Switching Protocols",
	200: "OK",
	201: "Created",
	202: "Accepted",
	203: "Non-Authoritative Information",
	204: "No Content",
	205: "Reset Content",
	206: "Partial Content",
	300: "Multiple Choices",
	301: "Moved Permanently",
	302: "Found",
	303: "See Other",
	304: "Not Modified",
	305: "Use Proxy",
	307: "Temporary Redirect",
	400: "Bad Request",
	401: "Unauthorized",
	402: "Payment Required",
	403: "Forbidden",
	404: "Not Found",
	405: "Method Not Allowed",
	406: "Not Acceptable",
	407: "Proxy Authentication Required",
	408: "Request Timeout",
	409: "Conflict",
	410: "Gone",
	411: "Length Required",
	412: "Precondition Failed",
	413: "Payload Too Large",
	414: "URI Too Long",
	415: "Unsupported Media Type",
	416: "Range Not Satisfiable",
	417: "Expectation Failed",
	426: "Upgrade Required",
	500: "Internal Server Error",
	501: "Not Implemented",
	502: "Bad Gateway",
	503: "Service Unavailable",
	504: "Gateway Timeout",
	505: "HTTP Version Not Supported",
};
Object.freeze(statusTexts);

const CR = -1;
const BODY = 0;
const COMMENT = 1;
const NAME = 2;
const SPACE = 3;
const VALUE = 4;

class EventSource {
	#client = null;
	#config;
	#data = "";
	#eventHandlers = { error:null, open:null, message:null };
	#eventListeners = {};
	#eventType = "";
	#host;
	#lastEventID = "";
	#origin;
	#path;
	#port;
	#readystate = this.CLOSED;
	#reconnectionTime = 10000;
	#url;
	#method;
	#headers;
	#body;
	
	constructor(href, options) {
		const url = new URL(href);
		const protocol = url.protocol;
		this.#host = url.hostname;
		let config, port;
		if (protocol == "http:") {
			this.#config = device.network.http;
			this.#port = url.port || 80;
		}
		else if (protocol == "https:") {
			this.#config = device.network.https;
			this.#port = url.port || 443;
		}
		else
			throw new URLError("only http or https")
		this.#origin = url.origin;
		let path = url.pathname;
		let query = url.search;
		if (query)
			path += query;
		this.#path = path;
		this.#url = url.href;
		this.#method = options.method || "GET";
		this.#headers = new Headers();
		this.#headers.set("accept", "text/event-stream");
		options.headers?.forEach((value, name) => this.#headers.set(name.toLowerCase(), value));
		if ((this.#method == "POST") || (this.#method == "PUT")) {
			let body = options.body;
			if (!body) 
				rejectResponse(new URLError(this.#method + " no body"));
			else if (!(body instanceof ArrayBuffer)) {
				body = body.toString();
				body = ArrayBuffer.fromString(body);
			}
			this.#body = body;
			const length = this.#headers.get("content-length");
			if (length == undefined) {
				this.#headers.set("content-length", body.byteLength);
			}
		}
		this.#connect();
	}
	get readystate() {
		return this.#readystate;
	}
	get onerror() {
		return this.#eventHandlers.error;
	}
	set onerror(handler) {
		this.#setEventHandler("error", handler);
	}
	get onopen() {
		return this.#eventHandlers.open;
	}
	set onopen(handler) {
		this.#setEventHandler("open", handler);
	}
	get onmessage() {
		return this.#eventHandlers.message;
	}
	set onmessage(handler) {
		this.#setEventHandler("message", handler);
	}
	get url() {
		return this.#url;
	}
	addEventListener(type, listener) {
		let listeners = this.#eventListeners[type];
		if (!listeners)
			listeners = this.#eventListeners[type] = [];
		listeners.push(listener);
	}
	close() {
		if (this.#client) {
			this.#client.close();
			this.#client = null;
		}
	}
	removeEventListener(type, listener) {
		let listeners = this.#eventListeners[type];
		if (listeners) {
			let index = listeners.find(item => item === listener);
			if (index >= 0)
				listeners.splice(index, 1);
		}
	}
	#callEventListeners(event) {
		let listeners = this.#eventListeners[event.type];
		if (listeners)
			listeners.forEach(listener => listener.call(null, event));
	}
	#connect() {
		const config = this.#config;
		const host = this.#host;
		const port = this.#port;
		const path = this.#path;
		this.#readystate = this.CONNECTING;
		const client = this.#client = new config.io({ 
			...config,
			host, 
			port,  
			onClose: () => {
				this.#client = null;
				this.#readystate = this.CLOSED;
				Timer.set(() => { this.#connect() }, this.#reconnectionTime)
			},
			onError: () => {
				this.#client = null;
				this.#onError();
			}
		});
		let method = this.#method;
		let headers = this.#headers;
		let body = this.#body;
		let length = body.byteLength;
		let offset = 0;
		let buffer = null;
		let index = 0, nameStart, nameStop, valueStart, valueStop;
		let state = BODY;
		let request = client.request({
			method,
			path,
			headers,
			onHeaders: (status, headers) => {
				if ((status == 200) && (headers.get("content-type").indexOf("text/event-stream") >= 0)) {
					this.#readystate = this.OPEN;
					const event = { type:"open" };
					this.#callEventListeners(event);
				}
				else {
					client.close();
					this.#onError({status: status, statusText:statusTexts[status]});
				}
			},
			onWritable(count) {
				if (body) {
					let remain = length - offset;
					if (remain > 0) {
						if (count > remain)
							count = remain;
						let view = new DataView(body, offset, count);
						this.write(view);
						offset += count;
					}
					else
						this.write();
				}
			},
			onReadable: (count) => {
				if (buffer)
					buffer = buffer.concat(request.read(count));
				else
					buffer = request.read(count);
				let array = new Uint8Array(buffer);
				let length = array.length;
				while (index < length) {
					const c = array[index];
					let line = false;
					switch(state) {
					case CR:
						 state = BODY;
						 if (c == 0x0A)
						 	break;
						 // continue
					case BODY:
						if ((c == 0x0A) || (c == 0x0D)) {
							this.#dispatchEvent();
							line = true;
						}
						else if (c == 0x3A)
							state = COMMENT;
						else {
							state = NAME;
							nameStart = index
						}
						break
					case COMMENT:
						if ((c == 0x0A) || (c == 0x0D))
							line = true;
						break
					case NAME:
						if (c == 0x3A) {
							nameStop = index;
							valueStart = index + 1;
							state = SPACE;
						}
						else if ((c == 0x0A) || (c == 0x0D)) {
							nameStop = index;
							this.#processField(String.fromArrayBuffer(buffer.slice(nameStart, nameStop)), "");
							line = true;
						}
						break;
					case SPACE:
						state = VALUE;
						if (c == 0x20) {
							valueStart++;
						 	break;
						 }
						 // continue
					case VALUE:
						if ((c == 0x0A) || (c == 0x0D)) {
							valueStop = index;
							this.#processField(String.fromArrayBuffer(buffer.slice(nameStart, nameStop)), String.fromArrayBuffer(buffer.slice(valueStart, valueStop)));
							line = true;
						}
						break;
					}
					index++;
					if (line) {
						buffer = buffer.slice(index);
						array = new Uint8Array(buffer);
						length = array.length;
						index = 0;					
						if (c == 0x0D)
							state = CR;
						else
							state = BODY;
					}
				}
			},
		});
	}
	#dispatchEvent() {
		let type = this.#eventType;
		let data = this.#data;
		const origin = this.#origin;
		const lastEventID = this.#lastEventID;
		this.#data = "";
		this.#eventType = "";
		if (data != "") {
			if (type == "")
				type = "message";
			if (data.endsWith("\n"))
				data = data.slice(0, -1);
			const event = { type, data, origin, lastEventID };
			this.#callEventListeners(event);
		}
	}
	#onError(error) {
		this.#client = null;
		this.#readystate = this.CLOSED;
		const event = { type:"error", ...error };
		this.#callEventListeners(event);
	}
	#processField(name, value) {
		switch (name) {
		case "event":
			this.#eventType = value;
			break;
		case "data":
			this.#data += value + "\n";
			break;
		case "id":
			if (value.index("\0") < 0)
				this.#lastEventID = value;
			break;
		case "retry":
			if (value.match(/^[0-9]+$/) != null)
				this.#reconnectionTime = parseInt(value);
			break;
		}
	}
	#setEventHandler(type, handler) {
		const former = this.#eventHandlers[type];
		if (typeof(former) == "function") {
			this.removeEventListener(type, former);
			this.#eventHandlers[type] = null;
		}
		if (typeof(handler) == "function") {
			this.addEventListener(type, handler);
			this.#eventHandlers[type] = handler;
		}
	}
}
EventSource.prototype.CONNECTING = 0;
EventSource.prototype.OPEN = 1;
EventSource.prototype.CLOSED = 2;

export default EventSource;
