#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "poly1305.h"

/* define get_ticks */
static uint64_t
get_ticks(void) {
	uint32_t lo, hi;
	__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)lo | ((uint64_t)hi << 32));
}

#include "bench-base.h"

int main(void) {
	poly1305_bench();
	return 0;
}
