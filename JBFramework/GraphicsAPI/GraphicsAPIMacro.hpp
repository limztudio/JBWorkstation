#ifndef _GRAPHICSAPIMACRO_H_
#define _GRAPHICSAPIMACRO_H_


#include <tchar.h>

#include <Common/String.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef _DEBUG
#define OBJECT_SET_NAME(V) V->SetName(_T(#V))
#define OBJECT_SET_NAME_INDEX(V, I) V->SetName(JBF::Common::Format(_T("%s_%u"), _T(#V), I).c_str())
#else
#define OBJECT_SET_NAME(V)
#define OBJECT_SET_NAME_INDEX(V, I)
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace __hidden_GraphicsAPI{
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

