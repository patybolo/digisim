#include "ds_gui.h"
#include <string.h>

#define GATE_W  80
#define GATE_H  50
#define PADDING (GATE_H / 10)

static void draw_gate_simple(DSGate *gate)
{
    Color box_color = gate->output ? GREEN : GRAY;
    DrawRectangle((int)gate->x, (int)gate->y, GATE_W, GATE_H, box_color);
    DrawRectangleLines((int)gate->x, (int)gate->y, GATE_W, GATE_H, DARKGRAY);

    const char *name = ds_gate_type_name(gate->type);
    int tw = MeasureText(name, 16);
    DrawText(name,
             (int)(gate->x + (GATE_W - tw) / 2),
             (int)(gate->y + (GATE_H - 16) / 2),
             16, BLACK);
}

void ds_render(DSState *state, Camera2D cam)
{
    ClearBackground((Color){30, 30, 30, 255});

    BeginMode2D(cam);

    /* Draw wires */
    /* TODO: Refine this algorithm, currently it just draws a straight line src->dst */
    
    for (int i = 0; i < state->wire_count; i++) {
        DSWire *w = &state->wires[i];
        DSGate *src = &state->gates[w->src_gate];
        DSGate *dst = &state->gates[w->dst_gate];
    
        /* For each wire, we calculate the height of the connect accordingly with the input count*/

        Vector2 point1 = { src->x + GATE_W,
                           src->y + GATE_H / 2.0f }; /* they come from the middle of the source gate */
                           
        Vector2 point2 = { dst->x,
                           dst->y + PADDING + (GATE_H / dst->input_count) * i };

        Color wire_color = src->output ? GREEN : GRAY;
        DrawLineEx(point1, point2, 2.0f, wire_color);
    }

    /* Draw gates */
    for (int i = 0; i < state->gate_count; i++) {
        if (state->render_mode == RENDER_SIMPLE)
            draw_gate_simple(&state->gates[i]);
        else
            draw_gate_simple(&state->gates[i]); /* TODO: advanced rendering */
    }

    EndMode2D();

    /* --- HUD --- */
    DrawText("DIGITAL SIMULATOR", 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("Speed: %.1fx", state->sim_speed), 10, 35, 16, RAYWHITE);
    DrawText(state->render_mode == RENDER_SIMPLE ? "[S]imple" : "[A]dvanced",
             10, 55, 16, RAYWHITE);

    
}

void ds_gui_handle_zoom(Camera2D cam)
{
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), cam);

        // Set the offset to where the mouse is
        cam.offset = GetMousePosition();

        // Set the target to match, so that the cam maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        cam.target = mouseWorldPos;

        // Zoom increment
        // Uses log scaling to provide consistent zoom speed
        float scale = 0.2f * wheel;
        cam.zoom = Clamp(expf(logf(cam.zoom) + scale), 0.125f, 64.0f);
    }
}