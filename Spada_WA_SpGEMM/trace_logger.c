// trace_logger.c
#include <stdio.h>
#include <stdlib.h>
#define N 4

double A[N][N], B[N][N], C[N][N];

int main() {
    FILE *ftrace = fopen("trace.txt", "w");
    FILE *frows  = fopen("a_row_index.txt", "w");
    if (!ftrace || !frows) {
        perror("file open failed");
        return 1;
    }

    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
            C[i][j] = 0.0;
        }

    for (int i=0; i<N; i++) {
        for (int k=0; k<N; k++) {
            double a = A[i][k];
            fprintf(ftrace, "r %p\n", (void*)&A[i][k]);
            fprintf(frows, "%d\n", i);

            for (int j=0; j<N; j++) {
                fprintf(ftrace, "r %p\n", (void*)&B[k][j]);
                fprintf(frows, "%d\n", i);

                C[i][j] += a * B[k][j];

                fprintf(ftrace, "w %p\n", (void*)&C[i][j]);
                fprintf(frows, "%d\n", i);
            }
        }
    }

    fclose(ftrace);
    fclose(frows);
    printf("✅ Trace and A-row index generated.\n");
    return 0;
}
