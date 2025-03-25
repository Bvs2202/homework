#include <stdio.h>
#include <string.h>

char *search_substr(char *str, char *substr)
{
	char *ptr_str = str;
	char *ptr_substr = substr;

	for(; *str != '\0'; str++)
	{
		ptr_str = str;
		ptr_substr = substr;

		while(*ptr_str == *ptr_substr && *ptr_substr != '\0')
		{
			ptr_str++;
			ptr_substr++;
		}

		if(*ptr_substr == '\0')
		{
			return str;
		}
	}
	return NULL;
}

int main()
{
	char str[100];
	char substr[100];
	char *result;

	printf("Введите строку: ");
	fgets(str, sizeof(str), stdin);
	if(str[0] != '\0' && str[strlen(str) - 1] == '\n')
	{
		str[strlen(str) - 1] = '\0';
	}

	printf("Введите подстроку: ");
	fgets(substr, sizeof(substr), stdin);
	if(substr[0] != '\0' && substr[strlen(substr) - 1] == '\n')
	{
		substr[strlen(substr) - 1] = '\0';
	}

	result = search_substr(str, substr);

	if(result == NULL)
	{
		printf("Подстрока не найдена.\n");
	}
	else
	{
		printf("%s\n", result);
	}

	return 0;
}