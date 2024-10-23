/*---
description: 
flags: [async, module]
---*/

import Listener from 'embedded:io/socket/listener';
import TCP from "embedded:io/socket/tcp";

await $NETWORK.connected;

$TESTMC.timeout(5_000);

const port = 1234;

new Listener({
    port,
    onReadable() {
        const from = this.read();
        if (undefined === from)
			return $DONE("Listen failed to return socket");

        new TCP({
            from,
            onWritable() {
                this.close();
            },
            onError() {
				$DONE("incoming socket failed");
            }
        });
    },
});

new TCP({ 
    address: '127.0.0.1',
    port,
    onError() {
        $DONE();		// remote closed socket
    },
});
