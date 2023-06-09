#ifndef _GRAPHICSAPIMACRO_H_
#define _GRAPHICSAPIMACRO_H_


#include <tchar.h>

#include <Common/String.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef _DEBUG
#define OBJECT_NAME(V) _T(#V)
#define OBJECT_NAME_INDEX(V, I) JBF::Common::FastFormat(_T("%s_%u"), _T(#V), I).c_str()

#define OBJECT_SET_NAME(V) V->SetName(OBJECT_NAME(V))
#define OBJECT_SET_NAME_INDEX(V, I) V->SetName(OBJECT_NAME_INDEX(V, I))
#else
#define OBJECT_NAME(V)
#define OBJECT_NAME_INDEX(V, I)

#define OBJECT_SET_NAME(V)
#define OBJECT_SET_NAME_INDEX(V, I)
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Graphics{
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

