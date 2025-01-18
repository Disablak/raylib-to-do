#include "stdio.h"
#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct Task
{
	char *desc;
} Task;

const int WIDTH = 640;
const int HEIGHT = 480;
const int DESK_LENGHT = 100;
const int TASK_HEIGHT = 30;
const int SCROLl_SPACE = 2;
const int TAB_COMPLETED = 0;
const int TAB_TO_DO = 1;
const int TASK_HEIGHT_WITH_SPACE = TASK_HEIGHT + SCROLl_SPACE;

const char *WINDOW_NAME = "To Do List";
const char *SAVES_NAME = "to-do-saves";
const char *TAB_NAMES = "Completed; TO-DO";
const char *BTN_ADD_TITLE = "Add";

int toggle_id = 1;

Rectangle input_rect = {0, HEIGHT - 40, WIDTH - 50, 30};
char input_text[40];
bool input_active = false;

Rectangle task_rect = {2, 100, WIDTH - 16, 30};
Rectangle task_btn_rect = {WIDTH - (14 + 50), 0, 50, 30};

Rectangle btn_add_rect = {WIDTH - 50, HEIGHT - 40, 50, 30};

Task *to_do_tasks;
int to_do_tasks_length;

Task *completed_tasks;
int completed_tasks_length;

Rectangle scroll_rect = {0, 30, WIDTH, HEIGHT - 70};
Rectangle scroll_content = {0, 30, WIDTH - 14, 1000};
Rectangle scroll_view = {0, 30, WIDTH - 14, HEIGHT - 70};
Vector2 scroll_vec;

int active_toggle = 0;
Rectangle toggle_group_bounds = { 0, 0, WIDTH / 2, 30 };

void LoadTasksFromFile();
void SaveTasksToFile();
void DrawAndHandleInputBox();
void DrawTaskElement(Task task, int y_pos, int task_id);
void DrawTaskToDo(Task task, int y_pos, int task_id);
void DrawTaskCompleted(Task, int y_pos, int task_id);
void MoveTaskFromToDoToComplete(int id);
char **SplitStringIntoRows(const char *input, int *rowCount);
void SetTextToDesc(char **desk, char *text, int *counter);
void AddNewTask(char *desc);
void CompleteTask(Task task);
void DeleteTask(int id);

int GetCurrentTabElementCount()
{
	if (toggle_id == TAB_COMPLETED)
	{
		return completed_tasks_length;
	}else
	{
		return to_do_tasks_length;
	}
}

void UpdateScrollSize()
{
	scroll_content.height = GetCurrentTabElementCount() * TASK_HEIGHT_WITH_SPACE;
	if (scroll_content.height < scroll_rect.height)
		scroll_content.height = scroll_rect.height;
}

void Cleanup()
{
	for (size_t i = 0; i < completed_tasks_length; i++)
		free(completed_tasks[i].desc);

	free(completed_tasks);

	for (size_t i = 0; i < to_do_tasks_length; i++)
		free(to_do_tasks[i].desc);

	free(to_do_tasks);
}

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(WIDTH, HEIGHT, WINDOW_NAME);
	SearchAndSetResourceDir("resources");

	LoadTasksFromFile();
	
	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(WHITE);

		GuiToggleGroup(toggle_group_bounds, TAB_NAMES, &toggle_id);

		UpdateScrollSize();
		GuiScrollPanel(scroll_rect, NULL, scroll_content, &scroll_vec, &scroll_view);

		BeginScissorMode(scroll_rect.x, scroll_rect.y, scroll_rect.width, scroll_rect.height);
		
		if (toggle_id == TAB_COMPLETED)
		{
			if (completed_tasks_length == 0)
				DrawText("There are no completed tasks", 160, scroll_content.height / 2, 20, BLACK);

			for (int i = 0; i < completed_tasks_length; i++)
			{
				DrawTaskCompleted(completed_tasks[i], scroll_vec.y + TASK_HEIGHT_WITH_SPACE + (i * TASK_HEIGHT_WITH_SPACE), i);
			}
		}
		else if (toggle_id == TAB_TO_DO)
		{
			if (to_do_tasks_length == 0)
				DrawText("Enter your first task and click 'Add'", 150, scroll_content.height / 2, 20, BLACK);

			for (int i = 0; i < to_do_tasks_length; i++)
			{
				DrawTaskToDo(to_do_tasks[i], scroll_vec.y + TASK_HEIGHT_WITH_SPACE + (i * TASK_HEIGHT_WITH_SPACE), i);
			}
		}
		
		EndScissorMode();

		if (toggle_id == TAB_TO_DO)
			DrawAndHandleInputBox();

		EndDrawing();
	}

	SaveTasksToFile();

	Cleanup();

	CloseWindow();
	return 0;
}

void DrawAndHandleInputBox()
{
	if (CheckCollisionPointRec(GetMousePosition(), input_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
	{
        input_active = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
	{
        input_active = false;
    }

	if (GuiTextBox(input_rect, input_text, sizeof(input_text), input_active)) 
	{
		input_active = !input_active;

		AddNewTask(input_text);
		memset(input_text, '\0', sizeof(input_text));
    }

	if (GuiButton(btn_add_rect, BTN_ADD_TITLE))
	{
		AddNewTask(input_text);
		memset(input_text, '\0', sizeof(input_text));
	}
}

void DrawTaskCompleted(Task task, int y_pos, int task_id)
{
	DrawTaskElement(task, y_pos, task_id);

	if (GuiButton(task_btn_rect, "Delete"))
	{
		DeleteTask(task_id);
	}
}

void DrawTaskToDo(Task task, int y_pos, int task_id)
{
	DrawTaskElement(task, y_pos, task_id);

	if (GuiButton(task_btn_rect, "Complete"))
	{
		MoveTaskFromToDoToComplete(task_id);
	}
}

void DrawTaskElement(Task task, int y_pos, int task_id)
{
	task_rect.y = y_pos;
	task_btn_rect.y = y_pos;

	GuiPanel(task_rect, NULL);
	DrawText(task.desc, task_rect.x + 6, task_rect.y + 6, 20, BLACK);
}

void LoadTasksFromFile()
{
	char *loaded_text;

	if (FileExists(SAVES_NAME))
	{
		loaded_text = LoadFileText(SAVES_NAME);
	}
	else
	{
		SaveFileText(SAVES_NAME, "\0");
		return;
		//loaded_text = malloc(100 * sizeof(char));
		//strcpy(loaded_text, "*first task\0");
	}

	if (loaded_text == NULL)
	{
		free(loaded_text);
		return;
	}

	int i = 0;
	while (loaded_text[i] != '\0')
	{
		if (loaded_text[i] == '*')
		{
			to_do_tasks_length++;
		}else if (loaded_text[i] == '/')
		{
			completed_tasks_length++;
		}
		i++;
	}
	
	to_do_tasks = (Task*)malloc(to_do_tasks_length * sizeof(Task));
	completed_tasks = (Task*)malloc(completed_tasks_length * sizeof(Task));

	int rowCount;
	char **rows = SplitStringIntoRows(loaded_text, &rowCount);
	
	int to_do_counter = 0;
	int completed_counter = 0;
	for (int task_i = 0; task_i < rowCount; task_i++)
	{
		char *row = rows[task_i];
		char firstChar = row[0];
		if (firstChar == '*')
		{
			SetTextToDesc(&to_do_tasks[to_do_counter].desc, row, &to_do_counter);
		}else if (firstChar == '/')
		{
			SetTextToDesc(&completed_tasks[completed_counter].desc, row, &completed_counter);
		}
		free(row);
	}
	free(rows);
	free(loaded_text);
}

void removeFirstChar(char *str) {
	if (str != NULL && strlen(str) > 0) 
	{
		memmove(str, str + 1, strlen(str)); // Shift the string left by one position
	}
}

void SetTextToDesc(char **desk, char *text, int *counter)
{
	removeFirstChar(text);
	*desk = malloc(DESK_LENGHT * sizeof(char));
	strcpy(*desk, text);
	(*counter)++;
}

void RemoveTaskFromToDo(int id)
{
	for (int i = id; i < to_do_tasks_length - 1; i++) 
	{
		to_do_tasks[i] = to_do_tasks[i + 1];
	}

	to_do_tasks_length--;
	to_do_tasks = realloc(to_do_tasks, to_do_tasks_length * sizeof(Task));
}

void MoveTaskFromToDoToComplete(int id)
{
	Task task = to_do_tasks[id];
	RemoveTaskFromToDo(id);
	CompleteTask(task);
}

char **SplitStringIntoRows(const char *input, int *rowCount) {
    char *inputCopy = strdup(input);
    if (!inputCopy) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    char **rows = NULL;
    *rowCount = 0;

    char *line = strtok(inputCopy, "\n");
    while (line) {
        rows = realloc(rows, (*rowCount + 1) * sizeof(char *));
        if (!rows) {
            printf("Memory reallocation failed\n");
            free(inputCopy);
            return NULL;
        }

        rows[*rowCount] = strdup(line);
        if (!rows[*rowCount]) {
            printf("Memory allocation failed\n");
            for (int i = 0; i < *rowCount; i++) {
                free(rows[i]);
            }
            free(rows);
            free(inputCopy);
            return NULL;
        }

        (*rowCount)++;
        line = strtok(NULL, "\n");
    }

    free(inputCopy);
    return rows;
}

void AddNewTask(char *desc)
{
	to_do_tasks_length++;
	to_do_tasks = (Task*)realloc(to_do_tasks, to_do_tasks_length * sizeof(Task));
	to_do_tasks[to_do_tasks_length - 1].desc = malloc(DESK_LENGHT * sizeof(char));
	strcpy(to_do_tasks[to_do_tasks_length - 1].desc, desc);
}

char *GetStringByTask(Task task, bool is_completed)
{
	char *line = malloc(DESK_LENGHT * sizeof(char));
	
	if (is_completed)
	{
		strcat(line, "/");
	}else
	{
		strcat(line, "*");
	}
	
	strcat(line, task.desc);
	strcat(line, "\n");

	return line;
}

void SaveTasksToFile()
{
	char *file_text = malloc((completed_tasks_length + to_do_tasks_length) * DESK_LENGHT * sizeof(char));
	int cur_char_idx = 0;
	for (size_t i = 0; i < completed_tasks_length; i++)
	{
		char *line = GetStringByTask(completed_tasks[i], true);
		strcat(file_text, line);
		free(line);
	}

	for (size_t i = 0; i < to_do_tasks_length; i++)
	{
		char *line = GetStringByTask(to_do_tasks[i], false);
		strcat(file_text, line);
		free(line);
	}
	
	SaveFileText(SAVES_NAME, file_text);

	free(file_text);
}

void CompleteTask(Task task)
{
	completed_tasks_length++;
	completed_tasks = realloc(completed_tasks, completed_tasks_length * sizeof(Task));
	completed_tasks[completed_tasks_length - 1] = task;
}

void DeleteTask(int id)
{
	free(completed_tasks[id].desc);

	for (int i = id; i < completed_tasks_length - 1; i++) 
	{
		completed_tasks[i] = completed_tasks[i + 1];
    }

	completed_tasks_length--;
	completed_tasks = realloc(completed_tasks, completed_tasks_length * sizeof(Task));
}
