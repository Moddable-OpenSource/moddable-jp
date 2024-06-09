/*---
description: 
flags: [module]
---*/

import files from "./files-fixture.js";
import {deleteTree} from "./files-fixture.js";

const paths = [
	"test",
	"test/dir1",
	"test/dir2",
	"test/dir2/test",
	"test/dir2/test/testing"
];

deleteTree(files, paths[0]);
paths.forEach(path => files.createDirectory(path));
paths.reverse();
paths.forEach(path => assert(files.delete(path)));
paths.forEach(path => assert.throws(Error, () => files.status(path)));
