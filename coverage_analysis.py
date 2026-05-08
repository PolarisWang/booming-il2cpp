import json, os, glob

fixtures_dir = 'D:/agent/booming-il2cpp/tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/Fixtures'
covered = set()
for f in sorted(glob.glob(fixtures_dir + '/*/aot-core-ir.json')):
    with open(f, 'r', encoding='utf-8') as fh:
        data = json.load(fh)
    for method in data.get('methods', []):
        for instr in method.get('instructions', []):
            op = instr.get('op', '')
            if op:
                covered.add(op)

structured = {
    'add', 'add.ovf', 'and', 'beq', 'bge', 'bge.un', 'bgt', 'ble', 'blt', 'bne.un',
    'box', 'br', 'brfalse', 'brtrue', 'call', 'calli', 'callvirt', 'castclass',
    'ceq', 'cgt', 'cgt.un', 'clt', 'conv.i1', 'conv.i2', 'conv.i4', 'conv.i8',
    'conv.ovf.i1', 'conv.ovf.u1', 'conv.r4', 'conv.r8', 'conv.u', 'conv.u1',
    'conv.u2', 'conv.u8', 'cpblk', 'div', 'dup', 'initobj', 'isinst', 'ldarg',
    'ldarga', 'ldc.i4', 'ldc.i8', 'ldc.r4', 'ldc.r8', 'ldelem', 'ldelem.ref',
    'ldelema', 'ldfld', 'ldflda', 'ldftn', 'ldind.i1', 'ldind.i2', 'ldind.i4',
    'ldind.i8', 'ldind.r4', 'ldind.r8', 'ldind.ref', 'ldind.u1', 'ldind.u2',
    'ldind.u4', 'ldlen', 'ldloc', 'ldloca', 'ldnull', 'ldobj', 'ldsfld',
    'ldsflda', 'ldstr', 'ldtoken', 'leave', 'localloc', 'mul', 'mul.ovf',
    'newarr', 'newobj', 'not', 'or', 'pop', 'rem', 'ret', 'shl', 'shr',
    'shr.un', 'sizeof', 'stelem', 'stelem.ref', 'stfld', 'stind.i1',
    'stind.i2', 'stind.i4', 'stind.i8', 'stind.r4', 'stind.r8', 'stind.ref',
    'stloc', 'stobj', 'stsfld', 'sub', 'sub.ovf', 'switch', 'throw',
    'unbox', 'unbox.any', 'xor'
}

linear_raw = {
    'add', 'and', 'box', 'call', 'callvirt', 'castclass', 'ceq', 'cgt', 'cgt.un',
    'clt', 'conv.i1', 'conv.i2', 'conv.i4', 'conv.i8', 'conv.r4', 'conv.r8',
    'conv.u', 'conv.u1', 'conv.u2', 'conv.u8', 'cpblk', 'cpobj', 'div', 'dup',
    'initobj', 'isinst', 'ldarg', 'ldarga', 'ldc.i4', 'ldc.i8', 'ldc.r4', 'ldc.r8',
    'ldelem.i1', 'ldelem.i2', 'ldelem.i4', 'ldelem.i8', 'ldelem.r4', 'ldelem.r8',
    'ldelem.ref', 'ldelem.u1', 'ldelem.u2', 'ldelem.u4',
    'ldelema', 'ldfld', 'ldflda', 'ldftn',
    'ldind.i1', 'ldind.i2', 'ldind.i4', 'ldind.i8', 'ldind.r4', 'ldind.r8',
    'ldind.ref', 'ldind.u1', 'ldind.u2', 'ldind.u4',
    'ldlen', 'ldloc', 'ldloca', 'ldnull', 'ldobj', 'ldsfld', 'ldsflda',
    'ldstr', 'ldtoken', 'localloc', 'mul', 'neg', 'newarr', 'newobj', 'not', 'or',
    'pop', 'rem', 'ret', 'shl', 'shr', 'shr.un', 'sizeof',
    'stelem.i1', 'stelem.i2', 'stelem.i4', 'stelem.i8', 'stelem.r4', 'stelem.r8',
    'stelem.ref',
    'stfld', 'stind.i1', 'stind.i2', 'stind.i4', 'stind.i8', 'stind.r4', 'stind.r8',
    'stind.ref', 'stloc', 'stobj', 'stsfld', 'sub', 'unbox', 'unbox.any', 'xor'
}

linear = set()
for op in linear_raw:
    if op.startswith('ldelem.') and op != 'ldelem.ref':
        linear.add('ldelem')
    elif op.startswith('stelem.') and op != 'stelem.ref':
        linear.add('stelem')
    else:
        linear.add(op)

eh_raw = {
    'add', 'and', 'box', 'call', 'callvirt', 'castclass', 'ceq', 'cgt', 'cgt.un',
    'cpblk', 'cpobj', 'div', 'initobj', 'isinst', 'ldarg',
    'ldc.i4', 'ldc.i8',
    'ldelem.i1', 'ldelem.i2', 'ldelem.i4', 'ldelem.i8', 'ldelem.r4', 'ldelem.r8',
    'ldelem.ref', 'ldelem.u1', 'ldelem.u2', 'ldelem.u4',
    'ldelema', 'ldfld', 'ldflda',
    'ldind.i1', 'ldind.i2', 'ldind.i4', 'ldind.i8', 'ldind.r4', 'ldind.r8',
    'ldind.ref', 'ldind.u1', 'ldind.u2', 'ldind.u4',
    'ldloc', 'ldnull', 'ldobj', 'ldsfld', 'ldsflda',
    'ldstr', 'ldtoken', 'localloc', 'mul', 'newarr', 'newobj', 'not', 'or',
    'rem', 'shl', 'shr', 'shr.un',
    'stelem.i1', 'stelem.i2', 'stelem.i4', 'stelem.i8', 'stelem.r4', 'stelem.r8',
    'stelem.ref',
    'stind.i1', 'stind.i2', 'stind.i4', 'stind.i8', 'stind.r4', 'stind.r8',
    'stind.ref', 'stloc', 'stobj', 'sub', 'unbox', 'unbox.any', 'xor'
}

eh = set()
for op in eh_raw:
    if op.startswith('ldelem.') and op != 'ldelem.ref':
        eh.add('ldelem')
    elif op.startswith('stelem.') and op != 'stelem.ref':
        eh.add('stelem')
    else:
        eh.add(op)

all_three = structured & linear & eh
uncovered = all_three - covered

print('=== Total opcodes covered by fixtures ===')
print(len(covered))
print()

print('=== Three-way intersection size ===')
print(len(all_three))
print()

print('=== UNCOVERED opcodes in all-3 intersection ===')
print('Count:', len(uncovered))
for op in sorted(uncovered):
    print(' ', op)
print()

# Show what's NOT covered from intersection but IS in structured only or linear only
print('=== In structured + linear (not needing EH) but uncovered ===')
s_l = (structured & linear) - covered - eh
print('Count:', len(s_l))
for op in sorted(s_l):
    print(' ', op)
print()

# Print coverage analysis per opcode
print('=== Detailed per-opcode coverage ===')
all_ops = all_three
for op in sorted(all_ops):
    c = 'YES' if op in covered else 'NO '
    print(f'  {c}  {op}')

# Opcodes only in structured (not in linear)
print()
print('=== Structured-only opcodes (not in linear) ===')
s_only = (structured - linear) - covered
for op in sorted(s_only):
    print(' ', op)
