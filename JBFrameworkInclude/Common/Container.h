#ifndef _CONTAINER_H_
#define _CONTAINER_H_


#include <../JBFramework/EASTL/slist.h>
#include <../JBFramework/EASTL/fixed_slist.h>
#include <../JBFramework/EASTL/list.h>
#include <../JBFramework/EASTL/fixed_list.h>

#include <../JBFramework/EASTL/vector.h>
#include <../JBFramework/EASTL/fixed_vector.h>

#include <../JBFramework/EASTL/deque.h>
#include <../JBFramework/EASTL/queue.h>
#include <../JBFramework/EASTL/stack.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        template<typename T, typename Allocator = EASTLAllocatorType>
        using SingleLinkedList = eastl::slist<T, Allocator>;

        template<typename T, size_t NodeCount, bool bEnableOverflow = false, typename OverflowAllocator = eastl::type_select<bEnableOverflow, EASTLAllocatorType, EASTLDummyAllocatorType>::type>
        using SingleLinkedFixedList = eastl::fixed_slist<T, NodeCount, bEnableOverflow, OverflowAllocator>;

        template<typename T, typename Allocator = EASTLAllocatorType>
        using DoubleLinkedList = eastl::list<T, Allocator>;

        template<typename T, size_t NodeCount, bool bEnableOverflow = false, typename OverflowAllocator = eastl::type_select<bEnableOverflow, EASTLAllocatorType, EASTLDummyAllocatorType>::type>
        using DoubleLinkedFixedList = eastl::fixed_list<T, NodeCount, bEnableOverflow, OverflowAllocator>;

        template<typename T, typename Allocator = EASTLAllocatorType>
        using Vector = eastl::vector<T, Allocator>;
        
        template<typename T, size_t NodeCount, bool bEnableOverflow = false, typename OverflowAllocator = eastl::type_select<bEnableOverflow, EASTLAllocatorType, EASTLDummyAllocatorType>::type>
        using FixedVector = eastl::fixed_vector<T, NodeCount, bEnableOverflow, OverflowAllocator>;

        template<typename T, unsigned SubarraySize = DEQUE_DEFAULT_SUBARRAY_SIZE(T), typename Allocator = EASTLAllocatorType>
        using Deque = eastl::deque<T, Allocator, SubarraySize>;

        template<typename T, typename Container>
        using FlexibleQueue = eastl::queue<T, Container>;
        template<typename T, unsigned SubarraySize = DEQUE_DEFAULT_SUBARRAY_SIZE(T), typename Allocator = EASTLAllocatorType>
        using Queue = FlexibleQueue<T, eastl::deque<T, Allocator, SubarraySize>>;

        template<typename T, typename Container>
        using FlexibleStack = eastl::stack<T, Container>;
        template<typename T, typename Allocator = EASTLAllocatorType>
        using Stack = FlexibleStack<T, Vector<T, Allocator>>;
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

