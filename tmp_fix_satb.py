import sys

path = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix 1: pre-write barrier - replace runtime if with compile-time conditional
idx1 = content.find('builder.AppendLine($"{indentation}    if (needsSatbBarrier)");')
if idx1 >= 0:
    old = content[idx1:idx1+188]
    print(f"Old pattern at {idx1}: {repr(old[:80])}...")
    new = '\t\t\tif (needsSatbBarrier)\n\t\t\t{\n\t\t\t\tbuilder.AppendLine($"{indentation}    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(chaos_address));");\n\t\t\t}'
    content = content.replace(old, new)
    print("Fix 1 applied")
else:
    print("Fix 1: NOT FOUND")

# Fix 2: post-write barrier (chaos_gc_dirty_card)
idx2 = content.find('builder.AppendLine($"{indentation}    chaos_gc_dirty_card(reinterpret_cast<void*>(chaos_address));");')
idx2_before = content.rfind('builder.AppendLine($"{indentation}    if (needsSatbBarrier)");', 0, idx2)
if idx2_before >= 0 and idx2_before > idx1 + 50:
    old2 = content[idx2_before:idx2+125]
    new2 = '\t\t\tif (needsSatbBarrier)\n\t\t\t{\n\t\t\t\tbuilder.AppendLine($"{indentation}    chaos_gc_dirty_card(reinterpret_cast<void*>(chaos_address));");\n\t\t\t}'
    content = content.replace(old2, new2)
    print("Fix 2 applied")
else:
    print("Fix 2: NOT FOUND")

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

# Verify
with open(path, 'r', encoding='utf-8') as f:
    lines = f.readlines()
    for i, line in enumerate(lines):
        if 'needsSatbBarrier' in line and 'if (needsSatbBarrier)' in line:
            print(f"  REMAINING needsSatbBarrier at line {i+1}: {line.rstrip()[:80]}")
