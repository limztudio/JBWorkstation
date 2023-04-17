#include <tchar.h>
#include <Windows.h>
#include <tlhelp32.h>

#include <string>
#include <filesystem>

#include <Common/Memory.h>
#include <Common/String.h>
#include <Error/Error.h>
#include <../JBFramework/Error/ErrorPipe.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static constexpr size_t BufferCount = 512;
static constexpr TCHAR LogFileName[] = _T("CurrentLog.log");


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline DWORD GetParentProcessId(){
    DWORD PPID = 0;
    DWORD PID = GetCurrentProcessId();

    HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    __try{
        if(SnapshotHandle == INVALID_HANDLE_VALUE)
            __leave;

        PROCESSENTRY32 PE32 = {};
        PE32.dwSize = sizeof(PE32);
        if(!Process32First(SnapshotHandle, &PE32))
            __leave;

        do{
            if(PE32.th32ProcessID == PID){
                PPID = PE32.th32ParentProcessID;
                break;
            }
        }
        while(Process32Next(SnapshotHandle, &PE32));

    }
    __finally{
        if(SnapshotHandle != INVALID_HANDLE_VALUE)
            CloseHandle(SnapshotHandle);
    }
    return PPID;
}

static inline JBF::Common::String<TCHAR> GetModuleDirectory(){
    const DWORD StrLen = GetModuleFileName(nullptr, nullptr, 0);
    if(!StrLen)
        return JBF::Common::String<TCHAR>();

    std::basic_string<TCHAR> Buffer(StrLen, 0);
    if(!GetModuleFileName(nullptr, Buffer.data(), StrLen))
        return JBF::Common::String<TCHAR>();

    std::filesystem::path Result(std::basic_string<TCHAR>(Buffer.begin(), Buffer.end()));
    Result = Result.parent_path();

    std::error_code ErrorCode;
    Result = std::filesystem::canonical(Result, ErrorCode);
    if(ErrorCode)
        return JBF::Common::String<TCHAR>();
    
    return JBF::Common::String<TCHAR>(Result.string<TCHAR>().c_str());
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){
    const DWORD ParentID = GetParentProcessId();
    if(!ParentID){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_NO_PARENT);
        assert(false);
        return -1;
    }

    const HANDLE ParentHandle = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, ParentID);
    if(!ParentHandle){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_CANNOT_LOOKUP_PARENT, ParentID);
        assert(false);
        return -1;
    }

    const JBF::Common::String<TCHAR> WorkingDirectory = GetModuleDirectory();
    if(WorkingDirectory.empty()){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_NO_MODULE_DIRECTORY);
        assert(false);
        return -1;
    }

    SetCurrentDirectory(WorkingDirectory.c_str());

    JBF::Common::UniquePtr<FILE, decltype(fclose)> File(nullptr, fclose);
    {
        FILE* Ptr = nullptr;
        _tfopen_s(&Ptr, LogFileName, _T("wt, ccs=UTF-8"));
        File.reset(Ptr);
    }
    if(!File){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_CANNOT_OPEN_LOG_FILE, std::filesystem::canonical(LogFileName).string<TCHAR>().c_str());
        assert(false);
        return -1;
    }

    JBF::ErrorPipe::Server<TCHAR> Pipe(ParentID);
    if(!Pipe.IsValid()){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::ERRORPIPE_SERVER_CREATE_FAILED);
        assert(false);
        return -1;
    }
    
    size_t CurCount = 0;
    JBF::Common::String<TCHAR> TmpStr;
    JBF::Common::String<TCHAR> StringBuffer;
    StringBuffer.reserve(BufferCount * JBF::ErrorPipe::PipeBufferSize);
    
    for(;;){
        DWORD ExitCode;
        if(!GetExitCodeProcess(ParentHandle, &ExitCode))
            break;

        if(ExitCode != STILL_ACTIVE)
            break;

        if(Pipe.Read(TmpStr)){
            StringBuffer += TmpStr;
            StringBuffer += _T("\n");
            ++CurCount;

            if(CurCount >= BufferCount){
                const size_t WrittenCount = fwrite(StringBuffer.c_str(), sizeof(TCHAR), StringBuffer.length(), File);
                if(WrittenCount != StringBuffer.length()){
                    JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_WRITE_MISMATCH, StringBuffer.length(), WrittenCount);
                    assert(false);
                    return -1;
                }
                
                CurCount = 0;
                StringBuffer.clear();
            }
        }
    }
    if(!StringBuffer.empty()){
        const size_t WrittenCount = fwrite(StringBuffer.c_str(), sizeof(TCHAR), StringBuffer.length(), File);
        if(WrittenCount != StringBuffer.length()){
            JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_WRITE_MISMATCH, StringBuffer.length(), WrittenCount);
            assert(false);
            return -1;
        }

        CurCount = 0;
        StringBuffer.clear();
    }
    
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

