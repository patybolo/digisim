#ifndef DS_GUI_H
#define DS_GUI_H

#include "raylib.h"
#include "ds_state.h"

/* Render the full simulation GUI */
void ds_render(DSState *state, Camera2D cam);

void ds_gui_handle_zoom(Camera2D cam);

/* TODO: Render the menu bar */
void ds_render_menu(void);

#endif