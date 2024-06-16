/*---
description: 
flags: [module]
---*/

import files from "./files-fixture.js";

const path = "testdir";
files.delete(path);
files.createDirectory(path);

assert(files.status(path).isDirectory());

assert(files.delete(path), "returns true if directory exists when deleting");
assert.throws(Error, () => files.status(path), "directory not deleted");
assert(!files.delete(path), "returns false if directory does not exist when deleting");
