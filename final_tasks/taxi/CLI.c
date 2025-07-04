#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>

enum Status {
	AVAILABLE_STATUS = 0,
	BUSY_STATUS = 1
};

struct Driver {
	pid_t pid;
	int fd_cli;
	int fd_driver;
	enum Status status;
	unsigned int task_timer;
};

int has_input()
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	return select(STDIN_FILENO + 1, &fds, NULL, NULL, NULL) > 0;
}

int create_driver(struct Driver *drivers)
{
	int sv[2] = {0};
	char cmd_input[128] = {0};
	char cmd_name[32] = {0};
	char buff[32] = {0};
	int arg1 = 0;
	int arg2 = 0;
	time_t rem_time, start_time, curr_time;

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) == -1)
	{
		perror("socketpair");
		return -1;
	}

	drivers->task_timer = 0;
	drivers->status = AVAILABLE_STATUS;
	drivers->fd_driver = sv[1];
	drivers->fd_cli = sv[0];
	drivers->pid = fork();

	if (drivers->pid == -1)
	{
		perror("fork");
		return -1;
	}
	else if (drivers->pid == 0)
	{
		close(sv[0]);
		printf("pid = %d\n", drivers->fd_cli);

		while(1)
		{
			memset(cmd_name, 0, sizeof(cmd_name));
			memset(cmd_input, 0, sizeof(cmd_input));
			arg1 = 0;
			arg2 = 0;
			read(sv[1], cmd_input, 128);

			sscanf(cmd_input, "%32s %d %d", cmd_name, &arg1, &arg2);

			if (drivers->status == BUSY_STATUS)
			{
				curr_time = time(NULL) - start_time;
				if (drivers->task_timer <= curr_time)
					drivers->status = AVAILABLE_STATUS;
			}

			if (strcmp("send_task", cmd_name) == 0)
			{
				if (drivers->status == AVAILABLE_STATUS)
				{
					start_time = time(NULL);
					drivers->task_timer = arg2;
					drivers->status = BUSY_STATUS;
				}
				else if (drivers->status == BUSY_STATUS)
				{
					rem_time = drivers->task_timer - (time(NULL) - start_time);
					memset(buff, 0, sizeof(buff));
					sprintf(buff, "Busy %lu", rem_time);
					write(sv[1], buff, strlen(buff));
				}
			}
			else if (strcmp("get_status", cmd_name) == 0)
			{
				memset(buff, 0, sizeof(buff));
				if (drivers->status == AVAILABLE_STATUS)
				{
					sprintf(buff, "%s", "Available");
				}
				else if (drivers->status == BUSY_STATUS)
				{
					rem_time = drivers->task_timer - (time(NULL) - start_time);
					sprintf(buff, "Busy %lu", rem_time);
				}
				write(sv[1], buff, strlen(buff));
			}
		}
	}
	else
	{
		close(sv[1]);
	}

	return 0;
}

int main()
{
	struct Driver drivers[32] = {0};
	int n = 0;
	char cmd_input[128] = {0};
	char cmd_name[32] = {0};
	char buff[32] = {0};
	int arg1 = 0;
	int arg2 = 0;
	int fd_max = 0;
	fd_set fds;

	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		for (int i = 0; i < 32; i++)
		{
			if (drivers[i].fd_cli != 0)
			{
				FD_SET(drivers[i].fd_cli, &fds);
				if (drivers[i].fd_cli > fd_max)
					fd_max = drivers[i].fd_cli;
			}
		}

		if (select(fd_max + 1, &fds, NULL, NULL, NULL) == 0)
			continue;

		if (FD_ISSET(STDIN_FILENO, &fds))
		{
			memset(cmd_input, 0, sizeof(cmd_input));
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
					for (int i = 0; i < 32; i++)
					{
						if (drivers[i].pid == 0)
						{
							if (create_driver(&(drivers[i])) == -1)
							{
								printf("Error create_driver\n");
								perror("create_driver");
							}
							else
							{
								close(drivers[i].fd_driver);
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
							write(drivers[i].fd_cli, cmd_input, strlen(cmd_input));
							break;
						}
					}
				}
				else if (strcmp("get_status", cmd_name) == 0)
				{
					for (int i = 0; i < 32; i++)
					{
						if (drivers[i].pid == arg1)
						{
							write(drivers[i].fd_cli, "get_status", strlen("get_status"));
							break;
						}
					}
				}
				else if (strcmp("get_drivers", cmd_name) == 0)
				{
					for (int i = 0; i < 32; i++)
					{
						if (drivers[i].pid != 0)
							write(drivers[i].fd_cli, "get_status", strlen("get_status"));
					}
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

		for (int i = 0; i < 32; i++)
		{
			if (drivers[i].pid != 0 &&
				FD_ISSET(drivers[i].fd_cli, &fds))
			{
				memset(buff, 0, sizeof(buff));
				if (read(drivers[i].fd_cli, buff, sizeof(buff)) >= 0)
					printf("index %d pid %d fd %d msg: %s\n",
							i, drivers[i].pid, drivers[i].fd_cli, buff);
			}
		}
	}

	printf("Completion of the program, good bye!\n");
	
	return 0;
}