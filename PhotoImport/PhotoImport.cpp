// PhotoImport.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PhotoImport.h"

//Consts
const WCHAR SourceDrvStr[] = L"Source drive";
const WCHAR LocalDrvStr[] = L"Local drive";
const WCHAR StorageExtStr[] = L"Extension";
const WCHAR StoragePathStr[] = L"Storage Path";

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
T_DRIVE drives[26];
WCHAR SettingsFileName[MAX_PATH_STRING];
UINT WorkingAreaY = 0;
UINT hComboSourceNum = 0;
UINT hComboLocalNum = 0;
BOOL DataLocked = FALSE;

// Window items
HWND hButnSettings;
HWND hButnSrcToStorage;
HWND hButnSrcToLocal;
HWND hButnLocalToStorage;
HWND hEditStorageExt[MAX_STORAGE_PATHS];
HWND hEditStoragePath[MAX_STORAGE_PATHS];
HWND hEditStorageBtn[MAX_STORAGE_PATHS];
HWND hComboSource;
HWND hComboLocal;

// Variables to be saved to .ini
BOOL stAutoUpdate = FALSE;
UINT stStoragePaths = 3;
UINT stStoragePathsC;
T_STORAGE_PATH stStoragePath[MAX_STORAGE_PATHS];
WCHAR stSourcePath[MAX_PATH_STRING];
WCHAR stLocalPath[MAX_PATH_STRING];
WCHAR stSourceDriveFilter[8];  // each byte enables/disables drive type 
WCHAR stLocalDriveFilter[8];   // DRIVE_REMOVABLE, DRIVE_FIXED, etc.

// DEBUG variables
BOOL f1, f2;
DWORD i0, i1, i2;
WCHAR currentpath[MAX_PATH_STRING];
static int sec = 0;
static WCHAR greeting[255];
//static char text[2] = { ' ', '\0' };


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Settings(HWND, UINT, WPARAM, LPARAM);
UINT				ReadSettings();
UINT				WriteSettings();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	DWORD SettingsFileNameLength;

	// Clear drives list on startup
	memset(drives, 0, sizeof(drives));

	SettingsFileNameLength = GetModuleFileName(NULL, SettingsFileName, MAX_PATH_STRING);
	if (SettingsFileNameLength > 5)
	{
		// Change extension to .ini
		SettingsFileName[SettingsFileNameLength - 1] = 'i';
		SettingsFileName[SettingsFileNameLength - 2] = 'n';
		SettingsFileName[SettingsFileNameLength - 3] = 'i';
		ReadSettings();
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PHOTOIMPORT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PHOTOIMPORT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;

} //_tWinMain


BOOL DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
} //DirectoryExists

UINT ReadSettings()
{
	UINT OK = 1;
	WCHAR ReadParam[MAX_LOADSTRING];
	WCHAR ParamName[MAX_LOADSTRING];
	UINT i;

	// Global/AutoUpdate
	OK *= GetPrivateProfileString(
				TEXT("Global"),
				TEXT("AutoUpdate"),
				TEXT("0"),
				ReadParam,
				MAX_LOADSTRING,
				SettingsFileName
		);
	stAutoUpdate = (ReadParam[0] == '0' ? FALSE : TRUE);

	//Storage/StoragePaths
	OK *= GetPrivateProfileString(
		TEXT("Storage"),
		TEXT("StoragePaths"),
		TEXT("3"),
		ReadParam,
		MAX_LOADSTRING,
		SettingsFileName
		);

	stStoragePaths = _wtol(ReadParam);
	if (stStoragePaths < 1)
	{
		stStoragePaths = 3;
	}

	if (stStoragePaths > MAX_STORAGE_PATHS)
	{
		stStoragePaths = MAX_STORAGE_PATHS;
	}

	//Storage/StoragePath[]
	for (i = 0; i < stStoragePaths; i++)
	{
		//ext1-2-3...
		wsprintf(ParamName, L"ext%d", i + 1);
		OK *= GetPrivateProfileString(
			TEXT("Storage"),
			ParamName,
			TEXT(""),
			stStoragePath[i].ext,
			MAX_LOADSTRING,
			SettingsFileName
			);

		//path1-2-3...
		wsprintf(ParamName, L"path%d", i + 1);
		OK *= GetPrivateProfileString(
			TEXT("Storage"),
			ParamName,
			TEXT(""),
			stStoragePath[i].path,
			MAX_LOADSTRING,
			SettingsFileName
			);
	}


	//SourceLocal/SourceDrive
	OK *= GetPrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("SourcePath"),
		TEXT("F:\\"),
		stSourcePath,
		MAX_PATH_STRING,
		SettingsFileName
		);

	//SourceLocal/LocalPath
	OK *= GetPrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("LocalPath"),
		TEXT("D:\\PhotoImport"),
		stLocalPath,
		MAX_PATH_STRING,
		SettingsFileName
		);

	//SourceLocal/SourceDriveFilter
	OK *= GetPrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("SourceDrvFilter"),
		TEXT("001000"),
		stSourceDriveFilter,
		sizeof(stSourceDriveFilter),
		SettingsFileName
		);

	//SourceLocal/SourcePath
	OK *= GetPrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("LocalDrvFilter"),
		TEXT("001100"),
		stLocalDriveFilter,
		sizeof(stLocalDriveFilter),
		SettingsFileName
		);

	return OK;

} //ReadSettings


UINT WriteSettings()
{
	UINT OK = 1;
	WCHAR WriteParam[MAX_LOADSTRING];
	WCHAR ParamName[MAX_LOADSTRING];
	UINT i;

	// Global/AutoUpdate
	OK *= WritePrivateProfileString(
				TEXT("Global"),
				TEXT("AutoUpdate"),
				stAutoUpdate ? TEXT("1") : TEXT("0"),
				SettingsFileName);

	OK *= _ltow_s(stStoragePaths, WriteParam, MAX_LOADSTRING, 10);
	OK *= WritePrivateProfileString(
		TEXT("Storage"),
		TEXT("StoragePaths"),
		WriteParam,
		SettingsFileName
		);

	//Storage/StoragePath[]
	for (i = 0; i < stStoragePathsC; i++)
	{
		//ext1-2-3...
		wsprintf(ParamName, L"ext%d", i + 1);
		OK *= WritePrivateProfileString(
			TEXT("Storage"),
			ParamName,
			stStoragePath[i].ext,
			SettingsFileName
			);

		//path1-2-3...
		wsprintf(ParamName, L"path%d", i + 1);
		OK *= WritePrivateProfileString(
			TEXT("Storage"),
			ParamName,
			stStoragePath[i].path,
			SettingsFileName
			);
	}

	//SourceLocal/SourcePath
	OK *= WritePrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("SourcePath"),
		stSourcePath,
		SettingsFileName
		);

	//SourceLocal/LocalPath
	OK *= WritePrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("LocalPath"),
		stLocalPath,
		SettingsFileName
		);

	//SourceLocal/SourceDriveFilter
	OK *= WritePrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("SourceDrvFilter"),
		stSourceDriveFilter,
		SettingsFileName
		);

	//SourceLocal/LocalDriveFilter
	OK *= WritePrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("LocalDrvFilter"),
		stLocalDriveFilter,
		SettingsFileName
		);

	return OK;

} //WriteSettings


INT ReadDriveList(HWND hWnd, TCHAR text[])
{

	INT n;
	static INT i = 0;
	BOOL Flag;
	BOOL prev_avail = FALSE;
	static DWORD prev_dr = 0;
	WCHAR path[] = L" :\\";
	WCHAR drive_fat[30];
	ULARGE_INTEGER NumFreeBytes, FreeBytesAvail, TotalBytes;
	DWORD drive_sn, drive_name_size;
	LRESULT result;

	DWORD dr = GetLogicalDrives(); // функци€ возвращает битовую маску

	if (dr == prev_dr) return 0;

	for (int x = 0; x < 26; x++) // проходимс€ циклом по битам
	{
		n = ((dr >> x) & 1); // узнаЄм значение текущего бита

		prev_avail = drives[x].avail;

		if (!prev_avail && n) // if new drive detected
		{
			path[0] = ('A' + x); // получаем литеру диска
			// здесь узнаЄм готово ли устройство
			/*
			WORD OldErrorMode;
			OldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS); // убираем показ ошибок
			BOOL ready = DirectoryExists(path); // пытаемc€ открыть корневую директорию
			SetErrorMode(OldErrorMode); // восстанавливаем старый режим показа ошибок
			*/
			drives[x].drive_type = GetDriveType(path); // узнаЄм тип диска

			if (drives[x].drive_type > 7)
			{
				continue;
			}
			else if ((stLocalDriveFilter[drives[x].drive_type] == L'0') &&
				(stSourceDriveFilter[drives[x].drive_type] == L'0'))
			{
				continue;
			}

			Flag = GetDiskFreeSpaceEx(path, 
				&FreeBytesAvail,
				&TotalBytes,
				&NumFreeBytes
				);

			Flag &= GetVolumeInformation(path,
				drives[x].label,
				sizeof(drives[x].label),
				&drive_sn,
				&drive_name_size,
				NULL,
				drive_fat,
				sizeof(drive_fat)
				);

			if (Flag)
			{
				drives[x].avail = TRUE;

				drives[x].totalMB = (UINT)((TotalBytes.QuadPart / 1024) / 1024);
				drives[x].freeMB = (UINT)((FreeBytesAvail.QuadPart / 1024) / 1024);

				i += wsprintf(text + i, L"%S: ", path);

				wsprintf(drives[x].cb_text, L"%S:[%s](%d.%dGb)",
					path,
					drives[x].label,
					drives[x].totalMB / 1000,
					(drives[x].totalMB % 1000) / 10
					);

				if (drives[x].drive_type <= 7)
				{
					if (stLocalDriveFilter[drives[x].drive_type] != L'0')
					{
						SendMessage(hComboLocal, CB_ADDSTRING, 0, (LPARAM)drives[x].cb_text);
						hComboLocalNum++;

						if (drives[x].cb_text[0] == stLocalPath[0])
						{
							SendMessage(hComboLocal, CB_SELECTSTRING, -1, (LPARAM)drives[x].cb_text);
						}
					}
						
					if (stSourceDriveFilter[drives[x].drive_type] != L'0')
					{
						SendMessage(hComboSource, CB_ADDSTRING, 0, (LPARAM)drives[x].cb_text);
						hComboSourceNum++;

						if (drives[x].cb_text[0] == stSourcePath[0])
						{
							SendMessage(hComboSource, CB_SELECTSTRING, -1, (LPARAM)drives[x].cb_text);
						}
					}
				}
				else
				{
					MessageBox(hWnd, L"Wrong drives[x].drive_type", L"", 0);
				}
			}

			InvalidateRect(hWnd, NULL, TRUE);

		}
		else if (prev_avail && !n) //drive lost
		{
			drives[x].avail = FALSE;

			result = SendMessage(hComboLocal, CB_FINDSTRING, -1, (LPARAM)drives[x].cb_text);
			if (result != CB_ERR)
			{
				SendMessage(hComboLocal, CB_DELETESTRING, result, NULL);
			}
			result = SendMessage(hComboSource, CB_FINDSTRING, -1, (LPARAM)drives[x].cb_text);
			if (result != CB_ERR)
			{
				SendMessage(hComboSource, CB_DELETESTRING, result, NULL);
			}

			InvalidateRect(hWnd, NULL, TRUE);
		}

	}

	prev_dr = dr;
	return 1;

} //ReadDriveList

INT ReadFields(HWND hWnd)
{
	LRESULT cb_local, cb_source, result;
	static LRESULT prev_cb_local, prev_cb_source;
	WCHAR tempString[MAX_LOADSTRING];
	UINT k;

	if (DataLocked) 
		return 0;

	cb_local = SendMessage(hComboLocal, CB_GETCURSEL, NULL, NULL);
	cb_source = SendMessage(hComboSource, CB_GETCURSEL, NULL, NULL);
	
	// Re-read local drive
	if (cb_local != CB_ERR)
	{
		result = SendMessage(hComboLocal, CB_GETLBTEXT, cb_local, (LPARAM)tempString);
		if (result != CB_ERR)
		{
			stLocalPath[0] = tempString[0];
		}
	}

	// Re-read source drive
	if (cb_source != CB_ERR)
	{
		result = SendMessage(hComboSource, CB_GETLBTEXT, cb_local, (LPARAM)tempString);
		if (result != CB_ERR)
		{
			stSourcePath[0] = tempString[0];
		}
	}

	// Re-read storage information
	for (k = 0; k < stStoragePathsC; k++)
	{
		result = SendMessage(hEditStorageExt[k], WM_GETTEXT, sizeof(stStoragePath[k].ext), (LPARAM)stStoragePath[k].ext);
		result = SendMessage(hEditStoragePath[k], WM_GETTEXT, sizeof(stStoragePath[k].path), (LPARAM)stStoragePath[k].path);
	}

	if ((prev_cb_local != cb_local) || (prev_cb_source != cb_source))
	{
		// Enable or Disable buttons
		if ((cb_local != CB_ERR) && (cb_source != CB_ERR))
		{
			//enable all buttons
			EnableWindow(hButnSrcToStorage, TRUE);
			EnableWindow(hButnSrcToLocal, TRUE);
			EnableWindow(hButnLocalToStorage, TRUE);
		}
		else if ((cb_local == CB_ERR) && (cb_source != CB_ERR))
		{
			//gray out buttons source->local and local->storage
			EnableWindow(hButnSrcToLocal, FALSE);
			EnableWindow(hButnLocalToStorage, FALSE);
			//enable source->storage button
			EnableWindow(hButnSrcToStorage, TRUE);
		}
		else if ((cb_local != CB_ERR) && (cb_source == CB_ERR))
		{
			//gray out buttons source->local and source->storage
			EnableWindow(hButnSrcToStorage, FALSE);
			EnableWindow(hButnSrcToLocal, FALSE);
			//enable local->storage button
			EnableWindow(hButnLocalToStorage, TRUE);
		}
		else //both are CB_ERR
		{
			//gray out all buttons
			EnableWindow(hButnSrcToStorage, FALSE);
			EnableWindow(hButnSrcToLocal, FALSE);
			EnableWindow(hButnLocalToStorage, FALSE);
		}
	}

	prev_cb_local = cb_local;
	prev_cb_source = cb_source;

	return 1;
} //ReadFields

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PHOTOIMPORT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PHOTOIMPORT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
} //MyRegisterClass


static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{

	if (uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}


BOOL BrowseFolder(WCHAR saved_path[])
{

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = L"Browse for folder...";
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)saved_path;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0)
	{
		//get the name of the folder and put it in path
		return SHGetPathFromIDListW(pidl, saved_path);
		 
	}

	return FALSE;
}
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;



   UINT i, j, k;

   i = IDS_ITEMS_START_X;
   j = IDS_ITEMS_START_Y;

   hInst = hInstance; // Store instance handle in our global variable

   // Create main window
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   // Buttons
   // first line
   hButnSrcToStorage = CreateWindow(L"button", L"Source ==>> Storage", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   i, j, 300 + IDS_ITEMS_STEP_X, IDS_ITEMS_SIZE_Y, hWnd, (HMENU)IDM_SRC_STORAGE, hInstance, NULL);
   i += 300;
   i += IDS_ITEMS_STEP_X + IDS_ITEMS_STEP_X;

   hButnSettings = CreateWindow(L"button", L"Settings", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   i, j, 80, IDS_ITEMS_SIZE_Y, hWnd, (HMENU)IDM_SETTINGS, hInstance, NULL);

   i = IDS_ITEMS_START_X;
   j += IDS_ITEMS_SIZE_Y;
   j += IDS_ITEMS_STEP_Y;

   // second line
   hButnSrcToLocal = CreateWindow(L"button", L"Source => Local", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   i, j, 150, IDS_ITEMS_SIZE_Y, hWnd, (HMENU)IDM_SRC_LOCAL, hInstance, NULL);
   i += 150;
   i += IDS_ITEMS_STEP_X;

   hButnLocalToStorage = CreateWindow(L"button", L"Local => Storage", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   i, j, 150, IDS_ITEMS_SIZE_Y, hWnd, (HMENU)IDM_LOCAL_STORAGE, hInstance, NULL);
   i += 150;
   i += IDS_ITEMS_STEP_X;

/*
   // plus and minus buttons
   hButnSrcToStorage = CreateWindow(L"button", L"+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   i, j, 35, IDS_ITEMS_SIZE_Y, hWnd, (HMENU)IDM_INC_STORAGE, hInstance, NULL);
   i += 35;
   i += IDS_ITEMS_STEP_X;

   hButnSrcToStorage = CreateWindow(L"button", L"-", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   i, j, 35, IDS_ITEMS_SIZE_Y, hWnd, (HMENU)IDM_DEC_STORAGE, hInstance, NULL);
*/
   j += IDS_ITEMS_SIZE_Y;
   j += IDS_ITEMS_STEP_Y;
   WorkingAreaY = j;

   // Left side
   j = WorkingAreaY;
   i = IDS_ITEMS_START_X;

   j += IDS_ITEMS_SIZE_Y;
//   j += IDS_ITEMS_STEP_Y;

   hComboSource = CreateWindow(L"COMBOBOX", L"combobox", WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST | CBS_HASSTRINGS, i, j, IDS_ITEMS_SIZE_LL, IDS_COMBO_SIZE, hWnd, 0, hInstance, 0);

   j += IDS_ITEMS_SIZE_Y;
//   j += IDS_ITEMS_STEP_Y;
   j += IDS_ITEMS_SIZE_Y;
   j += IDS_ITEMS_STEP_Y;

   hComboLocal = CreateWindow(L"COMBOBOX", L"combobox", WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST | CBS_HASSTRINGS, i, j, IDS_ITEMS_SIZE_LL, IDS_COMBO_SIZE, hWnd, 0, hInstance, 0);

   // Right side
   j = WorkingAreaY;
   j += IDS_ITEMS_SIZE_Y;
   //   j += IDS_ITEMS_STEP_Y;
   i += IDS_ITEMS_SIZE_LL;
   i += IDS_ITEMS_STEP_X;

   stStoragePathsC = stStoragePaths;

   for (k = 0; k < stStoragePathsC; k++)
   {
	   hEditStorageExt[k] = CreateWindow(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, i, j, 70, IDS_ITEMS_SIZE_Y, hWnd, NULL, hInstance, NULL);
	   hEditStoragePath[k] = CreateWindow(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, i + 70 + IDS_ITEMS_STEP_X, j, 160, IDS_ITEMS_SIZE_Y, hWnd, NULL, hInstance, NULL);
	   hEditStorageBtn[k] = CreateWindow(L"button", L"*", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, i + 70 + 160 + IDS_ITEMS_STEP_X * 2, j, IDS_ITEMS_SIZE_Y, IDS_ITEMS_SIZE_Y, hWnd, (HMENU)(IDM_SELECT_FOLDER + k), hInstance, NULL);

	   SendMessage(hEditStorageExt[k], WM_SETTEXT, NULL, (LPARAM)stStoragePath[k].ext);
	   SendMessage(hEditStoragePath[k], WM_SETTEXT, NULL, (LPARAM)stStoragePath[k].path);

	   j += IDS_ITEMS_SIZE_Y;
	   j += IDS_ITEMS_STEP_Y;
   }


   if (!hWnd)
   {
	   return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
} //InitInstance

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect; // стр-ра, определ€юща€ размер клиентской области
	COLORREF colorText = RGB(255, 0, 0); // задаЄм цвет текста
	UINT i, j, k;
//	WCHAR text[256];

	switch (message)
	{
	case WM_CREATE:
		greeting[0] = '\0';
		//ReadDriveList(hWnd, greeting);

		SetTimer(hWnd, 1, 1000, NULL);
		
		SetForegroundWindow(hWnd);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:

		switch (wmId)
		{
		case IDM_SETTINGS:
			ReadFields(hWnd);
			//wsprintf(text, L"%d - %s", k, stStoragePath[k].path);
			//MessageBox(hWnd, stStoragePath[k].path, text, 0);
			//MessageBox(hWnd, L"Under construction", L"Settings", 0);
			DataLocked = TRUE;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), hWnd, Settings);
			DataLocked = FALSE;
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			if ((wmId >= IDM_SELECT_FOLDER) && (wmId < IDM_SELECT_FOLDER + MAX_STORAGE_PATHS))
			{
				k = (wmId - IDM_SELECT_FOLDER);
				ReadFields(hWnd);

				//Lock settings
				DataLocked = TRUE;
				if (BrowseFolder(stStoragePath[k].path))
				{
					SendMessage(hEditStoragePath[k], WM_SETTEXT, NULL, (LPARAM)stStoragePath[k].path);
				}
				//UnLock settings
				DataLocked = FALSE;
			}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);

			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		// Print static strings
		j = WorkingAreaY;
		i = IDS_ITEMS_START_X;
		TextOut(hdc, i, j, SourceDrvStr, _tcslen(SourceDrvStr));
		j += IDS_ITEMS_SIZE_Y;
		//   j += IDS_ITEMS_STEP_Y;
		j += IDS_ITEMS_SIZE_Y;
		j += IDS_ITEMS_STEP_Y;

		TextOut(hdc, i, j, LocalDrvStr, _tcslen(LocalDrvStr));

		j = WorkingAreaY;
		i += IDS_ITEMS_SIZE_LL;
		i += IDS_ITEMS_STEP_X;

		TextOut(hdc, i, j, StorageExtStr, _tcslen(StorageExtStr));

		i += 70;
		i += IDS_ITEMS_STEP_X;

		TextOut(hdc, i, j, StoragePathStr, _tcslen(StoragePathStr));

		// For DEBUG
		// TBD
		TextOut(hdc,
			10, 210,
			greeting, _tcslen(greeting));
		// End application-specific layout section.
		
		
		GetClientRect(hWnd, &rect); // получаем ширину и высоту области дл€ рисовани€
		SetTextColor(hdc, colorText); // устанавливаем цвет контекстного устройства
		DrawText(hdc, (LPCWSTR)greeting, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // рисуем текст
		// End DEBUG

		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		sec++;
		ReadDriveList(hWnd, greeting);
		ReadFields(hWnd);

		//_itow_s(sec, greeting, 10);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		// Update information from fields
		ReadFields(hWnd);

		//Write settings
		WriteSettings();
		//		MessageBox(NULL, greeting, L"¬рем€ работы программы (сек.):", MB_ICONASTERISK | MB_OK);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
} //WndProc

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_AUTOUPDATE, (stAutoUpdate ? BST_CHECKED : BST_UNCHECKED));
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_AUTOUPDATE)
		{
			stAutoUpdate = (IsDlgButtonChecked(hDlg, IDC_AUTOUPDATE) == BST_CHECKED);
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
} //About

// Message handler for settings dialog.
INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT i;
	WCHAR TempString[4];
	BOOL NeedRestart;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		for (i = 0; i < 7; i++)
		{
			CheckDlgButton(hDlg, IDM_SOURCE_DRIVES + i, (stSourceDriveFilter[i] == '1' ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hDlg, IDM_LOCAL_DRIVES + i, (stLocalDriveFilter[i] == '1' ? BST_CHECKED : BST_UNCHECKED));
		}
		SetDlgItemText(hDlg, IDM_SOURCE_PATH, stSourcePath + 2);
		SetDlgItemText(hDlg, IDM_LOCAL_PATH, stLocalPath + 2);

		for (i = 1; i <= MAX_STORAGE_PATHS; i++)
		{
			_ltow_s(i, TempString, 3, 10);
			SendDlgItemMessage(hDlg, IDM_NUM_STORAGE, CB_ADDSTRING, 0, (LPARAM)TempString);
			if (i == stStoragePaths)
			{
				SendDlgItemMessage(hDlg, IDM_NUM_STORAGE, CB_SELECTSTRING, -1, (LPARAM)TempString);
			}
		}

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			// Save settings and exit
			NeedRestart = FALSE;

			for (i = 0; i < 7; i++)
			{
				TempString[0] = stSourceDriveFilter[i];
				TempString[1] = stLocalDriveFilter[i];
				stSourceDriveFilter[i] = IsDlgButtonChecked(hDlg, IDM_SOURCE_DRIVES + i) == BST_CHECKED ? '1' : '0';
				stLocalDriveFilter[i] = IsDlgButtonChecked(hDlg, IDM_LOCAL_DRIVES + i) == BST_CHECKED ? '1' : '0';
				if ((TempString[0] != stSourceDriveFilter[i]) || (TempString[1] != stLocalDriveFilter[i]))
				{
					NeedRestart = TRUE;
				}
			}

			GetDlgItemText(hDlg, IDM_SOURCE_PATH, stSourcePath + 2, MAX_PATH_STRING - 2);
			GetDlgItemText(hDlg, IDM_LOCAL_PATH, stLocalPath + 2, MAX_PATH_STRING - 2);

			i = SendDlgItemMessage(hDlg, IDM_NUM_STORAGE, CB_GETCURSEL, NULL, NULL);
			i++;

			if (i != stStoragePaths)
			{
				NeedRestart = TRUE;
				stStoragePaths = i;
			}

			if (NeedRestart)
			{
				MessageBox(hDlg, L"Need to restart program to apply changes", L"Information", 0);
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			// Exit without saving settings
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
} //Settings