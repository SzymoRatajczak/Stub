//it contains functions changing icons
#include "Icon.h"

bool changeIcon(const std::string & file, const std::string & icon)
{
	DWORD bytesRead;
	
	HANDLE hFile = NULL;
	
	HANDLE hUpdate = NULL;
	
	LPGRPICONDIR pGripIconDir = NULL;
	
	LPICONDIR pIconDir = NULL;
	
	LPICONIMAGE* pIconImages = NULL;


	//open icon's file

	if ((hFile = CreateFile((LPTSTR)icon.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return false;
	}

	//allocation memory on ICONDIR structure
	if ((pIconDir = (LPICONDIR)malloc(sizeof(ICONDIR)) )== NULL)
	{
		CloseHandle(hFile);
		return false;
	}

	//read structure
	//first paramter-plik to read
	//second parameter- what exactly i want to read
	//number of bytes to read
	//number of read bytes
	//template
	if (ReadFile(hFile, &(pIconDir->idReserved), sizeof(WORD), &bytesRead, NULL) == FALSE)
	{
		free(pIconDir);
		CloseHandle(hFile);
		return false;
	}

	//if value is egual size of word
	if (bytesRead !=sizeof(WORD))
	{
		free(pIconDir);
		CloseHandle(hFile);
		return false;
	}
	//if value is correct
	if (pIconDir->idReserved != 0)
	{
		free(pIconDir);
		CloseHandle(hFile);
		return false;

	}

	//read types
	if (ReadFile(hFile, &(pIconDir->idType), sizeof(WORD), &bytesRead, NULL) == FALSE)
	{

		free(pIconDir);
		CloseHandle(hFile);
		return  false;
	}

	//check size
	if (bytesRead != sizeof(WORD))
	{
		free(pIconDir);
		CloseHandle(hFile);
		return false;
	}

//check type of icon
if (pIconDir->idType != 1)
{
	free(pIconDir);
	CloseHandle(hFile);
	return false;
}


//read amount of images
if (ReadFile(hFile, &(pIconDir->idCount), sizeof(WORD), &bytesRead, NULL) == FALSE)
{
	free(pIconDir);
	CloseHandle(hFile);
	return false;
}

if (bytesRead != sizeof(WORD))
{
	free(pIconDir);
	CloseHandle(hFile);
	return false;
}

//relocations pIconDir  in order to get enough space for every images
if ((pIconDir = (LPICONDIR)realloc(pIconDir, (sizeof(WORD) * 3) + (sizeof(ICONDIRENTRY)*pIconDir->idCount))) == NULL)
{
	free(pIconDir);
	CloseHandle(hFile);
	return false;
}

//read ICONDIRENTRY's elements
if (ReadFile(hFile, pIconDir->idEntries, sizeof(ICONDIRENTRY) * pIconDir->idCount, &bytesRead, NULL) == FALSE)
{
	free(pIconDir);
	CloseHandle(hFile);
	return false;
}

//alocate memory  for pointer to ICONIMAGE structure
if ((pIconImages = (LPICONIMAGE*)malloc(sizeof(LPICONIMAGE) * pIconDir->idCount)) == NULL)
{
	free(pIconDir);
	CloseHandle(hFile);
	return false;
}

//loop's reading all images in icon file
for (int i = 0; i < pIconDir->idCount; i++)
{
	//alocate memory for each image
	if ((pIconImages[i] = (LPICONIMAGE)malloc(pIconDir->idEntries[i].dwBytesInRest)) == NULL)
	{
		for (int j=0; j < i; j++)
		{
			free(pIconImages[j]);
		}
		free(pIconImages);
		free(pIconDir);
		CloseHandle(hFile);
		return false;

	}

	//put cursor on image 
	if (SetFilePointer(hFile, pIconDir->idEntries[i].dwImageOffSet, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		for (int j=0; j < i; j++)
			free(pIconImages[j]);
		free(pIconImages);
		free(pIconDir);
		CloseHandle(hFile);
		return false;
	}

	//read data from image
	if (ReadFile(hFile, pIconImages[i], pIconDir->idEntries[i].dwBytesInRest, &bytesRead, NULL) == FALSE)
	{
		for (int j=0; j < i; j++)
			free(pIconImages[j]);
		free(pIconImages);
		free(pIconDir);
		CloseHandle(hFile);
		return false;
	}

	//check if you got proper amount of bytes
	if (bytesRead != pIconDir->idEntries[i].dwBytesInRest)
	{
		for (int j=0; j < i; j++)
			free(pIconImages[j]);
		free(pIconDir);
		free(pIconImages);
		CloseHandle(hFile);
		return false;
	}

	//allocate memeory for ICONDIR
	if ((pGripIconDir = (LPGRPICONDIR)malloc(sizeof(WORD) * 3) + (sizeof(GRPICONDIRENTRY)*pIconDir->idCount)) == NULL)
		{
		for (int j = 0; j < pIconDir->idCount; j++)
			free(pIconImages[j]);
		free(pIconImages);
		free(pIconDir);
		CloseHandle(hFile);
		return false;
}
	pGripIconDir->idReserved = 0;
	pGripIconDir->idType = 1;
	pGripIconDir->idCount = pIconDir->idCount;
	

	for (int i = 0; i < pGripIconDir->idCount; i++)
	{
		pGripIconDir->idEntries[i].bWidth = pIconDir->idEntries[i].bWidth;
		pGripIconDir->idEntries[i].bHeight = pIconDir->idEntries[i].bHeight;
		pGripIconDir->idEntries[i].bColorCount = pIconDir->idEntries[i].bColorCount;
		pGripIconDir->idEntries[i].bReserved = pIconDir->idEntries[i].bReserved;
		pGripIconDir->idEntries[i].wPlanes = pIconDir->idEntries[i].wPlanes;
		pGripIconDir->idEntries[i].bBitCount = pIconDir->idEntries[i].bBitCount;
		pGripIconDir->idEntries[i].bBytesInRest = pIconDir->idEntries[i].bBytesInRest;
		pGripIconDir->idEntries[i].nID = i + 1;



	}

	//start changing icon
	if ((hUpdate = BeginUpdateResourceA(file.c_str(),FALSE)) == NULL)
	{
		free(pGripIconDir);
		for (int j = 0; j < pIconDir->idCount; j++)
			free(pIconImages[j]);
		free(pIconImages);
		free(pIconDir);
		CloseHandle(hFile);
		return false;
	}


	//update  RT_GROUP_ICON resource
	if (UpdateResourceA(hUpdate, MAKEINTRESOURCEA(RT_GROUP_ICON), MAKEINTRESOURCEA(32512), MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), pGripIconDir, (sizeof(WORD) * 3) + (sizeof(GRPICONDIRENTRY)*pIconDir->idCount)) == FALSE)
	{
		free(pGripIconDir);
		for (int j; j < pIconDir->idCount; j++)
			free(pIconImages[j]);
		free(pIconImages);
		free(pIconDir);
		CloseHandle(hFile);
		return false;
	}

	//update RT_ICON resource
	for (int i = 0; i < pIconDir->idCount; i++)
	{
		if (UpdateResourceA(hUpdate, MAKEINTRESOURCEA(3), MAKEINTRESOURCEA(i + 1), MAKELANGID(LANG_NEUTRAL, LANG_NEUTRAL), pIconImages[i], pIconDir->idEntries[i].dwBytesInRest) == FALSE)
		{
			free(pGripIconDir);
			for (int j = 0; j < pIconDir->idCount; j++)
				free(pIconImages[j]);
			free(pIconImages);
			free(pIconDir);
			CloseHandle(hFile);
			return false;
		}

	}

	//save changes
	if (EndUpdateResourceA(hUpdate, FALSE) ==FALSE)
	{
		free(pGripIconDir);
		for (int j = 0; j<pIconDir->idCount; j++)
			free(pIconImages[j]);
		free(pIconImages);
		free(pIconDir);
		CloseHandle(hFile);
		return false;
	}

	//free memory and handlers
	free(pGripIconDir);
	for (int j = 0; j < pIconDir->idCount; j++)
		free(pIconImages[j]);
	free(pIconImages);
	free(pIconDir);
	CloseHandle(hFile);
	return false;



	}

}