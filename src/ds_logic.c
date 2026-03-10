#include "ds_logic.h"
#include <string.h>

/* --- Gate logic functions --- */

static Bit logic_not(Bit *inputs, int count)
{
    (void)count;
    return !inputs[0];
}

static Bit logic_and(Bit *inputs, int count)
{
    for (int i = 0; i < count; i++)
        if (!inputs[i]) return 0;
    return 1;
}

static Bit logic_nand(Bit *inputs, int count)
{
    return !logic_and(inputs, count);
}

static Bit logic_or(Bit *inputs, int count)
{
    for (int i = 0; i < count; i++)
        if (inputs[i]) return 1;
    return 0;
}

static Bit logic_nor(Bit *inputs, int count)
{
    return !logic_or(inputs, count);
}

static Bit logic_xor(Bit *inputs, int count)
{
    Bit result = 0;
    for (int i = 0; i < count; i++)
        result ^= inputs[i];
    return result;
}

static Bit logic_xnor(Bit *inputs, int count)
{
    return !logic_xor(inputs, count);
}

static Bit logic_const(Bit *inputs, int count)
{
    (void)inputs;
    (void)count;
    return 1;
}

/* Clock, Button, Toggle: output is managed externally by the simulation loop */
static Bit logic_passthrough(Bit *inputs, int count)
{
    (void)inputs;
    (void)count;
    return 0;
}

/* --- Name table --- */

static const char *gate_names[GATE_TYPE_COUNT] = {
    [GATE_NOT]    = "NOT",
    [GATE_AND]    = "AND",
    [GATE_NAND]   = "NAND",
    [GATE_OR]     = "OR",
    [GATE_NOR]    = "NOR",
    [GATE_XOR]    = "XOR",
    [GATE_XNOR]   = "XNOR",
    [GATE_CONST]  = "1",
    [GATE_CLOCK]  = "CLK",
    [GATE_BUTTON] = "BTN",
    [GATE_TOGGLE] = "TGL",
};

const char *ds_gate_type_name(DSGateType type)
{
    if (type >= 0 && type < GATE_TYPE_COUNT)
        return gate_names[type];
    return "???";
}

/* --- Logic function lookup --- */

Bit (*ds_gate_logic_fn(DSGateType type))(Bit *, int)
{
    switch (type) {
    case GATE_NOT:    return logic_not;
    case GATE_AND:    return logic_and;
    case GATE_NAND:   return logic_nand;
    case GATE_OR:     return logic_or;
    case GATE_NOR:    return logic_nor;
    case GATE_XOR:    return logic_xor;
    case GATE_XNOR:   return logic_xnor;
    case GATE_CONST:  return logic_const;
    case GATE_CLOCK:
    case GATE_BUTTON:
    case GATE_TOGGLE: return logic_passthrough;
    default:          return logic_passthrough;
    }
}

/* --- Gate lifecycle --- */

DSGate ds_gate_create(DSGateType type, int input_count)
{
    DSGate g = {0};
    g.type        = type;
    g.input_count = input_count;
    g.output      = 0;
    g.logic       = ds_gate_logic_fn(type);
    g.x           = 0;
    g.y           = 0;

    if (input_count > 0) {
        g.inputs = calloc(input_count, sizeof(Bit));
    } else {
        g.inputs = NULL;
    }

    return g;
}

void ds_gate_destroy(DSGate *gate)
{
    free(gate->inputs);
    gate->inputs     = NULL;
    gate->input_count = 0;
}

void ds_gate_eval(DSGate *gate)
{
    if (gate->logic)
        gate->output = gate->logic(gate->inputs, gate->input_count);
}