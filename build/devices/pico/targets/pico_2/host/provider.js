/*
 * Copyright (c) 2024  Moddable Tech, Inc.
 *
 *   This file is part of the Moddable SDK Runtime.
 *
 *   The Moddable SDK Runtime is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   The Moddable SDK Runtime is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with the Moddable SDK Runtime.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import Analog from "embedded:io/analog";
import Digital from "embedded:io/digital";
import DigitalBank from "embedded:io/digitalbank";
import I2C from "embedded:io/i2c";
import PWM from "embedded:io/pwm";
import Serial from "embedded:io/serial";
import SMBus from "embedded:io/smbus";
import SPI from "embedded:io/spi";

const device = {
	I2C: {
		default: {
			io: I2C,
			data: 4,
			clock: 5,
			port: 0
		}
	},
	Serial: {
		default: {
			io: Serial,
			receive: 1,
			transmit: 0
		}
	},
	SPI: {
		default: {
			io: SPI,
			clock: 18,
			in: 20,
			out: 19,
			port: 0
		}
	},
	Analog: {
		default: {
			io: Analog,
			pin: 40
		},
		A0: {
			io: Analog,
			pin: 40
		},
		A1: {
			io: Analog,
			pin: 41
		},
		A2: {
			io: Analog,
			pin: 42
		}
	},
	io: { Analog, Digital, DigitalBank, I2C, PWM, Serial, SMBus },
	pin: {
		led: 25,
		button: 45
	}
};

export default device;

