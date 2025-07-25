#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define SHM_SIZE 1024

int main()
{
	void *mem_ptr;
	int fd_shm;
	sem_t *sem_read; 
	sem_t *sem_write;
	char msg[] = "Hi!";

	fd_shm = shm_open("/my_shared", O_CREAT | O_RDWR | O_EXCL, 0666);
	if (fd_shm == -1)
	{
		perror("shm_open");
		return 1;
	}

	ftruncate(fd_shm, SHM_SIZE);

	sem_read = sem_open("/sem_read", O_CREAT | O_EXCL, 0666, 0);
	if (sem_read == (void *)-1)
	{
		perror("sem_open");
		close(fd_shm);
		return 1;
	}
	sem_write = sem_open("/sem_write", O_CREAT | O_EXCL, 0666, 1);
	if (sem_write == (void *)-1)
	{
		perror("sem_open");
		sem_close(sem_read);
		sem_unlink("/sem_read");
		close(fd_shm);
		return 1;
	}

	mem_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	if (mem_ptr == MAP_FAILED)
	{
		perror("mmap");
		close(fd_shm);
		sem_close(sem_read);
		sem_unlink("/sem_read");
		sem_close(sem_write);
		sem_unlink("/sem_write");
		return 1;
	}

	sem_wait(sem_write);
	strncpy((char *)mem_ptr, msg, SHM_SIZE - 1);
	sem_post(sem_write);

	sem_wait(sem_read);
	printf("%s\n", (char *)mem_ptr);

	munmap(mem_ptr, SHM_SIZE);
	close(fd_shm);
	shm_unlink("/my_shared");
	sem_close(sem_read);
	sem_unlink("/sem_read");
	sem_close(sem_write);
	sem_unlink("/sem_write");

	return 0;
}