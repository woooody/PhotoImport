#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100
#define MAX_PATH_STRING 256
#define MAX_STORAGE_PATHS 10

struct T_DRIVE
{
	BOOL avail;
	UINT drive_type;
	DWORD totalMB;
	DWORD freeMB;
	WCHAR label[12];
	WCHAR cb_text[50];
};

struct T_STORAGE_PATH
{
	WCHAR ext[MAX_LOADSTRING];
	WCHAR path[MAX_PATH_STRING];
};