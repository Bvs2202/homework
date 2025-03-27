#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
	pid_t pid;
	int status = 0;


	pid = fork();

	if (pid == -1)
	{
		printf("Ошибка создания процесса\n");
		return 1;
	}
	else if (pid == 0)
	{
		printf("child pid: %d, ppid: %d\n", getpid(), getppid());
		exit(0);
	}
	else
	{
		printf("parent pid: %d, ppid: %d\n", getpid(), getppid());

		wait(&status);
		if (WIFEXITED(status))
		{
			printf("child process finished with status %d\n", WEXITSTATUS(status));
		}
	}

	return 0;
}
