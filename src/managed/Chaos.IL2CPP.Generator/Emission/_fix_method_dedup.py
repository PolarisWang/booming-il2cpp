"""Fix GeneratedModule.cs — correct the method-name dedup logic (baseName)."""
FILE = r"D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GeneratedModule.cs"

with open(FILE, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix the buggy dedup logic: save baseName before modifying safeMethodName
old = """            if (methodNameCounts.TryGetValue(safeMethodName, out var methodCount))
            {
                safeMethodName = $"{safeMethodName}_{methodCount}";
                methodNameCounts[safeMethodName] = methodCount + 1;
            }"""

new = """            if (methodNameCounts.TryGetValue(safeMethodName, out var methodCount))
            {
                var baseName = safeMethodName;
                safeMethodName = baseName + "_" + methodCount;
                methodNameCounts[baseName] = methodCount + 1;
            }"""

assert old in content, 'Buggy code not found!'
content = content.replace(old, new, 1)
print('Fixed buggy dedup logic')

with open(FILE, 'w', encoding='utf-8') as f:
    f.write(content)
print('Done')
