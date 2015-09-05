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

// Window items
HWND hButnSettings;
HWND hButnSrcToStorage;
HWND hButnSrcToLocal;
HWND hButnLocalToStorage;
HWND hEditStorageExt[MAX_STORAGE_PATHS];
HWND hEditStoragePath[MAX_STORAGE_PATHS];
HWND hComboSource;
HWND hComboLocal;

// Variables to be saved to .ini
BOOL stAutoUpdate = FALSE;
UINT stStoragePaths = 3; 
T_STORAGE_PATH stStoragePath[MAX_STORAGE_PATHS];
WCHAR stSourcePath[MAX_PATH_STRING];
WCHAR stLocalPath[MAX_PATH_STRING];
WCHAR stSourceDriveFilter[8];  // each byte enables/disables drive type 
WCHAR stLocalDriveFilter[8];   // DRIVE_REMOVABLE, DRIVE_FIXED, etc.

// DEBUG variables
BOOL f1, f2;
DWORD i0, i1, i2;
WCHAR currentpath[MAX_PATH_STRING];


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
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
}


BOOL DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

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
		7,
		SettingsFileName
		);

	//SourceLocal/SourcePath
	OK *= GetPrivateProfileString(
		TEXT("SourceLocal"),
		TEXT("LocalDrvFilter"),
		TEXT("001100"),
		stLocalDriveFilter,
		7,
		SettingsFileName
		);

	return OK;
}


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
	for (i = 0; i < stStoragePaths; i++)
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

}



INT ReadDriveList(HWND hWnd, TCHAR text[])
{

	INT n;
	INT i = 0;
	BOOL Flag;
	BOOL prev_avail = FALSE;
	static DWORD prev_dr = 0;
	WCHAR path[] = L" :\\";
	WCHAR drive_fat[30];
	ULARGE_INTEGER NumFreeBytes, FreeBytesAvail, TotalBytes;
	DWORD drive_sn, drive_name_size;

	DWORD dr = GetLogicalDrives(); // функци€ возвращает битовую маску

	if (dr == prev_dr) return 0;

	for (int x = 0; x < 26; x++) // проходимс€ циклом по битам
	{
		n = ((dr >> x) & 1); // узнаЄм значение текущего бита

		prev_avail = drives[x].avail;
		drives[x].avail = FALSE;

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

				i += wsprintf(text + i, L"%S:[%s](%d.%dGb) ", 
					path, 
					drives[x].label,
					drives[x].totalMB / 1000,
					(drives[x].totalMB % 1000) / 10
					);

				wsprintf(drives[x].cb_text, L"%S:[%s](%d.%dGb) ",
					path,
					drives[x].label,
					drives[x].totalMB / 1000,
					(drives[x].totalMB % 1000) / 10
					);

			}


		}
		else if (prev_avail && n) //drive lost
		{
		
		
		}

	}

	InvalidateRect(hWnd, NULL, TRUE);
	prev_dr = dr;
	return 1;
}

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

   HWND hedit;
   HWND hCombo;


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
   hButnSrcToStorage = CreateWindow(L"button", L"Source => Local", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   i, j, 150, IDS_ITEMS_SIZE_Y, hWnd, (HMENU)IDM_SRC_LOCAL, hInstance, NULL);
   i += 150;
   i += IDS_ITEMS_STEP_X;

   hButnSrcToStorage = CreateWindow(L"button", L"Local => Storage", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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

   hComboSource = CreateWindow(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST, i, j, 150, IDS_ITEMS_SIZE_Y, hWnd, 0, hInstance, 0);

   j += IDS_ITEMS_SIZE_Y;
//   j += IDS_ITEMS_STEP_Y;
   j += IDS_ITEMS_SIZE_Y;
   j += IDS_ITEMS_STEP_Y;

   hComboLocal = CreateWindow(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST, i, j, 150, IDS_ITEMS_SIZE_Y, hWnd, 0, hInstance, 0);

   // Right side
   j = WorkingAreaY;
   j += IDS_ITEMS_SIZE_Y;
   //   j += IDS_ITEMS_STEP_Y;
   i += 150;
   i += IDS_ITEMS_STEP_X;

   for (k = 0; k < stStoragePaths; k++)
   {
	   hEditStorageExt[k] = CreateWindow(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, i, j, 70, IDS_ITEMS_SIZE_Y, hWnd, NULL, hInstance, NULL);
	   hEditStoragePath[k] = CreateWindow(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, i + 70 + IDS_ITEMS_STEP_X, j, 160, IDS_ITEMS_SIZE_Y, hWnd, NULL, hInstance, NULL);
	   
	   SendMessage(hEditStorageExt[k], WM_SETTEXT, NULL, (LPARAM)stStoragePath[k].ext);
	   SendMessage(hEditStoragePath[k], WM_SETTEXT, NULL, (LPARAM)stStoragePath[k].path);

	   j += IDS_ITEMS_SIZE_Y;
	   j += IDS_ITEMS_STEP_Y;
   }

   //hEditStorageExt[MAX_STORAGE_PATHS];
   //hEditStoragePath[MAX_STORAGE_PATHS];

//   IDS_ITEMS_STEP_Y
   
   hedit = CreateWindow(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 30, 240, 30, 230, hWnd, NULL, hInstance, NULL);
   hCombo = CreateWindow(L"COMBOBOX", L"combobox", WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST, 10, 280, 250, 500, hWnd, 0, hInstance, 0);

   SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"T1");
   SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"T2");
   SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"T3");

   //SendMessage(hCombo, CB_SELECTSTRING, 0, (LPARAM)L"T1");
   SendMessage(hCombo, CB_SETCURSEL, 0, NULL);

   
   SendMessage(hCombo, CB_DELETESTRING, 1, NULL);
   SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"T0");

   

   if (!hWnd)
   {
	   return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

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
	static int sec = 0;
	static WCHAR greeting[255];
	static char text[2] = { ' ', '\0' };
	UINT i, j;


	switch (message)
	{
	case WM_CREATE:
		greeting[0] = '\0';
		ReadDriveList(hWnd, greeting);

		SetTimer(hWnd, 1, 1000, NULL);
		//...

		SetForegroundWindow(hWnd);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:

		switch (wmId)
		{
		case IDM_SETTINGS:
			MessageBox(hWnd, L"Settings", L"", 0);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			//WriteSettings();
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		// Here your application is laid out.
		// For this introduction, we just print out "Hello, World!"
		// in the top left corner.
		
		j = WorkingAreaY;
		i = IDS_ITEMS_START_X;
		TextOut(hdc, i, j, SourceDrvStr, _tcslen(SourceDrvStr));
		j += IDS_ITEMS_SIZE_Y;
		//   j += IDS_ITEMS_STEP_Y;
		j += IDS_ITEMS_SIZE_Y;
		j += IDS_ITEMS_STEP_Y;

		TextOut(hdc, i, j, LocalDrvStr, _tcslen(LocalDrvStr));

		j = WorkingAreaY;
		i += 150;
		i += IDS_ITEMS_STEP_X;

		TextOut(hdc, i, j, StorageExtStr, _tcslen(StorageExtStr));

		i += 70;
		i += IDS_ITEMS_STEP_X;

		TextOut(hdc, i, j, StoragePathStr, _tcslen(StoragePathStr));

		TextOut(hdc,
			10, 210,
			greeting, _tcslen(greeting));
		// End application-specific layout section.
		

		GetClientRect(hWnd, &rect); // получаем ширину и высоту области дл€ рисовани€
		SetTextColor(hdc, colorText); // устанавливаем цвет контекстного устройства
		DrawText(hdc, (LPCWSTR)text, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER); // рисуем текст

		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case VK_HOME:case VK_END:case VK_PRIOR:
		case VK_NEXT:case VK_LEFT:case VK_RIGHT:
		case VK_UP:case VK_DOWN:case VK_DELETE:
		case VK_SHIFT:case VK_SPACE:case VK_CONTROL:
		case VK_CAPITAL:case VK_MENU:case VK_TAB:
		case VK_BACK:case VK_RETURN: case VK_ESCAPE:
			break;
		default:
			text[0] = (char)wParam;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		break;

	case WM_TIMER:
		sec++;
		ReadDriveList(hWnd, greeting);

		//_itow_s(sec, greeting, 10);
		//InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		//Write settings
		WriteSettings();
		//		MessageBox(NULL, greeting, L"¬рем€ работы программы (сек.):", MB_ICONASTERISK | MB_OK);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

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
}
