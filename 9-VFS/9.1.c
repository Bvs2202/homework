#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SIZE_TEXT 20

int main()
{
	char text[SIZE_TEXT] = "String from file";
	char buffer[SIZE_TEXT] = {0};
	int fd = 0;
	int len_text = strlen(text);
	
	fd = open("output.txt", O_WRONLY | O_CREAT, 0644);
	if (fd == -1)
	{
		printf("Ошибка открытия файла\n");
		return 1;
	}

	write(fd, text, len_text);

	close(fd);

	fd = open("output.txt", O_RDONLY);
	if (fd == -1)
	{
		printf("Ошибка открытия файла\n");
		return 1;
	}

	for (int i = 0; i < len_text; i++)
	{
		lseek(fd, -1 - i, SEEK_END);
		read(fd, &buffer[i], 1);
	}
	buffer[len_text] = '\0';	
	
	printf("%s\n", buffer);

	close(fd);

	return 0;
}