#include <tchar.h>

#include "tinyxml2.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if (defined(_MSC_VER) || defined(WIN32)) && defined(TINYXML2_DEBUG)
#define JBUILDER_DEBUG
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef JBUILDER_DEBUG
#include <crtdbg.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef JBUILDER_DEBUG
_CrtMemState startMemState;
_CrtMemState endMemState;

int gPass = 0;
int gFail = 0;

bool XMLTest(const char* testString, const char* expected, const char* found, bool echo = true, bool extraNL = false){
    bool pass;
    if(!expected && !found)
        pass = true;
    else if(!expected || !found)
        pass = false;
    else
        pass = !strcmp(expected, found);
    if(pass)
        printf("[pass]");
    else
        printf("[fail]");

    if(!echo)
        printf(" %s\n", testString);
    else{
        if(extraNL){
            printf(" %s\n", testString);
            printf("%s\n", expected);
            printf("%s\n", found);
        }
        else
            printf(" %s [%s][%s]\n", testString, expected, found);
    }

    if(pass)
        ++gPass;
    else
        ++gFail;
    
    return pass;
}
bool XMLTest(const char* testString, tinyxml2::XMLError expected, tinyxml2::XMLError found, bool echo = true, bool extraNL = false){
    return XMLTest(testString, tinyxml2::XMLDocument::ErrorIDToName(expected), tinyxml2::XMLDocument::ErrorIDToName(found), echo, extraNL);
}
bool XMLTest(const char* testString, bool expected, bool found, bool echo = true, bool extraNL = false){
    return XMLTest(testString, expected ? "true" : "false", found ? "true" : "false", echo, extraNL);
}
template<typename T>
bool XMLTest(const char* testString, T expected, T found, bool echo = true){
    bool pass = (expected == found);
    if(pass)
        printf("[pass]");
    else
        printf("[fail]");

    if (!echo)
        printf(" %s\n", testString);
    else{
        char expectedAsString[64];
        tinyxml2::XMLUtil::ToStr(expected, expectedAsString, sizeof(expectedAsString));

        char foundAsString[64];
        tinyxml2::XMLUtil::ToStr(found, foundAsString, sizeof(foundAsString));

        printf(" %s [%s][%s]\n", testString, expectedAsString, foundAsString);
    }

    if (pass)
        ++gPass;
    else
        ++gFail;
    
    return pass;
}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


using namespace tinyxml2;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int _tmain(int argc, TCHAR* argv[]){
#ifdef JBUILDER_DEBUG
    _CrtMemCheckpoint(&startMemState);
    // Enable MS Visual C++ debug heap memory leaks dump on exit
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
    {
        int leaksOnStart = _CrtDumpMemoryLeaks();
        XMLTest("No leaks on start?", FALSE, leaksOnStart);
    }

    {
        TIXMLASSERT(true);
    }
#endif
    
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

