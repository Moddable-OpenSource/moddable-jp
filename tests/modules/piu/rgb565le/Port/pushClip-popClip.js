/*---
description: 
flags: [onlyStrict]
---*/

const circleTexture = new Texture({ path: "circleish.png" });

const samplePort = new Port(null, {
	top: 0, bottom: 0, left: 0, right: 0,
	Behavior: class extends Behavior {
		onDraw(port) {
			port.fillTexture(circleTexture, "blue", 0, 0, port.width, port.height, 0, 0, 40, 40);
			port.pushClip(0, 0, 200, 100);
			port.fillTexture(circleTexture, "red", 0, 0, port.width, port.height, 0, 0, 40, 40);
			port.popClip();
		}
	}
})

new Application(null, {
	displayListLength: 3000,
	skin: new Skin({ fill: "white" }),
	contents: [ samplePort ]
});

screen.checkImage("55c1ce5862438c862993f0c575b8b4a6");