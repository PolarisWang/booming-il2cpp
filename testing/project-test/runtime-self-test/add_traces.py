"""Add debug traces into generated native-aot.generated.cpp."""
import sys

FILE = r'D:\agent\booming-il2cpp\testing\project-test\runtime-self-test\codegen\RuntimeSelfTest\generated\native-aot.generated.cpp'

with open(FILE, 'rb') as f:
    content = f.read()

# Fix 1: The old main trace was inserted with \\n instead of proper \n
# Search for broken main trace and remove it
old_broken = b'fprintf(stderr, "TRACE main: read sharedCounter after join=%llu\n'
if old_broken in content:
    # Remove the broken trace line (it's a line by itself)
    # Find the full line
    idx = content.find(old_broken)
    # Find start of this line
    line_start = content.rfind(b'\n', 0, idx) + 1
    # Find end of this line
    line_end = content.find(b'\n', idx)
    # Remove this line (from line_start to line_end+1)
    content = content[:line_start] + content[line_end+1:]
    print("Removed broken main trace")

# Fix 2: The old "TRACE b0: loop start" at line 3256 is currently a proper single line?
# Check if it's correct
idx = content.find(b'TRACE b0: loop start')
if idx >= 0:
    print(f"TRACE b0 loop start found at offset {idx}")
    # Check if it's properly formed
    chunk = content[idx:idx+50]
    print(f"  chunk: {chunk!r}")

# Now insert the traces properly

# Trace A: in b__0 after sharedCounter write (chaos_gc_dirty_card + } + // leave)
# Pattern:
# \t\tchaos_gc_dirty_card(chaos_object);\r\n
# \t}\r\n
# \t// leave (handled via structured EH branches)\r\n
# We need to find this specific instance (sharedCounter write, not workerDone write)
# The sharedCounter version has "field_...__sharedCounter" on the previous line

trace_a_marker = b'field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter = chaos_value;\r\n\t\tchaos_gc_dirty_card(chaos_object);\r\n\t}\r\n\t// leave (handled via structured EH branches)'

trace_a_insert = b'field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter = chaos_value;\r\n\t\tchaos_gc_dirty_card(chaos_object);\r\n\t\tfprintf(stderr, "TRACE b0: wrote sharedCounter=%llu\\n", (unsigned long long)chaos_value);\r\n\t}\r\n\t// leave (handled via structured EH branches)'

count_a = content.count(trace_a_marker)
print(f"Trace A marker count: {count_a}")

if count_a == 1:
    content = content.replace(trace_a_marker, trace_a_insert)
    print("Inserted trace A (b0 sharedCounter write)")
else:
    print(f"ERROR: Trace A marker found {count_a} times, expected 1")

# Trace B: in ThreadedGcAllocation after sharedCounter read in lock
# Pattern (4 tabs inner):
# \t\t\t\t_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter;\r\n
# \t\t\t}\r\n
# \t\t\tchaos_locals[5] = _s2;

trace_b_marker = b'\t\t\t\t_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter;\r\n\t\t\t}\r\n\t\t\tchaos_locals[5] = _s2;'
trace_b_insert = b'\t\t\t\t_s2 = chaos_object->field_RuntimeSelfTest_IntegrationProof___c__DisplayClass4_0__sharedCounter;\r\n\t\t\t\tfprintf(stderr, "TRACE main: read sharedCounter after join=%llu\\n", (unsigned long long)_s2);\r\n\t\t\t}\r\n\t\t\tchaos_locals[5] = _s2;'

count_b = content.count(trace_b_marker)
print(f"Trace B marker count: {count_b}")

if count_b == 1:
    content = content.replace(trace_b_marker, trace_b_insert)
    print("Inserted trace B (main sharedCounter read)")
else:
    print(f"ERROR: Trace B marker found {count_b} times, expected 1")

with open(FILE, 'wb') as f:
    f.write(content)

print("\nDone. Verifying...")
# Verify both traces
for t in [b'TRACE b0: wrote sharedCounter', b'TRACE main: read sharedCounter after join']:
    cnt = content.count(t)
    print(f"  '{t.decode()}': found {cnt} times")
