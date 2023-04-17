#ifndef _MEMORY_H_
#define _MEMORY_H_


#include <../JBFramework/EASTL/unique_ptr.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        template<typename T, typename Deleter = eastl::default_delete<T>>
        using UniquePtr = eastl::unique_ptr<T, Deleter>;
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif
