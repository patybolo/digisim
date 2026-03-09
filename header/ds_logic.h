#ifndef DS_LOGIC_H
#define DS_LOGIC_H

#include <stdbool.h>

typedef bool bit;

typedef enum {
    NOT,
    AND,
    NAND,
    OR,
    NOR,
    XOR,
    XNOR,
} DSGateType;

typedef struct __attribute__((aligned(64))){
    DSGateType type;
    int  input_channels;
    int  output_channels;
    // inputs is a dynamic array, having input_channels as the size
    bit *inputs;
    bit  output;
    /*
        TODO: check if type is correct whenever i implement the gates logic
        gate_logic is a function pointer to the logic implementation for this type of gate
    */
    void (*gate_logic)(bit *);
} DSGate;


#endif