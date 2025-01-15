#include "stdio.h"
#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct Task
{
	char *desc;
	Rectangle rect;
} Task;

const int WIDTH = 640;
const int HEIGHT = 480;
const char * WINDOW_NAME = "To Do List";

Rectangle text_box_rect = {0, HEIGHT - 40, WIDTH - 50, 30};
char text_box_message[40];
bool text_box_active = false;

Rectangle group_box_rect = {2, 100, WIDTH - 16, 30};
Rectangle element_btn_complete_rect = {WIDTH - (14 + 50), 0, 50, 30};

Task *to_do_tasks;
int to_do_tasks_length;

Task *completed_tasks;
int completed_tasks_length;

void LoadTasksFromFile();
void DrawAndHandleInputBox();
void DrawTaskElement(Task task, int y_pos, int task_id);
void MoveTaskFromToDoToComplete(int id);

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(WIDTH, HEIGHT, WINDOW_NAME);
	SearchAndSetResourceDir("resources");

	LoadTasksFromFile();

	Rectangle rec;
	rec.x = 0;
	rec.y = 0;
	rec.width = 100;
	rec.height = 40;

	const char * saves_name = "to-do-saves";
	char * loaded_text;

	if (FileExists(saves_name))
	{
		loaded_text = LoadFileText(saves_name);
	}
	else
	{
		SaveFileText(saves_name, '\0');
		loaded_text = "nothing";
	}

	Rectangle scroll_rect = {0, 30, WIDTH, HEIGHT - 70};
	Rectangle scroll_content = {0, 30, WIDTH - 14, 1000};
	Rectangle scroll_view = {0, 30, WIDTH - 14, HEIGHT - 70};
	Vector2 scroll_vec;

	int active_toggle = 0;
	int start_id_toggle = 1;
	Rectangle toggle_group_bounds = { 0, 0, WIDTH / 2, 30 };
	
	printf("loaded text %s", loaded_text);

	const char * DEBUG_TEXT = "Cur Toggle ";
	char toggle_degug_text[20];
	char cur_toggle_text[2] = "0";

	Rectangle btn_add_rect = {WIDTH - 50, HEIGHT - 40, 50, 30};
	const char * BTN_ADD_TITLE = "Add";

	
	
	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(WHITE);

		GuiToggleGroup(toggle_group_bounds, "Completed; TO-DO", &start_id_toggle);

		GuiScrollPanel(scroll_rect, NULL, scroll_content, &scroll_vec, &scroll_view);

		BeginScissorMode(scroll_rect.x, scroll_rect.y, scroll_rect.width, scroll_rect.height);
		
		sprintf(cur_toggle_text, "%d", start_id_toggle);
		strcpy(toggle_degug_text, DEBUG_TEXT);
		strcat(toggle_degug_text, cur_toggle_text);
		DrawText(toggle_degug_text, 200 + scroll_vec.x, 200 + scroll_vec.y, 40, BLACK);
		DrawText(text_box_message, 200 + scroll_vec.x, 300 + scroll_vec.y, 40, BLACK);

		//DrawTaskElement(test_task, scroll_vec.y + 32);
		//DrawTaskElement(test_task, scroll_vec.y + 32 + 30 + 2);
		for (int i = 0; i < to_do_tasks_length; i++)
		{
			DrawTaskElement(to_do_tasks[i], scroll_vec.y + 32 + (i * 32), i);
		}
		
		EndScissorMode();

        DrawAndHandleInputBox();

		GuiButton(btn_add_rect, BTN_ADD_TITLE);

		EndDrawing();
	}

	free(to_do_tasks);
	free(completed_tasks);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

void DrawAndHandleInputBox()
{
	if (CheckCollisionPointRec(GetMousePosition(), text_box_rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
	{
        text_box_active = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
	{
        text_box_active = false;
    }

	if (GuiTextBox(text_box_rect, text_box_message, sizeof(text_box_message), text_box_active)) 
	{
        text_box_active = !text_box_active;
    }
}

void DrawTaskElement(Task task, int y_pos, int task_id)
{
	group_box_rect.y = y_pos;
	element_btn_complete_rect.y = y_pos;

	GuiPanel(group_box_rect, NULL);
	DrawText(task.desc, group_box_rect.x + 6, group_box_rect.y + 6, 10, BLACK);

	if (GuiButton(element_btn_complete_rect, "Complete"))
	{
		MoveTaskFromToDoToComplete(task_id);
	}
}

void LoadTasksFromFile()
{
	to_do_tasks_length = 2;
	to_do_tasks = (Task*)malloc(to_do_tasks_length * sizeof(Task));
	
	to_do_tasks[0].desc = "First task";
	to_do_tasks[1].desc = "Second task";
}

void MoveTaskFromToDoToComplete(int id)
{
	for (int i = id; i < to_do_tasks_length - 1; i++) 
	{
		to_do_tasks[i] = to_do_tasks[i + 1];
    }

	to_do_tasks_length--;

	to_do_tasks = realloc(to_do_tasks, to_do_tasks_length * sizeof(Task));
}
