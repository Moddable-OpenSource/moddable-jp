/*---
description: 
flags: [module]
---*/

import storage from "./storage-fixture.js";

const path = "test";
let store = storage.open({path}); 
store.close();
store.close();
store.close();
