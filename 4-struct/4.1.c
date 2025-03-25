#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 100
#define SIZE_ABONENT 10

struct abonent
{
	char name[SIZE_ABONENT];
	char second_name[SIZE_ABONENT];
	char tel[SIZE_ABONENT];
};

void remove_newline(char *str)
{
	size_t len = strlen(str);

	if (len > 0 && str[len - 1] == '\n')
	{
		str[len - 1] = '\0'; 
	}
}

void add_new_abonent(struct abonent *node)
{
	for(int i = 0; i < N; i++)
	{
		if(node[i].name[0] == '\0')
		{
			printf("\nВведите имя: ");
			fgets(node[i].name, SIZE_ABONENT, stdin);
			remove_newline(node[i].name);

			printf("Введите фамилию: ");
			fgets(node[i].second_name, SIZE_ABONENT, stdin);
			remove_newline(node[i].second_name);

			printf("Введите номер телефона: ");
			fgets(node[i].tel, SIZE_ABONENT, stdin);
			remove_newline(node[i].tel);

			printf("Новый абонент добавлен.\n\n");

			return;
		}
	}
	printf("\nВ справочнике нет свободных мест.\n\n");
}

void delete_abonent(struct abonent *node)
{
	char tmp_name[SIZE_ABONENT];

	printf("\nВведите имя: ");
	fgets(tmp_name, SIZE_ABONENT, stdin);
	remove_newline(tmp_name);

	for(int i = 0; i < N; i++)
	{
		if(strcmp(node[i].name, tmp_name) == 0)
		{
			node[i] = (struct abonent){0};
			printf("\nАбонент удален.\n\n");
			return;
		}
	}
	printf("Абонент не найден.\n\n");
}

void search_abonent(struct abonent *node)
{
	char tmp_name[SIZE_ABONENT];
	int flag = 0;

	printf("\nВведите имя: ");
	fgets(tmp_name, SIZE_ABONENT, stdin);
	remove_newline(tmp_name);

	for(int i = 0; i < N; i++)
	{
		if(strcmp(node[i].name, tmp_name) == 0)
		{
			printf("%s %s %s\n", node[i].name, node[i].second_name, node[i].tel);
			flag = 1;
		}
	}
	if(flag == 0)
	{
		printf("Абонент не найден.\n");
	}
	printf("\n");
}

void print_node(struct abonent *node)
{
	int flag = 0;

	printf("\n");
	for(int i = 0; i < N; i++)
	{
		if(node[i].name[0] != '\0')
		{
			printf("%s %s %s\n", node[i].name, node[i].second_name, node[i].tel);
			flag = 1;
		}
	}
	if(flag == 0)
	{
		printf("Справочник пуст.\n");
	}
	printf("\n");
}

int main()
{
	struct abonent node[N] = {0};
	int operation = 0;

	while(1)
	{
		printf("1) Добавить абонента.\n2) Удалить абонента.\n3) Поиск абонентов по имени.\n4) Вывод всех записей.\n5) Выход.\nВведите номер операции: ");
		scanf("%d", &operation);
		getchar();

		switch(operation)
		{
			case 1:
				add_new_abonent(node);
				break;
			case 2:
				delete_abonent(node);
				break;
			case 3:
				search_abonent(node);
				break;
			case 4:
				print_node(node);
				break;
			case 5:
				printf("Завершение программы.\n");
				return 0;
			default:
				printf("Неверная операция.\n\n");
				break;
		}
	}

	return 1;
}