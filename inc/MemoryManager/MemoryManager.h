/**
 * @file	MemoryManager.h
 * @author  Giedrius Medzevicius <giedrius@8devices.com>
 *
 * @section LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Giedrius Medzevicius
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
 *
 * MemoryManager is a library that is used to dynamically allocate and free memory
 * on embedded devices. It can be implemented either as a wrapper library for
 * <stdlib.h> or as a custom stand-alone library.
 *
 */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include <stdint.h>

/**
 * Allocates a block of memory for an array of nmemb elements, each of them size bytes
 * long, and initializes all its bits to zero.
 *
 * @param nmemb Number of elements to allocate.
 * @param size Size of each element.
 *
 * @return A pointer to the memory block allocated by the function, or NULL if it
 * failed to do so.
 */
void* MemoryManager_calloc(uint32_t nmemb, uint32_t size);


/**
 * Allocates a block of size bytes of memory, returning a pointer to the beginning of
 * the block.
 *
 * @param size Size of the memory block, in bytes.
 *
 * @return A pointer to the memory block allocated by the function, or NULL if it
 * failed to do so.
 */
void* MemoryManager_malloc(uint32_t size);

/**
 * Changes the size of the memory block pointed to by ptr. If the function fails to
 * allocate the requested block of memory, a null pointer is returned, and the memory
 * block pointed to by argument ptr is not deallocated (it is still valid, and with
 * its contents unchanged).
 *
 * @param ptr Pointer to a memory block previously allocated with malloc, calloc or realloc.
 * @param size New size for the memory block, in bytes.
 *
 * @return A pointer to the reallocated memory block, which may be either the same as
 * ptr or a new location or a NULL on failure to allocate.
 */
void* MemoryManager_realloc(void* ptr, uint32_t size);


/**
 * A block of memory previously allocated by a call to malloc, calloc or realloc is
 * deallocated, making it available again for further allocations.
 *
 * @param ptr Pointer to a memory block previously allocated with malloc, calloc or realloc.
 *
 */
void MemoryManager_free(void* ptr);


#endif /* MEMORYMANAGER_H_ */
