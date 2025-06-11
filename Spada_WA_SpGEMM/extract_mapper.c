// extract_mapper.c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main() {
    FILE *ftrace = fopen("trace.txt", "r");
    FILE *frows  = fopen("a_row_index.txt", "r");
    FILE *fmap   = fopen("addr_to_arow.map", "w");

    if (!ftrace || !frows || !fmap) {
        perror("file open error");
        return 1;
    }

    char op;
    void *addr;
    int arow;

    while (fscanf(ftrace, "%c %p\n", &op, &addr) == 2 &&
           fscanf(frows, "%d\n", &arow) == 1) {
        fprintf(fmap, "%p %d\n", addr, arow);
    }

    fclose(ftrace);
    fclose(frows);
    fclose(fmap);

    printf("✅ addr_to_arow.map generated.\n");
    return 0;
}
