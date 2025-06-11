// spada_wadf_final.c
//
// Optimized Window-Based Adaptive Dataflow (WA) SpGEMM in C.
// Compile with: gcc -O2 -o spada_wadf_final spada_wadf_final.c
// Usage: adjust ALPHA and BETA constants and run; prints C matrix.

#include <stdio.h>
#include <stdlib.h>

// CSR sparse‐matrix structure
typedef struct {
    int rows, cols;
    int nnz;        // number of nonzeros
    int *row_ptr;   // size rows+1
    int *col_idx;   // size nnz
    float *vals;    // size nnz
} CSRMatrix;

// --- Allocate/free helpers ---
CSRMatrix *alloc_csr(int rows, int cols, int nnz) {
    CSRMatrix *M = malloc(sizeof(*M));
    M->rows = rows; M->cols = cols; M->nnz = nnz;
    M->row_ptr = calloc(rows+1, sizeof(int));
    M->col_idx = malloc(nnz * sizeof(int));
    M->vals    = malloc(nnz * sizeof(float));
    return M;
}

void free_csr(CSRMatrix *M) {
    free(M->row_ptr);
    free(M->col_idx);
    free(M->vals);
    free(M);
}

// --- Example matrices (replace with your loader) ---
CSRMatrix *make_example_A() {
    // A = [1 0 2; 0 3 4; 5 0 0]
    CSRMatrix *A = alloc_csr(3,3,5);
    int rp[4]={0,2,4,5}, ci[5]={0,2,1,2,0};
    float v[5]={1,2,3,4,5};
    for(int i=0;i<4;i++) A->row_ptr[i]=rp[i];
    for(int i=0;i<5;i++){A->col_idx[i]=ci[i]; A->vals[i]=v[i];}
    return A;
}

CSRMatrix *make_example_B() {
    // B = [7 0 9; 0 8 0; 10 0 0]
    CSRMatrix *B = alloc_csr(3,3,4);
    int rp[4]={0,2,3,4}, ci[4]={0,2,1,0};
    float v[4]={7,9,8,10};
    for(int i=0;i<4;i++) B->row_ptr[i]=rp[i];
    for(int i=0;i<4;i++){B->col_idx[i]=ci[i]; B->vals[i]=v[i];}
    return B;
}

// --- WA SpGEMM core ---
// α = win_h (window height), β = win_w (window width)
float *wadf_spgemm(const CSRMatrix *A, const CSRMatrix *B,
                   int alpha, int beta)
{
    int M=A->rows, K=A->cols, N=B->cols;
    // Dense output C[M][N]
    float *C = calloc(M*N, sizeof(float));

    int Mo = (M + alpha - 1)/alpha;
    int Ko = (K + beta  - 1)/beta;

    // Temporary storage for window nonzeros
    // Upper bound: alpha * (avg nnz per row)
    int max_window_nnz = alpha * ((A->nnz + M -1)/M + 1);
    int *win_m = malloc(max_window_nnz * sizeof(int));
    int *win_k = malloc(max_window_nnz * sizeof(int));
    float *win_v = malloc(max_window_nnz * sizeof(float));

    for(int mo=0; mo<Mo; ++mo) {
        int m0 = mo*alpha;
        int m1 = m0 + alpha < M ? m0+alpha : M;

        for(int ko=0; ko<Ko; ++ko) {
            int k0 = ko*beta;
            int k1 = k0 + beta < K ? k0+beta : K;

            // 1) Gather all (m,k,val) in this window
            int cnt=0;
            for(int m=m0; m<m1; ++m) {
                for(int p=A->row_ptr[m]; p<A->row_ptr[m+1]; ++p) {
                    int k = A->col_idx[p];
                    if(k>=k0 && k<k1) {
                        win_m[cnt]=m;
                        win_k[cnt]=k;
                        win_v[cnt]=A->vals[p];
                        cnt++;
                    }
                }
            }
            // 2) For each element in window, multiply with B-row
            for(int t=0; t<cnt; ++t) {
                int m = win_m[t], k = win_k[t];
                float aval = win_v[t];
                int bstart=B->row_ptr[k], bend=B->row_ptr[k+1];
                for(int q=bstart; q<bend; ++q) {
                    int j = B->col_idx[q];
                    float bval = B->vals[q];
                    C[m*N + j] += aval * bval;
                }
            }
        }
    }

    free(win_m);
    free(win_k);
    free(win_v);
    return C;
}

// --- Print C as dense ---
void print_C(float *C, int M, int N) {
    printf("C =\n");
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
            printf("%8.2f ", C[i*N+j]);
        }
        printf("\n");
    }
}

// --- Main ---
int main(){
    CSRMatrix *A = make_example_A();
    CSRMatrix *B = make_example_B();

    // Try different window shapes
    int alpha = 2, beta = 1;  // e.g., α=2,β=1 for outer-product style
    float *C = wadf_spgemm(A,B,alpha,beta);

    print_C(C, A->rows, B->cols);

    free(C);
    free_csr(A);
    free_csr(B);
    return 0;
}
