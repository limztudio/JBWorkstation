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
            Path(const std::filesystem::path& Rhs) : Actual(Rhs){}
            Path(std::filesystem::path&& Rhs)noexcept : Actual(std::move(Rhs)){}
        public:
            Path(const T* String) : Actual(String){}
            Path(const StringType& String) : Actual(String.c_str()){}
            Path(StringType&& String)noexcept : Actual(String.c_str()){ String.clear(); }
            Path(const StringViewType& String) : Actual(String.data()){}
            Path(StringViewType&& String)noexcept : Actual(String.data()){}

        public:
            Path& operator/=(const Path& Rhs){
                Actual /= Rhs.Actual;
                return *this;
            }
            Path& operator+=(const Path& Rhs){
                Actual += Rhs.Actual;
                return *this;
            }

        public:
            StringType String()const{ return StringType(Actual.string<T>().c_str()); }
            operator StringType()const{ return StringType(Actual.string<T>().c_str()); }


        public:
            bool Empty()const noexcept{ return Actual.empty(); }
            void Clear()noexcept{ Actual.clear(); }

        public:
            Path Parent()const noexcept{ return Path(Actual.parent_path()); }
            Path Filename()const noexcept{ return Path(Actual.filename()); }
            Path Stem()const noexcept{ return Path(Actual.stem()); }
            Path Extension()const noexcept{ return Path(Actual.extension()); }
            Path Absolute()const noexcept{
                std::error_code ErrorCode;
                const std::filesystem::path Result = std::filesystem::canonical(Actual, ErrorCode);
                return ErrorCode ? Path() : Path(std::move(Result));
            }

            
        private:
            std::filesystem::path Actual;


        public:
            template<typename K>
            friend bool operator==(const Path<K>& Lhs, const Path<K>& Rhs)noexcept;
            template<typename K>
            friend bool operator!=(const Path<K>& Lhs, const Path<K>& Rhs)noexcept;
        };

        template<typename T>
        inline Path<T> operator/(const Path<T>& Lhs, const Path<T>& Rhs){
            Path<T> Result(Lhs);
            Result /= Rhs;
            return Result;
        }
        template<typename T>
        inline Path<T> operator+(const Path<T>& Lhs, const Path<T>& Rhs){
            Path<T> Result(Lhs);
            Result += Rhs;
            return Result;
        }
        template<typename T>
        inline bool operator==(const Path<T>& Lhs, const Path<T>& Rhs)noexcept{
            std::error_code ErrorCode;
            const bool bRet = std::filesystem::equivalent(Lhs.Actual, Rhs.Actual, ErrorCode);
            return ErrorCode ? false : bRet;
        }
        template<typename T>
        inline bool operator!=(const Path<T>& Lhs, const Path<T>& Rhs)noexcept{
            std::error_code ErrorCode;
            const bool bRet = std::filesystem::equivalent(Lhs.Actual, Rhs.Actual, ErrorCode);
            return ErrorCode ? false : !bRet;
        }
        
        extern Path<TCHAR> GetModuleDirectory();
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

