// ex1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

//includes do exercício anterior…
#include <time.h>

#define LIM 10000

DWORD WINAPI Contar(LPVOID param);

void gotoxy(int x, int y);

//typedef struct { int n, nMin, nMax; } PARAM;
typedef struct { 
	int num, inicio, fim; 
} PARAM;

HANDLE *tH;
DWORD *tI;
//int *tP;
PARAM *tP;

int _tmain(int argc, LPTSTR argv[]) {

	//PARAM p; 
	//int y, i, n;
	int nT, nMin, nMax, i;
	
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	
	_tprintf(TEXT("argc=%d, "), argc);

	//for (i = 0; i < argc; i++) {
	//	//_tprintf(TEXT("argv=%s, "), argv[i]);
	//	if( _stscanf_s(argv[i], TEXT("%d"), &n) != 0) {
	//		n = _ttoi(argv[i]);
	//		_tprintf(TEXT("argv=%d, "), n);
	//	} else {
	//		_tprintf(TEXT("Parametros invalidos"));
	//		exit(1);
	//	}
	//}
	
	if (argc != 4) {
		_tprintf(TEXT("\n[ERRO] Parametros invalidos\n"));
		return 1;
	} else {
		nT = _ttoi(argv[1]);
		nMin = _ttoi(argv[2]);
		nMax = _ttoi(argv[3]);
		_tprintf(TEXT("\nVou criar %d threads para contar de %d a %d...\n"), nT, nMin, nMax);
		
		tH = (HANDLE *)malloc(nT * sizeof(HANDLE));
		tI = (DWORD *)malloc(nT * sizeof(DWORD));
		//tP = (int *)malloc(nT * sizeof(int));
		tP = (PARAM *)malloc(nT * sizeof(PARAM));

		for (i = 0; i < nT; i++) {
			//tP[i] = i;
			tP[i].num = i;
			tP[i].inicio = i * 1000;
			tP[i].fim = (i+1) * 1000;

			tH[i] = CreateThread(NULL, 0, Contar, (LPVOID)&tP[i], 0, &tI[i]);
			if (tH[i] != NULL) {
				_tprintf(TEXT("\nLancei uma thread com id %d"), tI[i]);
			}
		}
	}

	WaitForMultipleObjects(nT, tH, TRUE, INFINITE);

	_tprintf(TEXT("\nVou terminar..."));

	return 0;
}
/* ----------------------------------------------------- */
/* "Thread" - Funcao associada à Thread */
/* ----------------------------------------------------- */
//DWORD WINAPI Thread(LPVOID param) {
//	int i;
//	PARAM *y;
//	y = (PARAM *)param;
//	_tprintf(TEXT(" --- [Thread %d] Vou contar de %d a %d..."), y->num, y->inicio, y->fim);
//	Sleep(100);
//	for (i = y->inicio; i < y->fim; i++) {
//		gotoxy(40, 10 + y->num);
//		_tprintf(TEXT("[Thread %d]: %5d\n"), y->num, i);
//		Sleep(10);
//	}
//	return 0;
//}
DWORD WINAPI Contar(LPVOID param) {
	int i;
	PARAM *y;
	y = (PARAM *) param;
	//_tprintf(TEXT("\n[Thread %d]Vou começar a trabalhar\n"), GetCurrentThreadId());

	_tprintf(TEXT(" - [Thread %d] Vou contar de %d a %d "), y->num,y->inicio,y->fim);

	for (i = 0; y->inicio < y->fim; i++) {
		gotoxy(50, 10 + y->num);
		_tprintf(TEXT("T%5d: "), y->num, i);
		Sleep(10);
	}

#if 0
	Sleep(100);
	for (i = 0; i < LIM; i++) {
		//gotoxy(y, y);
		gotoxy(*y, *y);
		_tprintf(TEXT("Thread %5d "), i);
	}
#endif
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