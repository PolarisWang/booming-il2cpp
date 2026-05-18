import sys

path = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Debug: find the exact string around the builder pattern
idx = content.find('builder.AppendLine($"')
if idx >= 0:
    chunk = content[idx:idx+200]
    print("First builder pattern:")
    for ch in chunk[:80]:
        print(f'  {repr(ch)} = {ch!r}')

# The issue - the pattern uses $ and { which are special characters
# Use raw string matching with the actual bytes
target_3tab = '\t\t\tbuilder.AppendLine($"{indentation}            {closedCall}");'
print(f'\nTarget repr: {target_3tab!r}')
print(f'Target found: {target_3tab in content}')

# Let's find ALL occurrences and just do a careful replace
for target_line in ['{closedCall}', '{singleClosedCall}']:
    for i, line in enumerate(content.split('\n'), 1):
        if target_line in line:
            print(f'  Line {i}: {line.strip()}')

