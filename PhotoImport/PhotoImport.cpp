// PhotoImport.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PhotoImport.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

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

INT ReadDriveList(HWND hWnd, TCHAR text[])
{

	INT n;
	INT i = 0;
	//BOOL Flag;
	static DWORD prev_dr = 0;
	WCHAR path[5] = L" :\\\0";

	DWORD dr = GetLogicalDrives(); // функция возвращает битовую маску

	if (dr == prev_dr) return 0;

	for (int x = 0; x < 26; x++) // проходимся циклом по битам
	{
		n = ((dr >> x) & 1); // узнаём значение текущего бита
		if (n) // если единица - диск с номером x есть
		{

			//MessageBox(hWnd, L"1", L"", 0);

			WCHAR dl = ('A' + x); // получаем литеру диска
			path[0] = dl;

			WCHAR out[50] = L"\0";

			//MessageBox(hWnd, path, L"", 0);

			//_itow_s(sec, text, 10);
			text[i] = dl;
			// здесь узнаём готово ли устройство
			/*
			WORD OldErrorMode;
			OldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS); // убираем показ ошибок
			BOOL ready = DirectoryExists(path); // пытаемcя открыть корневую директорию
			SetErrorMode(OldErrorMode); // восстанавливаем старый режим показа ошибок
			*/
			/*

			if (ready)
			{
			*/
				UINT drive_type = GetDriveType(path); // узнаём тип диска

				i++;
				text[i] = drive_type + '0';
				i++;
				text[i] = '\0';
				/*
				out += L"\nТип диска: ";
				if (drive_type == DRIVE_REMOVABLE) out += L"REMOVABLE";
				else if (drive_type == DRIVE_FIXED)     out += L"FIXED";
				else if (drive_type == DRIVE_REMOTE)   out += L"REMOTE";
				else if (drive_type == DRIVE_CDROM)     out += L"CD-ROM";
				else if (drive_type == DRIVE_RAMDISK)   out += L"RAMDISK";
				else out += L"НЕИЗВЕСТНЫЙ_ТИП\n";

				// если это HDD - заприашиваем информацию о нем
				if (drive_type == DRIVE_FIXED)
				{
					unsigned __int64 FreeBytesAvailable;
					unsigned __int64 TotalNumberOfBytes;
					unsigned __int64 TotalNumberOfFreeBytes;
					char drive_label[30];
					char drive_fat[30];
					DWORD drive_sn;
					DWORD drive_name_size = sizeof(drive_label);

					// получаем данные о размерах
					Flag = ::GetDiskFreeSpaceEx(path.c_str(),
						(PULARGE_INTEGER)&FreeBytesAvailable,
						(PULARGE_INTEGER)&TotalNumberOfBytes,
						(PULARGE_INTEGER)&TotalNumberOfFreeBytes
						);
					if (Flag)
					{
						out += "\nСвободно на диске: " + AnsiString(TotalNumberOfFreeBytes) + "\n";
						out += "Всего на диске: " + AnsiString(TotalNumberOfBytes) + "\n";
					}
					else
					{
						out += "Ошибка в GetDiskFreeSpaceEx\n";
					}

					// получаем метку, серинийный номер и пр.
					Flag = GetVolumeInformation(path.c_str(),
						drive_label,
						sizeof(drive_label),
						&drive_sn,
						&drive_name_size,
						NULL,
						drive_fat,
						sizeof(drive_fat)
						);
					if (Flag)
					{
						out += "\nМетка тома: " + AnsiString(drive_label) + "\n";
						out += "Сер.номер: " + AnsiString(drive_sn) + "\n";
						out += "Файловая система: " + AnsiString(drive_fat) + "\n";
					}
					else
					{
						out += "Ошибка в GetVolumeInformation\n";
					}
				}
				*/


			/*
			}
			else
			{
				out += "НЕ ГОТОВ";
			}
			ShowMessage(out);
			*/
		}
	}
	//for (; i >= 0; i--) text[i] = path[i];

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
   HWND hButnSettings;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);


   hButnSettings = CreateWindow(L"button", L"Settings", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   300, 10, 80, 30, hWnd, (HMENU)IDM_SETTINGS, hInstance, NULL);

   if (!hButnSettings)
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
	RECT rect; // стр-ра, определяющая размер клиентской области
	COLORREF colorText = RGB(255, 0, 0); // задаём цвет текста
	static int sec = 0;
	static WCHAR greeting[100];
	static char text[2] = { ' ', '\0' };


	switch (message)
	{
	case WM_CREATE:
		greeting[0] = '\0';

		SetTimer(hWnd, 1, 1000, NULL);
		//...

		ReadDriveList(hWnd, greeting);

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
		
		TextOut(hdc,
			10, 10,
			greeting, _tcslen(greeting));
		// End application-specific layout section.
		

		GetClientRect(hWnd, &rect); // получаем ширину и высоту области для рисования
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
		//_itow_s(sec, greeting, 10);
		//InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
//		MessageBox(NULL, greeting, L"Время работы программы (сек.):", MB_ICONASTERISK | MB_OK);
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
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
