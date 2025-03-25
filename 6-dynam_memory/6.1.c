#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE_ABONENT 10

struct abonent
{
	char name[SIZE_ABONENT];
	char second_name[SIZE_ABONENT];
	char tel[SIZE_ABONENT];
	struct abonent *next;
};

void remove_newline(char *str)
{
	size_t len = strlen(str);

	if (len > 0 && str[len - 1] == '\n')
	{
		str[len - 1] = '\0'; 
	}
}

void add_new_abonent(struct abonent **node)
{
	struct abonent *new_abonent = NULL;
	struct abonent *last_abonent = NULL;

	new_abonent = malloc(sizeof(struct abonent));
	if(new_abonent == NULL)
	{
		printf("Ошибка выделения памяти.\n");
		return;
	}

	printf("\nВведите имя: ");
	fgets(new_abonent->name, SIZE_ABONENT, stdin);
	remove_newline(new_abonent->name);

	printf("Введите фамилию: ");
	fgets(new_abonent->second_name, SIZE_ABONENT, stdin);
	remove_newline(new_abonent->second_name);

	printf("Введите номер телефона: ");
	fgets(new_abonent->tel, SIZE_ABONENT, stdin);
	remove_newline(new_abonent->tel);

	new_abonent->next = NULL;

	if (*node == NULL)
	{
		*node = new_abonent;
	}
	else
	{
		last_abonent = *node;
		while (last_abonent->next != NULL)
		{
			last_abonent = last_abonent->next;
		}
		last_abonent->next = new_abonent;
	}

	printf("Новый абонент добавлен.\n\n");
}

void delete_abonent(struct abonent **node)
{
	struct abonent *curr_abonent = NULL;
	struct abonent *prev_abonent = NULL;
	char tmp_name[SIZE_ABONENT];

	if (*node == NULL)
	{
		printf("\nСправочник пуст.\n\n");
		return;
	}

	printf("\nВведите имя: ");
	fgets(tmp_name, SIZE_ABONENT, stdin);
	remove_newline(tmp_name);

	for(curr_abonent = *node; curr_abonent != NULL; curr_abonent = curr_abonent->next)
	{
		if(strcmp(curr_abonent->name, tmp_name) == 0)
		{
			if (prev_abonent == NULL)
			{
				*node = curr_abonent->next;
			}
			else
			{
				prev_abonent->next = curr_abonent->next;
			}
			free(curr_abonent);
			printf("\nАбонент удален.\n\n");
			return;
		}
		prev_abonent = curr_abonent;
	}
	printf("Абонент не найден.\n\n");
}

void search_abonent(struct abonent *node)
{
	struct abonent *curr_abonent = NULL;
	char tmp_name[SIZE_ABONENT];
	int flag = 0;

	printf("\nВведите имя: ");
	fgets(tmp_name, SIZE_ABONENT, stdin);
	remove_newline(tmp_name);

	for(curr_abonent = node; curr_abonent != NULL; curr_abonent = curr_abonent->next)
	{
		if(strcmp(curr_abonent->name, tmp_name) == 0)
		{
			printf("%s %s %s\n", curr_abonent->name, curr_abonent->second_name, curr_abonent->tel);
			flag = 1;
		}
	}

	if (flag == 0)
	{
		printf("Абонент не найден.\n\n");
	}
	else
	{
		printf("\n");
	}
}

void print_node(struct abonent *node)
{
	struct abonent *curr_abonent = NULL;
	
	if (node == NULL)
	{
		printf("Справочник пуст.\n\n");
		return;
	}

	printf("\n");
	for(curr_abonent = node; curr_abonent != NULL; curr_abonent = curr_abonent->next)
	{
		printf("%s %s %s\n", curr_abonent->name, curr_abonent->second_name, curr_abonent->tel);
	}

	printf("\n");
}

int main()
{
	struct abonent *node = NULL;
	struct abonent *tmp_abonent = NULL;
	int operation = 0;

	while(1)
	{
		printf("1) Добавить абонента.\n2) Удалить абонента.\n3) Поиск абонентов по имени.\n4) Вывод всех записей.\n5) Выход.\nВведите номер операции: ");
		scanf("%d", &operation);
		getchar();

		switch(operation)
		{
			case 1:
				add_new_abonent(&node);
				break;
			case 2:
				delete_abonent(&node);
				break;
			case 3:
				search_abonent(node);
				break;
			case 4:
				print_node(node);
				break;
			case 5:
				while (node != NULL)
				{
					tmp_abonent = node;
					node = node->next;
					free(tmp_abonent);
				}
				printf("Завершение программы.\n");
				return 0;
			default:
				printf("Неверная операция.\n\n");
				break;
		}
	}
}