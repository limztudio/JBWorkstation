#include <Common/Container.h>
#include <Common/Path.h>

#include <Windows.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        Path<TCHAR> GetModuleDirectory(){
            FixedVector<wchar_t, MAX_PATH, true> Buffer; 
            DWORD StrLen = 0;
            do{
                Buffer.resize(Buffer.size() + MAX_PATH);
                StrLen = GetModuleFileName(nullptr, Buffer.data(), static_cast<DWORD>(Buffer.size()));
            }
            while(StrLen >= Buffer.size());
            Buffer.resize(StrLen);
            
            if(!Buffer.size())
                return JBF::Common::Path<TCHAR>();

            JBF::Common::Path<TCHAR> Result(String<TCHAR>(Buffer.begin(), Buffer.end()));
            Result = Result.Parent();
            Result = Result.Absolute();

            return std::move(Result);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

