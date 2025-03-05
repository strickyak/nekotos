import re, sys

def err(s): print(s, file=sys.stderr)

if len(sys.argv) != 3:
    err("ERROR: Requires 2 arguments.")
    err("USAGE:   python3 n1preprocess.py input.c _tmp_output.c")
    sys.exit(13)

r = open(sys.argv[1], "r")
w = open(sys.argv[2], "w")

DoesDefineScreen = re.compile(r'\s*N1_DEFINE_SCREEN\s*[(]\s*(\w+)\s*,\s*(\d+)\s*[)]').match
DoesDefineRegion = re.compile(r'\s*N1_DEFINE_REGION\s*[(]\s*(\w+)\s*,\s*([\w ]+)[)]').match

ceiling = 0x4000
screens = []
regions = []

def DefineScreen(ds):
    global ceiling, screens
    name, pages = ds[1], int(ds[2])
    ceiling -= pages*256
    screens.append(dict(name=name, addr=ceiling, pages=pages))

def DefineRegion(dr):
    global screens
    name, ctype = dr[1], dr[2]
    regions.append((ctype, name))

lines = [x.rstrip() for x in r]
for line in lines:
    ds = DoesDefineScreen(line)
    dr = DoesDefineRegion(line)

    if ds: DefineScreen(ds)
    if dr: DefineRegion(dr)

def out(s): print(s, file=w)

out(f'// This file is generated from {sys.argv[1]} by n1preprocess.py')
out(f'')
out(f'#define N1_DEFINE_SCREEN(Name,NumPages)')
out(f'#define N1_DEFINE_REGION(Name)')
out(f'')

for scr in screens:
    out(f'#define  {scr["name"]}  ((byte*)0x{scr["addr"]:04x})  // {scr["pages"]} pages')
out(f'#define  _N1PRE_SCREENS   0x{ceiling:04x}')

addr = '_N1PRE_SCREENS'
for rgn in regions:
    addr += f' - sizeof ({rgn[1]})'
    out(f'#define  {rgn[0]}  (*({rgn[1]} *)({addr}))')
out(f'#define  _N1PRE_REGIONS   ({addr})')

out('')
out('int const _n1pre_final           __attribute__ ((section (".final"))) = 0xAEEE;')
out('int const _n1pre_final_startup   __attribute__ ((section (".final.startup"))) = 0xAEEF;')
out('')
out('')
out('')
out(f'#line 1 "{sys.argv[1]}"')
for line in lines:
    out(line)
out('')
out('')
out('')

out(f'#line 1000000 "{sys.argv[2]}"')
out('')
out('int const _n1pre_screens MORE_DATA = _N1PRE_SCREENS;')
out('int const _n1pre_regions MORE_DATA = _N1PRE_REGIONS;')
out('//END.')
