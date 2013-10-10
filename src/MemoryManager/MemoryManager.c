/**
 * @file	MemoryManager.c
 * @author  Giedrius Medzevicius <giedrius@8devices.com>
 *
 * @section LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 UAB 8devices
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 * MemoryManager is currently only a <stdlib.h> wrapper library.
 */


#include "MemoryManager/MemoryManager.h"

#include <stdlib.h>

#ifdef MEMORYMANAGER_STDLIB

void* MemoryManager_calloc(uint32_t nmemb, uint32_t size) {
	// Force memory allocation on zero size request
	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;

	return calloc(nmemb, size);
}

void* MemoryManager_malloc(uint32_t size) {
	// Force memory allocation on zero size request
	if (size == 0)
		size = 1;

	void* ptr = malloc(size);
	//printf("malloc(): size=%u ptr=%X\n", size, (uint32_t)ptr);
	return ptr;
}

void* MemoryManager_realloc(void* ptr, uint32_t size) {
	// Force memory allocation
	if (size == 0)
		size = 1;

	void* ptr_new = realloc(ptr, size);
	//printf("realloc(): size=%u ptr=%X ptr_new=%X\n", size, (uint32_t)ptr, (uint32_t)ptr_new);

	if ((ptr_new != NULL) && (ptr != ptr_new))
		MemoryManager_free(ptr);	// stdlib bug (in theory it should deallocate old pointer)

	return ptr_new;
}

void MemoryManager_free(void* ptr) {
	if (ptr == NULL) return;	// This fixes stlib from crashing
	//printf("free(): ptr=%X\n", (uint32_t)ptr);
	free(ptr);
}

#else

#ifndef MEMORYMANAGER_BLOCKSIZE
#define MEMORYMANAGER_BLOCKSIZE 	16
#endif

#ifndef MEMORYMANAGER_MEMORYSIZE
#define MEMORYMANAGER_MEMORYSIZE 	2048
#endif

#define MEMORYMANAGER_BLOCKCOUNT	(MEMORYMANAGER_MEMORYSIZE/MEMORYMANAGER_BLOCKSIZE)


static uint8_t MemoryManager_blockIDs[MEMORYMANAGER_BLOCKCOUNT];
static uint8_t MemoryManager_memory[MEMORYMANAGER_MEMORYSIZE];

void* MemoryManager_calloc(uint32_t nmemb, uint32_t size) {
	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;

	uint32_t totalSize = nmemb*size;

	void *ptr = MemoryManager_malloc(totalSize);

	if (ptr == NULL)
		return NULL;

	uint8_t *tmpPtr = ptr;
	while (totalSize--)
		*tmpPtr++ = 0;

	return ptr;
}

void* MemoryManager_malloc(uint32_t size) {
	if (size == 0)
		size = 1;

	uint32_t blockCount = (size+MEMORYMANAGER_BLOCKSIZE-1)/MEMORYMANAGER_BLOCKSIZE;

	uint32_t startBlock = 0;
	uint32_t freeBlocks = 0;
	uint32_t i;
	for (i=0; i<MEMORYMANAGER_BLOCKCOUNT; i++) {
		if (MemoryManager_blockIDs[i] == 0) {
			if (freeBlocks == 0)
				startBlock = i;
			freeBlocks++;
		} else {
			freeBlocks = 0;
		}

		if (freeBlocks == blockCount)
			break;
	}

	if (freeBlocks < blockCount)
		return NULL;

	uint32_t blockID = startBlock + 1;
	for (i=0; i<blockCount; i++) {
		MemoryManager_blockIDs[startBlock+i] = blockID;
	}

	return &MemoryManager_memory[startBlock*MEMORYMANAGER_BLOCKSIZE];
}

void* MemoryManager_realloc(void* ptr, uint32_t size) {
	if (ptr == NULL)
		return MemoryManager_malloc(size);

	if (size == 0) {
		MemoryManager_free(ptr);
		return ptr;
	}

	if (((uint32_t)ptr < (uint32_t)&MemoryManager_memory[0])
			|| ((uint32_t)ptr >= (uint32_t)&MemoryManager_memory[MEMORYMANAGER_MEMORYSIZE]))
		return NULL;

	uint32_t blockID = MemoryManager_blockIDs[((uint32_t)ptr-(uint32_t)&MemoryManager_memory[0])/MEMORYMANAGER_BLOCKSIZE];

	if (blockID == 0) // Asked to realloc in already empty space
		return MemoryManager_malloc(size);

	// Calculate block start and size
	uint32_t oldStartBlock = blockID-1;
	uint32_t oldBlockCount = 0;
	while (MemoryManager_blockIDs[oldStartBlock++] == blockID) {
		oldBlockCount++;
	}
	oldStartBlock = blockID-1;


	uint32_t newBlockCount = (size+MEMORYMANAGER_BLOCKSIZE-1)/MEMORYMANAGER_BLOCKSIZE;

	if (newBlockCount <= oldBlockCount) {
		// Shrink used memory
		uint32_t i;
		for (i=newBlockCount; i<oldBlockCount; i++) {
			MemoryManager_blockIDs[oldStartBlock+i] = 0;
		}

		return ptr;
	} else {
		uint32_t availBlockCount = oldBlockCount;
		uint32_t i;
		for (i=oldStartBlock+oldBlockCount; i<MEMORYMANAGER_BLOCKCOUNT; i++) {
			if (MemoryManager_blockIDs[i] == 0) {
				availBlockCount++;
			} else {
				break;
			}

			if (availBlockCount == newBlockCount)
				break;
		}

		if (newBlockCount <= availBlockCount) {
			// Extend used memory
			uint32_t i;
			for (i=oldBlockCount; i<newBlockCount; i++) {
				MemoryManager_blockIDs[oldStartBlock+i] = blockID;
			}

			return ptr;
		} else {
			// try to malloc
			void *newPtr = MemoryManager_malloc(size);
			uint8_t *writePtr = (uint8_t*)newPtr;
			uint8_t *readPtr = (uint8_t*)ptr;
			uint32_t dataSize = oldBlockCount*MEMORYMANAGER_BLOCKSIZE;

			while (dataSize--) {
				*writePtr++ = *readPtr++;
			}

			MemoryManager_free(ptr);
			return newPtr;
		}

	}
}

void MemoryManager_free(void* ptr) {
	if (((uint32_t)ptr < (uint32_t)&MemoryManager_memory[0])
			|| ((uint32_t)ptr >= (uint32_t)&MemoryManager_memory[MEMORYMANAGER_MEMORYSIZE]))
		return;

	uint32_t blockID = MemoryManager_blockIDs[((uint32_t)ptr-(uint32_t)&MemoryManager_memory[0])/MEMORYMANAGER_BLOCKSIZE];

	if (blockID == 0) // Asked to free empty memory
		return;

	uint32_t i = blockID-1;
	while (MemoryManager_blockIDs[i] == blockID) {
		MemoryManager_blockIDs[i++] = 0;
	}
}


#endif
