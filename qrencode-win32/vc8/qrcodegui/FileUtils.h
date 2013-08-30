#pragma once

class FileUtils
{
public:
	FileUtils(void) { m_strIniFile = L""; };
	~FileUtils(void) {};

	CString GetIniValue(CString strKey);
	void SetIniValue(CString strKey, CString strValue);

	void CheckCreateIni();

protected:
	CString m_strIniFile;
};

class FileSession
{
public:
	FileSession();
	~FileSession();

	void Reset();
	BOOL Generate(CString strPixel, CString strMargin, CString strError, CString strText, long fgcolor, long bgcolor);

	void CopyImageTo(CString strNewPath);
	CString GetErrorMessage() { return m_strErrorMsg; };
	CString GetImagePath() { return m_strImageFile; };
protected:
	BOOL RunQRcode(CString strPixel, CString strMargin, CString strError, long fgcolor, long bgcolor);

protected:
	CString m_strTextFile;
	CString m_strImageFile;
	CString m_strResultFile;

	CString m_strErrorMsg;
};
