#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>

/* Test-only LD_PRELOAD shim: initialize both builds' libc RNG identically.
 * This does not make wall-clock scheduling or physics fully deterministic. */
void srand(unsigned int ignored) {
    void (*real_srand)(unsigned int) = dlsym(RTLD_NEXT, "srand");
    const char *seed = getenv("Q3RALLY_BENCHMARK_SEED");
    (void)ignored;
    if (!real_srand) abort();
    real_srand(seed ? (unsigned int)strtoul(seed, NULL, 10) : 1337);
}
