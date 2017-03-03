#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include "resource.h"

void CenterWindow(HWND hWnd)
{
	RECT rect1, rect2;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect1, 0);
	GetWindowRect(hWnd, &rect2);
	int x = ((rect1.right - rect1.left) - (rect2.right - rect2.left)) / 2 + rect1.left;
	int y = ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)) / 2 + rect1.top;
	SetWindowPos(hWnd, 0, x, y, 0, 0, SWP_NOSIZE  | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
}

BOOL GetActualDialogBaseUnits(HWND hWnd, SIZE *baseUnit)
{
	RECT rect = { 4, 8, 0, 0 };
	BOOL result = MapDialogRect(hWnd, &rect);
	if (result)
	{
		baseUnit->cx = rect.left;
		baseUnit->cy = rect.top;
	}
	return result;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, unsigned msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		CenterWindow(hWnd);
		return TRUE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			SIZE size = { 0 };
			double xScale = 1.0;
			double yScale = 1.0;
			if (GetActualDialogBaseUnits(hWnd, &size))
			{
				xScale = (size.cx / 6.0);
				yScale = (size.cy / 13.0);
			}

			// 座標0,0に高さ幅100の四角形を拡縮して描画する
			RECT rect = { 0, 0, 100 * xScale, 100 * yScale };
			FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

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
