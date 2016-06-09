/*---------------------------------------------------------------------------------
--	SOURCE FILE:	Session.cpp -
--
--	PROGRAM:		Assignment2
--
--	FUNCTIONS:
--					BOOL sessionConnect(HWND winHandle)
--					BOOL sessionDisc()
--					BOOL getCommSettings()
--
--	DATE:			Oct 6, 2015
--
--	REVISIONS:		Oct 3, 2015 - first version
--
--	DESIGNER:		Gabriella Cheung
--
--	PROGRAMMER:		Gabriella Cheung
--
--	NOTES:
--	This file contains code that displays the commConfigDialog to the user when
--	the user tries to connect. It then passes those settings to Physical.cpp.
--
---------------------------------------------------------------------------------*/

#define STRICT
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include "session.h"
#include "Physical.h"
#include "Application.h"


COMMCONFIG	cc;
HANDLE hComm;
BOOL commConnected = FALSE;

/*---------------------------------------------------------------------------------
--	FUNCTION: sessionConnect
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 3, 2015 -
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	BOOL sessionConnect(HWND winHandle)
--
--	RETURNS:	True if serial port is connected and ready to send and receive.
--				False if initialization of serial port failed for any reason.
--	NOTES:
--	Calls createCommHandle in Physical.cpp to create handle to serial port. If
--	successful, it will get comm settings from the user, use those settings to set
--	the port then signal back to Application.cpp that connection was successful.
--
---------------------------------------------------------------------------------*/
/*BOOL sessionConnect()
{
	haveDevices = getDevices();
	if (haveDevices) {
		numReaders = getReaders();
		if (numReaders == 0)
		{
			return FALSE;
		}
		return TRUE;
		//startReading();
	}
	return FALSE;
}*/

/*---------------------------------------------------------------------------------
--	FUNCTION: sessionDisc
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 3, 2015 -
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	BOOL sessionDisc()
--
--	RETURNS:	True if serial port is successfully disconnected.
--				False if disconnection of serial port failed for any reason.
--	NOTES:
--	Disconnects the serial connection if a connection is made.
--
---------------------------------------------------------------------------------*/
BOOL sessionDisc()
{
	if (commConnected)
	{
		if (physicalDisconnect())
		{
			commConnected = FALSE;
		}
	}
	return commConnected;
}

/*---------------------------------------------------------------------------------
--	FUNCTION: getCommSettings
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 3, 2015 -
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	BOOL getCommSettings
--
--	RETURNS:	True if CommConfigDialog opened successfully
--				False if CommConfigDialog failed to open.
--	NOTES:
--	Prompts user to enter settings for the serial connection.
--
---------------------------------------------------------------------------------*/
/*BOOL getCommSettings()
{	
	GetCommConfig(hComm, &cc, &cc.dwSize);
	if (!CommConfigDialog(lpszCommName, wHandle, &cc))
	{
		return FALSE;
	}
	return TRUE;
}*/




