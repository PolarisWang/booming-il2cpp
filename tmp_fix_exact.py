import sys

path = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs'
with open(path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Line numbers (0-indexed) that need fixing in the non-multicast branch
# Lines 2222 and 2226 use {closedCall} instead of {singleClosedCall}
fix_lines = [2221, 2225]  # 0-indexed

for idx in fix_lines:
    if idx < len(lines):
        old_line = lines[idx]
        new_line = old_line.replace('{closedCall}', '{singleClosedCall}')
        if old_line != new_line:
            lines[idx] = new_line
            print(f'Fixed line {idx+1}: {new_line.strip()[:100]}')
        else:
            print(f'Line {idx+1}: no change needed: {old_line.strip()[:100]}')
    else:
        print(f'Line {idx+1}: out of range')

with open(path, 'w', encoding='utf-8') as f:
    f.writelines(lines)

# Verify
for i, line in enumerate(lines, 1):
    if '{closedCall}' in line and '                {closedCall}' not in line and 'chaos_result' not in line:
        print(f'  REMAINING: Line {i}: {line.strip()[:100]}')

print('Done')
