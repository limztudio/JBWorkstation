#include <Common/Container.h>
#include <Common/Path.h>

#include <Windows.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        Path<TCHAR> GetModuleDirectory(){
            FixedVector<wchar_t, MAX_PATH, true> buffer; 
            DWORD strLen = 0;
            do{
                buffer.resize(buffer.size() + MAX_PATH);
                strLen = GetModuleFileName(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
            }
            while(strLen >= buffer.size());
            buffer.resize(strLen);
            
            if(buffer.empty())
                return {};

            Path<TCHAR> result(String<TCHAR>(buffer.begin(), buffer.end()));
            result = result.Parent();
            result = result.Absolute();

            return result;
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

