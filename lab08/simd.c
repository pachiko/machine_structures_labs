#include <time.h>
#include <stdio.h>
#include <x86intrin.h>
#include "simd.h"

long long int sum(int vals[NUM_ELEMS]) {
    clock_t start = clock();

    long long int sum = 0;
    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS; i++) {
            if(vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    long long int sum = 0;

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
            if(vals[i] >= 128) sum += vals[i];
            if(vals[i + 1] >= 128) sum += vals[i + 1];
            if(vals[i + 2] >= 128) sum += vals[i + 2];
            if(vals[i + 3] >= 128) sum += vals[i + 3];
        }

        // TAIL CASE, for when NUM_ELEMS isn't a multiple of 4
        // NUM_ELEMS / 4 * 4 is the largest multiple of 4 less than NUM_ELEMS
        // Order is important, since (NUM_ELEMS / 4) effectively rounds down first
        // Can also be N - N % K
        for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
            if (vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_simd(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127); // This is a vector with 127s in it... Why might you need this? Comparator!
    long long int result = 0; // This is where you should put your final result!
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    int mask[4];
    for (int i = 0; i < 4; i++) mask[i] = (i < NUM_ELEMS % 4) ? 0xffffffff : 0x0;
    __m128i _remainder = _mm_loadu_si128((__m128i *) mask);

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */
        __m128i sum_vec = _mm_setzero_si128();

        unsigned int i;
        for(i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
            __m128i tmp = _mm_loadu_si128((__m128i *) (vals + i));
            __m128i gt = _mm_cmpgt_epi32(tmp, _127);
            tmp = _mm_and_si128(tmp, gt);
            sum_vec = _mm_add_epi32(sum_vec, tmp);
        }

        /* Hint: you'll need a tail case. */
        __m128i tmp = _mm_loadu_si128((__m128i *) (vals + i));
        __m128i gt = _mm_cmpgt_epi32(tmp, _127);
        tmp = _mm_and_si128(tmp, gt);
        tmp = _mm_and_si128(tmp, _remainder);
        sum_vec = _mm_add_epi32(sum_vec, tmp);

        int tmp_arr[4];
        _mm_storeu_si128((__m128i *) tmp_arr, sum_vec);
        result += tmp_arr[0] + tmp_arr[1] + tmp_arr[2] + tmp_arr[3];
    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}

long long int sum_simd_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127);
    long long int result = 0;
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    int mask[4];
    for (int i = 0; i < 4; i++) mask[i] = (i < NUM_ELEMS % 4) ? 0xffffffff : 0x0;
    __m128i _remainder = _mm_loadu_si128((__m128i *) mask);

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */
        /* Copy your sum_simd() implementation here, and unroll it */

        __m128i sum_vec = _mm_setzero_si128();

        unsigned int i;
        for(i = 0; i < NUM_ELEMS / 16 * 16; i += 16) {
            __m128i tmp = _mm_loadu_si128((__m128i *) (vals + i));
            __m128i gt = _mm_cmpgt_epi32(tmp, _127);
            tmp = _mm_and_si128(tmp, gt);
            sum_vec = _mm_add_epi32(sum_vec, tmp);

            tmp = _mm_loadu_si128((__m128i *) (vals + i + 4));
            gt = _mm_cmpgt_epi32(tmp, _127);
            tmp = _mm_and_si128(tmp, gt);
            sum_vec = _mm_add_epi32(sum_vec, tmp);

            tmp = _mm_loadu_si128((__m128i *) (vals + i + 8));
            gt = _mm_cmpgt_epi32(tmp, _127);
            tmp = _mm_and_si128(tmp, gt);
            sum_vec = _mm_add_epi32(sum_vec, tmp);

            tmp = _mm_loadu_si128((__m128i *) (vals + i + 12));
            gt = _mm_cmpgt_epi32(tmp, _127);
            tmp = _mm_and_si128(tmp, gt);
            sum_vec = _mm_add_epi32(sum_vec, tmp);
        }

        /* Hint: you'll need 1 or maybe 2 tail cases here. */
        for(; i < NUM_ELEMS / 4 * 4; i += 4) {
            __m128i tmp = _mm_loadu_si128((__m128i *) (vals + i));
            __m128i gt = _mm_cmpgt_epi32(tmp, _127);
            tmp = _mm_and_si128(tmp, gt);
            sum_vec = _mm_add_epi32(sum_vec, tmp);
        }

        __m128i tmp = _mm_loadu_si128((__m128i *) (vals + i));
        __m128i gt = _mm_cmpgt_epi32(tmp, _127);
        tmp = _mm_and_si128(tmp, gt);
        tmp = _mm_and_si128(tmp, _remainder);
        sum_vec = _mm_add_epi32(sum_vec, tmp);

        int tmp_arr[4];
        _mm_storeu_si128((__m128i *) tmp_arr, sum_vec);
        result += tmp_arr[0] + tmp_arr[1] + tmp_arr[2] + tmp_arr[3]; 
    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}
