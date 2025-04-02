#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define NAME_FILE "./MY_FIFO"

int main()
{
	int fd = 0;

	if (mkfifo(NAME_FILE, 0666) == -1)
	{
		printf("Ошибка создания pipe\n");
		return 1;
	}

	fd = open(NAME_FILE, O_WRONLY);
	if (fd == -1)
	{
		printf("Ошибка открытия pipe\n");
		return 1;
	}

	write(fd, "Hi!", 3);
	printf("Родитель отправил сообщение!\n");

	close(fd);

	return 0;
}