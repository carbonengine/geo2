#include "windows.h"

#define EVEFILEDESC "CCP Math Library\0"
#ifndef _DEBUG
#define EVEINTFILENAME "geo2\0"
#define EVEFILENAME "geo2.dll\0"
#else
#define EVEINTFILENAME "geo2_d\0"
#define EVEFILENAME "geo2_d.dll\0"
#endif
#define EVEFILETYPE VFT_DLL

#include "autoversion.h"
//standard file version thing
#include "../version/evebuildver.h"
