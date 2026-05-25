"""Option B: Always use full subject ID (with assembly prefix) for safe_name."""
FILE = r"D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GeneratedModule.cs"

with open(FILE, 'r', encoding='utf-8') as f:
    content = f.read()

# Replace: use group.Key (full subject ID with assembly prefix) instead of GetTypeDisplayName
old = """            var typeDisplayName = GetTypeDisplayName(group.Key);
            var safeName = SanitizeCppIdentifier(typeDisplayName);
            var safeNameLower = SanitizeCppIdentifierLowerFirst(typeDisplayName);"""

new = """            var safeName = SanitizeCppIdentifier(group.Key);
            var safeNameLower = SanitizeCppIdentifierLowerFirst(group.Key);"""

assert old in content, 'Old text not found!'
content = content.replace(old, new, 1)
print('Option B applied: safe_name now includes assembly prefix')

with open(FILE, 'w', encoding='utf-8') as f:
    f.write(content)
print('Done')
