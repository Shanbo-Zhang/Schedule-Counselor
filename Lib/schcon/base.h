/**
 *
 */

#ifndef ESCLIB_BASE_H
#define ESCLIB_BASE_H

#include "escapist/base.h"
#include "escapist/string.h"
#include "escapist/list.h"
#include "escapist/flag.h"

#ifdef ESCAPIST_OS_WINDOWS
#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")
#endif

#endif //ESCLIB_BASE_H
