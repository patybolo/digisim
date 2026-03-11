#ifndef DS_GUI_H
#define DS_GUI_H

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800

#define GATE_W  80
#define GATE_H  50
#define PADDING (GATE_H / 10)

#define TOP_LEFT_X 10
#define TOP_LEFT_Y 10
#define TOP_RIGHT_X WINDOW_WIDTH - 140 /* substract 100 to leave space for the text */
#define TOP_RIGHT_Y 0

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "ds_state.h"

typedef enum {
    SIM_NAME = 0x00,

    START_INSERT_TEXT,
    INSERT_NOT,
    INSERT_AND,
    INSERT_NAND,
    INSERT_OR,
    INSERT_NOR,
    INSERT_XOR,
    INSERT_XNOR,
    INSERT_CONST,
    INSERT_CLOCK,
    INSERT_BUTTON,
    INSERT_TOGGLE,
    TEXT_ITEM_COUNT,
} DSTextItem;

typedef enum {
    GKEY_NOT = 0x00,
    GKEY_AND,
    GKEY_NAND,
    GKEY_OR,
    GKEY_NOR,
    GKEY_XOR,
    GKEY_XNOR,
    GKEY_CONST,
    GKEY_CLOCK,
    GKEY_BUTTON,
    GKEY_TOGGLE,
    GKEY_COUNT,
} DSGateAddKey;

/* Render the full simulation GUI */
void ds_render(DSState *state, Camera2D cam);

/* Render the gate in the busy state */
void ds_render_busy_gate(DSBusyState *bstate);

/* Render the text menu */
void ds_gui_render_menu(DSState *state, DSBusyState *bstate, Font text_font);

/* Handle wheel zoom */
void ds_gui_handle_zoom(Camera2D *cam);

/* Handle camera panning */
void ds_gui_handle_drag(Camera2D *cam);

/* Handle button and toggle button */
void ds_gui_handle_buttons(DSState *state, Camera2D cam);

/* Handle gate-adding keys. Returned value corresponds to a DSGateType type. 
 * Returns -1 by default if no gate needs to be added */
int ds_gui_handle_add_keys(DSState *state);

/* Handle adding +1 to the input_count of the current gate*/
void ds_gui_handle_add_input(DSBusyState *bstate);

/* TODO: Handle gate deletion from simulation */
void ds_gui_handle_delete(DSState *state, Camera2D *cam);

/* TODO: handle insertion of wires with KEY_Q by holding and dragging to another gate */
void ds_gui_handle_connection(DSState *state);

#endif