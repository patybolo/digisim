#ifndef DS_STATE_H
#define DS_STATE_H

#include "ds_logic.h"
#include "ds_wiring.h"

typedef enum {
    RENDER_SIMPLE,      /* boxes with gate name */
    RENDER_ADVANCED,    /* gate diagrams (default) */
} DSRenderMode;

typedef struct {
    DSGate      *gates;
    int          gate_count;
    int          gate_capacity;

    DSWire      *wires;
    int          wire_count;
    int          wire_capacity;

    float        sim_speed;     /* simulation speed multiplier (1.0 = normal) */
    float        clock_acc;     /* accumulator for clock gates */
    DSRenderMode render_mode;

    /* --- INTERNALS ---*/

    float        _fps;
    float        _last_tick;
    

} DSState;

/* Initialize an empty simulation state */
void ds_state_init(DSState *state);

/* Free all allocated memory in the state */
void ds_state_destroy(DSState *state);

/* Add a gate to the simulation, returns its index */
int ds_state_add_gate(DSState *state, DSGateType type, int input_count, float x, float y);

/* Add a wire connecting src gate output to dst gate input slot */
void ds_state_add_wire(DSState *state, int src_gate, int dst_gate, int dst_input);

/* Update the position of a gate by its index. TODO: implement this by drag&drop*/
void ds_state_update_position(DSState *state, int gate_index, float x, float y);

/* Run one simulation tick: propagate wires then evaluate all gates */
void ds_state_tick(DSState *state, float dt);

#endif