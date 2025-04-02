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
	pid_t pid2;
	const char* paths[] = {"/bin/", "/usr/bin/"};
	char command[MAX_INPUT_SIZE] = {0};
	char *commands[MAX_INPUT_SIZE] = {0};
	char *args1[MAX_INPUT_SIZE] = {0};
	char *args2[MAX_INPUT_SIZE] = {0};
	char path[MAX_INPUT_SIZE] = {0};
	char *token;
	int status = 0;
	int count_args = 0;
	int fd[2] = {0};
	int flag = 0;

	while (1)
	{
		memset(command, 0, sizeof(command));
		memset(args1, 0, sizeof(args1));
		memset(commands, 0, sizeof(commands));
		memset(args2, 0, sizeof(args2));
		flag = 0;
		count_args = 0;

		printf("Введите имя программы и опции запуска программы (exit - выход): ");

		fgets(command, sizeof(command), stdin);

		command[strcspn(command, "\n")] = '\0';

		count_args = 0;
		token = strtok(command, "|");
		while (token != NULL)
		{
			commands[count_args++] = token;
			token = strtok(NULL, "|");
		}
		commands[count_args] = NULL;

		if (count_args > 1)
		{
			flag = 1;
		}

		count_args = 0;

		token = strtok(commands[0], " ");
		while (token != NULL)
		{
			args1[count_args++] = token;
			token = strtok(NULL, " ");
		}
		args1[count_args] = NULL;

		if (count_args == 0)
		{
			printf("Неверный ввод\n");
			continue;
		}

		if (strcmp(args1[0], "exit") == 0)
		{
			break;
		}

		if (flag == 1)
		{
			count_args = 0;
			token = strtok(commands[1], " ");
			while (token != NULL)	
			{
				args2[count_args++] = token;
				token = strtok(NULL, " ");
			}
			args2[count_args] = NULL;

			if (pipe(fd) == -1)
			{
				printf("Ошибка создания pipe\n");
				return 1;
			}
		}

		pid = fork();
		if (pid == -1)
		{
			printf("Ошибка создания процесса\n");
			return 1;
		}
		else if (pid == 0)
		{			
			if (flag == 1)
			{
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}

			for(int i = 0; i < 2; i++)
			{
				snprintf(path, sizeof(path), "%s%s", paths[i], args1[0]);
				execv(path, args1);
			}

			printf("Ошибка запуска программы\n");
			exit(1);
		}

		if (flag == 1)
		{
			pid2 = fork();
			if (pid2 == -1)
			{
				printf("Ошибка создания процесса\n");
				return 1;
			}
			else if (pid2 == 0)
			{
				close(fd[1]);
				dup2(fd[0], STDIN_FILENO);
				close(fd[0]);

				for(int i = 0; i < 2; i++)
				{
					snprintf(path, sizeof(path), "%s%s", paths[i], args2[0]);
					execv(path, args2);
				}

				printf("Ошибка запуска программы\n");

				close(fd[0]);
				close(fd[1]);
				
				exit(1);
			}
		}

		if (flag == 1)
		{
			close(fd[0]);
			close(fd[1]);
		}

		waitpid(pid, &status, 0);
		if (flag == 1)
		{
			waitpid(pid2, &status, 0);
		}
	}

	return 0;
}