#ifndef _MATRIX_TEST_UTILS_H_
#define _MATRIX_TEST_UTILS_H_

#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <riscv_matrix.h>

#define MLAE8(REG, SRC, N) \
    asm volatile ( \
        "mlae8.m " #REG ", (%0), %1" \
        : \
        : "r"(SRC), "r"(N) \
        : \
    )

#define MLBE8(REG, SRC, N) \
    asm volatile ( \
        "mlbe8.m " #REG ", (%0), %1" \
        : \
        : "r"(SRC), "r"(N) \
        : \
    )

#define MLCE32(REG, SRC, N) \
    asm volatile ( \
        "mlce32.m " #REG ", (%0), %1" \
        : \
        : "r"(SRC), "r"(N) \
        : \
    )

#define MSCE32(REG, SRC, N) \
    asm volatile ( \
        "msce32.m " #REG ", (%0), %1" \
        : \
        : "r"(SRC), "r"(N) \
        : \
    )

#define MMA(ACC, TR0, TR2) \
    asm volatile ( \
        "mmau.mm " #ACC ", " #TR0 ", " #TR2 "\n" \
        : \
        : \
        : \
    )

static inline void test_msyncRegReset() {
  asm volatile (".word 0xc0100077"); // msyncregreset tok1
}

static inline void test_mrelease() {
  asm volatile (".word 0xc4100077"); // mrelease tok1
}

static inline void test_macquire() {
  asm volatile (".word 0x00100293"); // addi t0, zero, 1
  asm volatile (".word 0xc8128077"); // macquire t0, tok1
}

typedef _Float16 fp16_t;
typedef float fp32_t;
typedef double fp64_t;

#define DISABLE_TIME_INTR 0x100
#define NOTIFY_PROFILER 0x101
#define NOTIFY_PROFILE_EXIT 0x102
#define GOOD_TRAP 0x0

void nemu_signal(int a){
    asm volatile ("mv a0, %0\n\t"
                  ".insn r 0x6B, 0, 0, x0, x0, x0\n\t"
                  :
                  : "r"(a)
                  : "a0");
}

#define CONFIGURATION_MTYPE(hi, li)                                            \
  do {                                                                         \
    msettypei((li));                                                           \
    msettypehi((hi));                                                          \
  } while (0);

#define SET_MBA0_I8() CONFIGURATION_MTYPE(0x0, 0x10)

#define EXCEPT_SCALAR_EQ_BASE(equality, except, actual, format, name)          \
  do {                                                                         \
    test_cases++;                                                              \
    if ((equality)) {                                                          \
      pass_cases++;                                                            \
      printf("%s:%d: %s PASS!\033[0m\n", __FILE__, __LINE__, (name));          \
    } else {                                                                   \
      printf("%s:%d: %s FAIL! expect: " format " actual: " format "\n",        \
             __FILE__, __LINE__, (name), (except), (actual));                  \
    }                                                                          \
  } while (0);

#define EXCEPT_I8_SCALAR_EQ(except, actual, name)                              \
  EXCEPT_SCALAR_EQ_BASE((except) == (actual), (except), (actual), "%d", name)

#define EXCEPT_U8_SCALAR_EQ(except, actual, name)                              \
  EXCEPT_SCALAR_EQ_BASE((except) == (actual), (except), (actual), "%d", name)

#define EXCEPT_I16_SCALAR_EQ(except, actual, name)                             \
  EXCEPT_SCALAR_EQ_BASE((except) == (actual), (except), (actual), "%d", name)

#define EXCEPT_U16_SCALAR_EQ(except, actual, name)                             \
  EXCEPT_SCALAR_EQ_BASE((except) == (actual), (except), (actual), "%u", name)

#define EXCEPT_I32_SCALAR_EQ(except, actual, name)                             \
  EXCEPT_SCALAR_EQ_BASE((except) == (actual), (except), (actual), "%d", name)

#define EXCEPT_U32_SCALAR_EQ(except, actual, name)                             \
  EXCEPT_SCALAR_EQ_BASE((except) == (actual), (except), (actual), "%u", name)

#define EXCEPT_I64_SCALAR_EQ(except, actual, name)                             \
  EXCEPT_SCALAR_EQ_BASE((except) == (actual), (except), (actual), "%ld", name)

#define EXCEPT_U64_SCALAR_EQ(except, actual, name)                             \
  EXCEPT_SCALAR_EQ_BASE((except) == (actual), (except), (actual), "%lu", name)

#define EXCEPT_FP16_SCALAR_EQ(except, actual, name)                            \
  EXCEPT_SCALAR_EQ_BASE((fp16_t)(except) == (actual), (fp16_t)(except),        \
                        (fp16_t)(actual), "%f", name)

#define EXCEPT_FP32_SCALAR_EQ(except, actual, name)                            \
  EXCEPT_SCALAR_EQ_BASE((fp32_t)(except) == (actual), (fp32_t)(except),        \
                        (fp32_t)(actual), "%f", name)

#define EXCEPT_FP64_SCALAR_EQ(except, actual, name)                            \
  EXCEPT_SCALAR_EQ_BASE((fp64_t)(except) == (actual), (fp64_t)(except),        \
                        (fp64_t)(actual), "%f", name)

#define EXCEPT_FP_SCALAR_LAX_EQ(except, actual, name)                          \
  EXCEPT_SCALAR_EQ_BASE(                                                       \
      ((((except) - (actual)) < 1e-5) && (((except) - (actual)) > -1e-5)),     \
      (except), (actual), "%f", name)

#define EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, type, format, name)              \
  do {                                                                         \
    test_cases++;                                                              \
    int i;                                                                     \
    type *casted_arr1 = (type *)(arr1);                                        \
    type *casted_arr2 = (type *)(arr2);                                        \
    for (i = 0; i < (len); ++i) {                                              \
      if (casted_arr1[i] != casted_arr2[i]) {                                  \
        printf("%s:%d: %s FAIL! First differ at index %d, "                    \
               "expect: " format " actual: " format "\n",                      \
               __FILE__, __LINE__, (name), i, casted_arr1[i], casted_arr2[i]); \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (i == (len)) {                                                          \
      pass_cases++;                                                            \
      printf("%s:%d: %s PASS!\n", __FILE__, __LINE__, (name));                 \
    }                                                                          \
  } while (0);

#define EXCEPT_U8_ARRAY_EQ(arr1, arr2, len, name)                              \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, uint8_t, "%d", name)

#define EXCEPT_I8_ARRAY_EQ(arr1, arr2, len, name)                              \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, int8_t, "%d", name)

#define EXCEPT_U16_ARRAY_EQ(arr1, arr2, len, name)                             \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, uint16_t, "%u", name)

#define EXCEPT_I16_ARRAY_EQ(arr1, arr2, len, name)                             \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, int16_t, "%d", name)

#define EXCEPT_U32_ARRAY_EQ(arr1, arr2, len, name)                             \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, uint32_t, "%u", name)

#define EXCEPT_I32_ARRAY_EQ(arr1, arr2, len, name)                             \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, int32_t, "%d", name)

#define EXCEPT_U64_ARRAY_EQ(arr1, arr2, len, name)                             \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, uint64_t, "%lu", name)

#define EXCEPT_I64_ARRAY_EQ(arr1, arr2, len, name)                             \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, int64_t, "%ld", name)

#define EXCEPT_FP16_ARRAY_EQ(arr1, arr2, len, name)                            \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, fp16_t, "%f", name)

#define EXCEPT_FP32_ARRAY_EQ(arr1, arr2, len, name)                            \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, fp32_t, "%f", name)

#define EXCEPT_FP64_ARRAY_EQ(arr1, arr2, len, name)                            \
  EXCEPT_ARRAY_EQ_BASE(arr1, arr2, len, fp64_t, "%f", name)

#define EXCEPT_ARRAY_LAX_EQ_BASE(arr1, arr2, len, type, format, name)          \
  do {                                                                         \
    test_cases++;                                                              \
    int i;                                                                     \
    type *casted_arr1 = (type *)(arr1);                                        \
    type *casted_arr2 = (type *)(arr2);                                        \
    for (i = 0; i < (len); ++i) {                                              \
      if ((casted_arr1[i] - casted_arr2[i] > 1e-1) ||                          \
          (casted_arr1[i] - casted_arr2[i] < -1e-1)) {                         \
        printf("%s:%d: %s FAIL! First differ at index %d: "                    \
               "expect: " format " actual: " format "\n",                      \
               __FILE__, __LINE__, (name), i, casted_arr1[i], casted_arr2[i]); \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    if (i == (len)) {                                                          \
      pass_cases++;                                                            \
      printf("%s:%d: %s PASS!\n", __FILE__, __LINE__, (name));                 \
    }                                                                          \
  } while (0);

#define EXCEPT_FP16_ARRAY_LAX_EQ(arr1, arr2, len, name)                        \
  EXCEPT_ARRAY_LAX_EQ_BASE(arr1, arr2, len, fp16_t, "%f", name)

#define EXCEPT_FP32_ARRAY_LAX_EQ(arr1, arr2, len, name)                        \
  EXCEPT_ARRAY_LAX_EQ_BASE(arr1, arr2, len, fp32_t, "%f", name)

#define EXCEPT_FP64_ARRAY_LAX_EQ(arr1, arr2, len, name)                        \
  EXCEPT_ARRAY_LAX_EQ_BASE(arr1, arr2, len, fp64_t, "%f", name)

#define DUMP_MATRIX(arr, row, col, format)                                     \
  do {                                                                         \
    for (int i = 0; i < (row); ++i) {                                          \
      for (int j = 0; j < (col); ++j) {                                        \
        printf(format " ", (arr)[i * (col) + j]);                              \
      }                                                                        \
      printf("\n");                                                            \
    }                                                                          \
  } while (0);

#endif // !_MATRIX_TEST_UTILS_H_
