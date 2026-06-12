import re

with open('src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs', 'r', encoding='utf-8-sig') as f:
    lines = f.readlines()

# Find and fix the stub return type line
for i, line in enumerate(lines):
    if 'RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR"' in line:
        indent = line[:len(line) - len(line.lstrip())]
        lines[i] = indent + '// Use correct C++ return type matching ABI carrier (Float32->float, Float64->double)\n'
        i += 1
        lines.insert(i, indent + 'string _cppRet = failReturnAbi.CarrierKindCode switch\n')
        i += 1
        lines.insert(i, indent + '{\n')
        i += 1
        lines.insert(i, indent + '    AotCoreIrAbiCarrierKind.Float32 => "float",\n')
        i += 1
        lines.insert(i, indent + '    AotCoreIrAbiCarrierKind.Float64 => "double",\n')
        i += 1
        lines.insert(i, indent + '    _ => "CHAOS_IL2CPP_INTPTR",\n')
        i += 1
        lines.insert(i, indent + '};\n')
        i += 1
        lines.insert(i, indent + 'var src = RenderSimpleExternalRuntimeHelper(_cppRet, failSymbol, "",\n')
        i += 1
        lines.insert(i, indent + '    ["    return ChaosExternalRuntimeFallback(\\"" + escapedCallee + "\\");"]);\n')
        break

with open('src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs', 'w', encoding='utf-8-sig') as f:
    f.writelines(lines)
print('Fixed ExternalRuntimeHelpers.cs')
