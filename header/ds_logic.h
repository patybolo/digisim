#ifndef DS_LOGIC_H
#define DS_LOGIC_H

typedef enum {
    NOT = 0xff00,
    AND,
    NAND,
    OR,
    NOR,
    XOR,
    XNOR,
} gate_t;

#endif