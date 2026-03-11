#include "ds_gui.h"
#include <string.h>

/* --- GUI TEXT LOOKUP TABLES --- */
static const char * const menu_text[TEXT_ITEM_COUNT] = {
    [SIM_NAME]       = "DIGITAL SIMULATOR",
    [INSERT_NOT]     = "[n] not",
    [INSERT_AND]     = "[a] and",
    [INSERT_NAND]    = "[d] nand",
    [INSERT_OR]      = "[o] or",
    [INSERT_NOR]     = "[r] nor",
    [INSERT_XOR]     = "[x] xor",
    [INSERT_XNOR]    = "[g] xnor",
    [INSERT_CONST]   = "[c] CONST",
    [INSERT_CLOCK]   = "[k] CLOCK",
    [INSERT_BUTTON]  = "[b] BUTTON",
    [INSERT_TOGGLE]  = "[t] TOGGLE",
};

/* --- GATE KEY LOOKUP TABLE --- */
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

static void draw_gate(DSGate *gate)
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
    /* TODO: Refine this algorithm, currently it just draws a straight line src->dst
       Hopefully I can implement something to render the lines in parts to straighten them...
    */
    
    for (int i = 0; i < state->wire_count; i++) {
        DSWire *w   = &state->wires[i];
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
        draw_gate(&state->gates[i]);
    }

    EndMode2D();    
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

void ds_gui_render_menu(DSState *state, DSBusyState *bstate, Font text_font)
{
    DrawText(menu_text[SIM_NAME], TOP_LEFT_X, TOP_LEFT_Y, 20, RAYWHITE);
    DrawText(TextFormat("Speed: %.1fx", state->sim_speed), 10, 35, 16, RAYWHITE);
    /* DrawText(state->render_mode == RENDER_SIMPLE ? "[S]imple" : "[A]dvanced",
             10, 55, 16, RAYWHITE); */
             
    /* Start from i=START_INSERT_TEXT to skip UI elements that aren't gate insertions*/
    for(int i = START_INSERT_TEXT; i < TEXT_ITEM_COUNT; i++) {
        DrawTextEx(text_font, menu_text[i],
            (Vector2){TOP_RIGHT_X, TOP_RIGHT_Y + (30 * i)},
            32,
            0,
            LIGHTGRAY);
    }

    if(bstate->is_busy) {
        DrawText(TextFormat("Input Count: %d", bstate->input_count),
                TOP_LEFT_X + 100,
                TOP_LEFT_Y + 30,
                24,
                 BLUE);
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

void ds_gui_handle_add_input(DSBusyState *bstate)
{
    if(!bstate->is_busy) return;

    if (IsKeyReleased(KEY_X))
        bstate->input_count++;
    else if (IsKeyReleased(KEY_Z) && bstate->input_count > 0)
        bstate->input_count--;
}