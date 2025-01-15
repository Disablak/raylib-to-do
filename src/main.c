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
const int HEIGHT = 640;
const char * WINDOW_NAME = "To Do List";

Rectangle text_box_rect = {0, HEIGHT - 40, WIDTH - 50, 30};
char text_box_message[40];
bool text_box_active = false;

Rectangle group_box_rect = {2, 100, WIDTH - 16, 30};
Rectangle element_btn_complete_rect = {WIDTH - 16 - 50, 0, 50, 30};

Task test_task;

void DrawAndHandleInputBox();
void DrawTaskElement(Task task, int y_pos);

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, WINDOW_NAME);

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");

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

	test_task.desc = "Some test task";
	
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

		DrawTaskElement(test_task, scroll_vec.y + 32);
		DrawTaskElement(test_task, scroll_vec.y + 32 + 30 + 2);
		
		EndScissorMode();

        DrawAndHandleInputBox();

		GuiButton(btn_add_rect, BTN_ADD_TITLE);

		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

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

void DrawTaskElement(Task task, int y_pos)
{
	group_box_rect.y = y_pos;
	element_btn_complete_rect.y = y_pos;

	GuiPanel(group_box_rect, NULL);
	DrawText(task.desc, group_box_rect.x + 6, group_box_rect.y + 6, 10, BLACK);
	GuiButton(element_btn_complete_rect, "Complete");
}
