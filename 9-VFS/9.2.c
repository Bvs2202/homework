#include <ncurses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_PATH 256
#define MAX_PANELS 2

struct file_entry {
	char name[256];
	int is_dir;
};

struct panel_data {
	char current_path[MAX_PATH];
	struct file_entry *files;
	int file_count;
	int selected;
};

struct panel_data panels[MAX_PANELS];
int active_panel = 0;

int cur_catalog(const struct dirent *entry)
{
	return strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0;
}

// Функция для сортировки (сначала директории, потом файлы)
int compare(const struct dirent **a, const struct dirent **b)
{
	int is_dir_a = ((*a)->d_type == DT_DIR);
	int is_dir_b = ((*b)->d_type == DT_DIR);
	
	if (is_dir_a && !is_dir_b)
	{
		return -1;
	}
	if (!is_dir_a && is_dir_b)
	{
		return 1;
	}

	return strcasecmp((*a)->d_name, (*b)->d_name);
}

// Функция для чтения содержимого директории
void read_directory(int panel_index)
{
	struct stat statbuf;
	struct dirent **entry = NULL;
	int n = 0;
	int has_parent = 0;
	char full_path[MAX_PATH];

	n = scandir(panels[panel_index].current_path, &entry, cur_catalog, compare);

	if (n < 0)
	{
		printf("Ошибка при чтении директории\n");
		if (entry)
		{
			free(entry);
		}

		return;
	}

	if (strcmp(panels[panel_index].current_path, "/") != 0)
	{
		has_parent = 1;
	}

	if (panels[panel_index].files)
	{
		free(panels[panel_index].files);
		panels[panel_index].files = NULL;
	}

	panels[panel_index].files = malloc((n + has_parent) * sizeof(struct file_entry));
	if (panels[panel_index].files == NULL)
	{
		printf("Ошибка при выделении памяти\n");
		return;
	}

	if (has_parent)
	{
		strncpy(panels[panel_index].files[0].name, "..", sizeof(panels[panel_index].files[0].name));
		panels[panel_index].files[0].name[sizeof(panels[panel_index].files[0].name) - 1] = '\0';
		panels[panel_index].files[0].is_dir = 1;
	}

	for (int i = 0; i < n; i++)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", panels[panel_index].current_path, entry[i]->d_name);
		if (stat(full_path, &statbuf) == 0)
		{
			strncpy(panels[panel_index].files[i + has_parent].name, entry[i]->d_name, sizeof(panels[panel_index].files[i + has_parent].name));
			panels[panel_index].files[i + has_parent].name[sizeof(panels[panel_index].files[i + has_parent].name) - 1] = '\0';
			panels[panel_index].files[i + has_parent].is_dir = S_ISDIR(statbuf.st_mode);
		}
		else
		{
			strncpy(panels[panel_index].files[i + has_parent].name, entry[i]->d_name, sizeof(panels[panel_index].files[i + has_parent].name));
			panels[panel_index].files[i + has_parent].name[sizeof(panels[panel_index].files[i + has_parent].name) - 1] = '\0';
			panels[panel_index].files[i + has_parent].is_dir = 0;
		}
	}

	panels[panel_index].file_count = n + has_parent;

	for (int i = 0; i < n; i++)
	{
		free(entry[i]);
	}
	free(entry);
}

void draw_panel(struct panel_data *panel, int y, int x, int width, int height, int is_active)
{
	int start = 0;
	int line_y = 0, line_x = 0;

	if (panel->selected >= height - 2)
	{
		start = panel->selected - (height - 3);
	}
	
	for (int i = start; i < panel->file_count && i < start + height - 2; i++)
	{
		line_y = i - start + 2;
		line_x = x + 2;
		
		if (i == panel->selected && is_active) attron(A_REVERSE);
		
		if (panel->files[i].is_dir)
		{
			attron(COLOR_PAIR(1));
			mvprintw(line_y, line_x, "[D] %s", panel->files[i].name);
			attroff(COLOR_PAIR(1));
		}
		else
		{
			attron(COLOR_PAIR(2));
			mvprintw(line_y, line_x, "[F] %s", panel->files[i].name);
			attroff(COLOR_PAIR(2));
		}
		
		if (i == panel->selected && is_active) attroff(A_REVERSE);
	}
	
	attron(COLOR_PAIR(3));
	mvprintw(y, x + 2, " %s ", panel->current_path);
	attroff(COLOR_PAIR(3));
}

// Функция для отрисовки интерфейса
void draw_interface()
{
	clear();
	int max_y, max_x;	
	int panel_width = 0;
	int panel_height = 0;

	getmaxyx(stdscr, max_y, max_x);

	panel_width = (max_x - 3) / 2;
	panel_height = max_y - 2;

	draw_panel(&panels[0], 1, 1, panel_width, panel_height, active_panel == 0);
	draw_panel(&panels[1], 1, panel_width + 2, panel_width, panel_height, active_panel == 1);

	mvprintw(max_y - 1, 0, "Tab - switch panel | Arrows - navigate | Enter - open | q - quit");

	refresh();
}

void handle_input(int ch)
{
	char new_path[MAX_PATH];
	char *slash;

	switch (ch)
	{
		case KEY_UP:
			if (panels[active_panel].selected > 0)
				panels[active_panel].selected--;
			break;
		case KEY_DOWN:
			if (panels[active_panel].selected < panels[active_panel].file_count - 1)
				panels[active_panel].selected++;
			break;
		case '\t':
			active_panel = (active_panel + 1) % MAX_PANELS;
			break;
		case '\n':
			if (panels[active_panel].files[panels[active_panel].selected].is_dir)
			{
				if (strcmp(panels[active_panel].files[panels[active_panel].selected].name, "..") == 0)
				{
					slash = strrchr(panels[active_panel].current_path, '/');
					if (slash)
					{
						*slash = '\0';
						if (strlen(panels[active_panel].current_path) == 0)
							strcpy(panels[active_panel].current_path, "/");
					}
				}
				else
				{
					snprintf(new_path, sizeof(new_path), "%s/%s", panels[active_panel].current_path, panels[active_panel].files[panels[active_panel].selected].name);
					strcpy(panels[active_panel].current_path, new_path);
				}

				panels[active_panel].selected = 0;
				read_directory(active_panel);
			}
			break;
	}
}

int main()
{
	int ch;
	
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);


	getcwd(panels[0].current_path, MAX_PATH);
	getcwd(panels[1].current_path, MAX_PATH);
	panels[0].selected = panels[1].selected = 0;
	read_directory(0);
	read_directory(1);

	while ((ch = getch()) != 'q')
	{
		handle_input(ch);
		draw_interface();
	}

	for (int i = 0; i < MAX_PANELS; i++)
	{
		if (panels[i].files) free(panels[i].files);
	}
	
	endwin();

	return 0;
}