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
	rec.x = 200;
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
	
	printf("loaded text %s", loaded_text);
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		DrawText(loaded_text, 200, 200, 40, WHITE);

		// draw our texture to the screen
		DrawTextureEx(wabbit, (Vector2){ 0, 0 }, 0, 5, WHITE);

		GuiButton(rec, "Click");

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
