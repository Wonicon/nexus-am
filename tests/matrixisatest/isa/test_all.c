#include "mma.h"
#include "data_move.h"
#include "load_store.h"
#include "eletwise.h"
#include "cvt.h"
#include "zmv.h"
#include "zmi2c.h"

int main() {
  asm volatile (
    "lui a0, 0x2002\n"
    "addiw a0, a0, 512\n"
    "csrs mstatus, a0"::
  );

  // test_load_store();
  // test_data_move();
  test_matmul();
  // test_eletwise();
  // test_cvt();
  // test_zmv();
  // test_zmi2c();
  printf("[pass/total]: [%d/%d]\n", pass_cases, test_cases);
  return 0;
}
