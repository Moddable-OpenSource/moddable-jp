import Storage from "embedded:x-storage"

let storage = globalThis.device?.storage;
storage ??= new Storage({});

function keys(store) {
	return Array.from(store).sort();
}

function emptyDomain(store) {
	for (let key of Array.from(store))
		store.delete(key);
	
	assert.sameValue(Array.from(store).length, 0, "emptyDomain failed");
}

export {storage as default, emptyDomain, keys}
