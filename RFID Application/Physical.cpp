/*---------------------------------------------------------------------------------
--	SOURCE FILE:	Physical.cpp -
--
--	PROGRAM:		Assignment2
--
--	FUNCTIONS:
--					BOOL getDevices()
--					int getReaders()
--					BOOL physicalConnect()
--					BOOL physicalDisconnect()
--					DWORD WINAPI startReading(LPVOID n)
--
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
--	This file contains code that searches for and connects to SkyeTek Readers.
--	Once connected, it sets the reader in scanning mode to scan for tags.
--	If one or more tags are discovered, the tag names are sent to Application.cpp
--	to be printed onto the screen.
--
---------------------------------------------------------------------------------*/
#define STRICT
#define _CRT_SECURE_NO_WARNINGS

#include "Physical.h"
#include "Application.h"

HANDLE hReceiveThread;
BOOL keepReading = TRUE;
OVERLAPPED ol;
SKYETEK_DEVICE_FILE readHandle;

unsigned short count;
unsigned int numDevices;
unsigned int numReaders;
LPSKYETEK_DEVICE *devices = NULL;
LPSKYETEK_READER *readers = NULL;
LPSKYETEK_TAG *lpTags = NULL;
LPSKYETEK_DATA lpData = NULL;
SKYETEK_STATUS st;

/*---------------------------------------------------------------------------------
--	FUNCTION: getDevices
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 17, 2015 - first version
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	BOOL getDevices()
--
--	RETURNS:	True if there are devices connected, false if no devices were found.
--				
--	NOTES:
--	Calls SkyeTek's DiscoverDevices function to get the devices currently connected.
--
---------------------------------------------------------------------------------*/
BOOL getDevices()
{
	numDevices = SkyeTek_DiscoverDevices(&devices);
	if (numDevices == 0)
	{
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------------
--	FUNCTION: getReaders
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 19, 2015 - first version
--
--	DESIGNER:	Tyler Trepanier-Bracken
--
--	PROGRAMMER:	Tyler Trepanier-Bracken
--
--	INTERFACE:	int getReaders()
--
--	RETURNS:	The number of readers detected. Zero if no readers were found.
--
--	NOTES:
--	Calls SkyeTek's DiscoverReaders function to get the readers currently connected.
--
---------------------------------------------------------------------------------*/
int getReaders()
{
	numReaders = SkyeTek_DiscoverReaders(devices, numDevices, &readers);
	if (numReaders == 0)
	{
		SkyeTek_FreeDevices(devices, numDevices);
		return 0;
	}
	if ((readHandle = readers[0]->lpDevice->readFD) == INVALID_HANDLE_VALUE)
	{
			return 0;
	}
	return numReaders;
}

/*---------------------------------------------------------------------------------
--	FUNCTION: startReading
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 17, 2015 - first version
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung, Tyler Trepanier-Bracken
--
--	INTERFACE:	DWORD WINAPI startReading(LPVOID n)
--
--	RETURNS:	0 when the thread ends and function stops running
--
--	NOTES:
--	Calls SkyeTek's GetTags function to retrieve information from tags in proximity
--	of the reader. Once tags are found, it gets the tag's code and writes the code
--	to the screen using Application's writeToScreen method.
---------------------------------------------------------------------------------*/
DWORD WINAPI startReading(LPVOID n)
{
	while (keepReading) {
		lpTags = NULL;
		count = 0;

		DWORD dwEventMask;
		if (GetOverlappedResult(readHandle, &ol, &dwEventMask, FALSE))
		{
			st = SkyeTek_GetTags(readers[0], AUTO_DETECT, &lpTags, &count);
			if (st == SKYETEK_TIMEOUT)
			{
				writeToScreen(TEXT("Reader timed out, please make sure reader is connected and select \"Connect\" again."));
				disconnect();
				continue;
			}
			else if (st != SKYETEK_SUCCESS)
			{
				continue;
			}

			// Loop through tags and read each if it has data
			for (unsigned short ix = 0; ix < count; ix++)
			{
				TCHAR temp[100] = (TEXT("Tag found: "));
				wcscat(temp, (LPCWSTR)lpTags[ix]->friendly);
				writeToScreen(temp);
			}

			// Free tags
			SkyeTek_FreeTags(readers[0], lpTags, count);
		}	
	}
	DWORD exitCode = 0;
	ExitThread(exitCode);
	return 0;
}

/*---------------------------------------------------------------------------------
--	FUNCTION: physicalConnect
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 17, 2015 -
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	BOOL physicalConnect()
--
--	RETURNS:	True if thread is created successfully and reader is ready.
--				False if unsuccessful.
--
--	NOTES:
--	Creates a thread for reading tag information from reader
--
---------------------------------------------------------------------------------*/
BOOL physicalConnect()
{
	memset(&ol, 0, sizeof(ol));
	ol.hEvent = CreateEvent(0, TRUE, FALSE, 0);

	DWORD rThreadId;
	hReceiveThread = CreateThread(NULL, 0, startReading, (LPVOID)0, 0, &rThreadId);
	keepReading = TRUE;
	if (!hReceiveThread)
	{
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------------
--	FUNCTION: physicalDisconnect
--
--	DATE:		Oct 3, 2015
--
--	REVISIONS:	Oct 17, 2015 -
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	BOOL physicalDisconnect()
--
--	RETURNS:	True if connection to reader was closed successfully.
--				False if unsuccessful.
--	NOTES:
--	Signal the end of the read thread and close the connection to the reader.
--
---------------------------------------------------------------------------------*/
BOOL physicalDisconnect()
{
	keepReading = FALSE;
	BOOL connected = TRUE;
	if (CloseHandle(hReceiveThread))
	{
		SkyeTek_FreeDevices(devices, numDevices);
		connected = FALSE;
	}
	return connected;
}