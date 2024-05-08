/*---
description: 
flags: [module]
---*/

import {File} from "file";
import config from "mc/config";

const path1 = config.file.root + "/test1.txt";

File.delete(path1);		// delete test file

assert.throws(TypeError, () => File(), "File called as function (file does not existx");
assert.throws(SyntaxError, () => new File, "File constructor requires 1 argument");
assert.throws(Error, () => new File({}), "empty object");
assert.throws(Error, () => new File(path1), "file doesn't exist");

(new File(path1, true)).close();		// create test file
assert(File.exists(path1));

assert.throws(TypeError, () => File(path1), "File called as function (file exists)");

let f = new File(path1);
f.close();

f = new File(path1, true);
f.close();
