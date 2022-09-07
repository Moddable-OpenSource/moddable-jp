/*---
description: https://github.com/web-platform-tests/wpt/url/resources/urltestdata.json
flags: [module]
---*/

import { runTests } from "./url_FIXTURE.js";

const tests = [
	{
		"input": "http://foo.bar/baz?qux#foo\"bar",
		"base": "about:blank",
		"href": "http://foo.bar/baz?qux#foo%22bar",
		"origin": "http://foo.bar",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "foo.bar",
		"hostname": "foo.bar",
		"port": "",
		"pathname": "/baz",
		"search": "?qux",
		"searchParams": "qux=",
		"hash": "#foo%22bar"
	},
	{
		"input": "http://foo.bar/baz?qux#foo<bar",
		"base": "about:blank",
		"href": "http://foo.bar/baz?qux#foo%3Cbar",
		"origin": "http://foo.bar",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "foo.bar",
		"hostname": "foo.bar",
		"port": "",
		"pathname": "/baz",
		"search": "?qux",
		"searchParams": "qux=",
		"hash": "#foo%3Cbar"
	},
	{
		"input": "http://foo.bar/baz?qux#foo>bar",
		"base": "about:blank",
		"href": "http://foo.bar/baz?qux#foo%3Ebar",
		"origin": "http://foo.bar",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "foo.bar",
		"hostname": "foo.bar",
		"port": "",
		"pathname": "/baz",
		"search": "?qux",
		"searchParams": "qux=",
		"hash": "#foo%3Ebar"
	},
	{
		"input": "http://foo.bar/baz?qux#foo`bar",
		"base": "about:blank",
		"href": "http://foo.bar/baz?qux#foo%60bar",
		"origin": "http://foo.bar",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "foo.bar",
		"hostname": "foo.bar",
		"port": "",
		"pathname": "/baz",
		"search": "?qux",
		"searchParams": "qux=",
		"hash": "#foo%60bar"
	},
	"# IPv4 parsing (via https://github.com/nodejs/node/pull/10317)",
	{
		"input": "http://1.2.3.4/",
		"base": "http://other.com/",
		"href": "http://1.2.3.4/",
		"origin": "http://1.2.3.4",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "1.2.3.4",
		"hostname": "1.2.3.4",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://1.2.3.4./",
		"base": "http://other.com/",
		"href": "http://1.2.3.4/",
		"origin": "http://1.2.3.4",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "1.2.3.4",
		"hostname": "1.2.3.4",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://192.168.257",
		"base": "http://other.com/",
		"href": "http://192.168.1.1/",
		"origin": "http://192.168.1.1",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "192.168.1.1",
		"hostname": "192.168.1.1",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://192.168.257.",
		"base": "http://other.com/",
		"href": "http://192.168.1.1/",
		"origin": "http://192.168.1.1",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "192.168.1.1",
		"hostname": "192.168.1.1",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://192.168.257.com",
		"base": "http://other.com/",
		"href": "http://192.168.257.com/",
		"origin": "http://192.168.257.com",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "192.168.257.com",
		"hostname": "192.168.257.com",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://256",
		"base": "http://other.com/",
		"href": "http://0.0.1.0/",
		"origin": "http://0.0.1.0",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "0.0.1.0",
		"hostname": "0.0.1.0",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://256.com",
		"base": "http://other.com/",
		"href": "http://256.com/",
		"origin": "http://256.com",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "256.com",
		"hostname": "256.com",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://999999999",
		"base": "http://other.com/",
		"href": "http://59.154.201.255/",
		"origin": "http://59.154.201.255",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "59.154.201.255",
		"hostname": "59.154.201.255",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://999999999.",
		"base": "http://other.com/",
		"href": "http://59.154.201.255/",
		"origin": "http://59.154.201.255",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "59.154.201.255",
		"hostname": "59.154.201.255",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	},
	{
		"input": "http://999999999.com",
		"base": "http://other.com/",
		"href": "http://999999999.com/",
		"origin": "http://999999999.com",
		"protocol": "http:",
		"username": "",
		"password": "",
		"host": "999999999.com",
		"hostname": "999999999.com",
		"port": "",
		"pathname": "/",
		"search": "",
		"hash": ""
	}
];

runTests(tests);
