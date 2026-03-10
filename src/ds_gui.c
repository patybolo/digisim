#include "ds_gui.h"
#include <string.h>

#define GATE_W  80
#define GATE_H  50

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
    for (int i = 0; i < state->wire_count; i++) {
        DSWire *w = &state->wires[i];
        DSGate *src = &state->gates[w->src_gate];
        DSGate *dst = &state->gates[w->dst_gate];

        Vector2 p1 = { src->x + GATE_W, src->y + GATE_H / 2.0f };
        Vector2 p2 = { dst->x,           dst->y + GATE_H / 2.0f };
        Color wire_color = src->output ? GREEN : GRAY;
        DrawLineEx(p1, p2, 2.0f, wire_color);
    }

    /* Draw gates */
    for (int i = 0; i < state->gate_count; i++) {
        if (state->render_mode == RENDER_SIMPLE)
            draw_gate_simple(&state->gates[i]);
        else
            draw_gate_simple(&state->gates[i]); /* TODO: advanced rendering */
    }

    EndMode2D();

    /* HUD */
    DrawText("DIGITAL SIMULATOR", 10, 10, 20, RAYWHITE);
    DrawText(TextFormat("Speed: %.1fx", state->sim_speed), 10, 35, 16, RAYWHITE);
    DrawText(state->render_mode == RENDER_SIMPLE ? "[S]imple" : "[A]dvanced",
             10, 55, 16, RAYWHITE);
}
