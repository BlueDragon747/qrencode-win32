// qrcodegui.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "qrcodegui.h"
using namespace Gdiplus;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
FileSession *gpFileSession = NULL;
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;


// Forward declarations of functions included in this code module:
INT_PTR CALLBACK	MainWnd(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ImageWnd(HWND, UINT, WPARAM, LPARAM);



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;

	wchar_t szPath[MAX_PATH] = {0}; 
	GetModuleFileName( NULL, szPath, MAX_PATH);
	CString strPath = szPath;
	int pos = strPath.ReverseFind('\\');
	SetCurrentDirectory(strPath.Left(pos));

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	gpFileSession = new FileSession;

	DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, MainWnd);

	delete gpFileSession;
	gpFileSession = NULL;
	
	GdiplusShutdown(gdiplusToken);

	return 0;
}

INT_PTR CALLBACK MainWnd(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_SIZE:
		{
			if(wParam != SIZE_RESTORED)
				return (INT_PTR)FALSE;

			LayoutCalc lc(LOWORD(lParam), HIWORD(lParam));
			SetWindowPos(GetDlgItem(hDlg, IDC_EDIT_TEXT), NULL, lc.x1(), lc.y1(), lc.w1(), lc.h1(), SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hDlg, IDOK), NULL, lc.x2(), lc.y2(), lc.w2(), lc.h2(), SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hDlg, IDCANCEL), NULL, lc.x3(), lc.y3(), lc.w3(), lc.h3(), SWP_NOZORDER);
		}
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
		{
			FileUtils fu;
			SetWindowText(GetDlgItem(hDlg, IDC_EDIT_PIXEL), fu.GetIniValue(L"pixel"));
			SetWindowText(GetDlgItem(hDlg, IDC_EDIT_MARGIN), fu.GetIniValue(L"margin"));

			SendMessage(GetDlgItem(hDlg, IDC_COMBO_LEVEL), CB_ADDSTRING, 0, (LPARAM)L"L (lowest)"); 
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_LEVEL), CB_ADDSTRING, 0, (LPARAM)L"M"); 
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_LEVEL), CB_ADDSTRING, 0, (LPARAM)L"Q"); 
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_LEVEL), CB_ADDSTRING, 0, (LPARAM)L"H (highest)");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_LEVEL), CB_SELECTSTRING, 0, (LPARAM)(LPCWSTR)fu.GetIniValue(L"level"));

			SetWindowPos(hDlg, NULL, 
				_wtoi(fu.GetIniValue(L"winx")), 
				_wtoi(fu.GetIniValue(L"winy")), 
				_wtoi(fu.GetIniValue(L"winw")), 
				_wtoi(fu.GetIniValue(L"winh")), 
				SWP_NOZORDER);

		    HICON hIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_QRCODEGUI));
			SendMessage (hDlg, WM_SETICON, WPARAM (ICON_SMALL), LPARAM (hIcon));
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			FileUtils fu;
			RECT rc;
			GetWindowRect(hDlg, &rc); 
			wchar_t buf[100];
			_itow_s(rc.left, buf, 100, 10);
			fu.SetIniValue(L"winx", buf);
			_itow_s(rc.top, buf, 100, 10);
			fu.SetIniValue(L"winy", buf);
			_itow_s(rc.right - rc.left, buf, 100, 10);
			fu.SetIniValue(L"winw", buf);
			_itow_s(rc.bottom - rc.top, buf, 100, 10);
			fu.SetIniValue(L"winh", buf);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDOK)
		{
			wchar_t szPixel[1024];
			wchar_t szMargin[1024];
			wchar_t szLevel[1024];
			wchar_t szText[32*1024];
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT_PIXEL), szPixel, 1024);
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT_MARGIN), szMargin, 1024);
			GetWindowText(GetDlgItem(hDlg, IDC_COMBO_LEVEL), szLevel, 1024);
			GetWindowText(GetDlgItem(hDlg, IDC_EDIT_TEXT), szText, 32*1024);
			BOOL bRes = gpFileSession->Generate(szPixel, szMargin, szLevel, szText);
			if(bRes)
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_IMAGE), hDlg, ImageWnd);
			}
			else
			{
				MessageBox(hDlg, gpFileSession->GetErrorMessage(), szTitle, MB_OK | MB_ICONERROR);
			}

			FileUtils fu;
			fu.SetIniValue(L"pixel", szPixel);
			fu.SetIniValue(L"margin", szMargin);
			fu.SetIniValue(L"level", szLevel);

			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

Image *myImage;

INT_PTR CALLBACK ImageWnd(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			myImage = new Image(gpFileSession->GetImagePath(), true);
		
			FileUtils fu;
			LayoutCalc lc(myImage->GetHeight(), myImage->GetHeight());

			SetWindowPos(hDlg, NULL, _wtoi(fu.GetIniValue(L"outputx")), 
				_wtoi(fu.GetIniValue(L"outputy")), lc.w7(), lc.h7(), SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hDlg, IDOK), NULL, lc.x8(), lc.y8(), lc.w8(), lc.h8(), SWP_NOZORDER);
			SetWindowPos(GetDlgItem(hDlg, IDCANCEL), NULL, lc.x9(), lc.y9(), lc.w9(), lc.h9(), SWP_NOZORDER);
		}
		return (INT_PTR)TRUE;

	case WM_PAINT:
		{
			if(!myImage)
				return 0;

	        HDC          hdc;
	        PAINTSTRUCT  ps;

		    hdc = BeginPaint(hDlg, &ps);
			Graphics graphics(hdc);
			graphics.DrawImage(myImage, LayoutCalc::mw, LayoutCalc::mw);
			EndPaint(hDlg, &ps);
		}
		return (INT_PTR)FALSE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			FileUtils fu;
			RECT rc;
			GetWindowRect(hDlg, &rc); 
			wchar_t buf[100];
			_itow_s(rc.left, buf, 100, 10);
			fu.SetIniValue(L"outputx", buf);
			_itow_s(rc.top, buf, 100, 10);
			fu.SetIniValue(L"outputy", buf);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{
			OPENFILENAME ofn;       // common dialog box structure
			ZeroMemory(&ofn, sizeof(ofn));
			wchar_t szFile[MAX_PATH];
			szFile[0] = '\0'; 
			 
			ofn.lStructSize = sizeof(OPENFILENAME); 
			ofn.hwndOwner = hDlg; 
			ofn.lpstrFilter = L"PNG Image\0*,png\0";
			ofn.lpstrFile= szFile; 
			ofn.nMaxFile = sizeof(szFile)/ sizeof(*szFile); 
			ofn.lpstrFileTitle = NULL; 
			ofn.nMaxFileTitle = 0; 
			ofn.lpstrInitialDir = (LPWSTR)NULL; 
			ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT; 
			ofn.lpstrTitle = szTitle; 

			if(GetSaveFileName(&ofn))
			{
				CString strName = ofn.lpstrFile;
				if( (-1 == strName.Find(L".png", strName.ReverseFind('.'))) 
					&& (-1 == strName.Find(L".PNG", strName.ReverseFind('.'))))
					strName += L".png";
				gpFileSession->CopyImageTo(strName);
			}

			return (INT_PTR)TRUE;
		}
		break;
	case WM_DESTROY:
		{
			delete myImage;
			myImage = NULL;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
