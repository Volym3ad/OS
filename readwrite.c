#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>

CRITICAL_SECTION criticalSection1, criticalSection2;
DWORD reader;

DWORD WINAPI writerThread(LPVOID) {
	EnterCriticalSection(&criticalSection1);
	printf("\nwriter entered");
	srand(time(NULL));
	Sleep(100 * rand()%11);
	printf("\nwriter left");
	LeaveCriticalSection(&criticalSection1);
	return 0;
}

DWORD WINAPI readerThread(LPVOID) {
	Sleep(15);
	EnterCriticalSection(&criticalSection2);
	if (reader == 0) 
		EnterCriticalSection(&criticalSection1);  
	++reader;
	printf("\nreader %d entered", reader);
	LeaveCriticalSection(&criticalSection2);
	srand(time(NULL));
	Sleep(100 * rand()%11);
	EnterCriticalSection(&criticalSection2);
	printf("\nreader %d left", reader);
	--reader;
	if (reader == 0)
		LeaveCriticalSection(&criticalSection1);   
	LeaveCriticalSection(&criticalSection2);
	return 0;
}

VOID main() {
	PHANDLE threadHandle;
	DWORD readerCount, writerCount;

	printf("Vvedite kolichestvo chitatelei: ");
	scanf("%d", &readerCount);
	printf("Vvedite kolichestvo pisatelei: ");
	scanf("%d", &writerCount);

	threadHandle = (PHANDLE)malloc((readerCount + writerCount) * sizeof(HANDLE));

	InitializeCriticalSection(&criticalSection1);
	InitializeCriticalSection(&criticalSection2);

	reader = 0;
	
	for (int i = 0; i < readerCount; ++i)
		threadHandle[i] = CreateThread(NULL, 0, readerThread, NULL, 0, NULL);
	for (int i = 0; i < writerCount; ++i)
		threadHandle[i + readerCount] = CreateThread(NULL, 0, writerThread, NULL, 0, NULL);

	WaitForMultipleObjects(readerCount + writerCount, threadHandle, TRUE, INFINITE);

	for (int i = 0; i < readerCount + writerCount; ++i)
		CloseHandle(threadHandle[i]);
	free(threadHandle);
	DeleteCriticalSection(&criticalSection1);
	DeleteCriticalSection(&criticalSection2);
	_getch();
}
