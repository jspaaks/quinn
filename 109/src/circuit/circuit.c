#include "circuit/circuit.h"
#include <stdio.h>
#include <stdint.h>

bool circuit_check (int irank, uint16_t input16) {

    uint16_t mask = 0;
    bool bits[16];
    for (size_t ibit = 0; ibit < 16; ibit++) {
        mask = 1 << ibit;
        bits[ibit] = (input16 & mask) > 0;
    }

    bool issatisfied =
        ( bits[0]  ||  bits[1] ) &&
        (!bits[1]  || !bits[3] ) &&
        ( bits[2]  ||  bits[3] ) &&
        (!bits[3]  || !bits[4] ) &&
        ( bits[4]  || !bits[5] ) &&
        ( bits[5]  || !bits[6] ) &&
        ( bits[5]  ||  bits[6] ) &&
        ( bits[6]  || !bits[15]) && 
        ( bits[7]  || !bits[8] ) &&
        (!bits[7]  || !bits[13]) &&
        ( bits[8]  ||  bits[9] ) &&
        ( bits[8]  || !bits[9] ) &&
        (!bits[9]  || !bits[10]) &&
        ( bits[9]  ||  bits[11]) &&
        ( bits[10] ||  bits[11]) &&
        ( bits[12] ||  bits[13]) &&
        ( bits[13] || !bits[14]) &&
        ( bits[14] ||  bits[15]);  // see the circuit on p. 97

    if (issatisfied) {
        fprintf(stdout, "%d) %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d (%u)\n",
            irank,
            bits[0], bits[1], bits[2], bits[3],
            bits[4], bits[5], bits[6], bits[7],
            bits[8], bits[9], bits[10], bits[11],
            bits[12], bits[13], bits[14], bits[15],
            input16);
        fflush(stdout);
    }
    return issatisfied;
}
