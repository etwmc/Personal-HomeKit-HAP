/* needs stdio.h, string.h, poly1305.h */

#define trials 32768

static uint8_t buffer[16384];
static uint64_t allticks[trials+1];
static uint64_t minticks;

static void
poly1305_bench(void) {
	static const poly1305_key key = {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}};
	static const size_t lengths[] = {1, 64, 160, 576, 4096, 0};
	uint8_t mac[16];
	size_t i, pass;

	if (!poly1305_power_on_self_test()) {
		printf("self check FAILED\n");
		return;
	} else {
		printf("self check passed\n\n");
	}

	memset(buffer, 0x5c, sizeof(buffer));

	for (i = 0; i < trials; i++)
		poly1305_auth(mac, buffer, 8192, &key);

	for (i = 0; lengths[i] != 0; i++) {
		for (pass = 0; pass < trials; pass++) {
			allticks[pass] = get_ticks();
			poly1305_auth(mac, buffer, lengths[i], &key);
		}
		allticks[pass] = get_ticks();
		for (pass = 0; pass < trials; pass++)
			allticks[pass] = allticks[pass + 1] - allticks[pass];

		for (pass = 0, minticks = ~0ull; pass < trials; pass++) {
			if (allticks[pass] < minticks)
				minticks = allticks[pass];
		}

		printf("%u bytes, %.0f cycles, %.2f cycles/byte\n", (unsigned int)lengths[i], (double)minticks, (double)minticks / lengths[i]);
	}
}
