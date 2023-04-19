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

#include <../JBFramework/EASTL/set.h>
#include <../JBFramework/EASTL/fixed_set.h>

#include <../JBFramework/EASTL/map.h>
#include <../JBFramework/EASTL/fixed_map.h>

#include <../JBFramework/EASTL/hash_set.h>
#include <../JBFramework/EASTL/fixed_hash_set.h>

#include <../JBFramework/EASTL/hash_map.h>
#include <../JBFramework/EASTL/fixed_hash_map.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        template<typename T, typename Allocator = EASTLAllocatorType>
        using SingleLinkedList = eastl::slist<T, Allocator>;

        template<typename T, size_t NodeCount, bool bEnableOverflow, typename OverflowAllocator = eastl::type_select<bEnableOverflow, EASTLAllocatorType, EASTLDummyAllocatorType>::type>
        using SingleLinkedFixedList = eastl::fixed_slist<T, NodeCount, bEnableOverflow, OverflowAllocator>;

        template<typename T, typename Allocator = EASTLAllocatorType>
        using DoubleLinkedList = eastl::list<T, Allocator>;

        template<typename T, size_t NodeCount, bool bEnableOverflow, typename OverflowAllocator = eastl::type_select<bEnableOverflow, EASTLAllocatorType, EASTLDummyAllocatorType>::type>
        using DoubleLinkedFixedList = eastl::fixed_list<T, NodeCount, bEnableOverflow, OverflowAllocator>;

        template<typename T, typename Allocator = EASTLAllocatorType>
        using Vector = eastl::vector<T, Allocator>;
        
        template<typename T, size_t NodeCount, bool bEnableOverflow, typename OverflowAllocator = eastl::type_select<bEnableOverflow, EASTLAllocatorType, EASTLDummyAllocatorType>::type>
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

        template<typename K, typename Compare = eastl::less<K>, typename Allocator = EASTLAllocatorType>
        using Set = eastl::set<K, Compare, Allocator>;
        template<typename K, size_t NodeCount, bool bEnableOverflow, typename Compare = eastl::less<K>, typename OverflowAllocator = EASTLAllocatorType>
        using FixedSet = eastl::fixed_set<K, NodeCount, bEnableOverflow, Compare, OverflowAllocator>;

        template<typename K, typename V, typename Compare = eastl::less<K>, typename Allocator = EASTLAllocatorType>
        using Map = eastl::map<K, V, Compare, Allocator>;
        template<typename K, typename V, size_t NodeCount, bool bEnableOverflow, typename Compare = eastl::less<K>, typename OverflowAllocator = EASTLAllocatorType>
        using FixedMap = eastl::fixed_map<K, V, NodeCount, bEnableOverflow, Compare, OverflowAllocator>;

        template<typename K, typename Hash = eastl::hash<K>, typename Predicate = eastl::equal_to<K>, typename Allocator = EASTLAllocatorType>
        using HashSet = eastl::hash_set<K, Hash, Predicate, Allocator, false>;
        template<typename K, size_t NodeCount, bool bEnableOverflow, size_t BucketCount = NodeCount << 1, typename Hash = eastl::hash<K>, typename Predicate = eastl::equal_to<K>, typename OverflowAllocator = EASTLAllocatorType>
        using FixedHashSet = eastl::fixed_hash_set<K, NodeCount, BucketCount, bEnableOverflow, Hash, Predicate, false, OverflowAllocator>;

        template<typename K, typename V, typename Hash = eastl::hash<K>, typename Predicate = eastl::equal_to<K>, typename Allocator = EASTLAllocatorType>
        using HashMap = eastl::hash_map<K, V, Hash, Predicate, Allocator, false>;
        template<typename K, typename V, size_t NodeCount, bool bEnableOverflow, size_t BucketCount = NodeCount << 1, typename Hash = eastl::hash<K>, typename Predicate = eastl::equal_to<K>, typename OverflowAllocator = EASTLAllocatorType>
        using FixedHashMap = eastl::fixed_hash_map<K, V, NodeCount, BucketCount, bEnableOverflow, Hash, Predicate, false, OverflowAllocator>;
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

