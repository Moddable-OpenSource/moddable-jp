import {Directory} from "embedded:x-files"

let files = globalThis.device?.files;
files ??= new Directory({
	path: "/Users/hoddie/tmp"		//@@
});

function scan(files, path) {
	const s = files.scan(path), result = [];
	let item;
	while (item = s.read())
		result.push(item);
	return result.sort();
}

function deleteTree(files, path) {
	try {
		if (files.status(path).isFile())
			return void files.delete(path);
	}
	catch {
		return;
	}
	const s = files.scan(path);
	while (true) {
		const name = s.read();
		if (!name) break;
		deleteTree(files, path + "/" + name);
	}

	files.delete(path);
}

export {files as default, deleteTree, scan}
