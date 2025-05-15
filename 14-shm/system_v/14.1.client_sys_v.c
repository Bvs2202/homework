#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_SIZE 1024

union semun {
	int val;
};


int main()
{
	key_t key_shm;
	key_t key_sem;
	union semun arg;
	int shmid;
	int semid;
	char msg[] = "Hello!";
	char *data;
	struct sembuf read_lock = {0, -1, 0};
	struct sembuf read_unlock = {0, 1, 0};
	struct sembuf write_lock = {1, -1, 0};
	struct sembuf write_unlock = {1, 1, 0};

	key_shm = ftok("14.1.server_sys_v.c", 1);
	if (key_shm == -1)
	{
		perror("ftok");
		return 1;
	}

	key_sem = ftok("14.1.server_sys_v.c", 2);
	if (key_sem == -1)
	{
		perror("ftok");
		return 1;
	}

	semid = semget(key_sem, 2, 0666);
	if (semid == -1)
	{
		perror("semget");
		return 1;
	}

	shmid = shmget(key_shm, SHM_SIZE, 0666);
	if (shmid == -1)
	{
		perror("shmget");
		return 1;
	}

	data = shmat(shmid, NULL, 0);
	if (data == (void *)-1)
	{
		perror("shmat");
		return 1;
	}

	semop(semid, &read_lock, 1);
	printf("%s\n", data);

	memset(data, 0, SHM_SIZE);
	strncpy(data, msg, SHM_SIZE - 1);
	semop(semid, &write_unlock, 1);

	return 0;
}