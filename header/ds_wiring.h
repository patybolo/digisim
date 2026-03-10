#ifndef DS_WIRING_H
#define DS_WIRING_H

/* A wire connects one gate's output to another gate's input */
typedef struct {
    int src_gate;   /* index of source gate in the state array */
    int dst_gate;   /* index of destination gate */
    int dst_input;  /* which input slot of the destination */
} DSWire;

#endif