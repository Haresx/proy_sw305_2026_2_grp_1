// Agenda Optima: version grafica (raylib).
// Mismos algoritmos y operaciones que la version de consola, con una interfaz
// minimalista, animacion de Merge Sort / Quick Sort y tiempo real de ejecucion.

#include "raylib.h"

#include "app.h"
#include "ui.h"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1100, 720, "Agenda Óptima");
    SetWindowMinSize(940, 640);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);            // Esc se usa para volver al menu, no para cerrar

    ui::iniciarFuentes();
    {
        App app;
        while (!WindowShouldClose() && !app.salir) {
            BeginDrawing();
            ClearBackground(ui::FONDO);
            app.frame();
            EndDrawing();
        }
    }
    ui::cerrarFuentes();
    CloseWindow();
    return 0;
}
