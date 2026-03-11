#ifndef DS_STATE_H
#define DS_STATE_H

#include "ds_logic.h"
#include "ds_wiring.h"

#define GATE_W  80
#define GATE_H  50

typedef enum {
    RENDER_SIMPLE,      /* boxes with gate name */
    RENDER_ADVANCED,    /* gate diagrams (default) */
} DSRenderMode;

typedef struct __attribute__((aligned(64))) {
    DSGate      *gates;
    int          gate_count;
    int          gate_capacity;

    DSWire      *wires;
    int          wire_count;
    int          wire_capacity;
    int          connected_wires;

    float        sim_speed;     /* simulation speed multiplier (1.0 = normal) */
    float        clock_acc;     /* accumulator for clock gates */

    bool         busy_adding;
} DSState;

typedef struct __attribute__((aligned(64))) {
    bool         is_busy;
    DSGate       gate;
    int          gate_index;    /* for those gates that already exist and we are just moving */
    int          input_count;

    /* We specify x and y in here because I wont touch gate fields, we only care about that when dropping the gate in-place */
    float        x;
    float        y;
} DSBusyState;

/* TODO: dragging logic for existing gates -> busystate -> drop in-place -> go back to original index*/


/* Initialize busy state, also used in reset */
void ds_bstate_init(DSBusyState *bstate);

/* Initialize an empty simulation state */
void ds_state_init(DSState *state);

/* Free all allocated memory in the state */
void ds_state_destroy(DSState *state);

/* Reset clock phase */
void ds_state_reset_clock(DSState *state);

/* Add a gate to the simulation, returns its index */
int ds_state_add_gate(DSState *state, DSGateType type, int input_count, float x, float y);

/* TODO: Moves gate state->gates[gate_index] to DSBusyState */
void ds_state_move_to_busy(DSState *state, DSBusyState *bstate, int gate_index);

/* TODO: Remove gate from state and push all gates to their appropiate place */
void ds_state_remove_gate(DSState *state, int gate_index);

/* Add a wire connecting src gate output to dst gate input slot */
void ds_state_add_wire(DSState *state, int src_gate, int dst_gate, int dst_input);

/* Update the position of a gate by its index. */
void ds_state_update_position(DSState *state, int gate_index, float x, float y);

/* Run one simulation tick: propagate wires then evaluate all gates */
void ds_state_tick(DSState *state, float dt);

/* When we detect a new gate key */
void ds_bstate_set(DSBusyState *bstate, int input_gkey);

/* Restore defaults for Busy */
void ds_bstate_reset(DSBusyState *bstate);

/* Snap current position of the busy gate to mouse */
void ds_bstate_snap_to_mouse(DSBusyState *bstate);

/* Drop gate in-place, transfer it from busy to the state */
void ds_bstate_drop_gate(DSBusyState *bstate, DSState *state);


#endif