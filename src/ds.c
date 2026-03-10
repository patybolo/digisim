#include "raylib.h"
#include "ds_state.h"
#include "ds_gui.h"

#define WIDTH  1200
#define HEIGHT 800

void ds_demo();

int main()
{
    InitWindow(WIDTH, HEIGHT, "DigiSim");
    SetTargetFPS(60);

    DSState state;
    ds_state_init(&state);
    ds_demo(&state);

    Camera2D cam = {0};
    cam.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* Simulation speed control */
        if (IsKeyPressed(KEY_UP) && state.sim_speed < 20.0f)   state.sim_speed += 0.5f;
        if (IsKeyPressed(KEY_DOWN) && state.sim_speed > 0.5f) state.sim_speed -= 0.5f;

        /* Toggle render mode */
        if (IsKeyPressed(KEY_TAB)) {
            state.render_mode = (state.render_mode == RENDER_SIMPLE)
                                ? RENDER_ADVANCED : RENDER_SIMPLE;
        }

        ds_gui_handle_buttons(&state, cam);
        ds_gui_handle_drag(&cam);
        ds_state_tick(&state, dt);
        ds_gui_handle_zoom(&cam);

        BeginDrawing();
        
            ds_render(&state, cam);
            
        EndDrawing();
    }

    /* Cleanup */
    ds_state_destroy(&state);
    CloseWindow();

    return 0;
}

void ds_demo(DSState *state)
{
    /* Demo: add a few gates to see something on screen */
    int c1   = ds_state_add_gate(state, GATE_CONST,  0, 50, 100);
    int c2   = ds_state_add_gate(state, GATE_CONST,  0, 50, 200);
    int c3   = ds_state_add_gate(state, GATE_CONST,  0, 50, 300);
    
    int but  = ds_state_add_gate(state, GATE_BUTTON, 0, 50, 400);

    int clk  = ds_state_add_gate(state, GATE_CLOCK,  0, 50, 500);

    int a    = ds_state_add_gate(state, GATE_AND,    5, 300, 300);

    ds_state_add_wire(state, c1,  a, 0);
    ds_state_add_wire(state, c2,  a, 1);
    ds_state_add_wire(state, c3,  a, 2);
    ds_state_add_wire(state, but, a, 3);
    ds_state_add_wire(state, clk, a, 4);
}