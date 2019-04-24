#
TARGET = gsensor

#
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -g -Wall  -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include
LDFLAGS =  -g -Wall 
CC = $(CROSS_COMPILE)gcc
ARCH= arm

build: $(TARGET)

gsensor: main.o ADXL345.o
	$(CC) $(LDFLAGS)   $^ -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) *.a *.o *~