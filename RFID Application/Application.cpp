/*---------------------------------------------------------------------------------
--	SOURCE FILE:	Application.cpp -
--
--	PROGRAM:		Assignment2
--
--	FUNCTIONS:
--					int APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE hprevInstance, PWSTR lspszCmdParam, int nCmdShow)
--					LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--					void writeToScreen(LPCWSTR c)
--					void connect()
--					void disconnect()
--					void clearScreen()
--
--	DATE:			Oct 17, 2015
--
--	REVISIONS:		Oct 17, 2015
--
--	DESIGNER:		Gabriella Cheung, Tyler Trepanier-Bracken
--
--	PROGRAMMER:		Gabriella Cheung, Tyler Trepanier-Bracken
--
--	NOTES:
--	This file starts the application. It displays the GUI to the user, and displays
--	an RFID's tag information from the RFID reader. The application starts in 
--	"Command" mode. To connect, user selects "Connect", and program will attempt to
--	connect to an RFID reader. If the connection is successful, the application goes
--	into "Connect" mode and it will display RFID tag information from the RFID reader.
--
---------------------------------------------------------------------------------*/
#define STRICT
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include "Application.h"
#include "Physical.h"

TCHAR Name[] = TEXT("RFID Tag Reader");
BOOL connected = FALSE;
int X = 0, Y = 0;
HDC hdc;
PAINTSTRUCT paintstruct;
TEXTMETRIC tm;
SIZE size;
HWND hwnd;
HMENU hMenu;
TCHAR lpszCommName[] = TEXT("com1");
BOOL haveDevices;
int numReaders;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void disconnect();
void connect();
void displayHelp();
void clearScreen();
TCHAR str[100][10000] = { TEXT("") };
int index;

#pragma warning (disable: 4096)

/*---------------------------------------------------------------------------------
--	FUNCTION:		WinMain
--
--	DATE:			October 17, 2015
--
--	DESIGNER:		Microsoft Corporation (Original Designer)
--
--	PROGRAMMER:		Tyler Trepanier-Bracken (Re-purposed for this application.)
--
--	INTERFACE:		int APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
--						PWSTR lspszCmdParam, int nCmdShow)
--
--	PARAMETERS:		HINSTANCE hInst
--						Handle to the current instance of the application.
--					HINSTANCE hprevInstance
--						Handle to the previous instance of the application.
--						For a Win32-based application, this parameter is always NULL.
--					PWSTR lspszCmdParam
--						Pointer to a null-terminated string that specifies the command 
--						line for the application, excluding the program name.
--					int nCmdShoW
--						Specifies how the window is to be shown.
--
--	RETURNS:		The exit value contained in that message's wParam
--					parameter indicates success, and that the function
--					terminates when it receives a WM_QUIT message. Zero
--					indicates that the function terminates before entering
--					the message loop.
--
--	NOTES:
--	This function is called by the system as the initial entry point for
--	Windows Embedded CE-based applications. Used to create the window
--	where the application will take place and attempt to verify if a COMM
--	port is available for use.
---------------------------------------------------------------------------------*/
int APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	PWSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX Wcl;

	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = 0;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //white background
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = TEXT("MYMENU"); // The menu Class
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl))
		return 0;

	hwnd = CreateWindowEx(
		0,                     // no extended styles 
		Name,       // global string containing name of window class
		Name,             // global string containing title bar text 
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_VISIBLE, // window styles 
		CW_USEDEFAULT,         // default horizontal position 
		CW_USEDEFAULT,         // default vertical position 
		CW_USEDEFAULT,         // default width 
		CW_USEDEFAULT,         // default height 
		(HWND)NULL,           // no parent for overlapped windows 
		(HMENU)NULL,          // use the window class menu 
		hInst,               // global instance handle  
		(PVOID)NULL           // pointer not needed 
		);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	hMenu = GetMenu(hwnd);
	EnableMenuItem(hMenu, IDM_DISC, MF_GRAYED);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

/*---------------------------------------------------------------------------------
--	FUNCTION:		Window Procedure
--
--	DATE:			October 17, 2015
--
--	DESIGNER:		Aman Abdulla (Original Designer)
--
--	PROGRAMMER:		Tyler Trepanier-Bracken
--
--	INTERFACE:		LRESULT CALLBACK WndProc(
--						HWND hwnd,
--						UINT Message,
--						WPARAM wParam,
--						LPARAM lParam)
--
--	PARAMETERS:		HWND hwnd
--						Window handle to designated for this specific application
--					UINT Message
--						Message received from Windows.
--					WPARAM wParam
--						Input received from keyboard.
--					LPARAM lParam
--						Input received from mouse.
--
--	RETURNS:		LRESULT CALLBACK.
--
--	NOTES:
--	Handles the continous stream of messages and input sent from Windows and
--	the user.
--
--	WM_COMMAND
--		IDM_CONN:	Connect to the SkyeTek RFID reader.
--		IDM_DISC:	Disconnect from the SkyeTek RFID reader.
--		IDM_CLRSCN:	Clears the screen.
--		IDM_HELP:	Display usage instructions on this window.
--		IDM_EXIT:	Destroy the window and close all available handles.
--
--	WM_CREATE
--		Recreates window when it is resized.
--
--	WM_SIZE
--		
--
--	WM_HSCROLL
--		Scroll the window horizontally.
--
--	WM_VSCROLL
--		Scroll the window vertically.
--
--	WM_PAINT
--		Acquires window context and paints them.
--
--	WM_DESTROY
--		Closes the connection if the RFID reader is still connected, elminiates all
--		application processes and closes the window.
--
---------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO si;
	TEXTMETRIC tm;
	static int xClient;     // width of client area 
	static int yClient;     // height of client area 
	static int xClientMax;  // maximum width of client area 

	static int xChar;       // horizontal scrolling unit 
	static int yChar;       // vertical scrolling unit 
	static int xUpper;      // average width of uppercase letters 

	static int xPos;        // current horizontal scrolling position 
	static int yPos;        // current vertical scrolling position 

	int i;                  // loop counter 

	int FirstLine;          // first line in the invalidated area 
	int LastLine;           // last line in the invalidated area 

	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_CONN:
			connect();
			break;
		case IDM_DISC:
			disconnect();
			break;
		case IDM_CLRSCN:
			clearScreen();
			break;
		case IDM_HELP:
			displayHelp();
			break;
		case IDM_EXIT:
			//Terminate program
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_CREATE:
		// Get the handle to the client area's device context.
		hdc = GetDC(hwnd);

		// Extract font dimensions from the text metrics. 
		GetTextMetrics(hdc, &tm);
		xChar = tm.tmAveCharWidth;
		xUpper = (tm.tmPitchAndFamily & 1 ? 3 : 2) * xChar / 2;
		yChar = tm.tmHeight + tm.tmExternalLeading;

		// Free the device context. 
		ReleaseDC(hwnd, hdc);

		// Set an arbitrary maximum width for client area. 
		// (xClientMax is the sum of the widths of 48 average 
		// lowercase letters and 12 uppercase letters.) 
		xClientMax = 48 * xChar + 12 * xUpper;

		return 0;

	case WM_SIZE:

		// Retrieve the dimensions of the client area. 
		yClient = HIWORD(lParam);
		xClient = LOWORD(lParam);

		// Set the vertical scrolling range and page size
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = index - 1;
		si.nPage = yClient / yChar;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		// Set the horizontal scrolling range and page size. 
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = 2 + xClientMax / xChar;
		si.nPage = xClient / xChar;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

		return 0;
		
	case WM_HSCROLL:
		// Get all the vertial scroll bar information.
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;

		// Save the position for comparison later on.
		GetScrollInfo(hwnd, SB_HORZ, &si);
		xPos = si.nPos;
		switch (LOWORD(wParam))
		{
			// User clicked the left arrow.
		case SB_LINELEFT:
			si.nPos -= 1;
			break;

			// User clicked the right arrow.
		case SB_LINERIGHT:
			si.nPos += 1;
			break;

			// User clicked the scroll bar shaft left of the scroll box.
		case SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;

			// User clicked the scroll bar shaft right of the scroll box.
		case SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;

			// User dragged the scroll box.
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hwnd, SB_HORZ, &si);

		// If the position has changed, scroll the window.
		if (si.nPos != xPos)
		{
			ScrollWindow(hwnd, xChar * (xPos - si.nPos), 0, NULL, NULL);
		}

		return 0;

	case WM_VSCROLL:
		// Get all the vertial scroll bar information.
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &si);

		// Save the position for comparison later on.
		yPos = si.nPos;
		switch (LOWORD(wParam))
		{

			// User clicked the HOME keyboard key.
		case SB_TOP:
			si.nPos = si.nMin;
			break;

			// User clicked the END keyboard key.
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

			// User clicked the top arrow.
		case SB_LINEUP:
			si.nPos -= 1;
			break;

			// User clicked the bottom arrow.
		case SB_LINEDOWN:
			si.nPos += 1;
			break;

			// User clicked the scroll bar shaft above the scroll box.
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

			// User clicked the scroll bar shaft below the scroll box.
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;

			// User dragged the scroll box.
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hwnd, SB_VERT, &si);

		// If the position has changed, scroll window and update it.
		if (si.nPos != yPos)
		{
			ScrollWindow(hwnd, 0, yChar * (yPos - si.nPos), NULL, NULL);
			UpdateWindow(hwnd);
		}

		return 0;
	case WM_PAINT:
		// Prepare the window for painting.
		hdc = BeginPaint(hwnd, &paintstruct);

		// Get vertical scroll bar position.
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(hwnd, SB_VERT, &si);
		yPos = si.nPos;

		// Get horizontal scroll bar position.
		GetScrollInfo(hwnd, SB_HORZ, &si);
		xPos = si.nPos;

		// Find painting limits.
		FirstLine = max(0, yPos + paintstruct.rcPaint.top / yChar);
		LastLine = min(index - 1, yPos + paintstruct.rcPaint.bottom / yChar);

		GetTextMetrics(hdc, &tm);
		for (i = FirstLine; i <= LastLine; i++)
		{
			if (str[i] == NULL)
				break;

			X = xChar * (1 - xPos);
			Y = yChar * (i - yPos);

			TCHAR * token;
			token = _tcstok(str[i], TEXT("\n"));
			if (token != NULL)
				TextOut(hdc, X, Y, token, wcslen(token));
			Y = Y + tm.tmHeight + tm.tmExternalLeading;
			
		}

		// Indicate that painting is finished.
		EndPaint(hwnd, &paintstruct);
		break;

	case WM_DESTROY:	// Terminate program
		if (connected) // if reader is still connected, disconnect first
		{
			disconnect();
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/*---------------------------------------------------------------------------------
--	FUNCTION: writeToScreen
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 17, 2015 - first version
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	void writeToScreen(LPCWSTR data)
--
--	RETURNS:	void
--
--	NOTES:
--	Writes characters to screen. It displays the characters on a new line, and it
--	also adds the characters to a 2D char array so all the characterss received
--	can be displayed if window is resized.
--
---------------------------------------------------------------------------------*/
void writeToScreen(LPCWSTR data)
{
	hdc = GetDC(hwnd);
	GetTextMetrics(hdc, &tm);
	TextOut(hdc, X, Y, data, wcslen(data));
	wcscat(str[index], data);
	wcscat(str[index++], TEXT("\n"));
	Y = Y + tm.tmHeight + tm.tmExternalLeading;
	ReleaseDC(hwnd, hdc); // Release device context
}

/*---------------------------------------------------------------------------------
--	FUNCTION: connect
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 17, 2015 - first version
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	void connect()
--
--	RETURNS:	void
--
--	NOTES:
--	Calls methods in Physical.cpp to detect and connect to RFID reader.
--	Displays success message to user if handle was created successfully, error
--	message if not.
--
---------------------------------------------------------------------------------*/
void connect()
{
	writeToScreen(TEXT("Attempt to connect to reader..."));
	haveDevices = getDevices();
	if (haveDevices) {
		numReaders = getReaders();
		if (numReaders == 0)
		{
			//display no readers connected message
			writeToScreen(TEXT("No reader found! Please connect reader and try again."));
		}
		else {
			//print to screen
			writeToScreen(TEXT("Connected to reader"));
			connected = physicalConnect();
			EnableMenuItem(hMenu, IDM_CONN, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_DISC, MF_ENABLED);
		}
	}
	else {
		//display no devices connected message
		writeToScreen(TEXT("No reader found! Please connect reader and try again."));
	}
}

/*---------------------------------------------------------------------------------
--	FUNCTION: disconnect
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 17, 2015 - first version
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	void disconnect()
--
--	RETURNS:	void
--
--	NOTES:
--	Calls physicalDisconnect method in Physical.cpp to signal disconnection from the
--	RFID reader. Displays success message to user if disconnection was successful,
--	error message if not.
--
---------------------------------------------------------------------------------*/
void disconnect()
{
	connected = physicalDisconnect();
	if (!connected)
	{
		writeToScreen(TEXT("Disconnected from reader successfully"));
		EnableMenuItem(hMenu, IDM_DISC, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_CONN, MF_ENABLED);
	}
	else {
		writeToScreen(TEXT("Error disconnecting from reader"));
	}
}

/*---------------------------------------------------------------------------------
--	FUNCTION: displayhelp
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 17, 2015 - first version
--
--	DESIGNER:	Tyler Trepanier-Bracken
--
--	PROGRAMMER:	Tyler Trepanier-Bracken
--
--	INTERFACE:	void displayHelp()
--
--	RETURNS:	void
--
--	NOTES:
--	Displays instructions on how to use the program on the screen.
--
---------------------------------------------------------------------------------*/
void displayHelp()
{
	writeToScreen(TEXT("Instructions on how to use this program:"));
	writeToScreen(TEXT("========================================"));
	writeToScreen(TEXT("Ensure that your reader is connected"));
	writeToScreen(TEXT("To connect, select \"Connect\" in the main menu"));
	writeToScreen(TEXT("To disconnect, select \"disconnect\" in the main menu"));
}

/*
=========================================================================
FUNCTION:		clearScreen

DATE:			September 26, 2015
REVISION:		October 16, 2015
Modified to calculate the window's size instead of
using hard-coded values.

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

INTERFACE:		int clearScreen(
HWND hwnd)

PARAMETERS:		HWND	hwnd
Window designed to be emptied of painted objects.

RETURNS:		void

NOTES:
This function takes in a windows object and clears the screen using many
space characters for the entire size of the window.

Requires two definitions: MAX_HEIGHT and MAX_WIDTH, these are the
window's size.
=========================================================================
*/
void clearScreen()
{
	int			width;						// width of the current window
	int			height;						// height of the current window
	LPCWSTR		space = L" ";				// Space character
	HDC			hdc = GetDC(hwnd);			// Used for displaying text on the screen
	RECT		rect;						// Contains the windows dimensions.

	if (GetWindowRect(hwnd, &rect))
	{
		width = rect.right - rect.left+10;
		height = rect.bottom - rect.top+10;
	}

	for (int y = 0; y <= height; y += 5) {
		for (int x = 0; x <= width; x += 3) {
			TextOut(hdc, x, y, space, wcslen(space));
		}
	}
	ReleaseDC(hwnd, hdc);
	for (int i = index; i > -1; i--) {
		wcscpy(str[i], TEXT("\0"));
	}
	ReleaseDC(hwnd, hdc);
	index = 0;
	X = Y = 0;
}
