#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 256


int main()
{
	pid_t pid;
	char command[MAX_INPUT_SIZE] = {0};
	char *args[MAX_INPUT_SIZE] = {0};
	char path[MAX_INPUT_SIZE] = {0};
	char *token;
	int status = 0;
	int count_args = 0;

	while (1)
	{
		memset(command, 0, sizeof(command));
		memset(args, 0, sizeof(args));

		printf("Введите имя программы и опции запуска программы (exit - выход): ");

		fgets(command, sizeof(command), stdin);

		command[strcspn(command, "\n")] = '\0';

		count_args = 0;
		token = strtok(command, " ");
		while (token != NULL && count_args < MAX_INPUT_SIZE)
		{
			args[count_args++] = token;
			token = strtok(NULL, " ");
		}
		args[count_args] = NULL;

		if (count_args == 0)
		{
			continue;
		}

		if (strcmp(args[0], "exit") == 0)
		{
			break;
		}

		pid = fork();
		if (pid == -1)
		{
			printf("Ошибка создания процесса\n");
			return 1;
		}
		else if (pid == 0)
		{			
			snprintf(path, sizeof(path), "/bin/%s", args[0]);

			execv(path, args);

			printf("Ошибка запуска программы\n");
			exit(1);
		}
		else
		{
			waitpid(pid, &status, 0);
		}
	}

	return 0;
}