#include "ds_state.h"
#include <stdlib.h>
#include <string.h>

#include "raylib.h" /* For the position updating and mouse handling in this file */

#define INITIAL_CAPACITY 16
#define CLOCK_PERIOD     1.0f  /* seconds per toggle at speed 1.0 */

void ds_bstate_init(DSBusyState *bstate)
{
    bstate->gate_index  = 0;
    bstate->is_busy     = false;
    bstate->x           = 0.0f;
    bstate->y           = 0.0f;
    bstate->input_count = 0;
}

void ds_state_init(DSState *state)
{
    memset(state, 0, sizeof(*state));

    state->sim_speed        = 1.0f;

    state->gate_capacity    = INITIAL_CAPACITY;
    state->gates            = calloc(state->gate_capacity, sizeof(DSGate));

    state->wire_capacity    = INITIAL_CAPACITY;
    state->wires            = calloc(state->wire_capacity, sizeof(DSWire));
    state->connected_wires  = 0;

    state->busy_adding      = false;
}

void ds_state_destroy(DSState *state)
{
    for (int i = 0; i < state->gate_count; i++)
        ds_gate_destroy(&state->gates[i]);
    free(state->gates);
    free(state->wires);
    memset(state, 0, sizeof(*state));
}

int ds_state_add_gate(DSState *state, DSGateType type, int input_count, float x, float y)
{
    if (state->gate_count >= state->gate_capacity) {
        state->gate_capacity *= 2;
        state->gates = realloc(state->gates, state->gate_capacity * sizeof(DSGate));
    }

    DSGate g = ds_gate_create(type, input_count);
    g.x = x;
    g.y = y;
    state->gates[state->gate_count] = g;
    return state->gate_count++;
}

void ds_state_add_wire(DSState *state, int src_gate, int dst_gate, int dst_input)
{
    if (state->wire_count >= state->wire_capacity) {
        state->wire_capacity *= 2;
        state->wires = realloc(state->wires, state->wire_capacity * sizeof(DSWire));
    }

    DSWire w = { .src_gate = src_gate, .dst_gate = dst_gate, .dst_input = dst_input };
    state->wires[state->wire_count++] = w;
}

void ds_state_update_position(DSState *state, int gate_index, float x, float y)
{
    state->gates[gate_index].x = x;
    state->gates[gate_index].y = y;
}

void ds_state_tick(DSState *state, float dt)
{
    /* Update clock accumulator */
    state->clock_acc += dt * state->sim_speed;

    /* Toggle clock gates when period elapses */
    if (state->clock_acc >= CLOCK_PERIOD) {
        state->clock_acc -= CLOCK_PERIOD;
        for (int i = 0; i < state->gate_count; i++) {
            if (state->gates[i].type == GATE_CLOCK)
                state->gates[i].output = !state->gates[i].output;
        }
    }

    /* Propagate wires: copy source outputs to destination inputs */
    for (int i = 0; i < state->wire_count; i++) {
        DSWire *w   = &state->wires[i];
        DSGate *src = &state->gates[w->src_gate];
        DSGate *dst = &state->gates[w->dst_gate];
        if (w->dst_input < dst->input_count)
            dst->inputs[w->dst_input] = src->output;
    }

    /* Evaluate all non-special gates */
    for (int i = 0; i < state->gate_count; i++) {
        DSGateType t = state->gates[i].type;
        if ((int)t <= START_SPECIAL_GATES) /* while it's "below" the special gates, normal logic */
            ds_gate_eval(&state->gates[i]);
    }
}

void ds_bstate_set(DSBusyState *bstate, int input)
{
    DSGate gate      = ds_gate_create(input, bstate->input_count);
    bstate->gate     = gate;
    bstate->x        = GetMousePosition().x - (GATE_W/2);
    bstate->y        = GetMousePosition().y - (GATE_H/2);
    bstate->is_busy  = true;
}

void ds_bstate_reset(DSBusyState *bstate)
{
    if(!bstate->is_busy) /* error handling (_kinda_) */
    {
        ds_bstate_init(bstate);
    }
}

void ds_bstate_snap_to_mouse(DSBusyState *bstate)
{
    Vector2 delta = GetMouseDelta();
    bstate->x += delta.x;
    bstate->y += delta.y;
}

void ds_bstate_drop_gate(DSBusyState *bstate, DSState *state)
{
    if(bstate->gate_index != 0) {
        /* Drop in place logic for an existing gate */
        ds_state_update_position(state, bstate->gate_index, bstate->x, bstate->y);
    }
    ds_state_add_gate(state, bstate->gate.type, bstate->input_count, bstate->x, bstate->y);
    
    bstate->is_busy     = false;
    state->busy_adding  = false; /* now it can handle gate keys again */

    ds_bstate_reset(bstate);
}