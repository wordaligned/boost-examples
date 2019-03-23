def skip(*v, **k): pass

put = skip

for line in open('graph.h'):
    if line.startswith('enum'): # Start enum
        typ = line.split()[1]
        put = print
        put(f'bp::enum_<{typ}>("{typ}")')
    elif line.strip() == '};':   # End enum
        put('\t;')
        put = skip
    else:
        val = line.strip('\t \n,')
        put(f'\t.value("{val}", {val})')
