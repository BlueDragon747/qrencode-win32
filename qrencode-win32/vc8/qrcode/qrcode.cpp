
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png/png.h"
#include "getopt.h"

#define strcasecmp strcmpi
#if _MSC_VER < 1900 
#define snprintf sprintf_s
#endif
#include "..\..\qrenc.c"
