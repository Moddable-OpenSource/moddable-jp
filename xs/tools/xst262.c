/*
 * Copyright (c) 2016-2024  Moddable Tech, Inc.
 *
 *   This file is part of the Moddable SDK Tools.
 * 
 *   The Moddable SDK Tools is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   The Moddable SDK Tools is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with the Moddable SDK Tools.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "xsAll.h"
#include "xsScript.h"
#include "xs.h"
#include "yaml.h"

extern void fxBuildAgent(xsMachine* the);
extern txScript* fxLoadScript(txMachine* the, txString path, txUnsigned flags);
extern void fxFulfillModuleFile(txMachine* the);
extern void fxRejectModuleFile(txMachine* the);
extern void fxRunLoop(txMachine* the);
extern void fxRunModuleFile(txMachine* the, txString path);
extern void fxRunProgramFile(txMachine* the, txString path, txUnsigned flags);
extern int main262(int argc, char* argv[]);

#ifdef mxMultipleThreads
	#define mxPoolSize 3
#else
	#define mxPoolSize 1
#endif

typedef struct sxContext txContext;
typedef struct sxPool txPool;
typedef struct sxResult txResult;

struct sxContext {
	txContext* next;
	txResult* result;
	yaml_document_t* document;
	yaml_node_t* includes;
	yaml_node_t* negative;
	char path[1];
};

struct sxPool {
	txContext* first;
	txInteger count;
	txCondition countCondition;
	txMutex countMutex;
	txResult* current;
	txMutex resultMutex;
#if mxWindows
	HANDLE threads[mxPoolSize];
#else
	pthread_t threads[mxPoolSize];
#endif
	char harnessPath[C_PATH_MAX];
	int testPathLength;
};

struct sxResult {
	txResult* next;
	txResult* parent;
	txResult* first;
	txResult* last;
	int testCount;
	int successCount;
	int pendingCount;
	char path[1];
};

static void fx_agent_stop(xsMachine* the);
static void fx_done(xsMachine* the);
static void fxCountResult(txPool* pool, txContext* context, int success, int pending);
static yaml_node_t *fxGetMappingValue(yaml_document_t* document, yaml_node_t* mapping, char* name);
static void fxPopResult(txPool* pool);
#ifdef mxMultipleThreads
static void fxPrintBusy(txPool* pool);
static void fxPrintClear(txPool* pool);
#endif
static void fxPrintResult(txPool* pool, txResult* result, int c);
static void fxPushResult(txPool* pool, char* path);
static void fxRunDirectory(txPool* pool, char* path);
static void fxRunFile(txPool* pool, char* path);
#if mxWindows
static unsigned int __stdcall fxRunFileThread(void* it);
#else
static void* fxRunFileThread(void* it);
#endif
static void fxRunContext(txPool* pool, txContext* context);
static int fxRunTestCase(txPool* pool, txContext* context, char* path, txUnsigned flags, int async, char* message);
static int fxStringEndsWith(const char *string, const char *suffix);

static void fxBuildCompartmentGlobals(txMachine* the);
static void fxLoadHook(txMachine* the);
static void fxRunProgramFileInCompartment(txMachine* the, txString path, txUnsigned flags);
static void fxRunModuleFileInCompartment(txMachine* the, txString path);

static int lockdown = 0;

int main262(int argc, char* argv[]) 
{
	txPool pool;
	char separator[2];
	char path[C_PATH_MAX];
	int error = 0;
	int argi = 1;
	int argj = 0;
	c_timeval from;
	c_timeval to;

	c_memset(&pool, 0, sizeof(txPool));
	fxCreateCondition(&(pool.countCondition));
	fxCreateMutex(&(pool.countMutex));
	fxCreateMutex(&(pool.resultMutex));
	{
	#if mxWindows
	#elif mxMacOSX
		pthread_attr_t attr; 
		pthread_t self = pthread_self();
   		size_t size = pthread_get_stacksize_np(self);
   		pthread_attr_init(&attr);
   		pthread_attr_setstacksize(&attr, size);
	#elif mxLinux
	#endif	
		for (argi = 0; argi < mxPoolSize; argi++) {
		#if mxWindows
			pool.threads[argi] = (HANDLE)_beginthreadex(NULL, 0, fxRunFileThread, &pool, 0, NULL);
		#elif mxMacOSX
			pthread_create(&(pool.threads[argi]), &attr, &fxRunFileThread, &pool);
		#else
			pthread_create(&(pool.threads[argi]), NULL, &fxRunFileThread, &pool);
		#endif
		}
	}
	
	fxInitializeSharedCluster(C_NULL);

	separator[0] = mxSeparator;
	separator[1] = 0;
	c_strcpy(path, "..");
	c_strcat(path, separator);
	c_strcat(path, "harness");
	if (!c_realpath(path, pool.harnessPath)) {
		fprintf(stderr, "### directory not found: %s\n", path);
		return 1;
	}
	c_strcat(pool.harnessPath, separator);
	if (!c_realpath(".", path)) {
		fprintf(stderr, "### directory not found: .\n");
		return 1;
	}
	c_strcat(path, separator);
	pool.testPathLength = mxStringLength(path);
	pool.current = NULL;
	fxPushResult(&pool, "");
	
	c_gettimeofday(&from, NULL);
	for (argi = 1; argi < argc; argi++) {
		if (!strcmp(argv[argi], "-l")) {
			lockdown = 1;
			continue;
		}
		if (!strcmp(argv[argi], "-lc")) {
			lockdown = -1;
			continue;
		}
		if (argv[argi][0] == '-')
			continue;
		if (c_realpath(argv[argi], path)) {
			argj++;
#if mxWindows
			DWORD attributes = GetFileAttributes(path);
			if (attributes != 0xFFFFFFFF) {
				if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
#else		
			struct stat a_stat;
			if (stat(path, &a_stat) == 0) {
				if (S_ISDIR(a_stat.st_mode)) {
#endif
					fxPushResult(&pool, path + pool.testPathLength);
					fxRunDirectory(&pool, path);
					fxPopResult(&pool);
				}
				else if (fxStringEndsWith(path, ".js") && !fxStringEndsWith(path, "_FIXTURE.js"))
					fxRunFile(&pool, path);
			}
		}
		else {
			fprintf(stderr, "### test not found: %s\n", argv[argi]);
			error = 1;
		}
	}
    fxLockMutex(&(pool.countMutex));
    while (pool.count > 0)
		fxSleepCondition(&(pool.countCondition), &(pool.countMutex));
	pool.count = -1;
	fxWakeAllCondition(&(pool.countCondition));
    fxUnlockMutex(&(pool.countMutex));
	for (argi = 0; argi < mxPoolSize; argi++) {
	#if mxWindows
		WaitForSingleObject(pool.threads[argi], INFINITE);
		CloseHandle(pool.threads[argi]);
	#else
		pthread_join(pool.threads[argi], NULL);
	#endif
	}
	
	fxTerminateSharedCluster(C_NULL);
	
	c_gettimeofday(&to, NULL);
	if (argj) {
		int seconds = to.tv_sec - from.tv_sec;
		int minutes = seconds / 60;
		int hours = minutes / 60;
		txResult* result = pool.current;
		int value;
		
	#ifdef mxMultipleThreads	
		fxPrintClear(&pool);
	#endif
		fprintf(stderr, "# %d:%.2d:%.2d\n", hours, minutes % 60, seconds % 60);
		
		if (result->testCount) {
			value = (10000 * result->successCount) / result->testCount;
			fprintf(stderr, "# %d.%.2d%%", value / 100, value % 100);
			if (result->pendingCount) {
				if (result->successCount + result->pendingCount == result->testCount)
					value = 10000 - value;
				else
					value = (10000 * result->pendingCount) / result->testCount;
				fprintf(stderr, " (%d.%.2d%%)", value / 100, value % 100);
				
				value = (10000 * result->successCount) / (result->testCount - result->pendingCount);
				fprintf(stderr, " [%d.%.2d%%]", value / 100, value % 100);
			}
		}
		else
			fprintf(stderr, "# 0.00%%");
		fxPrintResult(&pool, result, 0);
	}
	return error;
}

void fx_agent_stop(xsMachine* the)
{
	txAgentCluster* agentCluster = &gxAgentCluster;
	txAgent* agent = agentCluster->firstAgent;
	if (agent) {
		while (agent) {
			txAgent* next = agent->next;
			if (agent->thread) {
				#if mxWindows
					WaitForSingleObject(agent->thread, INFINITE);
					CloseHandle(agent->thread);
				#else
					pthread_join(agent->thread, NULL);
				#endif
			}
			c_free(agent);
			agent = next;
		}
		if (agentCluster->dataBuffer)
			c_free(agentCluster->dataBuffer);
		agentCluster->firstAgent = C_NULL;
		agentCluster->lastAgent = C_NULL;
		agentCluster->count = 0;
		agentCluster->dataBuffer = C_NULL;
		agentCluster->dataValue = 0;
		agentCluster->firstReport = C_NULL;
		agentCluster->lastReport = C_NULL;
	}
}

void fx_done(xsMachine* the)
{
	if ((xsToInteger(xsArgc) > 0) && (xsTest(xsArg(0))))
		*((txSlot*)the->rejection) = xsArg(0);
	else
		*((txSlot*)the->rejection) = xsUndefined;
}

void fxCountResult(txPool* pool, txContext* context, int success, int pending) 
{
	txResult* result = context->result;
    fxLockMutex(&(pool->resultMutex));
	while (result) {
		result->testCount++;
		result->successCount += success;
		result->pendingCount += pending;
		result = result->parent;
	}
    fxUnlockMutex(&(pool->resultMutex));
}

yaml_node_t *fxGetMappingValue(yaml_document_t* document, yaml_node_t* mapping, char* name)
{
	yaml_node_pair_t* pair = mapping->data.mapping.pairs.start;
	while (pair < mapping->data.mapping.pairs.top) {
		yaml_node_t* key = yaml_document_get_node(document, pair->key);
		if (!strcmp((char*)key->data.scalar.value, name)) {
			return yaml_document_get_node(document, pair->value);
		}
		pair++;
	}
	return NULL;
}

void fxPopResult(txPool* pool) 
{
	pool->current = pool->current->parent;
}

void fxPushResult(txPool* pool, char* path) 
{
	txResult* parent = pool->current;
	txResult* result = c_malloc(sizeof(txResult) + mxStringLength(path));
	if (!result) {
		c_exit(1);
	}
	result->next = NULL;
	result->parent = parent;
	result->first = NULL;
	result->last = NULL;
	result->testCount = 0;
	result->successCount = 0;
	result->pendingCount = 0;
	c_strcpy(result->path, path);
	if (parent) {
		if (parent->last)
			parent->last->next = result;
		else
			parent->first = result;
		parent->last = result;
	}
	pool->current = result;
}

#ifdef mxMultipleThreads
static int c = 0;

void fxPrintBusy(txPool* pool)
{
	fprintf(stderr, "\b\b\b\b\b\b\b\b# %6.6d", c++);	
}

void fxPrintClear(txPool* pool)
{
	fprintf(stderr, "\b\b\b\b\b\b\b\b");
	c++;
}
#endif

void fxPrintResult(txPool* pool, txResult* result, int c)
{
	int i = 0;
	while (i < c) {
		fprintf(stderr, "    ");
		i++;
	}
	fprintf(stderr, " %d/%d", result->successCount, result->testCount);
	if (result->pendingCount)
		fprintf(stderr, " (%d)", result->pendingCount);
	fprintf(stderr, " %s\n", result->path);
	result = result->first;
	c++;
	while (result) {
		fxPrintResult(pool, result, c);
		result = result->next;
	}
}

void fxRunDirectory(txPool* pool, char* path)
{
	typedef struct sxEntry txEntry;
	struct sxEntry {
		txEntry* nextEntry;
		char name[1];
	};

#if mxWindows
	size_t length;
	HANDLE findHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA findData;
	length = strlen(path);
	path[length] = '\\';
	length++;
	path[length] = '*';
	path[length + 1] = 0;
	findHandle = FindFirstFile(path, &findData);
	if (findHandle != INVALID_HANDLE_VALUE) {
		txEntry* entry;
		txEntry* firstEntry = NULL;
		txEntry* nextEntry;
		txEntry** address;
		do {
			if ((findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ||
				!strcmp(findData.cFileName, ".") ||
				!strcmp(findData.cFileName, ".."))
				continue;
			entry = malloc(sizeof(txEntry) + strlen(findData.cFileName));
			if (!entry)
				break;
			strcpy(entry->name, findData.cFileName);
			address = &firstEntry;
			while ((nextEntry = *address)) {
				if (strcmp(entry->name, nextEntry->name) < 0)
					break;
				address = &nextEntry->nextEntry;
			}
			entry->nextEntry = nextEntry;
			*address = entry;
		} while (FindNextFile(findHandle, &findData));
		FindClose(findHandle);
		while (firstEntry) {
			DWORD attributes;
			strcpy(path + length, firstEntry->name);
			attributes = GetFileAttributes(path);
			if (attributes != 0xFFFFFFFF) {
				if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
					fxPushResult(pool, firstEntry->name);
					fxRunDirectory(pool, path);
					fxPopResult(pool);
				}
				else if (fxStringEndsWith(path, ".js") && !fxStringEndsWith(path, "_FIXTURE.js"))
					fxRunFile(pool, path);
			}
			nextEntry = firstEntry->nextEntry;
			free(firstEntry);
			firstEntry = nextEntry;
		}
	}
#else
    DIR* dir;
	int length;
	dir = opendir(path);
	length = strlen(path);
	path[length] = '/';
	length++;
	if (dir) {
		struct dirent *ent;
		txEntry* entry;
		txEntry* firstEntry = NULL;
		txEntry* nextEntry;
		txEntry** address;
		while ((ent = readdir(dir))) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			entry = malloc(sizeof(txEntry) + strlen(ent->d_name));
			if (!entry)
				break;
			strcpy(entry->name, ent->d_name);
			address = &firstEntry;
			while ((nextEntry = *address)) {
				if (strcmp(entry->name, nextEntry->name) < 0)
					break;
				address = &nextEntry->nextEntry;
			}
			entry->nextEntry = nextEntry;
			*address = entry;
		}
		closedir(dir);
		while (firstEntry) {
			struct stat a_stat;
			strcpy(path + length, firstEntry->name);
			if (stat(path, &a_stat) == 0) {
				if (S_ISDIR(a_stat.st_mode)) {
					fxPushResult(pool, firstEntry->name);
					fxRunDirectory(pool, path);
					fxPopResult(pool);
				}
				else if (fxStringEndsWith(path, ".js") && !fxStringEndsWith(path, "_FIXTURE.js"))
					fxRunFile(pool, path);
			}
			nextEntry = firstEntry->nextEntry;
			free(firstEntry);
			firstEntry = nextEntry;
		}
	}
#endif
}

void fxRunFile(txPool* pool, char* path)
{
	txContext* context = c_malloc(sizeof(txContext) + c_strlen(path));
	txContext** address;
	txContext* former;
	if (!context) return;
	c_memset(context, 0, sizeof(txContext));
	context->result = pool->current;
	c_strcpy(context->path, path);
    fxLockMutex(&(pool->countMutex));
    while (pool->count == mxPoolSize)
		fxSleepCondition(&(pool->countCondition), &(pool->countMutex));
	address = &(pool->first);	
	while ((former = *address))
		address = &(former->next);
	*address = context;
	pool->count++;
	fxWakeAllCondition(&(pool->countCondition));
    fxUnlockMutex(&(pool->countMutex));
}

#if mxWindows
unsigned int __stdcall fxRunFileThread(void* it)
#else
void* fxRunFileThread(void* it)
#endif
{
	txPool* pool = it;
	txContext* context;
	for (;;) {
    	fxLockMutex(&(pool->countMutex));
    	while (pool->count == 0)
			fxSleepCondition(&(pool->countCondition), &(pool->countMutex));
		if (pool->count > 0) {
			context = pool->first;
			pool->first = context->next;
			pool->count--;
			fxWakeAllCondition(&(pool->countCondition));
			fxUnlockMutex(&(pool->countMutex));
			
			fxRunContext(pool, context);
			c_free(context);
		}
		else if (pool->count < 0) {
			fxUnlockMutex(&(pool->countMutex));
			break;
		}
	}
#if mxWindows
	return 0;
#else
	return NULL;
#endif
}

void fxRunContext(txPool* pool, txContext* context)
{
	txAgentCluster* agentCluster = &gxAgentCluster;
	char* path = context->path;
	FILE* file = NULL;
	size_t size;
	char* buffer = NULL;
	char* begin;
	char* end;
	yaml_parser_t _parser;
	yaml_parser_t* parser = NULL;
	yaml_document_t _document;
	yaml_document_t* document = NULL;
	yaml_node_t* root;
	yaml_node_t* value;
	int async = 0;
	int atomics = 0;
	int sloppy = 1;
	int strict = 1;
	int module = 0;
	int pending = 0;
	char message[1024];
	
	file = fopen(path, "rb");
	if (!file) goto bail;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	if (!size) goto bail;
	fseek(file, 0, SEEK_SET);
	buffer = malloc(size + 1);
	if (!buffer) goto bail;
	if (fread(buffer, 1, size, file) != size) goto bail;	
	buffer[size] = 0;
	fclose(file);
	file = NULL;
	
	begin = strstr(buffer, "/*---");
	if (!begin) goto bail;
	begin += 5;
	end = strstr(begin, "---*/");
	if (!end) goto bail;

	if (!yaml_parser_initialize(&_parser)) goto bail;
	parser = &_parser;
	yaml_parser_set_input_string(parser, (unsigned char *)begin, end - begin);
	if (!yaml_parser_load(parser, &_document)) goto bail;
	document = &_document;
	root = yaml_document_get_root_node(document);
	if (!root) goto bail;
		
	context->document = document;
	context->includes = fxGetMappingValue(document, root, "includes");
	value = fxGetMappingValue(document, root, "negative");
	if (value)
		context->negative = fxGetMappingValue(document, value, "type");
	else
		context->negative = NULL;
	
	value = fxGetMappingValue(document, root, "flags");
	if (value) {
		yaml_node_item_t* item = value->data.sequence.items.start;
		while (item < value->data.sequence.items.top) {
			yaml_node_t* node = yaml_document_get_node(document, *item);
			if (!strcmp((char*)node->data.scalar.value, "async")) {
				async = 1;
			}
			else if (!strcmp((char*)node->data.scalar.value, "onlyStrict")) {
				sloppy = 0;
				strict = 1;
				module = 0;
			}
			else if (!strcmp((char*)node->data.scalar.value, "noStrict")) {
				sloppy = 1;
				strict = 0;
				module = 0;
			}
			else if (!strcmp((char*)node->data.scalar.value, "module")) {
				sloppy = 0;
				strict = 0;
				module = 1;
			}
			else if (!strcmp((char*)node->data.scalar.value, "raw")) {
				sloppy = 1;
				strict = 0;
				module = 0;
			}
			else if (!strcmp((char*)node->data.scalar.value, "CanBlockIsFalse")) {
				sloppy = 0;
				strict = 0;
				module = 0;
				pending = 1;
			}
			item++;
		}
	}

	value = fxGetMappingValue(document, root, "features");
	if (value) {
		yaml_node_item_t* item = value->data.sequence.items.start;
		while (item < value->data.sequence.items.top) {
			yaml_node_t* node = yaml_document_get_node(document, *item);
			if (0
 			||	!strcmp((char*)node->data.scalar.value, "Array.fromAsync")
 			||	!strcmp((char*)node->data.scalar.value, "FinalizationRegistry.prototype.cleanupSome")
  			||	!strcmp((char*)node->data.scalar.value, "ShadowRealm")
 			||	!strcmp((char*)node->data.scalar.value, "Temporal")
 			||	!strcmp((char*)node->data.scalar.value, "arbitrary-module-namespace-names")
 			||	!strcmp((char*)node->data.scalar.value, "decorators")
 			||	!strcmp((char*)node->data.scalar.value, "import-assertions")
 			||	!strcmp((char*)node->data.scalar.value, "import-attributes")
 			||	!strcmp((char*)node->data.scalar.value, "iterator-helpers")
 			||	!strcmp((char*)node->data.scalar.value, "json-modules")
 			||	!strcmp((char*)node->data.scalar.value, "regexp-duplicate-named-groups")
#ifndef mxRegExpUnicodePropertyEscapes
 			||	!strcmp((char*)node->data.scalar.value, "regexp-unicode-property-escapes")
#endif
 			||	!strcmp((char*)node->data.scalar.value, "set-methods")
			) {
				sloppy = 0;
				strict = 0;
				module = 0;
				pending = 1;
			}
			if (!strcmp((char*)node->data.scalar.value, "Atomics")) {
				atomics = 1;
			}
			item++;
		}
	}
	if (atomics) {
		fxLockMutex(&(agentCluster->mainMutex));
	}

	if (lockdown) {
		if (sloppy) {
			sloppy = 0;
			pending = 1;
		}
	}
	
	if (sloppy) {
#ifdef mxMultipleThreads
		if (!fxRunTestCase(pool, context, path, mxProgramFlag | mxDebugFlag, async, message)) {
			fxPrintClear(pool);
			fprintf(stderr, "### %s (sloppy): %s\n", path + pool->testPathLength, message);
		}
		else
			fxPrintBusy(pool);
#else
		fprintf(stderr, "### %s (sloppy): ", path + pool->testPathLength);
		fxRunTestCase(pool, context, path, mxProgramFlag | mxDebugFlag, async, message);
		fprintf(stderr, "%s\n", message);
#endif
	}
	if (strict) {
#ifdef mxMultipleThreads
		if (!fxRunTestCase(pool, context, path, mxProgramFlag | mxDebugFlag | mxStrictFlag, async, message)) {
			fxPrintClear(pool);
			fprintf(stderr, "### %s (strict): %s\n", path + pool->testPathLength, message);
		}
		else
			fxPrintBusy(pool);
#else
		fprintf(stderr, "### %s (strict): ", path + pool->testPathLength);
		fxRunTestCase(pool, context, path, mxProgramFlag | mxDebugFlag | mxStrictFlag, async, message);
		fprintf(stderr, "%s\n", message);
#endif
	}
	if (module) {
#ifdef mxMultipleThreads
		if (!fxRunTestCase(pool, context, path, 0, async, message)) {
			fxPrintClear(pool);
			fprintf(stderr, "### %s (module): %s\n", path + pool->testPathLength, message);
		}
		else
			fxPrintBusy(pool);
#else
		fprintf(stderr, "### %s (module): ", path + pool->testPathLength);
		fxRunTestCase(pool, context, path, 0, async, message);
		fprintf(stderr, "%s\n", message);
#endif
	}
	if (atomics) {
		fxUnlockMutex(&(agentCluster->mainMutex));
	}

	if (pending) {
		fxCountResult(pool, context, 0, 1);
#ifdef mxMultipleThreads
		fxPrintBusy(pool);
#else
		fprintf(stderr, "### %s: SKIP\n", path + pool->testPathLength);
#endif
	}
bail:	
	context->negative = NULL;
	context->includes = NULL;
	context->document = NULL;
	if (document)
		yaml_document_delete(document);
	if (parser)
		yaml_parser_delete(parser);
	if (buffer)
		free(buffer);
	if (file)
		fclose(file);
}

int fxRunTestCase(txPool* pool, txContext* context, char* path, txUnsigned flags, int async, char* message)
{
	xsCreation _creation = {
		16 * 1024 * 1024, 	/* initialChunkSize */
		16 * 1024 * 1024, 	/* incrementalChunkSize */
		1 * 1024 * 1024, 	/* initialHeapCount */
		1 * 1024 * 1024, 	/* incrementalHeapCount */
		256 * 1024, 		/* stackCount */
		1024, 				/* initialKeyCount */
		1024,				/* incrementalKeyCount */
		1993, 				/* nameModulo */
		127,				/* symbolModulo */
		64 * 1024,			/* parserBufferSize */
		1993,				/* parserTableModulo */
	};
	xsCreation* creation = &_creation;
	xsMachine* machine;
	char buffer[C_PATH_MAX];
	int success = 0;	
	machine = xsCreateMachine(creation, "xst262", NULL);
	xsBeginHost(machine);
	{
		xsVars(5);
		xsTry {
			fxBuildAgent(the);
			c_strcpy(buffer, pool->harnessPath);
			c_strcat(buffer, "sta.js");
			fxRunProgramFile(the, buffer, mxProgramFlag | mxDebugFlag);
			c_strcpy(buffer, pool->harnessPath);
			c_strcat(buffer, "assert.js");
			fxRunProgramFile(the, buffer, mxProgramFlag | mxDebugFlag);
			if (async) {
				xsResult = xsNewHostFunction(fx_done, 1);
				xsSet(xsGlobal, xsID("$DONE"), xsResult);
				xsVar(0) = xsString("Test did not run to completion");
			}
			else
				xsVar(0) = xsUndefined;
			if (context->includes) {
				yaml_node_item_t* item = context->includes->data.sequence.items.start;
				while (item < context->includes->data.sequence.items.top) {
					yaml_node_t* node = yaml_document_get_node(context->document, *item);
					c_strcpy(buffer, pool->harnessPath);
					c_strcat(buffer, (char*)node->data.scalar.value);
					fxRunProgramFile(the, buffer, mxProgramFlag | mxDebugFlag);
					item++;
				}
			}
			mxPop();
			if (lockdown)
				xsCall0(xsGlobal, xsID("lockdown"));
			the->rejection = &xsVar(0);
			if (flags) {
				if (lockdown < 0)
					fxRunProgramFileInCompartment(the, path, flags);
				else
					fxRunProgramFile(the, path, flags);
			}
			else {
				if (lockdown < 0)
					fxRunModuleFileInCompartment(the, path);
				else
					fxRunModuleFile(the, path);
			}
			fxRunLoop(the);
			if (xsTest(xsVar(0))) 
				xsThrow(xsVar(0));
			if (context->negative) {
				snprintf(message, 1024, "# Expected a %s but got no errors", context->negative->data.scalar.value);
			}
			else {
				snprintf(message, 1024, "OK");
				success = 1;
			}
		}
		xsCatch {
			if (context->negative) {
				txString name;
				xsResult = xsGet(xsException, xsID("constructor"));
				name = xsToString(xsGet(xsResult, xsID("name")));
				if (strcmp(name, (char*)context->negative->data.scalar.value))
					snprintf(message, 1024, "# Expected a %s but got a %s", context->negative->data.scalar.value, name);
				else {
					snprintf(message, 1024, "OK");
					success = 1;
				}
			}
			else {
				snprintf(message, 1024, "# %s", xsToString(xsException));
			}
		}
	}
	xsEndHost(machine);
	if (machine->abortStatus) {
		success = 0;
		if ((machine->abortStatus == XS_NOT_ENOUGH_MEMORY_EXIT) || (machine->abortStatus == XS_STACK_OVERFLOW_EXIT)) {
			if (context->negative) {
				if (!strcmp("RangeError", (char*)context->negative->data.scalar.value)) {
					snprintf(message, 1024, "OK");
					success = 1;
				}
			}
		}
		if (!success) {
			char *why = (machine->abortStatus <= XS_UNHANDLED_REJECTION_EXIT) ? gxAbortStrings[machine->abortStatus] : "unknown";
			snprintf(message, 1024, "# %s", why);
			success = 0;
		}
	}
	fx_agent_stop(machine);
	xsDeleteMachine(machine);

	fxCountResult(pool, context, success, 0);
	return success;
}

int fxStringEndsWith(const char *string, const char *suffix)
{
	size_t stringLength = strlen(string);
	size_t suffixLength = strlen(suffix);
	return (stringLength >= suffixLength) && (0 == strcmp(string + (stringLength - suffixLength), suffix));
}

void fxBuildCompartmentGlobals(txMachine* the)
{
	txSlot* realm = mxProgram.value.reference->next->value.module.realm;
	txSlot* global = mxRealmGlobal(realm)->value.reference;
	txSlot* environment = mxRealmClosures(realm)->value.reference;
	txSlot* slot;
	txSlot* property;
	
	mxPush(mxObjectPrototype);
	property = fxLastProperty(the, fxNewHostObject(the, NULL));
	slot = global->next->next;
	while (slot) {
		if (slot->ID > mxID(_globalThis)) {
// 			fprintf(stderr, "%s\n", fxGetKeyName(the, slot->ID));
			property = fxNextSlotProperty(the, property, slot, slot->ID, XS_NO_FLAG);
		}
		slot = slot->next;
	}
	slot = environment->next->next;
	while (slot) {
		if (slot->kind == XS_CLOSURE_KIND) {
// 			fprintf(stderr, "%s\n", fxGetKeyName(the, slot->ID));
			property = fxNextSlotProperty(the, property, slot->value.closure, slot->ID, XS_NO_FLAG);
		}
		slot = slot->next;
	}
	mxPullSlot(mxResult);
}

void fxLoadHook(txMachine* the)
{
	xsVars(2);
	xsVar(0) = xsNewObject();
	xsSet(xsVar(0), xsID("namespace"), xsArg(0));
	xsVar(1) = xsGet(xsGlobal, xsID("Promise"));
	xsResult = xsCall1(xsVar(1), xsID("resolve"), xsVar(0));
}

void fxRunModuleFileInCompartment(txMachine* the, txString path)
{
	xsVar(1) = xsNewObject();
	fxBuildCompartmentGlobals(the);
	xsSet(xsVar(1), xsID("globals"), xsResult);
	xsVar(2) = xsNewHostFunction(fxLoadHook, 1);
	xsSet(xsVar(1), xsID("loadHook"), xsVar(2));
	xsVar(2) = xsNew1(xsGlobal, xsID("Compartment"), xsVar(1));
	xsResult = xsCall1(xsVar(2), xsID("import"), xsString(path));
	xsVar(3) = xsNewHostFunction(fxFulfillModuleFile, 1);
	xsVar(4) = xsNewHostFunction(fxRejectModuleFile, 1);
	xsCall2(xsResult, xsID("then"), xsVar(3), xsVar(4));
}

void fxRunProgramFileInCompartment(txMachine* the, txString path, txUnsigned flags)
{
	xsVar(1) = xsNewObject();
	fxBuildCompartmentGlobals(the);
	xsSet(xsVar(1), xsID("globals"), xsResult);

	xsVar(2) = xsNewHostFunction(fxLoadHook, 1);
	xsSet(xsVar(1), xsID("loadHook"), xsVar(2));
	xsVar(2) = xsNew1(xsGlobal, xsID("Compartment"), xsVar(1));
	{
		txSlot* module = mxVarv(2)->value.reference;
		txSlot* realm = mxModuleInstanceInternal(module)->value.module.realm;
		txScript* script = fxLoadScript(the, path, flags);

		xsVar(1) = xsGet(xsGlobal, xsID("$262"));
		xsVar(1) = xsGet(xsVar(1), xsID("evalScript"));
		mxFunctionInstanceHome(mxVarv(1)->value.reference)->value.home.module = module;
			
		mxModuleInstanceInternal(module)->value.module.id = fxID(the, path);
		fxRunScript(the, script, mxRealmGlobal(realm), C_NULL, mxRealmClosures(realm)->value.reference, C_NULL, module);
	}
}








