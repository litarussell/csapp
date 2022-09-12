#include <stdio.h>

#include "cpu/register.h"
#include "memory/instruction.h"

int main() {
    reg.rax = 0x1234abcd5678ffaa;

    printf("eax: %08x\n", reg.eax);
    printf("ax: %04x\n", reg.ax);
    printf("al: %02x\n", reg.al);
    printf("ah: %02x\n", reg.ah);

    return 0;
}
