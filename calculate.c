#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#include <xmmintrin.h>
#include <immintrin.h>

#define TEST_W 4096
#define TEST_H 4096

#include "impl.c"

static long diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

int main(void)
{
    struct timespec start, end;
    srand(time(NULL));

    for (int i = 0; i < 100; ++i) {
        int *src  = (int *) malloc(sizeof(int) * (TEST_W + i * 96) * (TEST_H + i * 96));
        int *out = (int *) malloc(sizeof(int) * (TEST_W + i * 96) * (TEST_H + i * 96));

        for (int y = 0; y < TEST_H; y++)
            for (int x = 0; x < TEST_W; x++)
                *(src + y * TEST_W + x) = rand();

        printf("%d, ", TEST_W + i * 96);

        clock_gettime(CLOCK_REALTIME, &start);
        naive_transpose(src, out, TEST_W + i * 96, TEST_H + i * 96);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld, ", diff_in_us(start, end));

        clock_gettime(CLOCK_REALTIME, &start);
        sse_transpose(src, out, TEST_W + i * 96, TEST_H + i * 96);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld, ", diff_in_us(start, end));

        clock_gettime(CLOCK_REALTIME, &start);
        sse_prefetch_transpose(src, out, TEST_W + i * 96, TEST_H + i * 96);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld, ", diff_in_us(start, end));

        clock_gettime(CLOCK_REALTIME, &start);
        avx_transpose(src, out, TEST_W + i * 96, TEST_H + i * 96);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld, ", diff_in_us(start, end));

        clock_gettime(CLOCK_REALTIME, &start);
        avx_prefetch_transpose(src, out, TEST_W + i * 96, TEST_H + i * 96);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld\n", diff_in_us(start, end));

        free(src);
        free(out);
    }

    return 0;
}
