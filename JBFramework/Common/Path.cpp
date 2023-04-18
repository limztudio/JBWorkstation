#include <Common/Path.h>

#include <Windows.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        Path<TCHAR> GetModuleDirectory(){
            const DWORD StrLen = GetModuleFileName(nullptr, nullptr, 0);
            if(!StrLen)
                return JBF::Common::Path<TCHAR>();

            String<TCHAR> Buffer(StrLen, 0);
            if(!GetModuleFileName(nullptr, Buffer.data(), StrLen))
                return JBF::Common::Path<TCHAR>();

            JBF::Common::Path<TCHAR> Result(String<TCHAR>(Buffer.begin(), Buffer.end()));
            Result = Result.Parent();
            Result = Result.Absolute();

            return std::move(Result);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

