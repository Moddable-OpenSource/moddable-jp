/*---
description: 
flags: [module]
---*/

import files from "./files-fixture.js";

const pathFile = "testfile.dat";
const pathDir = "testdir";

files.delete(pathFile);
files.delete(pathDir);

assert.throws(Error, () => files.status(pathFile));
assert.throws(Error, () => files.status(pathDir));

assert.throws(Error, () => files.status());

files.createDirectory(pathDir);
files.openFile({path: pathFile, mode: "w+"}).close();

assert.throws(SyntaxError, () => files.status.call(new $TESTMC.HostObject, pathFile));
assert.throws(SyntaxError, () => files.status.call(new $TESTMC.HostObject, pathDir));
assert.throws(Error, () => files.status("./" + pathFile));
assert.throws(Error, () => files.status("./" + pathDir));
assert.throws(Error, () => files.status("../"));

files.delete(pathFile);
files.delete(pathDir);
