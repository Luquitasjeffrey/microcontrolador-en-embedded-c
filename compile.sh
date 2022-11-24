C=avr-gcc
CFLAGS= "-Os -mmcu=atmega328p"
FILE="driver.c"
OUTPUT="driver"
$C $CFLAGS $FILE -c
$C driver.o -o $OUTPUT
