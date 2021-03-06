#include "windows.h"
#include "stdafx.h"
#include "math.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

struct Shape
{
	int x1, y1, x2, y2;
	float angle;
	int force;
	int Xi, Yi;
	bool flag;
	int startTime;
} myShape;

bool isDrawing = true, isMoving = false;

int X, Y;
int tmNow;
float tmDelta;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) 
{
	TCHAR szClassName[] = L"Lab_1"; 
	HWND hMainWnd; 
	MSG msg; 
	WNDCLASSEX wc;

	wc.cbSize = sizeof(wc); 
	wc.style = NULL; 
	wc.lpfnWndProc = WndProc; 
	wc.lpszMenuName = NULL; 
	wc.lpszClassName = szClassName;
	wc.cbWndExtra = NULL; 
	wc.cbClsExtra = NULL; 
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); 
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	wc.hInstance = hInst; 
	if (!RegisterClassEx(&wc)) 
	{
		MessageBox(NULL, L"Error class registration", L"", MB_OK);
		return NULL; 
	}
	hMainWnd = CreateWindow
	(
		szClassName, 
		L"Lab_1", 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT,
		NULL, 
		CW_USEDEFAULT, 
		NULL, 
		(HWND)NULL, 
		NULL, 
		HINSTANCE(hInst), 
		NULL
	); 
	if (!hMainWnd) 
	{
		MessageBox(NULL, L"Error", L"", MB_OK);
		return NULL;
	}
	ShowWindow(hMainWnd, nCmdShow); 
	UpdateWindow(hMainWnd);
	while (GetMessage(&msg, NULL, NULL, NULL)) 
	{ 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	HBRUSH brush;

	HDC hdcMem;
	HBITMAP hbmMem;
	HANDLE hOld;

	switch (uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(NULL);
			break;
		//Таймер
		case WM_CREATE:
			SetTimer(hWnd, 1, 1, NULL);
			break;
		case WM_TIMER:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_KEYDOWN:
			//Управление стрелочками
			switch (wParam) 
			{
				case VK_UP:
					myShape.y1 -= 5;
					myShape.y2 = myShape.y1 + 100;
					InvalidateRect(hWnd, NULL, FALSE);
					break;
				case VK_DOWN:
					myShape.y1 += 5;
					myShape.y2 = myShape.y1 + 100;
					InvalidateRect(hWnd, NULL, FALSE);
					break;
				case VK_LEFT:
					myShape.x1 -= 5;
					myShape.x2 = myShape.x1 + 100;
					InvalidateRect(hWnd, NULL, FALSE);
					break;
				case VK_RIGHT:
					myShape.x1 += 5;
					myShape.x2 = myShape.x1 + 100;
					InvalidateRect(hWnd, NULL, FALSE);
					break;
			}
			break;
		case WM_RBUTTONDOWN:
			isDrawing = true;
			isMoving = false;
			break;
		case WM_LBUTTONDOWN:
			isDrawing = false;
			isMoving = true;
			GetClientRect(hWnd, &rect);
			//Создание шарика
			X = LOWORD(lParam);
			Y = HIWORD(lParam);
			myShape.startTime = GetTickCount();
			myShape.angle = 3.14 - atan2(500 - Y, 500 - X);
			myShape.force = 8;
			myShape.Xi = 500;
			myShape.Yi = rect.bottom - 500;
			myShape.flag = false;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_PAINT:
			if (isDrawing)
			{
				GetClientRect(hWnd, &rect);
				hDC = BeginPaint(hWnd, &ps);
				hdcMem = CreateCompatibleDC(hDC);
				hbmMem = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
				hOld = SelectObject(hdcMem, hbmMem);

				brush = CreateSolidBrush(RGB(0, 255, 0));
				SelectObject(hdcMem, brush);
				Rectangle(hdcMem, myShape.x1, myShape.y1, myShape.x2, myShape.y2);

				BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
				SelectObject(hdcMem, hOld);
				DeleteObject(hbmMem);
				DeleteDC(hdcMem);
				EndPaint(hWnd, &ps);
				break;
			}	
			if (isMoving)
			{
				//Отскоки шарика
				GetClientRect(hWnd, &rect);
				tmNow = GetTickCount();
				tmDelta = (tmNow - myShape.startTime) / 10;
				myShape.x1 = myShape.force * cos(myShape.angle) * tmDelta + myShape.Xi;
				myShape.y1 = (rect.bottom - myShape.force * sin(myShape.angle) * tmDelta) - myShape.Yi;
				if (((myShape.y1 >= rect.bottom - 50) || (myShape.y1 <= 0)) && (myShape.flag == false))
				{
					myShape.angle = 2 * 3.14 - myShape.angle;
					myShape.Xi = myShape.x1;
					myShape.Yi = rect.bottom - myShape.y1;
					myShape.startTime = GetTickCount();
					myShape.flag = true;
				}
				if (((myShape.x1 <= 0) || (myShape.x1 >= rect.right - 50)) && (myShape.flag == false))
				{
					myShape.angle = 3.14 - myShape.angle;
					myShape.Xi = myShape.x1;
					myShape.Yi = rect.bottom - myShape.y1;
					myShape.startTime = GetTickCount();
					myShape.flag = true;
				}
				if ((myShape.x1 > 0) && (myShape.x1 < rect.right - 50) && (myShape.y1 > 0) && (myShape.y1 < rect.bottom - 50))
				{
					myShape.flag = false;
				}
				myShape.x2 = myShape.x1 + 50;
				myShape.y2 = myShape.y1 + 50;
				hDC = BeginPaint(hWnd, &ps);
				//Двойная буферизация
				hdcMem = CreateCompatibleDC(hDC);
				hbmMem = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
				hOld = SelectObject(hdcMem, hbmMem);

				brush = CreateSolidBrush(RGB(0, 255, 0));
				SelectObject(hdcMem, brush);
				Ellipse(hdcMem, myShape.x1, myShape.y1, myShape.x2, myShape.y2);

				BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
				SelectObject(hdcMem, hOld);
				DeleteObject(hbmMem);
				DeleteDC(hdcMem);
				EndPaint(hWnd, &ps);
				break;
			}
		case WM_MOUSEMOVE:
			if (isDrawing)
			{
				myShape.x1 = LOWORD(lParam);
				myShape.y1 = HIWORD(lParam);
				myShape.x2 = myShape.x1 + 100;
				myShape.y2 = myShape.y1 + 100;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case WM_MOUSEWHEEL:
			//Движение мышью
				if (GET_WHEEL_DELTA_WPARAM(wParam) < 0 && GetAsyncKeyState(VK_LSHIFT) < 0)
				{
					myShape.x1 -= 15;
					myShape.x2 = myShape.x1 + 100;
				}
				else
				{
					if (GET_WHEEL_DELTA_WPARAM(wParam) > 0 && GetAsyncKeyState(VK_LSHIFT) < 0)
					{
						myShape.x1 += 15;
						myShape.x2 = myShape.x1 + 100;
					}
					else
					{
						if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
						{
							myShape.y1 -= 15;
							myShape.y2 = myShape.y1 + 100;
						}
						else
						{
							myShape.y1 += 15;
							myShape.y2 = myShape.y1 + 100;
						}
					}
				}
				InvalidateRect(hWnd, NULL, FALSE);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return NULL;
}