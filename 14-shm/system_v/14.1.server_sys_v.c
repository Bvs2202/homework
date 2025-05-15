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
	char msg[] = "Hi!";
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

	semid = semget(key_sem, 2, 0666 | IPC_CREAT);
	if (semid == -1)
	{
		perror("semget");
		return 1;
	}

	arg.val = 0;
	if (semctl(semid, 0, SETVAL, arg) == -1)
	{
		perror("semctl");
		return 1;
	}
	if (semctl(semid, 1, SETVAL, arg) == -1)
	{
		perror("semctl");
		return 1;
	}

	shmid = shmget(key_shm, SHM_SIZE, 0666 | IPC_CREAT);
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

	strncpy(data, msg, SHM_SIZE - 1);
	semop(semid, &read_unlock, 1);

	semop(semid, &write_lock, 1);
	printf("%s\n", data);

	shmdt(data);
	shmctl(shmid, IPC_RMID, NULL);
	semctl(semid, 0, IPC_RMID);

	return 0;
}