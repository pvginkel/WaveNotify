#ifndef _INC_INCLUDE
#define _INC_INCLUDE

#pragma once

// Windows 2000 minimum

#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#define WINVER 0x0500

// Internet Explorer 5.0 minimum

#define _WIN32_IE 0x0500

// Lean and mean

#define WIN32_LEAN_AND_MEAN

// And strict

#define STRICT

// Kill some irritating warnings

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4995)

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <tchar.h>

#include "resource.h"

#endif // _INC_INCLUDE
