/*---
description: 
flags: [module]
---*/

import files from "./files-fixture.js";

const fails = [
	"/foo",
	"../foo",
	"./foo",
	"foo/..",
	"foo/../",
	"foo/.",
	"foo/../",
	"foo/../bar",
	"foo/./bar",
	"foo//bar",
	"foo///bar"
];

fails.forEach(path => {
	assert.throws(Error, () => files.delete(path), `should reject invalid path: "${path}"`);
});
