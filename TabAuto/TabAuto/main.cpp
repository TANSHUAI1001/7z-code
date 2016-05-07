#include <windows.h>
#include <tchar.h>
#include <stdio.h>

DWORD WINAPI GetFinalPathNameByHandle(
	_In_  HANDLE hFile,
	_Out_ LPTSTR lpszFilePath,
	_In_  DWORD  cchFilePath,
	_In_  DWORD  dwFlags
	);

#define BUFSIZE MAX_PATH

void __cdecl _tmain(int argc, TCHAR *argv[])
{
	TCHAR Path[BUFSIZE];
	HANDLE hFile;
	DWORD dwRet;

	printf("\n");
	if( argc != 2 )
	{
		printf("ERROR:\tIncorrect number of arguments\n\n");
		printf("%s <file_name>\n", argv[0]);
		return;
	}

	hFile = CreateFile(argv[1],               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if( hFile == INVALID_HANDLE_VALUE)
	{
		printf("Could not open file (error %d\n)", GetLastError());
		return;
	}

	dwRet = GetFinalPathNameByHandle( hFile, Path, BUFSIZE, VOLUME_NAME_NT );
	if(dwRet < BUFSIZE)
	{
		_tprintf(TEXT("\nThe final path is: %s\n"), Path);
	}
	else printf("\nThe required buffer size is %d.\n", dwRet);

	CloseHandle(hFile);
	system("pause");
}
