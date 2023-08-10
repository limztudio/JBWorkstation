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
            inline String<char> ToString(char, T v){ return eastl::to_string(v); }
            template<typename T>
            inline String<wchar_t> ToString(wchar_t, T v){ return eastl::to_wstring(v); }            
        };
        template<typename V, typename T = TCHAR>
        inline String<T> ToString(V value){ return __hidden::ToString(T(), value); }

        template<typename T = TCHAR, typename... ARGS>
        inline String<T> Format(const T* text, ARGS&&... args){
            static thread_local std::locale Locale = {};
            std::basic_string<T> Result = std::format<T>(Locale, text, std::forward<ARGS>(args)...);
            return String<T>(Result.begin(), Result.end());
        }
        template<typename T = TCHAR, typename... ARGS>
        inline String<T> Format(const StringView<T>& text, ARGS&&... args){
            static thread_local std::locale Locale = {};
            std::basic_string<T> Result = std::format<T>(Locale, text.data(), std::forward<ARGS>(args)...);
            return String<T>(Result.begin(), Result.end());
        }

        template<typename T = TCHAR, typename... ARGS>
        inline String<T> FastFormat(const T* Text, ARGS&&... Args){
            String<T> Result;
            Result.sprintf(Text, std::forward<ARGS>(Args)...);
            return std::move(Result);
        }
        template<typename T = TCHAR, typename... ARGS>
        inline String<T> FastFormat(const StringView<T>& text, ARGS&&... args){
            String<T> Result;
            Result.sprintf(text.data(), std::forward<ARGS>(args)...);
            return std::move(Result);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

