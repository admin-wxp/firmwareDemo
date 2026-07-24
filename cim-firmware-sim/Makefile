
**文件：`Makefile`**

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -g

.PHONY: all clean test

all: firmware/firmware_test firmware/libcimfw.so

firmware/firmware_test: firmware/reg.c firmware/adc.c firmware/main.c
	$(CC) $(CFLAGS) -o $@ $^

firmware/libcimfw.so: firmware/reg.c firmware/adc.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $^

test: all
	pytest tests/ -v
	bash scripts/run_regression.sh

clean:
	rm -f firmware/firmware_test firmware/libcimfw.so