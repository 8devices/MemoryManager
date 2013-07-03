MemoryManager
=============

Dynamic memory managing library for embedded systems.

Primarily developed as a wrapper library would allow easy portability in the future of currently existing 
embedded system projects that use dynamic memory allocation. This library is only intended for memory
allocation and deallocation and has 4 methods to do so:

MemoryManager_calloc
MemoryManager_malloc
MemoryManager_realloc
MemoryManager_free

All of these 4 methods (should) do the same as specified in C stdlib documentation (http://www.cplusplus.com/reference/cstdlib/).
MemoryManager library can be copied into existing projects and compiled in-situ or included as separately compiled library.
