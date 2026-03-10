#ifndef DS_LOGIC_H
#define DS_LOGIC_H

#include <stdbool.h>
#include <stdlib.h>

#define START_SPECIAL_GATES 0xFF

/* Bit: logically 1 bit, backed by bool (1 byte minimum addressable) */
typedef bool Bit;

typedef enum {
    /* Logic gates */
    GATE_NOT = 0x00,
    GATE_AND,
    GATE_NAND,
    GATE_OR,
    GATE_NOR,
    GATE_XOR,
    GATE_XNOR,
    
    /* Special gates */
    GATE_CONST = START_SPECIAL_GATES,     /* always outputs 1 */
    GATE_CLOCK,     /* toggles output periodically */
    GATE_BUTTON,    /* outputs 1 while held */
    GATE_TOGGLE,    /* toggles output on click */
    GATE_TYPE_COUNT
} DSGateType;

typedef struct {
    DSGateType type;
    Bit       *inputs;      /* dynamic array of input bits */
    int        input_count;
    Bit        output;
    Bit      (*logic)(Bit *inputs, int count);
    /* GUI positioning */
    float      x, y;
} DSGate;

/* Returns the display name string for a gate type */
const char *ds_gate_type_name(DSGateType type);

/* Returns the appropriate logic function for a gate type */
Bit (*ds_gate_logic_fn(DSGateType type))(Bit *, int);

/* Creates a gate with allocated inputs and assigned logic */
DSGate ds_gate_create(DSGateType type, int input_count);

/* Frees the dynamic input array of a gate */
void ds_gate_destroy(DSGate *gate);

/* Evaluates the gate's logic and stores result in gate->output */
void ds_gate_eval(DSGate *gate);

#endif