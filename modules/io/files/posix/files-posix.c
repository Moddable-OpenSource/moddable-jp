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
 
#include "xsmc.h"
#include "xsHost.h"
#include "mc.xs.h"      // for xsID_ values

#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#define kCreateFilePermissions (0666)
#define kCreateDirectoryPermissions (0777)
#define kOpenDirFlags (O_RDONLY)

/*
	helpers
*/

#define throwIf(a) _throwIf(the, a)

static void _throwIf(xsMachine *the, int result)
{
	if (result < 0)
		xsUnknownError(strerror(errno));
}

#define getPath(s) _getPath(the, &s)

enum {
	kPathStateAny,
	kPathStateSlash,
	kPathStateDot,
	kPathStateDotDot,
};

static char *_getPath(xsMachine *the, xsSlot *slot)
{
	char *path = xsmcToString(*slot);
	char *p = path;
	int state = kPathStateSlash;

	if (0 == c_read8(p))
		return "./";

	while (true) {
		switch (c_read8(p++)) {
			case '.': 
				if (kPathStateSlash == state)
					state = kPathStateDot;
				else if (kPathStateDot == state)
					state = kPathStateDotDot;
				else
					state = 0;
				break;
			case '/':
				if ((kPathStateSlash == state) || (kPathStateDot == state) || (kPathStateDotDot == state))
					xsUnknownError("bad path");
				state = kPathStateSlash;
				break;
			case 0:
				if ((kPathStateDot == state) || (kPathStateDotDot == state))
					xsUnknownError("bad path");
				return path;
			default:
				state = kPathStateAny;
				break;
		}
	}

	return NULL;		// can never reach here
}

#if mxLinux
#include <linux/openat2.h>
#include <sys/syscall.h>

static int do_openat2(int dirfd, const char *pathname, int flags)
{
	struct open_how how = {0};
	how.flags = flags;
	how.mode = 0;
	how.resolve = RESOLVE_BENEATH;
	return syscall(SYS_openat2, dirfd, pathname, &how, sizeof(how));
}
#else /* !mxLinux */
static int do_openat2(int dirfd, const char *pathname, int flags)
{
	return openat(dirfd, pathname, flags);
}
#endif

/*
	File
*/

struct xsFileRecord {
	int		fd;
};
typedef struct xsFileRecord xsFileRecord;
typedef struct xsFileRecord *xsFile;

void xs_fileposix_destructor(void *data)
{
	xsFile f = data;
	if (f)
		close(f->fd);
}

#define getFile(slot) ((xsFile)xsmcGetHostChunkValidate(slot, xs_fileposix_destructor))->fd

void xs_fileposix(xsMachine *the)
{
	xsUnknownError("use openFile");
}

void xs_fileposix_close(xsMachine *the)
{
	if (!xsGetHostChunkIf(xsThis)) 
		return;
		
	close(getFile(xsThis));
	xsmcSetHostData(xsThis, NULL);
}

void xs_fileposix_read(xsMachine *the)
{
	int fd = getFile(xsThis);
	void *buffer;
	xsUnsignedValue length;
	int position = xsmcToInteger(xsArg(1));
	uint8_t returnLength = 0;

	int type = xsmcTypeOf(xsArg(0));
	if ((xsIntegerType == type) || (xsNumberType == type)) {
 		length = xsmcToInteger(xsArg(0));
		xsmcSetArrayBufferResizable(xsResult, NULL, length, length);
		xsArg(0) = xsResult;
		buffer = xsmcToArrayBuffer(xsResult);
	}
	else {
		xsResult = xsArg(0);
		xsmcGetBufferWritable(xsResult, &buffer, &length);
		returnLength = 1;
	}

	int result = pread(fd, buffer, length, position);
	throwIf(result);

	if (returnLength)
		xsmcSetInteger(xsResult, result);
	else if ((xsUnsignedValue)result != length)
		xsmcSetArrayBufferLength(xsResult, result);
}

void xs_fileposix_write(xsMachine *the)
{
	int fd = getFile(xsThis);
	int position = xsmcToInteger(xsArg(1));
	void *buffer;
	xsUnsignedValue length;
	xsmcGetBufferWritable(xsArg(0), &buffer, &length);

	throwIf(pwrite(fd, buffer, length, position));
}

void xs_fileposix_status(xsMachine *the)
{
	int fd = getFile(xsThis);
	struct stat statbuf;

	throwIf(fstat(fd, &statbuf));

	xsResult = xsArg(0);

	xsmcVars(1);
	xsmcSetInteger(xsVar(0), statbuf.st_size);
	xsmcSet(xsResult, xsID_size, xsVar(0));

	xsmcSetInteger(xsVar(0), statbuf.st_mode);
	xsmcSet(xsResult, xsID_mode, xsVar(0));
}

void xs_fileposix_setSize(xsMachine *the)
{
	int fd = getFile(xsThis);
	throwIf(ftruncate(fd, xsmcToInteger(xsArg(0))));
}

void xs_fileposix_flush(xsMachine *the)
{
	int fd = getFile(xsThis);
	throwIf(fsync(fd));
}

/*
	Directory
*/

struct xsDirectoryRecord {
	int		fd;
};
typedef struct xsDirectoryRecord xsDirectoryRecord;
typedef struct xsDirectoryRecord *xsDirectory;

void xs_direectoryposix_destructor(void *data)
{
	xsDirectory d = data;
	if (d)
		close(d->fd);
}

#define getDirectory(slot) ((xsDirectory)xsmcGetHostChunkValidate(slot, xs_direectoryposix_destructor))->fd

void xs_direectoryposix(xsMachine *the)
{
	xsTrace("Directory constructor is temporary for bootstrapping. Will throw in the future.\n");

	xsmcGet(xsResult, xsArg(0), xsID_path);
	char *path = xsmcToString(xsResult);		//@@ not checked with getPath.... for bootstrap
	xsDirectoryRecord d;
	
	struct stat buf;
	throwIf(stat(path, &buf));
	
	if (!S_ISDIR(buf.st_mode))
		xsUnknownError("not directory");

	d.fd = open(path, O_RDONLY);
	throwIf(d.fd);

	xsmcSetHostChunk(xsThis, &d, sizeof(d));
}

void xs_direectoryposix_close(xsMachine *the)
{
	if (!xsGetHostChunkIf(xsThis)) 
		return;
		
	close(getDirectory(xsThis));
	xsmcSetHostData(xsThis, NULL);
}

void xs_direectoryposix_openFile(xsMachine *the)
{
	int fd = getDirectory(xsThis);

	xsResult = xsNewHostInstance(xsArg(1));

	xsmcVars(1);
	xsmcGet(xsVar(0), xsArg(0), xsID_mode);
	char *modestr;
	int mode;
	if (xsUndefinedType == xsmcTypeOf(xsVar(0)))
		modestr = "r";
	else
		modestr = xsmcToString(xsVar(0));
	if (!c_strcmp(modestr, "r"))
		mode = O_RDONLY;
	else if (!c_strcmp(modestr, "r+"))
		mode = O_RDWR;
	else if (!c_strcmp(modestr, "w"))
		mode = O_WRONLY | O_CREAT | O_TRUNC;
	else if (!c_strcmp(modestr, "w+"))
		mode = O_RDWR | O_CREAT | O_TRUNC;
	else
		xsUnknownError("invalid mode");

	xsmcGet(xsVar(0), xsArg(0), xsID_path);
	char *path = getPath(xsVar(0));

	struct stat buf;
	int result = fstatat(fd, path, &buf, kOpenDirFlags);
	if (result < 0) {
		if (!(mode & O_CREAT))
			throwIf(result);
	}
	else {
		if (!S_ISREG(buf.st_mode))
			xsUnknownError("not file");
	}

	xsFileRecord f;
	f.fd = openat(fd, path, mode, kCreateFilePermissions);
	throwIf(f.fd);

	xsmcSetHostChunk(xsResult, &f, sizeof(f));
}

void xs_direectoryposix_openDirectory(xsMachine *the)
{
	int fd = getDirectory(xsThis);

	xsResult = xsNewHostInstance(xsArg(1));

	xsmcVars(1);
	xsmcGet(xsVar(0), xsArg(0), xsID_path);
	char *path = getPath(xsVar(0));

	struct stat buf;
	throwIf(fstatat(fd, path, &buf, 0));

	if (!S_ISDIR(buf.st_mode))
		xsUnknownError("not directory");

	xsDirectoryRecord d;
	d.fd = do_openat2(fd, path, kOpenDirFlags);
	throwIf(d.fd); 

	xsmcSetHostChunk(xsResult, &d, sizeof(d));
}

void xs_direectoryposix_delete(xsMachine *the)
{
	int fd = getDirectory(xsThis);
	char *path = getPath(xsArg(0));

	struct stat buf;
	int result = fstatat(fd, path, &buf, 0);
	if (result < 0) {
		if (ENOENT == errno) {
			xsmcSetFalse(xsResult);
			return;
		}
		throwIf(result);
	}

	int flags = S_ISDIR(buf.st_mode) ? AT_REMOVEDIR : 0;
	throwIf(unlinkat(fd, path, flags));
	xsmcSetTrue(xsResult);
}

void xs_direectoryposix_move(xsMachine *the)
{
	int fd = getDirectory(xsThis), toFD = fd;
	char *fromPath = getPath(xsArg(0));
	char *toPath = getPath(xsArg(1));

	if (xsmcArgc > 2)
		toFD = getDirectory(xsArg(2));

	fromPath = getPath(xsArg(0));		// refresh pointer

	throwIf(renameat(fd, fromPath, toFD, toPath));
}

void xs_direectoryposix_status(xsMachine *the)
{
	int fd = getDirectory(xsThis);
	struct stat statbuf;
	char *path = getPath(xsArg(0));
	int flags = 0;
	
	if (xsmcTest(xsArg(1))) {
		xsmcGet(xsResult, xsArg(1), xsID_resolveTarget);
		if (xsmcTest(xsResult))
			flags = AT_SYMLINK_NOFOLLOW;
	}

	throwIf(fstatat(fd, path, &statbuf, flags));

	xsResult = xsArg(2);

	xsmcVars(1);
	xsmcSetInteger(xsVar(0), statbuf.st_size);
	xsmcSet(xsResult, xsID_size, xsVar(0));

	xsmcSetInteger(xsVar(0), statbuf.st_mode);
	xsmcSet(xsResult, xsID_mode, xsVar(0));
}

void xs_direectoryposix_createDirectory(xsMachine *the)
{
	int fd = getDirectory(xsThis);
	char *path = getPath(xsArg(0));

	int result = mkdirat(fd, path, kCreateDirectoryPermissions);
	if (result < 0) {
		if (EEXIST == errno) {
			xsmcSetFalse(xsResult);
			return;
		}
		throwIf(result);
	}
	xsmcSetTrue(xsResult);
}

void xs_direectoryposix_createLink(xsMachine *the)
{
	int fd = getDirectory(xsThis);
	char *path = getPath(xsArg(0));
	char *target = getPath(xsArg(1));

	throwIf(symlinkat(target, fd, path));
}

void xs_direectoryposix_readLink(xsMachine *the)
{
	int fd = getDirectory(xsThis);
	char *path = getPath(xsArg(0));
	char *s;

	xsmcSetStringBuffer(xsResult, NULL, 1024);
	s = xsmcToString(xsResult);
	ssize_t length = readlinkat(fd, path, s, 1024 - 1);
	throwIf(length);
	s[length] = 0;
}

/*
	Scan
*/

struct xsScanRecord {
	int		fd;
	DIR		*dir;
};
typedef struct xsScanRecord xsScanRecord;
typedef struct xsScanRecord *xsScan;

void xs_direectoryposix_scan(xsMachine *the)
{
	int fd = getDirectory(xsThis);

	xsResult = xsNewHostInstance(xsArg(1));

	char *path = getPath(xsArg(0));

	struct stat buf;
	throwIf(fstatat(fd, path, &buf, 0));

	if (!S_ISDIR(buf.st_mode))
		xsUnknownError("not directory");

	xsScanRecord s;
	s.fd = openat(fd, path, kOpenDirFlags);
	throwIf(s.fd);

	s.dir = fdopendir(s.fd);
	xsmcSetHostChunk(xsResult, &s, sizeof(s));
}

void xs_scanposix_destructor(void *data)
{
	xsScan s = data;
	if (!s) return;

	closedir(s->dir);
	close(s->fd);
}

#define getScan(slot) ((xsScan)xsmcGetHostChunkValidate(slot, xs_scanposix_destructor))->dir

void xs_scanposix(xsMachine *the)
{
	xsUnknownError("use scan");
}

void xs_scanposix_close(xsMachine *the)
{
	if (!xsGetHostChunkIf(xsThis)) 
		return;

	xs_scanposix_destructor(xsmcGetHostChunkValidate(xsThis, xs_scanposix_destructor));
	xsmcSetHostData(xsThis, NULL);
}

void xs_scanposix_read(xsMachine *the)
{
	DIR *dir = getScan(xsThis);
	struct dirent *de;

	while (true) {
		de = readdir(dir);
		if (!de) {
			xs_scanposix_close(the);
			return;
		}
		if (c_strcmp(".", de->d_name) && c_strcmp("..", de->d_name))
			break;
	}
	xsmcSetString(xsResult, de->d_name);
}

/*
	Stat
*/

void xs_stat_isFile(xsMachine *the)
{
	xsmcGet(xsResult, xsThis, xsID_mode);
	xsmcSetBoolean(xsResult, S_ISREG(xsmcToInteger(xsResult)));
}

void xs_stat_isDirectory(xsMachine *the)
{
	xsmcGet(xsResult, xsThis, xsID_mode);
	xsmcSetBoolean(xsResult, S_ISDIR(xsmcToInteger(xsResult)));
}

void xs_stat_isSymbolicLink(xsMachine *the)
{
	xsmcGet(xsResult, xsThis, xsID_mode);
	xsmcSetBoolean(xsResult, S_ISLNK(xsmcToInteger(xsResult)));
}
