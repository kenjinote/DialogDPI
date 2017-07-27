﻿#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <math.h>
#include "resource.h"

#pragma pack(push, 1)
struct DLGTEMPLATEEX_PART1 {
	WORD      dlgVer;
	WORD      signature;
	DWORD     helpID;
	DWORD     exStyle;
	DWORD     style;
	WORD      cDlgItems;
	short     x;
	short     y;
	short     cx;
	short     cy;
};
#pragma pack(pop)

BYTE* AdvanceThrough_sz_Or_Ord(BYTE* pData)
{
	WORD* pWArr = (WORD*)pData;
	if (*pWArr == 0x0000)
	{
		pWArr++;
	}
	else if (*pWArr == 0xFFFF)
	{
		pWArr++;
		pWArr++;
	}
	else
	{
		WCHAR z;
		do
		{
			z = *pWArr;
			pWArr++;
		} while (z != 0);
	}
	return (BYTE*)pWArr;
}

BYTE* AdvanceThrough_String(BYTE* pData, LPWSTR pOutStr)
{
	WCHAR* pWStr = (WCHAR*)pData;
	WCHAR z;
	do
	{
		z = *pWStr;
		pWStr++;
	} while (z != 0);
	if (pOutStr)
	{
		int nLn = (int)(pWStr - (WCHAR*)pData);
		CopyMemory(pOutStr, pData, nLn * sizeof(WCHAR));
	}
	return (BYTE*)pWStr;
}

HFONT GetFontFromDialogTemplate(LPCTSTR lpszResourceID)
{
	HFONT hFont = NULL;
	HRSRC hResource = FindResource(0, lpszResourceID, RT_DIALOG);
	if (hResource)
	{
		HGLOBAL hDialogTemplate = LoadResource(GetModuleHandle(0), hResource);
		if (hDialogTemplate)
		{
			LPCDLGTEMPLATE lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);
			DWORD dwszDialogTemplate = SizeofResource(GetModuleHandle(0), hResource);
			if (lpDialogTemplate && dwszDialogTemplate)
			{
				LPCDLGTEMPLATE lpDialogTemplateToUse = lpDialogTemplate;
				DLGTEMPLATEEX_PART1* pDTX1 = (DLGTEMPLATEEX_PART1*)lpDialogTemplate;
				if (pDTX1->signature == 0xFFFF && pDTX1->dlgVer == 1)
				{
					//Now get thru variable length elements
					BYTE* pData = (BYTE*)(pDTX1 + 1);

					//sz_Or_Ord menu;
					pData = AdvanceThrough_sz_Or_Ord(pData);

					//sz_Or_Ord windowClass;
					pData = AdvanceThrough_sz_Or_Ord(pData);

					//title
					WCHAR strTitle[1024];
					pData = AdvanceThrough_String(pData, strTitle);

					//Now pointsize of the font
					//This member is present only if the style member specifies DS_SETFONT or DS_SHELLFONT.
					if (pDTX1->style & (DS_SETFONT | DS_SHELLFONT))
					{
						//Font size in pts
						BYTE* pPtr_FontSize = pData;
						WORD ptFontSize = *(WORD*)pData;
						pData += sizeof(WORD);

						//WORD wFontWeight = *(WORD*)pData;
						pData += sizeof(WORD);

						//BYTE italic = *(BYTE*)pData;
						pData += sizeof(BYTE);

						//BYTE charset = *(BYTE*)pData;
						pData += sizeof(BYTE);

						//Font face name
						WCHAR strFontFaceName[LF_FACESIZE];
						//BYTE* pPtr_FontFaceName = pData;
						pData = AdvanceThrough_String(pData, strFontFaceName);

						if (lstrcmpW(strFontFaceName, L"MS Shell Dlg") == 0)
						{
							lstrcpy(strFontFaceName, TEXT("MS Shell Dlg 2"));
						}

						hFont = CreateFontW(-MulDiv(ptFontSize, 96, 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0, 0, strFontFaceName);
					}
				}
			}
		}
	}
	return hFont;
}

BOOL GetActualDialogBaseUnits(HWND hWnd, double *width, double *height)
{
	RECT rect = { 4, 8, 0, 0 };
	BOOL result = MapDialogRect(hWnd, &rect);
	if (result)
	{
		*width = rect.left / 4.0;
		*height = rect.top / 8.0;
	}
	return result;
}

BOOL GetActualDialogBaseUnits2(HWND hWnd, double *width, double *height)
{
	HDC hdc = GetDC(hWnd);
	HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	TEXTMETRIC tm = { 0 };
	GetTextMetrics(hdc, &tm);
	*height = tm.tmHeight / 8.0;
	SIZE size = { 0 };
	GetTextExtentPoint32(hdc, TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"), 52, &size);
	SelectObject(hdc, hOldFont);
	*width = ((int)(size.cx / 26 + 1) / 2) / 4.0;
	ReleaseDC(hWnd, hdc);
	return TRUE;
}

BOOL GetActualDialogBaseUnits3(HWND hWnd, double *width, double *height, LPCTSTR lpszResourceID)
{
	HDC hdc = GetDC(hWnd);
	HFONT hFont = (HFONT)GetFontFromDialogTemplate(lpszResourceID);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	TEXTMETRIC tm = { 0 };
	GetTextMetrics(hdc, &tm);
	*height = tm.tmHeight / 8.0;
	SIZE size = { 0 };
	GetTextExtentPoint32(hdc, TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"), 52, &size);
	SelectObject(hdc, hOldFont);
	*width = ((int)(size.cx / 26 + 1) / 2) / 4.0;
	ReleaseDC(hWnd, hdc);
	DeleteObject(hFont);
	return TRUE;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, unsigned msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			int nTop = 0;

			RECT rect;
			GetClientRect(GetDlgItem(hWnd, IDC_STATIC_RECT), &rect);
			TCHAR szText[1024];
			wsprintf(szText, TEXT("コントロールの大きさ: %d x %d"), rect.right - rect.left, rect.bottom - rect.top);
			TextOut(hdc, rect.right - rect.left + 10, rect.top, szText, lstrlen(szText));
			nTop += 110;

			double w, h;
			double xScale = 1.0;
			double yScale = 1.0;

			if (GetActualDialogBaseUnits(hWnd, &w, &h))
			{
				xScale = (w / (6.0 / 4.0));
				yScale = (h / (13.0 / 8.0));
			}

			//①
			RECT rect0 = { 0, 0, 66, 61 };
			MapDialogRect(hWnd, &rect0);
			OffsetRect(&rect0, 0, (int)(nTop * yScale));
			FillRect(hdc, &rect0, (HBRUSH)GetStockObject(BLACK_BRUSH));
			wsprintf(szText, TEXT("①描画短形の大きさ: %d x %d"), rect0.right - rect0.left, rect0.bottom - rect0.top);
			TextOut(hdc, rect0.right - rect0.left + 10, rect0.top, szText, lstrlen(szText));
			nTop += 110;

			// ②
			RECT rect1 = { 0, (LONG)round(nTop * yScale), (LONG)round(99 * xScale), (LONG)round(nTop * yScale) + (LONG)round(99.125 * yScale) };
			FillRect(hdc, &rect1, (HBRUSH)GetStockObject(BLACK_BRUSH));
			wsprintf(szText, TEXT("②描画短形の大きさ: %d x %d"), rect1.right - rect1.left, rect1.bottom - rect1.top);
			TextOut(hdc, rect1.right - rect1.left + 10, rect1.top, szText, lstrlen(szText));
			nTop += 110;

			// ③
			if (GetActualDialogBaseUnits2(hWnd, &w, &h))
			{
				xScale = (w / (6.0 / 4.0));
				yScale = (h / (13.0 / 8.0));
			}
			RECT rect2 = { 0, (LONG)round(nTop * yScale), (LONG)round(99 * xScale), (LONG)round(nTop * yScale) + (LONG)round(99.125 * yScale) };
			FillRect(hdc, &rect2, (HBRUSH)GetStockObject(BLACK_BRUSH));
			wsprintf(szText, TEXT("③描画短形の大きさ: %d x %d"), rect2.right - rect2.left, rect2.bottom - rect2.top);
			TextOut(hdc, rect2.right - rect2.left + 10, rect2.top, szText, lstrlen(szText));
			nTop += 110;

			// ④
			double yOldScale = yScale;
			if (GetActualDialogBaseUnits3(hWnd, &w, &h, MAKEINTRESOURCE(IDD_DIALOG1)))
			{
				xScale = (w / (6.0 / 4.0));
				yScale = (h / (13.0 / 8.0));
			}
			RECT rect3 = { 0, (LONG)round(nTop * yOldScale), (LONG)round(99 * xScale), (LONG)round(nTop * yOldScale) + (LONG)round(99.125 * yScale) };
			FillRect(hdc, &rect3, (HBRUSH)GetStockObject(BLACK_BRUSH));
			wsprintf(szText, TEXT("④描画短形の大きさ: %d x %d"), rect3.right - rect3.left, rect3.bottom - rect3.top);
			TextOut(hdc, rect3.right - rect3.left + 10, rect3.top, szText, lstrlen(szText));
			nTop += 110;

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc);
	return 0;
}
