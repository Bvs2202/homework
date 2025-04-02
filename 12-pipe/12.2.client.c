#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define NAME_FILE "./MY_FIFO"

int main()
{
	int fd = 0;
	char buffer[64] = {0};

	fd = open(NAME_FILE, O_RDONLY);
	if (fd == -1)
	{
		printf("Ошибка открытия pipe\n");
		return 1;
	}

	read(fd, buffer, sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	printf("Дочерний процесс прочитал: %s\n", buffer);

	close(fd);

	unlink(NAME_FILE);

	return 0;
}