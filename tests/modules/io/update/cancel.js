/*---
description: 
flags: [module]
---*/

import update from "embedded:x-update";
import flash from "embedded:x-flash";

const running = flash.open({path: "running"});
const ota = update.open({partition: flash.open({path: "nextota"})});

const blockSize = running.status().blockSize;
ota.write(running.read(0, blockSize));
ota.close();
