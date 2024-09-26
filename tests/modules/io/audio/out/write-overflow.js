/*---
description: 
flags: [async, module]
---*/

import AudioOut from "embedded:io/audioout";

let out = new AudioOut({
	onWritable(count) {
		$DO(() => {
			assert.throws(Error, () => this.write(new ArrayBuffer(count * 2)));
			this.close();
		})();
	}
});

out.start();
