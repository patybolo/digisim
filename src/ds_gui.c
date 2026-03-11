#include "ds_gui.h"
#include <string.h>

#define GATE_W  80
#define GATE_H  50
#define PADDING (GATE_H / 10)

#define TOP_LEFT_X 10
#define TOP_LEFT_Y 10
#define TOP_RIGHT_X WINDOW_WIDTH - 100 /* substract 100 to leave space for the text */
#define TOP_RIGHT_Y 10

/* --- GUI TEXT LOOKUP TABLES --- */
static const char * const menu_text[TEXT_ITEM_COUNT] = {
    [SIM_NAME]       = "DIGITAL SIMULATOR",
    [INSERT_NOT]     = "[N]OT",
    [INSERT_AND]     = "[A]ND",
    [INSERT_NAND]    = "NAN[D]",
    [INSERT_OR]      = "[O]R",
    [INSERT_NOR]     = "NO[R]",
    [INSERT_XOR]     = "[X]OR",
    [INSERT_XNOR]    = "XNOR (g)",
    [INSERT_CONST]   = "[C]ONST",
    [INSERT_CLOCK]   = "CLOC[K]",
    [INSERT_BUTTON]  = "[B]UTTON",
    [INSERT_TOGGLE]  = "[T]OGGLE",
};

static const int gate_keys[GKEY_COUNT] = {
    [GKEY_NOT]       = KEY_N,
    [GKEY_AND]       = KEY_A,
    [GKEY_NAND]      = KEY_D,
    [GKEY_OR]        = KEY_O,
    [GKEY_NOR]       = KEY_R,
    [GKEY_XOR]       = KEY_X,
    [GKEY_XNOR]      = KEY_G,
    [GKEY_CONST]     = KEY_C,
    [GKEY_CLOCK]     = KEY_K,
    [GKEY_BUTTON]    = KEY_B,
    [GKEY_TOGGLE]    = KEY_T
};

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

void ds_render(DSState *state, Camera2D cam, Font text_font)
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
    ds_gui_render_menu(state, text_font);

    
}

void ds_render_busy_gate(DSBusyState *bstate)
{
    /* Don't use the regular gate drawing, a bit of boilerplate but worth it */
    DrawRectangle((int)bstate->x, (int)bstate->y, GATE_W, GATE_H, BLUE);
    DrawRectangleLines((int)bstate->x, (int)bstate->y, GATE_W, GATE_H, DARKGRAY);

    const char *name = ds_gate_type_name(bstate->gate.type);
    int tw = MeasureText(name, 16);
    DrawText(name,
             (int)(bstate->x + (GATE_W - tw) / 2),
             (int)(bstate->y + (GATE_H - 16) / 2),
             16, BLACK);
}

void ds_gui_render_menu(DSState *state, Font text_font)
{
    DrawText(menu_text[SIM_NAME], TOP_LEFT_X, TOP_LEFT_Y, 20, RAYWHITE);
    DrawText(TextFormat("Speed: %.1fx", state->sim_speed), 10, 35, 16, RAYWHITE);
    DrawText(state->render_mode == RENDER_SIMPLE ? "[S]imple" : "[A]dvanced",
             10, 55, 16, RAYWHITE);
             
    /* Start from i=1 to skip SIM_NAME*/
    for(int i = 1; i < TEXT_ITEM_COUNT; i++) {
        DrawTextEx(text_font,menu_text[i],
            (Vector2){TOP_RIGHT_X, TOP_RIGHT_Y + (30 * i)},
            24,
            0,
            LIGHTGRAY);
    }
}

void ds_gui_handle_zoom(Camera2D *cam)
{
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *cam);

        // Set the offset to where the mouse is
        cam->offset = GetMousePosition();

        // Set the target to match, so that the cam maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        cam->target = mouseWorldPos;

        // Zoom increment
        // Uses log scaling to provide consistent zoom speed
        float scale = 0.2f * wheel;
        cam->zoom = Clamp(expf(logf(cam->zoom) + scale), 0.125f, 64.0f);
    }
}

void ds_gui_handle_drag(Camera2D *cam)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        Vector2 delta = GetMouseDelta();
        cam->target = Vector2Add(cam->target, Vector2Scale(delta, -1.0f / cam->zoom));
    }
}

void ds_gui_handle_buttons(DSState *state, Camera2D cam)
{
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), cam);
    bool isDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool isPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    for (int i = 0; i < state->gate_count; i++) {
        DSGate *g = &state->gates[i];
        if (g->type != GATE_BUTTON && g->type != GATE_TOGGLE)
            continue;

        Rectangle gate_rect = { g->x, g->y, GATE_W, GATE_H };
        bool hovered = CheckCollisionPointRec(mouseWorldPos, gate_rect);

        if (g->type == GATE_BUTTON) {
            g->output = (isDown && hovered) ? 1 : 0;
        } else if (g->type == GATE_TOGGLE) {
            if (isPressed && hovered) {
                g->output = !g->output;
            }
        }
    }
}

int ds_gui_handle_add_keys(DSState *state)
{
    int gate_type = -1; /* if it reaches end of control on -1, no gate needs to be added. */
    if(state->busy_adding) return gate_type;

    for(int i = 0; i < GKEY_COUNT; i++) {
        if (IsKeyDown(gate_keys[i])) {
            gate_type = i;
            break; /* only handle one key per tick */
        }
    }

    return gate_type;
}