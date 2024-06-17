import PWM from "pins/pwm";
import config from "mc/config";
import Button from "button";
import LED from "led";

class Backlight extends PWM {
	constructor(brightness = 100) {
		super({pin: config.backlight});
		this.write(brightness);
	}
	write(value) {
		value /= 100;
		if (value <= 0)
			value = 1023;
		else if (value >= 1)
			value = 0;
		else {
			value *= value;
			value = 1 - value;	// PWM is inverted from brightness (0 is full power, 1023 is no power)
			value *= 1023;
		}
		super.write(value);
	}
}

class Flash {
	constructor(options) {
		return new Button({
			...options,
			pin: 0,
			invert: true
		});
	}
}

globalThis.Host = Object.freeze({
	Backlight,
	LED: {
		Default: class {
			constructor(options) {
				return new LED({
					...options,
					pin: 2
				});
			}
		}
	},
	Button: {
		Default: Flash,
		Flash
	}
}, true);
