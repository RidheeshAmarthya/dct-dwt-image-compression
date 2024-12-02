//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Creates a white RGB image with a black line and displays it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************


// Include class files
#include "Image.h"
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
//MyImage			inImage;					// image objects
MyImage				inImageDCT;					// Image for DCT transformation
MyImage				inImageDWT;					// Image for DWT transformation
int input;
int powers_of_4[] = { 0, 4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144 }; // Powers of 4 from 4^1 to 4^10
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// The rest of command line argument is truncated.
	if (lpCmdLine[0] == 0) {
		wprintf(L"No command line argument.");
		return -1;
	}
	int cnt = 0;
	while (lpCmdLine[cnt] != ' ' && lpCmdLine[cnt] != 0) {
		cnt++;
	}
	lpCmdLine[cnt] = 0;
	printf("The first parameter was: %s\n", lpCmdLine);

	LPSTR secondArg = lpCmdLine + cnt + 1;
	if (*secondArg != 0) {
		printf("The second parameter was: %s\n", secondArg);
	}

	input = atoi(secondArg);

	// Set up two images

	int w = 512;
	int h = 512;

	// Set width and height for both images
	inImageDCT.setWidth(w);
	inImageDCT.setHeight(h);

	inImageDWT.setWidth(w);
	inImageDWT.setHeight(h);

	// Load the input image path from command line
	inImageDCT.setImagePath(lpCmdLine);
	inImageDWT.setImagePath(lpCmdLine);

	// Read the input image data for both images

	if (input >= 0) {
		inImageDCT.ReadImage();
		inImageDWT.ReadImage();

		// Apply transformations for current iteration
		inImageDCT.DCT(input);
		inImageDWT.DWT(input);
	}


	// Loop from 0 to 10 and apply transformations
	//for (int i = 0; i <= 10; ++i) {
	//	printf("Iteration: %d\n", i);

	//	// Apply transformations for current iteration
	//	inImageDCT.DCT(65536);
	//	inImageDWT.DWT(8);

	//	// Force window to repaint with updated images
	//	InvalidateRect(NULL, NULL, TRUE);

	//	// Optional: Add a small delay between iterations if needed for better visualization
	//	Sleep(500);
	//}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	SetWindowPos(hWnd, HWND_TOP, 100, 100, 1040, 571, SWP_NOZORDER | SWP_NOMOVE); //changes window size
	SetTimer(hWnd, 1, 100, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

int power_index = 0; // Index to track which power of 4 we're at
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int iteration = 0;  // Keep track of which iteration we're on
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message)
	{
	case WM_TIMER:
		if (input == -1) {
			if (iteration <= 64) {

				printf("Iteration: %d\n", iteration);

				inImageDCT.ReadImage();

				// Apply DCT transformation for current iteration
				inImageDCT.DCT(4096 * iteration);

				// Apply DWT transformation only if the current iteration matches a power of 4

				if (power_index < 10 && iteration % (64 / 10) == 0 || iteration == 1) {
					inImageDWT.ReadImage();
					inImageDWT.DWT(4 * powers_of_4[power_index]);
					power_index++;
				}

				// Invalidate the window to trigger WM_PAINT
				InvalidateRect(hWnd, &rt, TRUE);

				// Force immediate repaint
				UpdateWindow(hWnd); // This ensures WM_PAINT is processed immediately

				iteration++; // Move to next iteration
			}
		}

		else if (input == -2) {
			if (iteration <= 64) {
				printf("Iteration: %d\n", iteration);

				inImageDCT.ReadImage();
				inImageDWT.ReadImage();

				// Apply transformations for current iteration
				inImageDCT.DCT(4096 * iteration);
				inImageDWT.DWT(4096 * iteration);

				// Invalidate the window to trigger WM_PAINT
				InvalidateRect(hWnd, &rt, TRUE);

				// Force immediate repaint
				UpdateWindow(hWnd);  // This ensures WM_PAINT is processed immediately

				iteration++;  // Move to next iteration
			}
		}


		else {
			KillTimer(hWnd, 1);  // Stop timer after last iteration
		}
		break;


	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = inImageDCT.getWidth();
		bmi.bmiHeader.biHeight = -inImageDCT.getHeight();  // Top-down DIB
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = inImageDCT.getWidth() * inImageDCT.getHeight();

		//BITMAPINFO bmi2;
		//memset(&bmi, 0, sizeof(bmi2));
		//bmi2.bmiHeader.biSize = sizeof(bmi2.bmiHeader);
		//bmi2.bmiHeader.biWidth = inImageDWT.getWidth();
		//bmi2.bmiHeader.biHeight = -inImageDWT.getHeight();  // Top-down DIB
		//bmi2.bmiHeader.biPlanes = 1;
		//bmi2.bmiHeader.biBitCount = 24;
		//bmi2.bmiHeader.biCompression = BI_RGB;
		//bmi2.bmiHeader.biSizeImage = inImageDWT.getWidth() * inImageDWT.getHeight();

		// Draw the first image (DCT) at (0, 0)
		SetDIBitsToDevice(hdc,
			0, 0, inImageDCT.getWidth(), inImageDCT.getHeight(),
			0, 0, 0, inImageDCT.getHeight(),
			inImageDCT.getImageData(), &bmi, DIB_RGB_COLORS);

		// Draw the second image (DWT) at (512, 0)
		SetDIBitsToDevice(hdc,
			512, 0, inImageDWT.getWidth(), inImageDWT.getHeight(),
			0, 0, 0, inImageDWT.getHeight(),
			inImageDWT.getImageData(), &bmi, DIB_RGB_COLORS);

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}


