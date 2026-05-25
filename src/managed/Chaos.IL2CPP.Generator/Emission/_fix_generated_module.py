"""Fix GeneratedModule.cs — add method-name dedup + safe_name collision detection."""
FILE = r"D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GeneratedModule.cs"

with open(FILE, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix 1a: Add methodNameCounts after methodModels declaration
old1a = "            var methodModels = new ScriptObject[methods.Length];\n            for (int mi = 0; mi < methods.Length; mi++)"
new1a = "            var methodModels = new ScriptObject[methods.Length];\n            var methodNameCounts = new Dictionary<string, int>(StringComparer.Ordinal);\n            for (int mi = 0; mi < methods.Length; mi++)"
assert old1a in content, 'Fix 1a failed'
content = content.replace(old1a, new1a, 1)
print('Fix 1a applied')

# Fix 1b: Add dedup logic after safeMethodName
old1b = """            var safeMethodName = SanitizeCppIdentifier(rawMethodName);

                methodModels[mi] = new ScriptObject"""
new1b = """            var safeMethodName = SanitizeCppIdentifier(rawMethodName);

            // Uniquify method names: when two managed overloads collapse to the
            // same C++ identifier at the ABI level, append _1, _2, ... suffix.
            if (methodNameCounts.TryGetValue(safeMethodName, out var methodCount))
            {
                var baseName = safeMethodName;
                safeMethodName = baseName + "_" + methodCount;
                methodNameCounts[baseName] = methodCount + 1;
            }
            else
            {
                methodNameCounts[safeMethodName] = 1;
            }

                methodModels[mi] = new ScriptObject"""
assert old1b in content, 'Fix 1b failed'
content = content.replace(old1b, new1b, 1)
print('Fix 1b applied')

# Fix 2: Insert safe_name collision dedup before `return new ScriptObject`
old2 = "        }\n\n        return new ScriptObject"
new2 = """        }

        // Detect duplicate safe_names across type groups and uniquify by
        // appending the assembly prefix.  The converter may emit methods for
        // the same C# type from different assemblies, and GetTypeDisplayName()
        // strips the assembly prefix, causing C2011 struct redefinition errors.
        var safeNameGroups = new Dictionary<string, List<int>>(StringComparer.Ordinal);
        for (int gi = 0; gi < typeGroupModels.Length; gi++)
        {
            var name = (string)typeGroupModels[gi]["safe_name"];
            if (!safeNameGroups.TryGetValue(name, out var list))
            {
                list = new List<int>();
                safeNameGroups[name] = list;
            }
            list.Add(gi);
        }
        foreach (var kvp in safeNameGroups)
        {
            if (kvp.Value.Count <= 1) continue;
            foreach (var gi in kvp.Value)
            {
                var groupKey = typeGroups[gi].Key;
                var slashIdx = groupKey.IndexOf('/');
                var assemblyPart = slashIdx > 0 ? groupKey[..slashIdx] : groupKey;
                var assemblySafe = SanitizeCppIdentifier(assemblyPart);
                var oldName = (string)typeGroupModels[gi]["safe_name"];
                var newName = oldName + "_" + assemblySafe;
                typeGroupModels[gi]["safe_name"] = newName;
                typeGroupModels[gi]["safe_name_lower"] =
                    SanitizeCppIdentifierLowerFirst(newName);
            }
        }

        return new ScriptObject"""
assert old2 in content, 'Fix 2 failed'
content = content.replace(old2, new2, 1)
print('Fix 2 applied')

with open(FILE, 'w', encoding='utf-8') as f:
    f.write(content)
print('Done')
