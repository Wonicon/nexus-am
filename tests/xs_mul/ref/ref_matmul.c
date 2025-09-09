#include <inttypes.h>
#include <stdio.h>

#define NUM_CHANNELS 8

#define ALIGNMENT (NUM_CHANNELS * 64)

#define A_STRIDE 576
//#define B_STRIDE 64
#define B_STRIDE 576
#define C_STRIDE 1152

enum {
  MM = 256,
  KK = 512,
  NN = 256
};

// Init in compilation to avoid memset emu cost.
static int8_t aa[MM * A_STRIDE] __attribute__((aligned(ALIGNMENT))) =
{
  #include "aa_data.h"
};
static int8_t bb[NN * B_STRIDE] __attribute__((aligned(ALIGNMENT))) =
{
  #include "bb_data.h"
};
static int32_t cc[MM * C_STRIDE / 4] __attribute__((aligned(ALIGNMENT))) =
{
  #include "cc_data.h"
};

static int calc_cnt = 0;

void tile_matmul(int32_t *acc, int8_t *a, int8_t *b)
{
  for (int m = 0; m < MM; m++) {
    for (int n = 0; n < NN; n++) {
      int32_t sum = 0;
      for (int k = 0; k < KK; k++) {
        int8_t elem_a = a[m * A_STRIDE + k];
        int8_t elem_b = b[n * B_STRIDE + k];
        sum += elem_a * elem_b;
      }
      acc[m * C_STRIDE / 4 + n] += sum;
    }
  }

  calc_cnt++;
}

int main()
{
  tile_matmul(cc, aa, bb);

  for (int m = 0; m < MM; m++) {
    for (int n = 0; n < C_STRIDE / sizeof(int32_t); n++) {
      int32_t e = cc[m * C_STRIDE / sizeof(int32_t) + n];
      if (n < NN) {
        printf("0x%x", e);
      } else {
        printf("0x0");
      }

      if (n < (C_STRIDE / sizeof(int32_t)) - 1) {
        printf(", ");
      }
    }
    if (m < MM - 1) {
      printf(",\n");
    }
  }
  return 0;
}
