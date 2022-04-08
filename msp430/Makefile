CC=msp430-elf-g++
HEX430=hex430

STDLIB_INC=-I/Applications/ti/ccs1040/ccs/ccs_base/msp430/include_gcc -I/Applications/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_macos/msp430-elf/include
MSP430_TARGET=-mmcu=msp430f5529 -mhwmult=f5series
WARNINGS=-Wall

out/simple_1.o: simple_1.cc
	mkdir -p out
	${CC} ${WARNINGS} ${STDLIB_INC} ${MSP430_TARGET} -O0 -c -o out/simple_1.o simple_1.cc
out/simple_1.out: out/simple_1.o
	mkdir -p out
	${CC} ${WARNINGS} ${STDLIB_INC} ${MSP430_TARGET} -O0 -o out/simple_1.out out/simple_1.o
out/simple_1.hex: out/simple_1.out
	mkdir -p out
	${HEX430} --ti_txt out/simple_1.out -o out/simple_1.hex --order LS

all: out/simple_1.hex

clean:
	rm -rf out
