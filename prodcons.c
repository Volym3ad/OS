#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>

HANDLE semaphoreProducer, semaphoreConsumer, semaphoreCreatingAvailable, semaphoreDeletingAvailable;
int listLength, size;
CRITICAL_SECTION output;

struct List {
	List *next;
	List *previous;
	int indexOf;
} *pointer, *begin;

void create(int size) {
	begin = new List;
	List *prev = begin;
	begin->indexOf = 1;
	begin->previous = NULL;
	pointer = begin;
	for (int i = 1; i < size; ++i) {
		pointer = new List;
		pointer->previous = prev;
		prev->next = pointer;
		prev = pointer;
		pointer->indexOf = i + 1;
	}
	pointer->next = NULL;
}

void removeFirst() {
	if (begin != pointer) {
		begin = begin->next;
		delete begin->previous;
		begin->previous = NULL;
	}
	else {
		delete begin;
		begin = NULL;
		pointer = NULL;
	}
}

void add(List *p) {
	if (pointer != NULL) {
		pointer->next = p;
		p->previous = pointer;
		p->indexOf = pointer->indexOf + 1;
		p->next = NULL;
		pointer = p;
	}
	else {
		pointer = p;
		pointer->next = NULL;
		pointer->previous = NULL;
		pointer->indexOf = 1;
		begin = pointer;
	}
}

DWORD WINAPI producer(LPVOID) {
	WaitForSingleObject(semaphoreProducer, INFINITE);
	WaitForSingleObject(semaphoreCreatingAvailable, INFINITE);
	add((List*)malloc(sizeof(List)));
	EnterCriticalSection(&output);
	++size;
	printf("Producer: Size=%d\n", size);
	LeaveCriticalSection(&output);
	ReleaseSemaphore(semaphoreDeletingAvailable, 1, NULL);
	ReleaseSemaphore(semaphoreProducer, 1, NULL);
	return 0;
}

DWORD WINAPI consumer(LPVOID) {
	WaitForSingleObject(semaphoreDeletingAvailable, INFINITE);
	WaitForSingleObject(semaphoreConsumer, INFINITE);
	removeFirst();
	EnterCriticalSection(&output);
	--size;
	printf("Consumer: Size=%d\n", size);
	LeaveCriticalSection(&output);
	ReleaseSemaphore(semaphoreCreatingAvailable, 1, NULL);
	ReleaseSemaphore(semaphoreConsumer, 1, NULL);	
	return 0;
}

VOID main( VOID )
{
	PHANDLE threads;
	INT N, M;
	printf("Vvedite maksimalnoe kolichestvo elementov v spiske: ");
	scanf("%d", &listLength);
	printf("Vvedite kolichestvo potrbitelei: ");
	scanf("%d", &N);
	printf("Vvedite kolichestvo proizvoditelei: ");
	scanf("%d", &M);

	threads = (PHANDLE)malloc((N + M) * sizeof(List));
	create(listLength);
	size = listLength;
	printf("Size=%d\n", size);
	InitializeCriticalSection(&output);
	semaphoreProducer = CreateSemaphore(NULL, 1, 1, NULL);
	semaphoreConsumer = CreateSemaphore(NULL, 1, 1, NULL);
	semaphoreCreatingAvailable = CreateSemaphore(NULL, 0, listLength, NULL);
	semaphoreDeletingAvailable = CreateSemaphore(NULL, listLength, listLength, NULL);
	
	for(int i = 0; i < N; ++i)
		threads[i] = CreateThread(NULL, 0, consumer, NULL, 0, NULL);
	for(int i = N; i < M + N; ++i)
		threads[i] = CreateThread(NULL, 0, producer, NULL, 0, NULL);

	WaitForMultipleObjects(N + M, threads, TRUE, INFINITE);

	for (int i = 0; i < N + M; ++i)
		CloseHandle(threads[i]);
	free(threads);

	_getch();
}
