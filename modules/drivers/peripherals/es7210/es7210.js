/*
 * Copyright (c) 2024 Moddable Tech, Inc.
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

/**
 * ES7210 ADC driver for M5Stack CoreS3 microphone
 * Based on M5Unified implementation
 */
class ES7210 {
	#io;

	constructor(options) {
		this.#io = new options.sensor.io({
			hz: 400_000,
			address: 0x40,
			...options.sensor
		});
	}

	readByte(address) {
		return this.#io.readUint8(address);
	}

	writeByte(address, value) {
		return this.#io.writeUint8(address, value);
	}

	/**
	 * Initialize ES7210 for M5Stack CoreS3
	 * Configuration based on M5Unified _microphone_enabled_cb_cores3
	 */
	initialize() {
		// Reset
		this.writeByte(0x00, 0xFF); // RESET_CTL

		// ES7210 register initialization sequence from M5Unified
		const initSequence = [
			{ reg: 0x00, value: 0x41 }, // RESET_CTL
			{ reg: 0x01, value: 0x1f }, // CLK_ON_OFF
			{ reg: 0x06, value: 0x00 }, // DIGITAL_PDN
			{ reg: 0x07, value: 0x20 }, // ADC_OSR
			{ reg: 0x08, value: 0x10 }, // MODE_CFG
			{ reg: 0x09, value: 0x30 }, // TCT0_CHPINI
			{ reg: 0x0A, value: 0x30 }, // TCT1_CHPINI
			{ reg: 0x20, value: 0x0a }, // ADC34_HPF2
			{ reg: 0x21, value: 0x2a }, // ADC34_HPF1
			{ reg: 0x22, value: 0x0a }, // ADC12_HPF2
			{ reg: 0x23, value: 0x2a }, // ADC12_HPF1
			{ reg: 0x02, value: 0xC1 },
			{ reg: 0x04, value: 0x01 },
			{ reg: 0x05, value: 0x00 },
			{ reg: 0x11, value: 0x60 },
			{ reg: 0x40, value: 0x42 }, // ANALOG_SYS
			{ reg: 0x41, value: 0x70 }, // MICBIAS12
			{ reg: 0x42, value: 0x70 }, // MICBIAS34
			{ reg: 0x43, value: 0x1B }, // MIC1_GAIN
			{ reg: 0x44, value: 0x1B }, // MIC2_GAIN
			{ reg: 0x45, value: 0x00 }, // MIC3_GAIN
			{ reg: 0x46, value: 0x00 }, // MIC4_GAIN
			{ reg: 0x47, value: 0x00 }, // MIC1_LP
			{ reg: 0x48, value: 0x00 }, // MIC2_LP
			{ reg: 0x49, value: 0x00 }, // MIC3_LP
			{ reg: 0x4A, value: 0x00 }, // MIC4_LP
			{ reg: 0x4B, value: 0x00 }, // MIC12_PDN
			{ reg: 0x4C, value: 0xFF }, // MIC34_PDN
			{ reg: 0x01, value: 0x14 }, // CLK_ON_OFF
		];

		for (const {reg, value} of initSequence) {
			this.writeByte(reg, value);
		}
	}

	/**
	 * Power down ES7210
	 */
	powerDown() {
		this.writeByte(0x00, 0xFF); // RESET_CTL
	}
}

export default ES7210;
