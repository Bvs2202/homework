#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
	pid_t pid;

	printf("Введите PID процесса: ");
	scanf("%d", &pid);

	if (kill(pid, SIGINT) < 0)
	{
		perror("kill");
		exit(1);
	}

	printf("Сигнал SIGINT отправлен процессу %d\n", pid);

	return 0;
}