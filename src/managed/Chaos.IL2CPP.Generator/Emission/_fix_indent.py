"""Fix the indentation of the dedup logic in GeneratedModule.cs."""
FILE = r"D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GeneratedModule.cs"

with open(FILE, 'r', encoding='utf-8') as f:
    content = f.read()

# The buggy dedup logic at wrong indentation (12sp instead of 16sp)
# It was inserted between `var safeMethodName` (16sp) and `methodModels[mi]` (16sp)
# but at 12sp level. Remove it and re-insert at 16sp.

old_buggy = """\n            // Uniquify method names: when two managed overloads collapse to the\n            // same C++ identifier at the ABI level, append _1, _2, ... suffix.\n            if (methodNameCounts.TryGetValue(safeMethodName, out var methodCount))\n            {\n                var baseName = safeMethodName;\n                safeMethodName = baseName + "_" + methodCount;\n                methodNameCounts[baseName] = methodCount + 1;\n            }\n            else\n            {\n                methodNameCounts[safeMethodName] = 1;\n            }\n"""

# The correct version at 16sp indentation
new_fixed = """\n                // Uniquify method names: when two managed overloads collapse to the\n                // same C++ identifier at the ABI level, append _1, _2, ... suffix.\n                if (methodNameCounts.TryGetValue(safeMethodName, out var methodCount))\n                {\n                    var baseName = safeMethodName;\n                    safeMethodName = baseName + "_" + methodCount;\n                    methodNameCounts[baseName] = methodCount + 1;\n                }\n                else\n                {\n                    methodNameCounts[safeMethodName] = 1;\n                }\n"""

assert old_buggy in content, 'Buggy code not found!'
content = content.replace(old_buggy, new_fixed, 1)
print('Fixed indentation of dedup logic')

with open(FILE, 'w', encoding='utf-8') as f:
    f.write(content)
print('Done')
