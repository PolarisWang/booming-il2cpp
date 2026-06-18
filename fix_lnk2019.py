import sys

# Fix 2: LNK2019 cross-assembly - Add IsSameModuleMethod check
path = r'D:\agent\chaos-il2cpp\src\managed\Chaos.IL2CPP.Generator\RuntimeSupport\NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs'
with open(path, encoding='utf-8') as f:
    content = f.read()

old = (
    '\t\tif (_methodsBySubjectId.TryGetValue(callee, out var aotMethod) && aotMethod.Instructions.Count > 0)\n'
    '\t\t{\n'
    '\t\t\thelperDefinition = null;\n'
    '\t\t\t_externalRuntimeHelperCache[callee] = null;\n'
    '\t\t\treturn false;\n'
    '\t\t}\n'
    '\t\tif (_methodsBySubjectId.TryGetValue(originalCallee, out var aotMethod2) && aotMethod2.Instructions.Count > 0)\n'
    '\t\t{\n'
    '\t\t\thelperDefinition = null;\n'
    '\t\t\t_externalRuntimeHelperCache[callee] = null;\n'
    '\t\t\treturn false;\n'
    '\t\t}'
)

new = (
    '\t\tif (_methodsBySubjectId.TryGetValue(callee, out var aotMethod) && aotMethod.Instructions.Count > 0\n'
    '\t\t\t&& IsSameModuleMethod(callee))\n'
    '\t\t{\n'
    '\t\t\thelperDefinition = null;\n'
    '\t\t\t_externalRuntimeHelperCache[callee] = null;\n'
    '\t\t\treturn false;\n'
    '\t\t}\n'
    '\t\tif (_methodsBySubjectId.TryGetValue(originalCallee, out var aotMethod2) && aotMethod2.Instructions.Count > 0\n'
    '\t\t\t&& IsSameModuleMethod(originalCallee))\n'
    '\t\t{\n'
    '\t\t\thelperDefinition = null;\n'
    '\t\t\t_externalRuntimeHelperCache[callee] = null;\n'
    '\t\t\treturn false;\n'
    '\t\t}'
)

if old not in content:
    print('ERROR: Pattern not found in file!', file=sys.stderr)
    # Debug: show what's at the relevant position
    lines = content.split('\n')
    for i, line in enumerate(lines[277:290], start=278):
        print(f'  {i}: {repr(line)}', file=sys.stderr)
    sys.exit(1)

content = content.replace(old, new, 1)
with open(path, 'w', encoding='utf-8') as f:
    f.write(content)
print('LNK2019 fix applied successfully')
