#ifndef _PATH_H_
#define _PATH_H_


#include "String.h"

#include <string>
#include <filesystem>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        template<typename T = TCHAR>
        class Path{
        private:
            typedef String<T> StringType;
            typedef StringView<T> StringViewType;

            
        public:
            Path() = default;
            Path(const Path&) = default;
            Path(Path&&)noexcept = default;
            ~Path() = default;
            Path& operator=(const Path&) = default;
            Path& operator=(Path&&)noexcept = default;

        private:
            Path(const std::filesystem::path& rhs) : actual(rhs){}
            Path(std::filesystem::path&& rhs)noexcept : actual(std::move(rhs)){}
        public:
            Path(const T* string) : actual(string){}
            Path(const StringType& string) : actual(string.c_str()){}
            Path(StringType&& string)noexcept : actual(string.c_str()){ string.clear(); }
            Path(const StringViewType& string) : actual(string.data()){}
            Path(StringViewType&& string)noexcept : actual(string.data()){}

        public:
            Path& operator/=(const Path& rhs){
                actual /= rhs.actual;
                return *this;
            }
            Path& operator+=(const Path& rhs){
                actual += rhs.actual;
                return *this;
            }

        public:
            StringType String()const{ return StringType(actual.string<T>().c_str()); }
            operator StringType()const{ return StringType(actual.string<T>().c_str()); }


        public:
            bool Empty()const noexcept{ return actual.empty(); }
            void Clear()noexcept{ actual.clear(); }

        public:
            Path Parent()const noexcept{ return Path(actual.parent_path()); }
            Path Filename()const noexcept{ return Path(actual.filename()); }
            Path Stem()const noexcept{ return Path(actual.stem()); }
            Path Extension()const noexcept{ return Path(actual.extension()); }
            Path Absolute()const noexcept{
                std::error_code errorCode;
                const std::filesystem::path result = std::filesystem::canonical(actual, errorCode);
                return errorCode ? Path() : Path(result);
            }

            
        private:
            std::filesystem::path actual;


        public:
            template<typename K>
            friend bool operator==(const Path<K>& lhs, const Path<K>& rhs)noexcept;
            template<typename K>
            friend bool operator!=(const Path<K>& lhs, const Path<K>& rhs)noexcept;
        };

        template<typename T>
        inline Path<T> operator/(const Path<T>& lhs, const Path<T>& rhs){
            Path<T> result(lhs);
            result /= rhs;
            return result;
        }
        template<typename T>
        inline Path<T> operator+(const Path<T>& lhs, const Path<T>& rhs){
            Path<T> result(lhs);
            result += rhs;
            return result;
        }
        template<typename T>
        inline bool operator==(const Path<T>& lhs, const Path<T>& rhs)noexcept{
            std::error_code errorCode;
            const bool bRet = std::filesystem::equivalent(lhs.actual, rhs.actual, errorCode);
            return errorCode ? false : bRet;
        }
        template<typename T>
        inline bool operator!=(const Path<T>& lhs, const Path<T>& rhs)noexcept{
            std::error_code errorCode;
            const bool bRet = std::filesystem::equivalent(lhs.actual, rhs.actual, errorCode);
            return errorCode ? false : !bRet;
        }
        
        extern Path<TCHAR> GetModuleDirectory();
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

