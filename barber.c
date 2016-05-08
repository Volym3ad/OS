#include <stdio.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <malloc.h>

sem_t sem_clients, sem_free_chairs, sem_work;
int id, N, threads_count;

void* client(void *arg)
{
	int temp = rand()%5;
	int curr_id = ++id;
	sleep(1 + temp);
	if (!sem_trywait(&sem_free_chairs))
	{
		printf("Client %d entered.\n", curr_id);
		sem_post(&sem_clients);
		sem_wait(&sem_work);
		sem_post(&sem_free_chairs);
	}
	else
		printf("Client %d went away.\n", curr_id);
	return 0;
}

void* barber(void *arg)
{
	int temp, value;
	while (1)
	{
		sem_getvalue(&sem_clients, &value);
		if (value == 0)
			printf("Barber is sleeping.\n");
		sem_wait(&sem_clients);
		printf("Barber is working.\n");
		temp = rand()%2;
		sleep(1 + temp);
		sem_post(&sem_work);
	}
	return 0;
}

int main()
{
	int i;
	srand(time(NULL));
	pthread_t *threads_id, barber_id;
	id = 0;

	printf("Enter the number of chairs: ");
	scanf("%d", &N);
	printf("Enter the number of clients: ");
	scanf("%d", &threads_count);

	threads_id = (pthread_t*)malloc(threads_count * sizeof(pthread_t));
	sem_init(&sem_clients, 0, 0);
	sem_init(&sem_free_chairs, 0, N + 1);
	sem_init(&sem_work, 0, 0);

	pthread_create(&barber_id, NULL, barber, NULL);
	for(i = 0; i < threads_count; ++i)
	{
		sleep(1);
		pthread_create(&threads_id[i], NULL, client, NULL);
	}
	for (i = 0; i < threads_count; ++i)
		pthread_join(threads_id[i], NULL);

	sem_destroy(&sem_clients);
	sem_destroy(&sem_free_chairs);
	free(threads_id);
	return 0;
}
