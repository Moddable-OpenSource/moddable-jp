import flash from "embedded:x-flash"

const path = "xs_test";

const f = globalThis.device?.flash ?? flash;

export {f as default, path}
