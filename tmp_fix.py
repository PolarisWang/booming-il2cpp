import sys
p = sys.argv[1]
with open(p, encoding='utf-8') as f:
    content = f.read()

old_start = "// Use SubjectInstanceFactory for valid instances. Ref structs and"
old_end = 'return $"SubjectInstanceFactory.Create<global::{qualified.Replace(\'+\', \'.\')}>()";'

idx_start = content.find(old_start)
idx_end = content.find(old_end, idx_start)
if idx_start >= 0 and idx_end >= 0:
    idx_end += len(old_end)
    # Find the closing of this block - look for '}' before next method
    rest = content[idx_end:]
    brace = rest.find('\n        }')
    if brace >= 0:
        idx_end += brace + 1

    new_block = '''            // Use new T() for concrete types (new CHAOS_IL2CPP_NEW_GC),
            // SubjectInstanceFactory for abstract/interface, default(T)! for ref structs.
            try {
                var t = Type.GetType(typeFullName, false);
                if (t != null && t.IsValueType && t.IsByRefLike)
                    return $"default(global::{qualified.Replace('+', '.')})!";
            } catch { }
            if (_abstractTypeNames != null && _abstractTypeNames.Contains(typeFullName))
                return $"SubjectInstanceFactory.Create<global::{qualified.Replace('+', '.')}>()";
            return $"new global::{qualified.Replace('+', '.')}()";'''

    content = content[:idx_start] + new_block + content[idx_end:]
    with open(p, 'w', encoding='utf-8') as f:
        f.write(content)
    print("Replaced")
else:
    print(f"Pattern at {idx_start}, end at {idx_end}")
