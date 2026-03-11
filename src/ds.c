#include "raylib.h"
#include "ds_state.h"
#include "ds_gui.h"

#define FPS 144

void ds_demo();

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DigiSim");
    SetTargetFPS(FPS);

    Font text_font = LoadFontEx("resources/opensans.ttf", 32, NULL, 0);

    DSState state;
    DSBusyState bstate;
    ds_bstate_init(&bstate);
    ds_state_init(&state);
    ds_demo(&state);

    Camera2D cam = {0};
    cam.zoom = 1.0f;

    while (!WindowShouldClose()) {
        /* TODO: ensure we are rendering the frame under 1/FPS */
        float dt = GetFrameTime();

        /* Simulation speed control */
        if (IsKeyPressed(KEY_UP) && state.sim_speed < 20.0f) 
            state.sim_speed += 0.5f;
        if (IsKeyPressed(KEY_DOWN) && state.sim_speed > 0.5f) 
            state.sim_speed -= 0.5f;

        ds_gui_handle_buttons(&state, cam);
        ds_gui_handle_drag(&cam);
        ds_gui_handle_zoom(&cam);
        ds_gui_handle_add_input(&bstate);

        int input_gkey = ds_gui_handle_add_keys(&state);
        if (input_gkey != -1) 
            ds_bstate_set(&bstate, input_gkey);

        if(bstate.is_busy) {
            ds_bstate_snap_to_mouse(&bstate);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                ds_bstate_drop_gate(&bstate, &state);
        }

        ds_state_tick(&state, dt);

        BeginDrawing();

            ds_render(&state, cam);
            ds_gui_render_menu(&state, &bstate, text_font);
            if(bstate.is_busy)
                ds_render_busy_gate(&bstate);
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
    
    int but  = ds_state_add_gate(state, GATE_TOGGLE, 0, 50, 400);

    int clk  = ds_state_add_gate(state, GATE_CLOCK,  0, 50, 500);

    int a    = ds_state_add_gate(state, GATE_AND,    5, 300, 300);

    ds_state_add_wire(state, c1,  a, 0);
    ds_state_add_wire(state, c2,  a, 1);
    ds_state_add_wire(state, c3,  a, 2);
    ds_state_add_wire(state, but, a, 3);
    ds_state_add_wire(state, clk, a, 4);
}