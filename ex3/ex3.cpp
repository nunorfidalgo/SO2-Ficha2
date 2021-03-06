// ex3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*
SetThreatPriority(); -> HANDLE
colocar uma threat a low e as outras a high
*/


// baseado no codigo do ex8.
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

DWORD WINAPI Contar(LPVOID param);

void gotoxy(int x, int y);

typedef struct {
	int num, inicio, fim;
} PARAM;

DWORD *threatId;
HANDLE *hT;
PARAM *p;

HANDLE ghMutex;

SYSTEMTIME st, lt, lt1;
FILETIME ft;

int _tmain(int argc, LPTSTR argv[]) {
	int i, n, inicio, fim;

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	if (argc != 4) {
		_tprintf(TEXT("\n[ERRO] Parametros invalidos\n"));
		return 1;
	}

	n = _ttoi(argv[1]);
	inicio = _ttoi(argv[2]);
	fim = _ttoi(argv[3]);

	threatId = (DWORD *)malloc(n * sizeof(DWORD));
	hT = (HANDLE *)malloc(n * sizeof(HANDLE));
	p = (PARAM *)malloc(n * sizeof(PARAM));


	// mutex
	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex
	if (ghMutex == NULL)
	{
		_tprintf(TEXT("CreateMutex error: %d\n"), GetLastError());
		return 1;
	}

	// time
	GetSystemTime(&st);
	GetLocalTime(&lt);

	_tprintf(TEXT("The system time is: %02d:%02d\n"), st.wHour, st.wMinute);
	_tprintf(TEXT("The local  time is: %02d:%02d\n"), lt.wHour, lt.wMinute);

	//_tprintf(TEXT("The local time in milliseconds: %02dms\n"), lt.wMilliseconds);

	//GetSystemTime(&st);              // Gets the current system time
	//SystemTimeToFileTime(&st, &ft);  // Converts the current system time to file time format

	SYSTEMTIME stH;
	FILETIME ftH;
	ULARGE_INTEGER ulH1, ulH2;

	GetSystemTime(&stH);	// SYSTEMTIME: horas, min, seg,...
	SystemTimeToFileTime(&stH, &ftH); // FILETIME: 32bits + 32bits: dwhighDateTime:dwLowDateTime

	ulH1.HighPart = ftH.dwHighDateTime;  // ULARGE_INTEGER: 64 bits
	ulH1.LowPart = ftH.dwLowDateTime;

	_tprintf(TEXT("%llu"), ulH1.QuadPart);


	for (i = 0; i < n; i++) {

		p[i].num = i;
		p[i].inicio = inicio + (i) * (fim - inicio) / n;
		p[i].fim = inicio + (i + 1) * (fim - inicio) / n;

		hT[i] = CreateThread(NULL, 0, Contar, (LPVOID)&p[i], 0, &threatId[i]);
		if (hT[i] != NULL) {
			_tprintf(TEXT("\nLancei uma thread com id %d"), threatId[i]);

			// Thread Prioritys
			if( i == 0 )
				SetThreadPriority(hT[i], THREAD_PRIORITY_LOWEST);
			else
				SetThreadPriority(hT[i], THREAD_PRIORITY_HIGHEST);
		}
	}

	WaitForMultipleObjects(n, hT, TRUE, INFINITE);
	CloseHandle(ghMutex);

	free(threatId);
	free(hT);

	GetLocalTime(&lt);
	//_tprintf(TEXT("The local time in milliseconds: %02dms\n"), lt.wMilliseconds);

	GetSystemTime(&stH);	// SYSTEMTIME: horas, min, seg,...
	SystemTimeToFileTime(&stH, &ftH); // FILETIME: 32bits + 32bits: dwhighDateTime:dwLowDateTime

	ulH2.HighPart = ftH.dwHighDateTime;  // ULARGE_INTEGER: 64 bits
	ulH2.LowPart = ftH.dwLowDateTime;

	_tprintf(TEXT("%llu"), ulH2.QuadPart);

	ulH2.QuadPart -= ulH1.QuadPart;
	_tprintf(TEXT("DIIF: %llu"), ulH2.QuadPart);

	ftH.dwHighDateTime = ulH2.HighPart;
	ftH.dwLowDateTime = ulH2.LowPart;
	FileTimeToSystemTime(&ftH, &stH);
	_tprintf(TEXT("Tempo execucao %02d:%02d:%02d:%02d\n"), stH.wHour, stH.wMinute, stH.wMinute, stH.wMilliseconds);

	_tprintf(TEXT("\nVou terminar..."));

	//_tprintf(TEXT("\nExecução: %02dms..."), (lt.wMilliseconds - lt1.wMilliseconds));
	return 0;
}
/* ----------------------------------------------------- */
/* "Thread" - Funcao associada à Thread */
/* ----------------------------------------------------- */

DWORD WINAPI Contar(LPVOID param) {
	int i;
	PARAM *y;
	y = (PARAM *)param;

	DWORD dwCount = 0, dwWaitResult;

	// Codigo da threat Contar
	_tprintf(TEXT(" - [Thread %d] Vou contar de %d a %d..."), y->num, y->inicio, y->fim);
	Sleep(100);
	for (i = y->inicio; i < y->fim; i++) {
		dwWaitResult = WaitForSingleObject(
			ghMutex,    // handle to mutex
			INFINITE);  // no time-out interval

		switch (dwWaitResult) {
			// The thread got ownership of the mutex
		case WAIT_OBJECT_0:
			__try {
				gotoxy(0, 14 + y->num);
				_tprintf(TEXT("[Thread %d]: %5d\n"), y->num, i);
			}
			__finally {
				// Release ownership of the mutex object
				if (!ReleaseMutex(ghMutex))
				{
					// Handle error.
					_tprintf(TEXT("[Thread %d]: Erro mutex"), y->num);
				}
			}
			break;

			// The thread got ownership of an abandoned mutex
			// The database is in an indeterminate state
		case WAIT_ABANDONED:
			return FALSE;
		}
		Sleep(10);
	}
	return 0;
}

void gotoxy(int x, int y) {
	static HANDLE hStdout = NULL;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	if (!hStdout)
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hStdout, coord);
}
