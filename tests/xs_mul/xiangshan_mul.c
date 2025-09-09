#include "utils.h"
#include <riscv_matrix.h>
#include <klib.h>

// Rows of matrix A
#define M 256
// Columns of matrix A and rows of matrix B
#define K 512
// Columns of matrix B
#define N 512

#define AB_STRIDE 576
#define C_STRIDE 1152

// these are actually set in NEMU-Matrix/src/isa/riscv64/instr/rvmatrix/mreg.h
// unused
#define tmmax 64
#define tkmax 256
#define tnmax 64

#define L2_Banks 8
#define ALIGNMENT (L2_Banks * 64)

// __attribute__((aligned(64))) int8_t A[M_padding][K_padding]; // Matrix A
// __attribute__((aligned(64))) int8_t B[K_padding][N_padding]; // Matrix B
// __attribute__((aligned(64))) int8_t C[M_padding][N_padding]; // Result matrix C

static __attribute__((aligned(ALIGNMENT))) int8_t A[M * AB_STRIDE] = {
  #include "aa_data.h"
}; // Matrix A

static __attribute__((aligned(ALIGNMENT))) int8_t B[N * AB_STRIDE] = {
  #include "bb_data.h"
}; // Matrix B

static __attribute__((aligned(ALIGNMENT))) uint32_t C[M * C_STRIDE / 4] = {
  #include "cc_data.h"
}; // Result matrix C

static const uint32_t ANS[M * C_STRIDE / sizeof(uint32_t)] = {
  #include "ans_data.h"
};

// In XiangShan EMU environment, printing has a significant impact on cycles.
// Therefore we suggest enable printing only on checking stage.
// #define _printf(x, ...)
#define _printf(...) printf(__VA_ARGS__)

static __attribute__((noinline)) void print_matrix_result(uint32_t *c, uint32_t *ans)
{
  // Check tile result
  int index = 0;
  while (index != M * C_STRIDE) {
    int row = index / C_STRIDE;
    int col = index % C_STRIDE;

    // Jump padding area
    if (col >= N * sizeof(uint32_t)) {
      index += (C_STRIDE - col);
      assert(index % C_STRIDE == 0);
      continue;
    }

#define get_elem_ptr(arr, r, c) &arr[(r * C_STRIDE + c) / sizeof(uint32_t)]
    uint32_t curr = *get_elem_ptr(c, row, col);
    uint32_t curr_ans = *get_elem_ptr(ans, row, col);
    if (curr_ans != curr) {
      printf("[x][%02d,%02d]: expect 0x%x get 0x%x row_addr %p elem_addr %p\n", row, col / sizeof(uint32_t), curr_ans, curr, get_elem_ptr(c, row, 0), get_elem_ptr(c, row, col));
    }

    index += sizeof(uint32_t);
    // index += C_STRIDE;
  }
#undef get_elem_ptr
}

void __attribute__((noinline)) test_xiangshan_mm() {
  int tile_m = 0, tile_k = 0, tile_n = 0;
  //msettype(E8, M1, BA);
  SET_MBA0_I8();
  tile_m = msettilem(tmmax);
  tile_n = msettilen(tnmax);
  tile_k = msettilek(tkmax);
  _printf("msettile m %d n %d k %d\n", tile_m, tile_n, tile_k);

  // Initialize matrices A and B with some values (for testing)
  test_msyncRegReset();

  // Perform block matrix multiplication
  _printf("Mat mul:\n");
  uint8_t *C_i8 = (uint8_t *)C;
  for (int m = 0; m < M; m += tile_m * 2) {
    for (int n = 0; n < N; n += tile_n * 2) {
      MLCE32(acc0, &C_i8[m * C_STRIDE + n * sizeof(uint32_t)], C_STRIDE);
      if (n + tile_n < N) {
        MLCE32(acc1, &C_i8[m * C_STRIDE + (n + tile_n) * sizeof(uint32_t)], C_STRIDE);
      }
      if (m + tile_m < M) {
        MLCE32(acc2, &C_i8[(m + tile_m) * C_STRIDE + n * sizeof(uint32_t)], C_STRIDE);
      }
      if ((n + tile_n < N) && (m + tile_m < M)) {
        MLCE32(acc3, &C_i8[(m + tile_m) * C_STRIDE + (n + tile_n) * sizeof(uint32_t)], C_STRIDE);
      }
      for (int k = 0; k < K; k += tile_k) {
        MLAE8(tr0, &A[m * AB_STRIDE + k], AB_STRIDE);
        MLBE8(tr1, &B[n * AB_STRIDE + k], AB_STRIDE);
        MMA(acc0, tr0, tr1);

        if (n + tile_n < N) {
          MLBE8(tr2, &B[(n + tile_n) * AB_STRIDE + k], AB_STRIDE);
          MMA(acc1, tr0, tr2);
        }

        if (m + tile_m < M) {
          MLAE8(tr3, &A[(m + tile_n) * AB_STRIDE + k], AB_STRIDE);
          MMA(acc2, tr3, tr1);
        }

        if ((m + tile_m < M) && (n + tile_n < N)) {
          MMA(acc3, tr3, tr2);
        }
      }

      MSCE32(acc0, &C_i8[m * C_STRIDE + n * sizeof(uint32_t)], C_STRIDE);
      if (n + tile_n < N) MSCE32(acc1, &C_i8[m * C_STRIDE + (n + tile_n) * sizeof(uint32_t)], C_STRIDE);
      if (m + tile_m < M) MSCE32(acc2, &C_i8[(m + tile_m) * C_STRIDE + (n) * sizeof(uint32_t)], C_STRIDE);
      if (m + tile_m < M && n + tile_n < N) MSCE32(acc3, &C_i8[(m + tile_m) * C_STRIDE + (n + tile_n) * sizeof(uint32_t)], C_STRIDE);
    }
  }
  test_mrelease();
  _printf("Finish issuing matrix instructions, waiting for release\n");
  test_macquire();

  // Check result matrix C
  _printf("Check result (only mismatched elements):\n");
  print_matrix_result(C, ANS);
}

int main() {
  // Enable matrix
  asm volatile (
    "lui t0, 0x2002\n"
    "addiw t0, t0, 512\n"
    "csrs mstatus, t0"::
  );

  _printf("Hello, RISC-V World!\n");
  test_xiangshan_mm();
  _printf("Matrix Multiplication Test Done\n");
  nemu_signal(GOOD_TRAP);
  return 0;
}
