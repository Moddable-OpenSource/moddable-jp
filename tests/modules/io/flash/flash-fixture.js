import Flash from "embedded:x-flash"

const path = "xs_test";

let flash = globalThis.device?.flash;
flash ??= new Flash({});

export {flash as default, path}
