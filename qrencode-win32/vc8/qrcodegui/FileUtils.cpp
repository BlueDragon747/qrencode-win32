#include "StdAfx.h"
#include "FileUtils.h"

CString FileUtils::GetIniValue(CString strKey)
{
	CheckCreateIni();

	wchar_t buf[1024];
	GetPrivateProfileString(L"qrcodegui", strKey, L"", buf, 1024, m_strIniFile);

	return CString(buf);
}

void FileUtils::SetIniValue(CString strKey, CString strValue)
{
	CheckCreateIni();

	WritePrivateProfileString(L"qrcodegui", strKey, strValue, m_strIniFile);
}

void FileUtils::CheckCreateIni()
{
	if(!m_strIniFile.IsEmpty())
		return;

	wchar_t path[1024];
	CString strPath;
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path);
	strPath = path;

	m_strIniFile = strPath;
	m_strIniFile += L"\\QRCodeGui\\Settings.ini";

	if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_strIniFile))
	{
		strPath += L"\\QRCodeGui";
		::CreateDirectory(strPath, NULL);

		strPath += L"\\Settings.ini";
		HANDLE hFile = ::CreateFile(strPath,
			GENERIC_WRITE,          // open for writing
			0,                      // do not share
			NULL,                   // default security
			CREATE_NEW,             // create new file only
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);

		if(hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwWritten;
			char buf[] = "[qrcodegui]\noutputx=20\noutputy=20\nwinx=20\nwiny=20\nwinw=400\nwinh=300\npixel=3\nmargin=3\nlevel=L";
			::WriteFile(hFile, buf, sizeof(buf), &dwWritten, NULL);
			::CloseHandle(hFile);
		}
	}
}

FileSession::FileSession()
{
	m_strTextFile = L"";
	m_strImageFile = L"";
	m_strResultFile = L"";
}

FileSession::~FileSession()
{
	Reset();
}


void FileSession::Reset()
{
	m_strErrorMsg = L"";

	if(!m_strTextFile.IsEmpty())
		::DeleteFile(m_strTextFile);

	if(!m_strImageFile.IsEmpty())
		::DeleteFile(m_strImageFile);

	if(!m_strResultFile.IsEmpty())
		::DeleteFile(m_strResultFile);
}


void FileSession::CopyImageTo(CString strNewPath)
{
	if(!m_strImageFile.IsEmpty())
		::CopyFile(m_strImageFile, strNewPath, FALSE);
}

BOOL FileSession::RunQRcode(CString strPixel, CString strMargin, CString strError, long fgcolor, long bgcolor)
{
	CString strCmd;
	CString strFgColorPart;
	CString strBgColorPart;
	if(fgcolor)
		strFgColorPart.Format(L"--foreground=%06X ", fgcolor);
	if(bgcolor)
		strBgColorPart.Format(L"--background=%06X ", bgcolor);

	strCmd.Format(L"cmd.exe /c qrcode.exe %s%s-s %s -m %s -l %c -o %s 2> %s < %s", 
		strFgColorPart,
		strBgColorPart,
		strPixel,
		strMargin, 
		strError[0],
		m_strImageFile,
		m_strResultFile,
		m_strTextFile
	);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	if( !CreateProcess( NULL,   // No module name (use command line)
						const_cast<LPWSTR>((LPCWSTR)strCmd),        // Command line
						NULL,           // Process handle not inheritable
						NULL,           // Thread handle not inheritable
						FALSE,          // Set handle inheritance to FALSE
						CREATE_NO_WINDOW,
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

	return TRUE;
}

BOOL FileSession::Generate(CString strPixel, CString strMargin, CString strError, CString strText, long fgcolor, long bgcolor)
{
	Reset();

	DWORD dwWritten = 0;
	long micro_version = 0;
	wchar_t lpTempPathBuffer[MAX_PATH];
	wchar_t szTempFileName[MAX_PATH];
	GetTempPath(MAX_PATH, lpTempPathBuffer); 

	GetTempFileName(lpTempPathBuffer, TEXT("QRT"), 0, szTempFileName);
	m_strTextFile = szTempFileName;

	char buf[7*32*1024];
	DWORD len = ::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)strText, strText.GetLength(), buf, 7*32*1024, NULL, NULL);

	if(!len)
	{
		m_strErrorMsg = "Conversion to UTF-8 string failed.";
		return FALSE;
	}
	
	HANDLE hFile = ::CreateFile(m_strTextFile, GENERIC_WRITE, 0, NULL, 
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	::WriteFile(hFile, buf, len, &dwWritten, NULL);
	::CloseHandle(hFile);

	GetTempFileName(lpTempPathBuffer, TEXT("QRI"), 0, szTempFileName);
	m_strImageFile = szTempFileName;

	GetTempFileName(lpTempPathBuffer, TEXT("QRR"), 0, szTempFileName);
	m_strResultFile = szTempFileName;

	if(!RunQRcode(strPixel, strMargin, strError, fgcolor, bgcolor))
	{
		m_strErrorMsg = "QRCode.exe execution failed.";
		return FALSE;
	}

	{
		char buf[32*1024];
		DWORD dwRead = 0;
		HANDLE hFile = ::CreateFile(m_strResultFile, GENERIC_READ, 0, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		::ReadFile(hFile, buf, 32*1024, &dwRead, NULL);
		::CloseHandle(hFile);
		if(dwRead)
		{
			wchar_t bufw[32*1024];	
			ZeroMemory(bufw, 32*1024*sizeof(wchar_t));
			::MultiByteToWideChar(CP_OEMCP, 0, buf, dwRead, bufw, 32*1024); 
			m_strErrorMsg = bufw;
			return FALSE;
		}
	}

	return TRUE;
}


