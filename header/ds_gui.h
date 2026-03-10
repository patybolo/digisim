#ifndef DS_GUI_H
#define DS_GUI_H

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "ds_state.h"

/* Render the full simulation GUI */
void ds_render(DSState *state, Camera2D cam);

/* Handle wheel zoom */
void ds_gui_handle_zoom(Camera2D *cam);

/* Handle camera panning */
void ds_gui_handle_drag(Camera2D *cam);

/* Handle button and toggle button */
void ds_gui_handle_buttons(DSState *state, Camera2D cam);

/* TODO: Render the menu bar */
void ds_render_menu(void);

#endif