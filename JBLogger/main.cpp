#include <tchar.h>
#include <Windows.h>
#include <tlhelp32.h>

#include <chrono>

#include <Common/Memory.h>
#include <Common/String.h>
#include <Common/Path.h>
#include <Error/Error.h>
#include <../JBFramework/Error/ErrorPipe.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static constexpr size_t bufferCount = 512;
static constexpr TCHAR logFileName[] = _T("CurrentLog.log");


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline DWORD GetParentProcessId(){
    DWORD ppid = 0;
    DWORD pid = GetCurrentProcessId();

    auto deleteHandle = [](HANDLE* handle){
        if(*handle != INVALID_HANDLE_VALUE)
            CloseHandle(*handle);
    };
    JBF::Common::UniquePtr<std::remove_pointer_t<HANDLE>, decltype(deleteHandle)> snapshotHandle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0), deleteHandle);
    if(snapshotHandle.get() == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32 pe32 = {};
    pe32.dwSize = sizeof(pe32);
    if(!Process32First(snapshotHandle.get(), &pe32))
        return 0;
    
    do{
        if(pe32.th32ProcessID == pid){
            ppid = pe32.th32ParentProcessID;
            break;
        }
    }
    while(Process32Next(snapshotHandle.get(), &pe32));
    
    return ppid;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){
    const DWORD parentID = GetParentProcessId();
    if(!parentID){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_NO_PARENT);
        assert(false);
        return -1;
    }

    const HANDLE parentHandle = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, parentID);
    if(!parentHandle){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_CANNOT_LOOKUP_PARENT, parentID);
        assert(false);
        return -1;
    }

    const JBF::Common::Path<TCHAR> workingDirectory = JBF::Common::GetModuleDirectory();
    if(workingDirectory.Empty()){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_NO_MODULE_DIRECTORY);
        assert(false);
        return -1;
    }

    SetCurrentDirectory(workingDirectory.String().c_str());

    JBF::Common::UniquePtr<FILE, void(*)(FILE*)> file(nullptr, [](FILE* f){ fclose(f); });
    {
        FILE* ptr = nullptr;
        _tfopen_s(&ptr, logFileName, _T("wt, ccs=UTF-8"));
        file.reset(ptr);
    }
    if(!file){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_CANNOT_OPEN_LOG_FILE, JBF::Common::Path<TCHAR>(logFileName).Absolute().String().c_str());
        assert(false);
        return -1;
    }

    JBF::ErrorPipe::Server<TCHAR> pipe(parentID);
    if(!pipe.IsValid()){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::ERRORPIPE_SERVER_CREATE_FAILED);
        assert(false);
        return -1;
    }

    if(!pipe.WaitUntilConnected()){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::ERRORPIPE_SERVER_CONNECT_FAILED);
        assert(false);
        return -1;
    }
    
    size_t curCount = 0;
    JBF::Common::String<TCHAR> tmpStr;
    JBF::Common::String<TCHAR> stringBuffer;
    stringBuffer.reserve(bufferCount * JBF::ErrorPipe::pipeBufferSize);
    
    for(;;){
        DWORD exitCode;
        if(!GetExitCodeProcess(parentHandle, &exitCode))
            break;

        if(exitCode != STILL_ACTIVE)
            break;

        if(pipe.Read(tmpStr)){
            const auto currentTime = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
            stringBuffer += std::format(_T("[{:%X}] "), currentTime).c_str();
            stringBuffer += tmpStr;
            stringBuffer += _T("\n");
            ++curCount;

            if(curCount >= bufferCount){
                const size_t writtenCount = fwrite(stringBuffer.c_str(), sizeof(TCHAR), stringBuffer.length(), file.get());
                if(writtenCount != stringBuffer.length()){
                    JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_WRITE_MISMATCH, stringBuffer.length(), writtenCount);
                    assert(false);
                    return -1;
                }
                
                curCount = 0;
                stringBuffer.clear();
            }
        }
    }
    if(!stringBuffer.empty()){
        const size_t writtenCount = fwrite(stringBuffer.c_str(), sizeof(TCHAR), stringBuffer.length(), file.get());
        if(writtenCount != stringBuffer.length()){
            JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_WRITE_MISMATCH, stringBuffer.length(), writtenCount);
            assert(false);
            return -1;
        }

        curCount = 0;
        stringBuffer.clear();
    }
    
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

