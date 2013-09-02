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

//#include "stdio.h"

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
		MemoryManager_free(ptr);	// XXX: seems like a stdlib bug (in theory it should deallocate old pointer)

	return ptr_new;
}

void MemoryManager_free(void* ptr) {
	if (ptr == NULL) return;	// This fixes stlib from crashing
	//printf("free(): ptr=%X\n", (uint32_t)ptr);
	free(ptr);
}
