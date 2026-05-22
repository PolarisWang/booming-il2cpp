"""Modify native-aot.generated.cpp: replace traces and add iteration trace."""
import os

FILE = r'D:\agent\booming-il2cpp\testing\project-test\runtime-self-test\codegen\RuntimeSelfTest\generated\native-aot.generated.cpp'

with open(FILE, 'rb') as f:
    data = f.read()

# Step 1: Replace "TRACE b0: loop start" with "TRACE b0: iter=%lld"
old = b'\tfprintf(stderr, "TRACE b0: loop start\\n");'
new = b'\tfprintf(stderr, "TRACE b0: iter=%lld\\n", (long long)chaos_locals[0]);'
assert data.count(old) == 1, f"Expected 1 occurrence of old, found {data.count(old)}"
data = data.replace(old, new)
print("Step 1: Replaced loop start -> iter trace")

# Step 2: Add "TRACE b0: final i=%lld" after loop-exit blt
# Find: // blt (structured EH branch)\r\n followed by _s5 = chaos_args[0]
# But NOT the blt inside the loop (there's a second blt for workerDone)
# The first blt is in b__0, followed by workerDone stuff
# Pattern: blt\r\n\t_s5 = chaos_args[0];\r\n\t{\r\n\t\tauto* chaos_object = ...__DisplayClass4_0*>(_s5);
old2 = b'\t// blt (structured EH branch)\r\n\t_s5 = chaos_args[0];\r\n\t{\r\n\t\tauto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s5);'
# Only one should match (the one in b__0 after the loop, where _s5 reads this.sync for workerDone)
new2 = b'\t// blt (structured EH branch)\r\n\tfprintf(stderr, "TRACE b0: final i=%lld\\n", (long long)chaos_locals[0]);\r\n\t_s5 = chaos_args[0];\r\n\t{\r\n\t\tauto* chaos_object = reinterpret_cast<chaos_type_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0*>(_s5);'
if data.count(old2) == 1:
    data = data.replace(old2, new2)
    print("Step 2: Added final-i trace after blt")
else:
    print(f"Step 2a: old2 count = {data.count(old2)}")

# Also check if the blt trace was already added (from previous run)
# Look for "final i" in the data
if b'TRACE b0: final i=' in data:
    print("  (already present)")

with open(FILE, 'wb') as f:
    f.write(data)
print("Done")
