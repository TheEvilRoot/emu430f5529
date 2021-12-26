import re
import struct

with open('gistfile1.txt', 'r') as f:
    c = f.read()

RE = r'(([0-9A-Fa-f]{2}\s{0,1}){2,4})\s+' 
X = c.split('\n')

out = open('shellcode', 'w+b')

for line in X:
    Y = re.findall(RE, line)
    for x, _ in Y:
        x = x.strip('\n ') 
        x = x.split(' ') 
        if len(x) not in [2, 4]:
            print('wrong', x)
        else:
            x = [struct.pack('B', int(i, 16)) for i in x] 
            x = b''.join(x) 
            out.write(x)
            print(x)
out.close()
        
        

