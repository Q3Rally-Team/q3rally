CC ?= gcc
CFLAGS ?= -I.. -I../.. -DARCH_STRING="x86_64" -Wall -Wextra -std=c99

TEST_BINARY := test_g_frictioncalc
SRCS := test_g_frictioncalc.c ../g_rally_hazard.c

all: $(TEST_BINARY)

$(TEST_BINARY): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -lm -o $@

run: $(TEST_BINARY)
	./$(TEST_BINARY)

clean:
	rm -f $(TEST_BINARY) *.o

.PHONY: all run clean
