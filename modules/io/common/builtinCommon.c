/*
 * Copyright (c) 2019-2024  Moddable Tech, Inc.
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

#include "xsmc.h"
#include "mc.xs.h"			// for xsID_ values

#include "xsHost.h"
#include "builtinCommon.h"

#if kPinBanks

#if ESP32
	#include "soc/soc_caps.h"
	portMUX_TYPE gCommonCriticalMux = portMUX_INITIALIZER_UNLOCKED;

	static uint32_t gDigitalAvailable[kPinBanks] = {
		SOC_GPIO_VALID_GPIO_MASK & 0xFFFFFFFF,
#if kPinBanks > 1
		SOC_GPIO_VALID_GPIO_MASK >> 32
#endif
	};
#elif defined(__ets__)
	static uint32_t gDigitalAvailable[kPinBanks] = {
		(1 <<  0) |
		(1 <<  1) |
		(1 <<  2) |
		(1 <<  3) |
		(1 <<  4) |
		(1 <<  5) |
		(1 << 12) |
		(1 << 13) |
		(1 << 14) |
		(1 << 15) |
		(1 << 16)
	};
#elif nrf52
	#include "nrf_drv_gpiote.h"
	static uint32_t gDigitalAvailable[kPinBanks] = {
		0xFFFFFFFF,
		0x0000FFFF
	};
#elif defined(PICO_BUILD)
    critical_section_t gCommonCriticalMux;

	static uint8_t builtinInitialized = 0;
	static uint32_t gDigitalAvailable[kPinBanks] = {
		0x3FFFFFFF,		//@@
#if CYW43_LWIP
		0x00000001		//@@
#else
		0x00000000		//@@
#endif
	};
#endif

uint8_t builtinArePinsFree(uint32_t bank, uint32_t pins)
{
	return ((bank < kPinBanks) && (pins == (gDigitalAvailable[bank] & pins))) ? 1 : 0;
}

uint8_t builtinUsePins(uint32_t bank, uint32_t pins)
{
	if ((bank >= kPinBanks) || (pins != (gDigitalAvailable[bank] & pins)))
		return 0;

	gDigitalAvailable[bank] &= ~pins;
	return 1;
}

void builtinFreePins(uint32_t bank, uint32_t pins)
{
	if (bank < kPinBanks) {
#if nrf52
		int i;
		for (i=0; i<32; i++) {
			if (pins & (1 << i))
				nrf_gpio_cfg_default((bank * 32) + i);
		}
#endif
		gDigitalAvailable[bank] |= pins;
	}
}
#endif /* kPinBanks */

xsSlot *builtinGetCallback(xsMachine *the, xsIdentifier id)
{
	xsSlot slot;
	xsmcGet(slot, xsArg(0), id);
	return fxToReference(the, &slot);
}

static const char *gFormats[] = {
	"number",
	"buffer",
	"string;ascii",
	"string;utf8",
	"socket/tcp",

	"uint8",
	"int8",
	"uint16",
	"int16",
	"uint32",
	"int32",
	"uint64",
	"int64",

	NULL
};

void builtinGetFormat(xsMachine *the, uint8_t format)
{
	if ((0 == format) || (format >= kIOFormatInt64))
		xsRangeError("bad format");

	xsmcSetString(xsResult, (char *)gFormats[format - 1]);
}

uint8_t builtinSetFormat(xsMachine *the)
{
	char *format = xsmcToString(xsArg(0));
	uint8_t i;
	
	for (i = 0; i < kIOFormatInt64; i++) {
		if (!c_strcmp(gFormats[i], format))
			return i + 1;
	}

	xsRangeError("unknown");
}

void builtinInitializeTarget(xsMachine *the)
{
	if (xsmcHas(xsArg(0), xsID_target)) {
		xsSlot target;

		xsmcGet(target, xsArg(0), xsID_target);
		xsmcSet(xsThis, xsID_target, target);
	}
}

uint8_t builtinInitializeFormat(xsMachine *the, uint8_t format)
{
	if (xsmcHas(xsArg(0), xsID_format)) {
		xsSlot slot;

		xsmcGet(slot, xsArg(0), xsID_format);
		if (!xsmcTest(slot))
			return format;

		char *fmt = xsmcToString(slot);
		uint8_t i;
		for (i = 0; i < kIOFormatInt64; i++) {
			if (!c_strcmp(gFormats[i], fmt))
				return i + 1;
		}

		return kIOFormatInvalid;
	}

	return format;
}

// standard converstion to signed integer but disallows undefined 
int32_t builtinGetSignedInteger(xsMachine *the, xsSlot *slot)
{
	xsType type = fxTypeOf(the, slot);
	if (xsUndefinedType == type)
		xsUnknownError("invalid");

	return fxToInteger(the, slot);;
}

// standard converstion to unsigned integer but disallows undefined 
uint32_t builtinGetUnsignedInteger(xsMachine *the, xsSlot *slot)
{
	xsIntegerValue value;
	xsType type = fxTypeOf(the, slot);
	if (xsUndefinedType == type)
		xsUnknownError("invalid");

	value = fxToInteger(the, slot);
	if (value < 0)
		xsRangeError("negative");

	return (uint32_t)value;
}

#if defined(PICO_BUILD)
uint8_t builtinInitIO()
{
	if (!builtinInitialized) {
		critical_section_init(&gCommonCriticalMux);
		builtinInitialized = 1;
	}
}
#endif

