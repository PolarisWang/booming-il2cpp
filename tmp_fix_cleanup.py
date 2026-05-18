import sys

path = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix 1: Remove the duplicated singleClosedCall line with bad indentation,
# and fix the remaining one's indentation
old_dup = (
    '\t\t\t\tstring singleClosedCall = "chaos_closed_function(chaos_delegate->chaos_delegate_target"'
    ' + ((parameterAbis.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(parameterAbis))) + ");'
    '\n\t\t\t\tstring singleClosedCall = "chaos_closed_function(chaos_delegate->chaos_delegate_target"'
    ' + ((parameterAbis.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(parameterAbis))) + ");'
)
new_single = (
    '\t\tstring singleClosedCall = "chaos_closed_function(chaos_delegate->chaos_delegate_target"'
    ' + ((parameterAbis.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(parameterAbis))) + ");'
)
if old_dup in content:
    content = content.replace(old_dup, new_single, 1)
    print("Fix: deduped singleClosedCall")
else:
    # Try to find the duplicated lines as separate entries
    count = content.count('string singleClosedCall')
    print(f"Found {count} occurrences of 'string singleClosedCall'")
    # If there are 2, we just need to remove one (the second one with wrong indent)
    if count >= 2:
        idx_first = content.find('\t\tstring singleClosedCall')
        idx_second = content.find('string singleClosedCall', idx_first + 5)
        # Find the full second line
        line_end = content.find('\n', idx_second)
        # Remove the duplicated line
        content = content[:idx_second] + content[line_end+1:]
        print("Fix: removed duplicated singleClosedCall line")
        # Fix indentation of the first
        content = content.replace(
            '\t\t\t\tstring singleClosedCall',
            '\t\tstring singleClosedCall',
            1
        )

# Fix 2: Update non-multicast branch closedCall -> singleClosedCall
count_closed = content.count('builder.AppendLine($"{indentation}            {closedCall}");')
print(f"Found {count_closed} occurrences of 'closedCall' in non-multicast builder lines")
content = content.replace(
    'builder.AppendLine($"{indentation}            {closedCall}");',
    'builder.AppendLine($"{indentation}            {singleClosedCall}");'
)
count_after = content.count('builder.AppendLine($"{indentation}            {closedCall}");')
print(f"After fix: {count_after} remaining")

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print("All cleanup fixes applied")
