#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
	int fd[2] = {0};
	int pid = 0;
	char buffer[64] = {0};

	if (pipe(fd) == -1)
	{
		printf("Ошибка создания pipe\n");
		return 1;
	}

	pid = fork();
	if (pid == -1)
	{
		printf("Ошибка создания процесса\n");
		return 1;
	}
	else if (pid == 0)
	{
		close(fd[1]);

		read(fd[0], buffer, sizeof(buffer));
		buffer[sizeof(buffer) - 1] = '\0';

		printf("Дочерний процесс прочитал: %s\n", buffer);

		close(fd[0]);

		exit(0);
	}
	else
	{
		close(fd[0]);

		write(fd[1], "Hi!", 3);
		printf("Родитель отправил сообщение!\n");

		close(fd[1]);
	}

	wait(NULL);

	return 0;
}