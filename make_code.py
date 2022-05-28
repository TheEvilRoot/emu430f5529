#!/usr/bin/env python3

import struct 

patterns = {
    # single operands
    "rrc":     "000100 000 XXXXXXX",
    "swpb":    "000100 001 XXXXXXX",
    "rra":     "000100 010 XXXXXXX",
    "sxt":     "000100 011 XXXXXXX",
    "push":    "000100 100 XXXXXXX",
    "call":    "000100 101 XXXXXXX",
    "reti":    "000100 110 0000000",

    # jumps
    "jne/jnz": "001 000 XXXXXXXXXX",
    "jeq/jz":  "001 001 XXXXXXXXXX",
    "jnc/jlo": "001 010 XXXXXXXXXX",
    "jc/jhs":  "001 011 XXXXXXXXXX",
    "jn":      "001 100 XXXXXXXXXX",
    "jge":     "001 101 XXXXXXXXXX",
    "jl":      "001 110 XXXXXXXXXX",
    "jmp":     "001 111 XXXXXXXXXX",

    # two operand
    "mov":  "0100 XXXX XXXX XXXX",
    "add":  "0101 XXXX XXXX XXXX",
    "addc": "0110 XXXX XXXX XXXX",
    "subc": "0111 XXXX XXXX XXXX",
    "sub":  "1000 XXXX XXXX XXXX",
    "cmp":  "1001 XXXX XXXX XXXX",
    "dadd": "1010 XXXX XXXX XXXX",
    "bit":  "1011 XXXX XXXX XXXX",
    "bic":  "1100 XXXX XXXX XXXX",
    "bis":  "1101 XXXX XXXX XXXX",
    "xor":  "1110 XXXX XXXX XXXX",
    "and":  "1111 XXXX XXXX XXXX"
}

def process(name, pattern):
    pattern = pattern.replace(' ', '')
    xs = pattern.count('X')
    vs = 2 ** xs
    print('processing pattern %8s with %2d Xs which makes %4d variations' % (name, xs, vs))
    # find indexes of xs
    poss = [i for i, x in enumerate(list(pattern)) if x == 'X'] 
    # for for every variation
    poss = list(reversed(poss))
    inss = []
    for i in range(vs):
        # copy values
        cur = list(pattern)
        # val is number repr. of variation for xs
        val = i

        # fill all xs with val's bits
        for x in range(len(poss)):
            cur[poss[x]] = chr(ord('0') + (val & 0x1))
            val >>= 0x1
        cur = ''.join(cur)
        value = int(cur, 2)
        # pack into 16bit instruction
        ins = struct.pack("<H", value) 
        print("Variation 0x%04X produces %16s which is 0x%04X -> %r" % (i, cur, value, ins))
        inss.append(ins)
    print('%s produced %4d instructions' % (name, len(inss)))
    return inss

inss = []
for name, pattern in patterns.items():
    inss += process(name, pattern)
code = b''.join(inss)
print('in the end produced %4d instructions from %2d patterns' % (len(inss), len(patterns)))
print('which is 0x%08X bytes of code' % (len(code), ))

fnum = 0
while len(code) > 0x0:
    fname = 'out/code%d.dat' % (fnum,) 
    print('writing portion of code to %8s. code left 0x%08X bytes' % (fname, len(code)))
    with open(fname, 'w+b') as handle:
        handle.write(code[:0x7FFF])
    fnum += 1
    code = code[0x0FFF:]
print('code is written with %d files. final length is %x' % (fnum, len( code)))
