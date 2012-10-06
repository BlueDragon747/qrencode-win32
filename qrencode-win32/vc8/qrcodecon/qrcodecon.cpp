// qrcodecon.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define MAX_DATA_SIZE (7090 * 16) /* from the specification */
#define CMD_SIZE (MAX_DATA_SIZE + 4096) /* from the specification */


BOOL IsOption(wchar_t *p);
BOOL IsOptionWithParameter(wchar_t *p);
wchar_t *readStdin();

int _tmain(int argc, _TCHAR* argv[])
{
	char bufOut[CMD_SIZE] = {0};
	TCHAR bufCmd[CMD_SIZE] = {0};
	TCHAR *intext = NULL;
	if(argc == 1)
		wcsncat_s(bufCmd, L"qrcode.exe -h", CMD_SIZE);
	else
		wcsncat_s(bufCmd, L"qrcode.exe", CMD_SIZE);

	int i;
	for(i = 1; i < argc; i++)
	{
		if(!IsOption(argv[i]))
			break;

		wcsncat_s(bufCmd, L" ", CMD_SIZE);
		wcsncat_s(bufCmd, argv[i], CMD_SIZE);
		if(IsOptionWithParameter(argv[i]) && (i + 1 < argc))
		{
			i++;
			wcsncat_s(bufCmd, L" ", CMD_SIZE);
			wcsncat_s(bufCmd, argv[i], CMD_SIZE);
		}
	}

	if(i < argc)
	{
		wcsncat_s(bufCmd, L" \"", CMD_SIZE);
		wcsncat_s(bufCmd, argv[i], CMD_SIZE);
		wcsncat_s(bufCmd, L"\"", CMD_SIZE);
	}
	else
	{
		intext = readStdin();
		wcsncat_s(bufCmd, L" \"", CMD_SIZE);
		wcsncat_s(bufCmd, intext, CMD_SIZE);
		wcsncat_s(bufCmd, L"\"", CMD_SIZE);
	}

	DWORD len = ::WideCharToMultiByte(CP_UTF8, 0, bufCmd, wcslen(bufCmd), bufOut, CMD_SIZE, NULL, NULL);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	if( !CreateProcessA( NULL,   // No module name (use command line)
						bufOut,        // Command line
						NULL,           // Process handle not inheritable
						NULL,           // Thread handle not inheritable
						TRUE,          // Set handle inheritance to FALSE
						0,
						NULL,           // Use parent's environment block
						NULL,           // Use parent's starting directory 
						&si,            // Pointer to STARTUPINFO structure
						&pi )           // Pointer to PROCESS_INFORMATION structure
	) 
	{
		return FALSE;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );
	
	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return 0;
}

BOOL IsOption(wchar_t *arg)
{
	if( (!wcscmp(arg, L"-h")) ||
		(!wcscmp(arg, L"--help")) ||

		(!wcscmp(arg, L"-o")) ||
		(!wcscmp(arg, L"-s")) ||
		(!wcscmp(arg, L"-l")) ||
		(!wcscmp(arg, L"-v")) ||
		(!wcscmp(arg, L"-m")) ||
		(!wcscmp(arg, L"-d")) ||
		(!wcscmp(arg, L"-t")) ||

		(!wcscmp(arg, L"-S")) ||
		(!wcscmp(arg, L"-k")) ||
		(!wcscmp(arg, L"-c")) ||
		(!wcscmp(arg, L"-i")) ||
		(!wcscmp(arg, L"-8")) ||
		(!wcscmp(arg, L"-M")) ||
		(!wcscmp(arg, L"-V"))
		)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IsOptionWithParameter(wchar_t *arg)
{
	if(	(!wcscmp(arg, L"-o")) ||
		(!wcscmp(arg, L"-s")) ||
		(!wcscmp(arg, L"-l")) ||
		(!wcscmp(arg, L"-v")) ||
		(!wcscmp(arg, L"-m")) ||
		(!wcscmp(arg, L"-d")) ||
		(!wcscmp(arg, L"-t")) 
		)
	{
		return TRUE;
	}

	return FALSE;
}

static wchar_t *readStdin()
{
	char buffer[MAX_DATA_SIZE];
	static wchar_t bufferOut[MAX_DATA_SIZE];
	memset(bufferOut, 0, sizeof(bufferOut));
	int ret;

	ret = fread(buffer, 1, MAX_DATA_SIZE, stdin);
	if(ret == 0) {
		fprintf(stderr, "No input data.\n");
		return L"";
	}
	if(feof(stdin) == 0) {
		fprintf(stderr, "Input data is too large.\n");
		return L"";
	}

	buffer[ret] = '\0';

	::MultiByteToWideChar(CP_OEMCP, 0, buffer, ret, bufferOut, MAX_DATA_SIZE);

	return bufferOut;
}