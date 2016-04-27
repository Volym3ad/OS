#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#define COUNT 2

pthread_mutex_t *forks, output;
int id, N;

void* philosopher(void* arg)
{
	int curr_id = ++id, i = 0;
	int left = curr_id, right = curr_id + 1;
	if (right == N)
		right = 0;
	while (i < COUNT)
	{
		++i;
		if (!pthread_mutex_trylock(&forks[left]))
		{
			if (!pthread_mutex_trylock(&forks[right]))
			{
				pthread_mutex_lock(&output);
				printf("Philosopher %d is eating\n", curr_id);
				pthread_mutex_unlock(&output);
				int temp = rand()%3;
				sleep(1 + temp);
				pthread_mutex_unlock(&forks[right]);
			}
			pthread_mutex_unlock(&forks[left]);
		}
	}
	return 0;
}

int main()
{
	int i;
	id = 0;
	srand(time(NULL));
	pthread_t *threads_id;
	printf("Enter the amount of philosophers: ");
	scanf("%d", &N);

	forks = (pthread_mutex_t*)malloc(N * sizeof(pthread_mutex_t));
	threads_id = (pthread_t*)malloc(N * sizeof(pthread_t));
	pthread_mutex_init(&output, NULL);

	for (i = 0; i < N; ++i)
		pthread_mutex_init(&forks[i], NULL);
	for (i = 0; i < N; ++i)
		pthread_create(&threads_id[i], NULL, philosopher, NULL);
	for (i = 0; i < N; ++i)
		pthread_join(threads_id[i], NULL);

	free(forks);
	free(threads_id);
	return 0;
}
