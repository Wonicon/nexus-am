# Test matrix multiplication with szie larger than the AME tile size
# A, B: I8, M 256, N 256, K 512
# AB_Stride 576
# B is pre-transposed
# C: I32, M 256, N 256
# C_Stride: 256 * 4B / 64B = 16
# C_Stride must be greater than 16 * 64B, set to 18 * 64B = 1152B,
# with 288 - 256 = 32 extra padding elements
SCRIPT_DIR=$(realpath $(dirname "$0"))
python $SCRIPT_DIR/gen_data.py 256 512 576 1 a >aa_data.h
python $SCRIPT_DIR/gen_data.py 256 512 576 1 b >bb_data.h
python $SCRIPT_DIR/gen_data.py 256 256 1152 4 c >cc_data.h
