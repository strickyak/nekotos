# ../games/blue/blue.c:DEFINE_SCREEN(G, 12);

import re, sys

MATCH = re.compile(r'^\s*DEFINE_SCREEN\s*[(]\s*(\S+)\s*[,]\s*(\S+)\s*[)]\s*;').match

mem = 0x4000  # 16K RAM limit.

print('  .area  .absolute')
#print('      ORG 0')
#print('zero  RMB  1')

for line in sys.stdin:
    m = MATCH(line)
    if m:
        label, pages = m[1], int(m[2])
        mem -= pages * 256
        print('_%s  EQU  $%04x' % (label, mem))

print('_Screen_Start  EQU  $%04x' % mem)
