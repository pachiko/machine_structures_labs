#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

void transpose_blocking_helper(int sX, int nX, int sY, int nY, int n, int *dst, int *src) {
    int eX = sX + nX;
    int eY = sY + nY;

    for (int x = sX; x < eX; x++) {
        for (int y = sY; y < eY; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src) {
    for (int i = 0; i < n; i += blocksize) {
        for (int j = 0; j < n; j+= blocksize) {
            int ni = (i + blocksize < n) ? blocksize : n - i;
            int nj = (j + blocksize < n) ? blocksize : n - j;
            transpose_blocking_helper(i, ni, j, nj, n, dst, src);
        }
    }
}
