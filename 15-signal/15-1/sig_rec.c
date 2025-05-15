#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void handler(int signum)
{
	printf("Сигнал поступил: %d\n", signum);
}

int main()
{
	struct sigaction sa;
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	sa.sa_handler = handler;
	sa.sa_mask = set;
	sa.sa_flags = 0;

	if (sigaction(SIGUSR1, &sa, NULL) < 0)
	{
		perror("sigaction");
		exit(1);
	}

	while (1)
	{
		printf("Жду сигнала, PID = %d\n", getpid());
		sleep(1);
	}

	return 0;
}