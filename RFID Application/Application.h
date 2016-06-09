#define IDM_COM1			100
#define IDM_COM2			101
#define IDM_COM3			102
#define IDM_COM4			103
#define IDM_COM5			104
#define IDM_COM6			105
#define IDM_DISC			106
#define IDM_EXIT			107
#define IDM_HELP			108
#define IDM_CONN			109
#define IDM_CLRSCN			110

TCHAR lpszCommName[];
void writeToScreen(LPCWSTR c);
void disconnect();