/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>


///////////////////////////////////////////////////////////////////////////////
// ReadMe
//
// This file implements the default application allocator. 
// You can replace this allocator.cpp file with a different one,
// you can define EASTL_USER_DEFINED_ALLOCATOR below to ignore this file,
// or you can modify the EASTL config.h file to redefine how allocators work.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_USER_DEFINED_ALLOCATOR // If the user hasn't declared that he has defined an allocator implementation elsewhere...

	namespace eastl
	{

		/// gDefaultAllocator
		/// Default global allocator instance. 
		EASTL_API allocator   gDefaultAllocator;
		EASTL_API allocator* gpDefaultAllocator = &gDefaultAllocator;

		EASTL_API allocator* GetDefaultAllocator()
		{
			return gpDefaultAllocator;
		}

		EASTL_API allocator* SetDefaultAllocator(allocator* pAllocator)
		{
			allocator* const pPrevAllocator = gpDefaultAllocator;
			gpDefaultAllocator = pAllocator;
			return pPrevAllocator;
		}

	} // namespace eastl


#endif // EASTL_USER_DEFINED_ALLOCATOR


void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line){
    return _aligned_malloc(size, 1);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line){
    return _aligned_malloc(size, alignment);
}
void operator delete[](void* p, size_t size){
    _aligned_free(p);
}








