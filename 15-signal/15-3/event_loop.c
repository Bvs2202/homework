#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main()
{
	sigset_t set;
	int signum;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	if (sigprocmask(SIG_BLOCK, &set, NULL) < 0)
	{
		perror("sigprocmask");
		exit(1);
	}

	while (1)
	{
		printf("Жду сигнал. PID = %d\n", getpid());
		sigwait(&set, &signum);
		printf("Получил сигнал %d\n", signum);
	}

	return 0;
}