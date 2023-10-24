#include "XMLIO.h"

#include <iostream>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if (defined(_MSC_VER) || defined(WIN32)) && defined(TINYXML2_DEBUG)
#define JBXML_DEBUG
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef JBXML_DEBUG
#include <crtdbg.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef JBXML_DEBUG
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


constexpr char XMLMessageHeader[] = "XMLIO::Debug::";


_CrtMemState startMemState;
_CrtMemState endMemState;

int gPass = 0;
int gFail = 0;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool XMLTest(const char* testString, const char* expected, const char* found, bool echo = true, bool extraNL = false){
    bool pass;
    if(!expected && !found)
        pass = true;
    else if(!expected || !found)
        pass = false;
    else
        pass = !strcmp(expected, found);

    std::cout << XMLMessageHeader;
    
    if(pass)
        std::cout << "[pass]";
    else
        std::cout << "[fail]";

    if(!echo)
        std::cout << " " << testString << std::endl;
    else{
        if(extraNL){
            std::cout << " " << testString << std::endl;
            std::cout << expected << std::endl;
            std::cout << found << std::endl;
        }
        else
            std::cout << " " << testString << " [" << expected << "][" << found << "]" << std::endl;
    }

    if(pass)
        ++gPass;
    else
        ++gFail;
    
    return pass;
}

bool XMLTest(const char* testString, XMLIO::XMLError expected, XMLIO::XMLError found, bool echo = true, bool extraNL = false){
    return XMLTest(testString, XMLIO::XMLDocument::ErrorIDToName(expected), XMLIO::XMLDocument::ErrorIDToName(found), echo, extraNL);
}

bool XMLTest(const char* testString, bool expected, bool found, bool echo = true, bool extraNL = false){
    return XMLTest(testString, expected ? "true" : "false", found ? "true" : "false", echo, extraNL);
}

template<typename T>
bool XMLTest(const char* testString, T expected, T found, bool echo = true){
    const bool pass = (expected == found);

    std::cout << XMLMessageHeader;
    
    if(pass)
        std::cout << "[pass]";
    else
        std::cout << "[fail]";

    if (!echo)
        std::cout << " " << testString << std::endl;
    else{
        char expectedAsString[64];
        XMLIO::XMLUtil::ToStr(expected, expectedAsString, sizeof(expectedAsString));

        char foundAsString[64];
        XMLIO::XMLUtil::ToStr(found, foundAsString, sizeof(foundAsString));

        std::cout << " " << testString << " [" << expectedAsString << "][" << foundAsString << "]" << std::endl;
    }

    if (pass)
        ++gPass;
    else
        ++gFail;
    
    return pass;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


XMLIODebug::XMLIODebug(){
    _CrtMemCheckpoint(&startMemState);
    // Enable MS Visual C++ debug heap memory leaks dump on exit
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
    {
        const int leaksOnStart = _CrtDumpMemoryLeaks();
        XMLTest("No leaks on start?", FALSE, leaksOnStart);
    }

    {
        TIXMLASSERT(true);
    }
}
XMLIODebug::~XMLIODebug(){
    _CrtMemCheckpoint(&endMemState);

    _CrtMemState diffMemState;
    _CrtMemDifference(&diffMemState, &startMemState, &endMemState);
    _CrtMemDumpStatistics(&diffMemState);

    {
        int leaksBeforeExit = _CrtDumpMemoryLeaks();
        XMLTest("No leaks before exit?", FALSE, leaksBeforeExit);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


XMLIODebug::XMLIODebug(){}
XMLIODebug::~XMLIODebug(){}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

