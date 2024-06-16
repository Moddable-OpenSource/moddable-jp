if (!globalThis.Host?.Backlight && !globalThis.device?.peripheral?.Backlight)
	throw new Error("backlight unsupported");
