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


static constexpr size_t BufferCount = 512;
static constexpr TCHAR LogFileName[] = _T("CurrentLog.log");


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static inline DWORD GetParentProcessId(){
    DWORD PPID = 0;
    DWORD PID = GetCurrentProcessId();

    HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    do{
        if(SnapshotHandle == INVALID_HANDLE_VALUE)
            break;

        PROCESSENTRY32 PE32 = {};
        PE32.dwSize = sizeof(PE32);
        if(!Process32First(SnapshotHandle, &PE32))
            break;

        do{
            if(PE32.th32ProcessID == PID){
                PPID = PE32.th32ParentProcessID;
                break;
            }
        }
        while(Process32Next(SnapshotHandle, &PE32));
    }
    while(false);

    if(SnapshotHandle != INVALID_HANDLE_VALUE)
        CloseHandle(SnapshotHandle);
    
    return PPID;
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

    const JBF::Common::Path<TCHAR> WorkingDirectory = JBF::Common::GetModuleDirectory();
    if(WorkingDirectory.Empty()){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_NO_MODULE_DIRECTORY);
        assert(false);
        return -1;
    }

    SetCurrentDirectory(WorkingDirectory.String().c_str());

    JBF::Common::UniquePtr<FILE, void(*)(FILE*)> File(nullptr, [](FILE* f){ fclose(f); });
    {
        FILE* Ptr = nullptr;
        _tfopen_s(&Ptr, LogFileName, _T("wt, ccs=UTF-8"));
        File.reset(Ptr);
    }
    if(!File){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::LOGGER_CANNOT_OPEN_LOG_FILE, JBF::Common::Path<TCHAR>(LogFileName).Absolute().String().c_str());
        assert(false);
        return -1;
    }

    JBF::ErrorPipe::Server<TCHAR> Pipe(ParentID);
    if(!Pipe.IsValid()){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::ERRORPIPE_SERVER_CREATE_FAILED);
        assert(false);
        return -1;
    }

    if(!Pipe.WaitUntilConnected()){
        JBF::Error::ShowFatalMessage(JBF::Error::FatalCode::ERRORPIPE_SERVER_CONNECT_FAILED);
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
            const auto CurrentTime = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
            StringBuffer += std::format(_T("[{:%X}] "), CurrentTime).c_str();
            StringBuffer += TmpStr;
            StringBuffer += _T("\n");
            ++CurCount;

            if(CurCount >= BufferCount){
                const size_t WrittenCount = fwrite(StringBuffer.c_str(), sizeof(TCHAR), StringBuffer.length(), File.get());
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
        const size_t WrittenCount = fwrite(StringBuffer.c_str(), sizeof(TCHAR), StringBuffer.length(), File.get());
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

