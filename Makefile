CC ?= gcc
CFLAGS = -msse2 --std gnu99 -O0 -Wall -Wextra

EXEC = naive_transpose sse_transpose sse_prefetch_transpose

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

SRCS_common = main.c

naive_transpose: main.c
	$(CC) $(CFLAGS) -DNAIVE -o $@ $(SRCS_common)

sse_transpose: main.c
	$(CC) $(CFLAGS) -DSSE -o $@ $(SRCS_common)

sse_prefetch_transpose: main.c
	$(CC) $(CFLAGS) -DSSE_PREFETCH -o $@ $(SRCS_common)

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

clean:
	$(RM) $(EXEC) *.o perf.*
