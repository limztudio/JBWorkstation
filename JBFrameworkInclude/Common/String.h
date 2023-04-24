#ifndef _STRING_H_
#define _STRING_H_


#include <tchar.h>
#include <format>

#include <../JBFramework/EASTL/string.h>
#include <../JBFramework/EASTL/string_view.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        template<typename T = TCHAR>
        using String = eastl::basic_string<T>;

        template<typename T = TCHAR>
        using StringView = eastl::basic_string_view<T>;
        
        namespace __hidden{
            template<typename T>
            inline String<char> ToString(char, T V){ return eastl::to_string(V); }
            template<typename T>
            inline String<wchar_t> ToString(wchar_t, T V){ return eastl::to_wstring(V); }            
        };
        template<typename V, typename T = TCHAR>
        inline String<T> ToString(V Value){ return __hidden::ToString(T(), Value); }

        template<typename T = TCHAR, typename... ARGS>
        inline String<T> Format(const T* Text, ARGS&&... Args){
            static thread_local std::locale Locale = {};
            std::basic_string<T> Result = std::format<T>(Locale, Text, std::forward<ARGS>(Args)...);
            return String<T>(Result.begin(), Result.end());
        }
        template<typename T = TCHAR, typename... ARGS>
        inline String<T> Format(const StringView<T>& Text, ARGS&&... Args){
            static thread_local std::locale Locale = {};
            std::basic_string<T> Result = std::format<T>(Locale, Text.data(), std::forward<ARGS>(Args)...);
            return String<T>(Result.begin(), Result.end());
        }

        template<typename T = TCHAR, typename... ARGS>
        inline String<T> FastFormat(const T* Text, ARGS&&... Args){
            String<T> Result;
            Result.sprintf(Text, std::forward<ARGS>(Args)...);
            return std::move(Result);
        }
        template<typename T = TCHAR, typename... ARGS>
        inline String<T> FastFormat(const StringView<T>& Text, ARGS&&... Args){
            String<T> Result;
            Result.sprintf(Text.data(), std::forward<ARGS>(Args)...);
            return std::move(Result);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

