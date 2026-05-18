import sys

path = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

changes = 0

# Fix 1: openFnType line (2118) - simpler approach via unique substrings
old1 = 'string.IsNullOrEmpty(sigCache) ? (returnType + "(*)()") : string.Concat(returnType, "(*)(", sigCache, ")")'
new1 = 'parameterAbis.Count == 0 ? (returnType + "(*)()") : string.Concat(returnType, "(*)(", sigCache, ")")'
if old1 in content:
    content = content.replace(old1, new1)
    changes += 1
    print("Fix 1a applied")
else:
    print("Fix 1a NOT FOUND")

# Fix 1b: closedFnType line (2119)
old1b = '(string.IsNullOrEmpty(sigCache) ? (returnType + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)") : (returnType + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, " + sigCache + ")"))'
new1b = '(parameterAbis.Count == 0 ? (returnType + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)") : (returnType + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, " + sigCache + ")"))'
if old1b in content:
    content = content.replace(old1b, new1b)
    changes += 1
    print("Fix 1b applied")
else:
    print("Fix 1b NOT FOUND")

# Fix 2a: Add singleClosedCall line
# Find the line starting with 'string closedCall = "chaos_closed_function(chaos_invocation_delegate...'
import re
match = re.search(r'^\t\tstring closedCall = "chaos_closed_function\(chaos_invocation_delegate', content, re.MULTILINE)
if match:
    # Find the end of this line (semicolon)
    line_start = match.start()
    line_end = content.index(';', line_start) + 1
    closed_line = content[line_start:line_end]
    indent = '\t\t'
    single_closed = closed_line.replace('chaos_invocation_delegate', 'chaos_delegate')
    single_closed = single_closed.replace('string closedCall', 'string singleClosedCall')
    insert_pos = line_end
    content = content[:insert_pos] + '\n' + indent + single_closed + content[insert_pos:]
    changes += 1
    print("Fix 2a applied")

    # Fix 2b: Update non-multicast branch to use singleClosedCall
    # The non-multicast branch has deeper indentation (3 tabs for the builder calls)
    content = content.replace(
        'builder.AppendLine($"{indentation}            {closedCall}");',
        'builder.AppendLine($"{indentation}            {singleClosedCall}");'
    )
    content = content.replace(
        'builder.AppendLine($"{indentation}            const auto chaos_result = {closedCall}");',
        'builder.AppendLine($"{indentation}            const auto chaos_result = {singleClosedCall}");'
    )
    changes += 1
    print("Fix 2b applied")
else:
    print("Fix 2a: closedCall line NOT FOUND")

# Fix 3: needsSatbBarrier - convert runtime if/else blocks to compile-time C# conditionals
# Pre-write barrier (4 lines, uses 2 tabs)
old_pre_lines = '\t\tbuilder.AppendLine($"{indentation}    if (needsSatbBarrier)");\n\t\tbuilder.AppendLine($"{indentation}    {{");\n\t\tbuilder.AppendLine($"{indentation}        BgcSatbPreWriteBarrier(reinterpret_cast<void**>(chaos_address));");\n\t\tbuilder.AppendLine($"{indentation}    }}");'
new_pre_lines = '\t\tif (needsSatbBarrier)\n\t\t{\n\t\t\tbuilder.AppendLine($"{indentation}    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(chaos_address));");\n\t\t}'
if old_pre_lines in content:
    content = content.replace(old_pre_lines, new_pre_lines, 1)
    changes += 1
    print("Fix 3a applied")
else:
    print("Fix 3a NOT FOUND")

# Post-write barrier (4 lines, uses 2 tabs)
old_post_lines = '\t\tbuilder.AppendLine($"{indentation}    if (needsSatbBarrier)");\n\t\tbuilder.AppendLine($"{indentation}    {{");\n\t\tbuilder.AppendLine($"{indentation}        chaos_gc_dirty_card(reinterpret_cast<void*>(chaos_address));");\n\t\tbuilder.AppendLine($"{indentation}    }}");'
new_post_lines = '\t\tif (needsSatbBarrier)\n\t\t{\n\t\t\tbuilder.AppendLine($"{indentation}    chaos_gc_dirty_card(reinterpret_cast<void*>(chaos_address));");\n\t\t}'
if old_post_lines in content:
    content = content.replace(old_post_lines, new_post_lines, 1)
    changes += 1
    print("Fix 3b applied")
else:
    print("Fix 3b NOT FOUND")

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

# Quick validation
for i, line in enumerate(content.split('\n'), 1):
    if 'needsSatbBarrier)' in line and line.lstrip().startswith('builder'):
        print(f"  WARNING: Line {i} still has runtime needsSatbBarrier: {line.strip()[:80]}")

print(f"Total changes: {changes}")
