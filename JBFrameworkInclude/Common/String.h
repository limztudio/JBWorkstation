#ifndef _STRING_H_
#define _STRING_H_


#include <tchar.h>

#include <../JBFramework/EASTL/string.h>
#include <../JBFramework/EASTL/string_view.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        template <typename T = TCHAR>
        using String = eastl::basic_string<T>;

        template <typename T = TCHAR>
        using StringView = eastl::basic_string_view<T>;
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

