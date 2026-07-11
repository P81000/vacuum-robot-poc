#include <string>

#include "raylib.h"

int main(void) {
    const int screen_width = 800;
    const int screen_height = 800;

    InitWindow(screen_width, screen_height, "Vacuum PoC");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Visualizer Env Isolated & Running", screen_width / 2 - 165, screen_height / 2, 20, DARKBROWN);
        DrawCircle(screen_width / 2, screen_height / 2 + 50, 25, BLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
