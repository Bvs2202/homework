#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	sigset_t block;

	sigemptyset(&block);
	sigaddset(&block, SIGINT);

	if (sigprocmask(SIG_BLOCK, &block, NULL) < 0)
	{
		perror("sigprocmask");
		exit(1);
	}

	printf("SIGINT заблокирована. PID = %d\n", getpid());

	while(1)
	{
		pause();
	}

	return 0;
}