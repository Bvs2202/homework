#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>

struct Driver {
	pid_t pid;
	int fd_cli;
	int fd_driver;
};

int create_driver(struct Driver *drivers)
{
	int n = 0;
	int sv[2] = {0};
	char cmd_input[128] = {0};
	char cmd_name[32] = {0};
	int arg1 = 0;
	int arg2 = 0;

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) == -1)
	{
		perror("socketpair");
		return -1;
	}
	printf("1\n");
	drivers->fd_driver = sv[0];
	drivers->fd_cli = sv[1];

	drivers->pid = fork();
	if (drivers->pid == -1)
	{
		perror("fork");
		return -1;
	}
	else if (drivers->pid == 0)
	{
		close(sv[0]);
		while(1)
		{
			memset(cmd_name, 0, sizeof(cmd_name));
			arg1 = 0;
			arg2 = 0;
			printf("pid = %d\n", getpid());
			read(sv[1], cmd_input, 128);
			printf("Пришла команда!\n");

			n = sscanf(cmd_input, "%32s %d %d", cmd_name, &arg1, &arg2);

			if (strcmp("send_task", cmd_name) == 0 && n == 3)
			{

			}
			else if (strcmp("get_status", cmd_name) == 0 && n == 2)
			{

			}

		}
	}
	else
	{
		close(sv[1]);
	}

	return 0;
}

int send_task()
{
	return 0;
}

int get_status()
{
	return 0;
}

int get_drivers()
{
	return 0;
}

int main()
{
	struct Driver drivers[32] = {0};
	int n = 0;
	char cmd_input[128] = {0};
	char cmd_name[32] = {0};
	int arg1 = 0;
	int arg2 = 0;

	while (1)
	{
		printf("Enter the command: ");
		fgets(cmd_input, sizeof(cmd_input), stdin);
		cmd_input[strcspn(cmd_input, "\n")] = 0;

		if (strcmp(cmd_input, "exit") == 0)
		{
			break;
		}

		memset(cmd_name, 0, sizeof(cmd_name));
		arg1 = 0;
		arg2 = 0;

		n = sscanf(cmd_input, "%32s %d %d", cmd_name, &arg1, &arg2);

		if (n >= 1)
		{
			if (strcmp("create_driver", cmd_name) == 0)
			{
				printf("create driver\n");
				for (int i = 0; i < 32; i++)
				{
					if (drivers[i].pid == 0)
					{
						if (create_driver(&(drivers[i])) == -1)
						{
							printf("Error create_driver\n");
							perror("create_driver");
						}

						break;
					}
				}
			}
			else if (strcmp("send_task", cmd_name) == 0 && n == 3)
			{
				for (int i = 0; i < 32; i++)
				{
					if (drivers[i].pid == arg1)
					{
						printf("send_task\n");
						write(drivers[i].fd_driver, cmd_input, strlen(cmd_input));
						printf("send_task complete\n");
						break;
					}
				}
			}
			else if (strcmp("get_drivers", cmd_name) == 0)
			{

			}
			else
			{
				printf("Wrong command!\n");
				continue;
			}
		}
		else
		{
			printf("The command is not entered\n");
		}
	}

	printf("Completion of the program, good bye!\n");
	
	return 0;
}