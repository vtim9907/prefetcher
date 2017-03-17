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

/* provide the implementations of naive_transpose,
 * sse_transpose, sse_prefetch_transpose
 */

#include "impl.c"

typedef struct {
    void (*transpose)(int *src, int *dst, int w, int h);
} transposeClass;

#ifdef NAIVE
void naive_init (transposeClass *naive)
{
    naive -> transpose = naive_transpose;
}
#endif

#ifdef SSE
void sse_init (transposeClass *sse)
{
    sse -> transpose = sse_transpose;
}
#endif

#ifdef SSE_PREFETCH
void sse_prefetch_init (transposeClass *sse_prefetch)
{
    sse_prefetch -> transpose = sse_prefetch_transpose;
}
#endif

#ifdef AVX
void avx_init (transposeClass *avx)
{
    avx -> transpose = avx_transpose;
}
#endif

#ifdef AVX_PREFETCH
void avx_prefetch_init (transposeClass *avx_prefetch)
{
    avx_prefetch -> transpose = avx_prefetch_transpose;
}
#endif

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

int main()
{
    /* create a handler to handle a method of transpose */
    transposeClass *transpose_handler = malloc(sizeof(transposeClass));

#ifdef NAIVE
    naive_init(transpose_handler);
#endif
#ifdef SSE
    sse_init(transpose_handler);
#endif
#ifdef SSE_PREFETCH
    sse_prefetch_init(transpose_handler);
#endif
#ifdef AVX
    avx_init(transpose_handler);
#endif
#ifdef AVX_PREFETCH
    avx_prefetch_init(transpose_handler);
#endif

    /* verify the result of 4x4 matrix */
    {
        int testin[16] = { 0, 1,  2,  3,  4,  5,  6,  7,
                           8, 9, 10, 11, 12, 13, 14, 15
                         };
        int testout[16];
        int expected[16] = { 0, 4,  8, 12, 1, 5,  9, 13,
                             2, 6, 10, 14, 3, 7, 11, 15
                           };

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++)
                printf(" %2d", testin[y * 4 + x]);
            printf("\n");
        }
        printf("\n");
        sse_transpose(testin, testout, 4, 4);
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++)
                printf(" %2d", testout[y * 4 + x]);
            printf("\n");
        }
        assert(0 == memcmp(testout, expected, 16 * sizeof(int)) &&
               "Verification fails");
    }

    {
        struct timespec start, end;
        int *src  = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out = (int *) malloc(sizeof(int) * TEST_W * TEST_H);

        srand(time(NULL));
        for (int y = 0; y < TEST_H; y++)
            for (int x = 0; x < TEST_W; x++)
                *(src + y * TEST_W + x) = rand();

        clock_gettime(CLOCK_REALTIME, &start);
        transpose_handler -> transpose(src, out, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);

#ifdef NAIVE
        printf("naive: \t\t %ld us\n", diff_in_us(start, end));
#endif
#ifdef SSE
        printf("sse: \t\t %ld us\n", diff_in_us(start, end));
#endif
#ifdef SSE_PREFETCH
        printf("sse prefetch: \t %ld us\n", diff_in_us(start, end));
#endif
#ifdef AVX
        printf("avx: \t\t %ld us\n", diff_in_us(start, end));
#endif
#ifdef AVX_PREFETCH
        printf("avx prefetch: \t %ld us\n", diff_in_us(start, end));
#endif

        free(src);
        free(out);
    }

    return 0;
}
