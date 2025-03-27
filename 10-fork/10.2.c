#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
	pid_t pid_1, pid_2;
	int status = 0;

	pid_1 = fork();
	if (pid_1 == -1)
	{
		printf("Ошибка создания первого процесса\n");
		return 1;
	}
	else if (pid_1 == 0)
	{   
		pid_t pid_3, pid_4;

		printf("дочерний процесс 1 pid: %d, ppid: %d\n", getpid(), getppid());

		pid_3 = fork();
		if (pid_3 == -1)
		{
			printf("Ошибка создания третьего процесса\n");
			exit(1);
		}
		else if (pid_3 == 0)
		{
			printf("дочерний процесс 3 pid: %d, ppid: %d\n", getpid(), getppid());
			exit(0);
		}
		pid_4 = fork();
		if (pid_4 == -1)
		{
			printf("Ошибка создания четвёртого процесса\n");
			exit(1);
		}
		else if (pid_4 == 0)
		{
			printf("дочерний процесс 4 pid: %d, ppid: %d\n", getpid(), getppid());
			exit(0);
		}

		waitpid(pid_3, &status, 0);
		if (WIFEXITED(status))
		{
			printf("дочерний процесс 3 завершился с кодом %d\n", WEXITSTATUS(status));
		}

		waitpid(pid_4, &status, 0);
		if (WIFEXITED(status))
		{
			printf("дочерний процесс 4 завершился с кодом %d\n", WEXITSTATUS(status));
		}

		exit(0);
	}

	pid_2 = fork();
	if (pid_2 == -1)
	{
		printf("Ошибка создания второго процесса\n");
		return 1;
	}
	else if (pid_2 == 0)
	{
		pid_t pid_5;

		printf("дочерний процесс 2 pid: %d, ppid: %d\n", getpid(), getppid());

		pid_5 = fork();
		if (pid_5 == -1)
		{
			printf("Ошибка создания пятого процесса\n");
			exit(1);
		}
		else if (pid_5 == 0)
		{
			printf("дочерний процесс 5 pid: %d, ppid: %d\n", getpid(), getppid());
			exit(0);
		}

		waitpid(pid_5, &status, 0);
		if (WIFEXITED(status))
		{
			printf("дочерний процесс 5 завершился с кодом %d\n", WEXITSTATUS(status));
		}
		exit(0);
	}

	printf("родительский процесс pid: %d, ppid: %d\n", getpid(), getppid());

	waitpid(pid_1, &status, 0);
	if (WIFEXITED(status))
	{
		printf("дочерний процесс 1 завершился с кодом %d\n", WEXITSTATUS(status));
	}

	waitpid(pid_2, &status, 0);
	if (WIFEXITED(status))
	{
		printf("дочерний процесс 2 завершился с кодом %d\n", WEXITSTATUS(status));
	}

	return 0;
}