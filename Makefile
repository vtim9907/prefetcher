CC ?= gcc
CFLAGS = -msse2 -mavx2 --std gnu99 -O0 -Wall -Wextra

EXEC = naive_transpose sse_transpose sse_prefetch_transpose avx_transpose avx_prefetch_transpose test_time

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

SRCS_common = main.c

test_time: calculate.c
	$(CC) $(CFLAGS) -o $@ calculate.c

naive_transpose: main.c
	$(CC) $(CFLAGS) -DNAIVE -o $@ $(SRCS_common)

sse_transpose: main.c
	$(CC) $(CFLAGS) -DSSE -o $@ $(SRCS_common)

sse_prefetch_transpose: main.c
	$(CC) $(CFLAGS) -DSSE_PREFETCH -o $@ $(SRCS_common)

avx_transpose: main.c
	$(CC) $(CFLAGS) -DAVX -o $@ $(SRCS_common)

avx_prefetch_transpose: main.c
	$(CC) $(CFLAGS) -DAVX_PREFETCH -o $@ $(SRCS_common)

cache-test: $(EXEC)
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./naive_transpose
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./sse_transpose
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./sse_prefetch_transpose
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./avx_transpose
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./avx_prefetch_transpose

gencsv:
	./test_time > test_time.csv

plot: test_time.csv
	gnuplot scripts/runtime.gp

clean:
	$(RM) $(EXEC) *.o perf.*
