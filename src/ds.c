#include "raylib.h"
#include "ds_state.h"
#include "ds_gui.h"

#define WIDTH  800
#define HEIGHT 600

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "DigiSim");
    SetTargetFPS(60);

    DSState state;
    ds_state_init(&state);

    /* Demo: add a few gates to see something on screen */
    int c   = ds_state_add_gate(&state, GATE_CONST, 0, 50, 100);
    int a   = ds_state_add_gate(&state, GATE_AND,   2, 200, 100);
    int clk = ds_state_add_gate(&state, GATE_CLOCK, 0, 50, 200);

    ds_state_add_wire(&state, c,   a, 0);
    ds_state_add_wire(&state, clk, a, 1);

    Camera2D cam = {0};
    cam.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* Simulation speed control */
        if (IsKeyPressed(KEY_UP))   state.sim_speed += 0.5f;
        if (IsKeyPressed(KEY_DOWN) && state.sim_speed > 0.5f) state.sim_speed -= 0.5f;

        /* Toggle render mode */
        if (IsKeyPressed(KEY_TAB))
            state.render_mode = (state.render_mode == RENDER_SIMPLE)
                                ? RENDER_ADVANCED : RENDER_SIMPLE;

        ds_state_tick(&state, dt);

        BeginDrawing();
            ds_render(&state, cam);
        EndDrawing();
    }

    ds_state_destroy(&state);
    CloseWindow();

    return 0;
}