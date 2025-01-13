#include "stdio.h"
#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const int WIDTH = 640;
const int HEIGHT = 640;
const char * WINDOW_NAME = "To Do List";


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

	Rectangle scroll_rect = {0, 30, WIDTH, 500};
	Rectangle scroll_content = {0, 30, WIDTH - 14, 1000};
	Rectangle scroll_view = {0, 30, WIDTH - 14, 500};
	Vector2 scroll_vec;

	int active_toggle = 0;
	int start_id_toggle = 0;
	Rectangle toggle_group_bounds = { 0, 0, WIDTH / 2, 30 };
	
	printf("loaded text %s", loaded_text);

	const char * DEBUG_TEXT = "Cur Toggle ";
	char toggle_degug_text[20];
	char cur_toggle_text[2] = "0";
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		BeginDrawing();

		ClearBackground(BLACK);

		GuiToggleGroup(toggle_group_bounds, "Completed; TO-DO", &start_id_toggle);

		GuiScrollPanel(scroll_rect, NULL, scroll_content, &scroll_vec, &scroll_view);

		BeginScissorMode(scroll_rect.x, scroll_rect.y, scroll_rect.width, scroll_rect.height);
		
		sprintf(cur_toggle_text, "%d", start_id_toggle);
		strcpy(toggle_degug_text, DEBUG_TEXT);
		strcat(toggle_degug_text, cur_toggle_text);
		DrawText(toggle_degug_text, 200 + scroll_vec.x, 200 + scroll_vec.y, 40, BLACK);
		
		EndScissorMode();

		//GuiButton(rec, "Click");

		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
