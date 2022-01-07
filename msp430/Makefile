CC=msp430-elf-g++

STDLIB_INC=-I/Applications/ti/ccs1040/ccs/ccs_base/msp430/include_gcc -I/Applications/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_macos/msp430-elf/include
MSP430_TARGET=-mmcu=msp430f5529 -mhwmult=f5series  -gdwarf-3 -gstrict-dwarf
WARNINGS=-Wall -mlarge -mcode-region=none -mdata-region=lower

out/simple_1.o: simple_1.cc
	mkdir -p out
	${CC} ${WARNINGS} ${STDLIB_INC} ${MSP430_TARGET} -c -o out/simple_1.o simple_1.cc
out/simple_1.out: out/simple_1.o
	mkdir -p out
	${CC} ${WARNINGS} ${STDLIB_INC} ${MSP430_TARGET} -o out/simple_1.out out/simple_1.o

all: out/simple_1.out

clean:
	rm -rf out
