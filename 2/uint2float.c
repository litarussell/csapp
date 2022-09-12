#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

uint32_t uint2float(uint32_t u) {
    if (u == 0x00000000) {
        return 0x00000000;
    }

    int n = 31;

    // 除去符号位, 从最高位开始找到第一个1所在的位置
    while (0 <= n && (((u >> n) & 0x1) == 0x0)) {
        n = n - 1;
    }

    uint32_t f, e;

    // u <= 0000 0000 1111 1111 1111 1111 1111 1111
    //      seee eeee efff ffff ffff ffff ffff ffff
    // 阶码最多只有一个1
    if (u <= 0x00ffffff) {
        // 不需要近似
        uint32_t mask = 0xffffffff >> (32 - n);
        f = (u & mask) << (23 - n);
        e = n + 127;
        return (e << 23) | f;
    } else {
        // u >= 0000 0001 0000 0000 0000 0000 0000 0000
        // 需要近似
        uint64_t a = 0;
        a += u;

        // 计算 G R S
        uint32_t g = (a >> (n - 23)) & 0x1;
        uint32_t r = (a >> (n - 24)) & 0x1;
        uint32_t s = 0x0;

        for (int j = 0; j < n - 24; j++) {
            s = s | ((u >> j) & 0x1);
        }

        // 计算进位
        a = a >> (n - 23);
        // 0    1    ?    ... ?
        // [24] [23] [22] ... [0]
        /* Rounding Rules
            +-------+-------+-------+-------+
            |   G   |   R   |   S   |       |
            +-------+-------+-------+-------+
            |   0   |   0   |   0   |   +0  | round down
            |   0   |   0   |   1   |   +0  | round down
            |   0   |   1   |   0   |   +0  | round down
            |   0   |   1   |   1   |   +1  | round up
            |   1   |   0   |   0   |   +0  | round down
            |   1   |   0   |   1   |   +0  | round down
            |   1   |   1   |   0   |   +1  | round up
            |   1   |   1   |   1   |   +1  | round up
            +-------+-------+-------+-------+
        carry = R & (G | S) by K-Map
        */
       
        if (r & (g | s) == 0x1) {
            a = a + 1;
        }

        // 检查进位
        if ((a >> 23) == 0x1) {
            // 0    1    ?    ... ?
            // [24] [23] [22] ... [0]
            f = a & 0x007fffff;
            e = n + 127;
            return (e << 23) | f;
        }
        else if ((a >> 23) == 0x2) {
            // 此时说明尾数全为1, 进位导致23位尾数全为0
            // 1    0    0    ... 0
            // [24] [23] [22] ... [0]
            e = n + 1 + 127;
            return (e << 23);
        }

        // 无穷
        return 0x7f800000;
    }
}

int main() {
    uint32_t u = 0x000fffff;

    uint32_t f = uint2float(u);

    float* f1 = &f;
    printf("f: %f\n", *f1);

    return 0;
}
