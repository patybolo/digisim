#ifndef DS_STATE_H
#define DS_STATE_H

#include "ds_logic.h"

typedef struct __attribute__((aligned(64))) {
    DSGate *gates;
    
} DSState;

#endif