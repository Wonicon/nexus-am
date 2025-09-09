# Update ABC data

Back up original aa_data.h, bb_data.h, cc_data.h.

Generate new random A, B, C matrix data:

```sh
bash scripts/gen_all.sh
```

# Build

```sh
bash build.sh
```

# Note

Keep M, N, K and A, B, C stride consistent across source files and scripts.
