"""Remove TryCreateMarshalCopyRuntimeHelperDefinition method body.
File uses \t indentation for member declarations."""

with open('D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs', 'r', encoding='utf-8') as f:
    content = f.read()

start_marker = '\tprivate bool TryCreateMarshalCopyRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)'
idx_start = content.find(start_marker)
assert idx_start >= 0, 'start marker not found'

# Find the end: look for '}\n}\n' which is the class-level closing brace
# Or find next method starting with \tprivate
next_method = content.find('\n\tprivate ', idx_start + len(start_marker))
if next_method >= 0:
    # Include the blank line before next method
    content = content[:idx_start] + content[next_method:]
else:
    # This is the last method, remove to end
    # Find the class closing brace
    content = content[:idx_start] + '\n}\n'

with open('D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs', 'w', encoding='utf-8') as f:
    f.write(content)

# ===== Update dispatch chain =====
with open('D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs', 'r', encoding='utf-8') as f:
    content = f.read()

old_dispatch = '\t\tif (TryCreateStringRuntimeHelperDefinition(callee, out helperDefinition))\n\t\t{\n\t\t\treturn true;\n\t\t}\n\n\t\tif (TryCreateMarshalingRuntimeHelperDefinition(callee, out helperDefinition))\n\t\t{\n\t\t\treturn true;\n\t\t}'
new_dispatch = '\t\tif (TryCreateStringRuntimeHelperDefinition(callee, out helperDefinition))\n\t\t{\n\t\t\treturn true;\n\t\t}'

assert old_dispatch in content, 'dispatch code not found'
content = content.replace(old_dispatch, new_dispatch, 1)

with open('D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs', 'w', encoding='utf-8') as f:
    f.write(content)

print('Done')
