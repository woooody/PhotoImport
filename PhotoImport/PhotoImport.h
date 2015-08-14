#pragma once

#include "resource.h"

#define NUM_FILE_TYPES 5


struct T_DRIVE
{
	BOOL avail;
	UINT drive_type;
	ULARGE_INTEGER totalbytes;
	ULARGE_INTEGER freebytes;
	WCHAR label[12];
	WCHAR cb_text[50];
};
