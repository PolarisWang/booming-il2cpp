"""Add trace showing iterations value before blt in b__0 loop."""
import os

FILE = r'D:\agent\booming-il2cpp\testing\project-test\runtime-self-test\codegen\RuntimeSelfTest\generated\native-aot.generated.cpp'

with open(FILE, 'rb') as f:
    data = f.read()

# In b__0, after i++ and before blt comparison:
# _s1 = chaos_locals[0];    // i
# _s2 = chaos_args[0];      // this
# _s2 = this.iterations;    // read iterations
# // blt

# Add trace after reading iterations, before blt
# Pattern: \t\t_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations;\r\n\t}\r\n\t// blt
# This pattern should be specific to b__0 (in other places the field names differ)

# Actually, let me check if there are multiple occurrences of this pattern
# Search for the field name + iterations
old = b'\t\t_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations;\r\n\t}\r\n\t// blt (structured EH branch)'
new = b'\t\t_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__iterations;\r\n\t\tfprintf(stderr, "TRACE b0: iterations=%lld\\n", (long long)_s2);\r\n\t}\r\n\t// blt (structured EH branch)'

count = data.count(old)
print(f"Count: {count}")

if count == 1:
    data = data.replace(old, new)
    print("Added iterations trace")
elif count > 1:
    # Find the one in b__0 context
    # b__0 starts at the function body signature
    idx = data.find(b'ThreadedGcAllocation_b__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)')
    if idx >= 0:
        # Find the iteration field read AFTER b__0
        search_from = idx
        pattern_idx = -1
        for _ in range(count):
            found = data.find(old, search_from)
            if found >= 0:
                pattern_idx = found
                search_from = found + 1
        if pattern_idx >= 0:
            data = data[:pattern_idx] + new + data[pattern_idx + len(old):]
            print(f"Added iterations trace at offset {pattern_idx}")
else:
    # Check if already added
    if b'TRACE b0: iterations=' in data:
        print("Already present")
    else:
        print("Pattern not found!")

with open(FILE, 'wb') as f:
    f.write(data)
print("Done")
