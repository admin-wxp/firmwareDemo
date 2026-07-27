CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = fw_sim
LIB = libadcsim.so

SRCS = main.c adc_ctrl.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET) $(LIB)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(LIB): adc_ctrl.o
	$(CC) -shared -fPIC -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(LIB)

