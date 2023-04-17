#include <tchar.h>
#include <Windows.h>
#include <tlhelp32.h>

#include <string>
#include <filesystem>

#include <Common/String.h>
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
    if(!ParentID)
        return -1;

    const HANDLE ParentHandle = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, ParentID);
    if(!ParentHandle)
        return -1;

    const JBF::Common::String<TCHAR> WorkingDirectory = GetModuleDirectory();
    if(WorkingDirectory.empty())
        return -1;

    SetCurrentDirectory(WorkingDirectory.c_str());

    FILE* File = nullptr;
    _tfopen_s(&File, LogFileName, _T("wt, ccs=UTF-8"));
    if(!File)
        return -1;

    JBF::ErrorPipe::Server<TCHAR> Pipe(ParentID);
    if(!Pipe.IsValid())
        return -1;
    
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
                if(WrittenCount != StringBuffer.length())
                    break;
                
                CurCount = 0;
                StringBuffer.clear();
            }
        }
    }

    fclose(File);
    File = nullptr;
    
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

