#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
    FILE *trace = fopen("trace.txt", "r");
    FILE *arows = fopen("a_row_index.txt", "r");
    FILE *map   = fopen("addr_to_arow.map", "w");

    if (!trace || !arows || !map) {
        perror("file open");
        return 1;
    }

    char op;
    uint64_t addr;
    int a_row;

    while (fscanf(trace, "%c 0x%lx\n", &op, &addr) == 2 &&
           fscanf(arows, "%d\n", &a_row) == 1)
    {
        fprintf(map, "0x%lx %d\n", addr, a_row);
    }

    fclose(trace);
    fclose(arows);
    fclose(map);

    printf("Generated addr_to_arow.map\n");
    return 0;
}
