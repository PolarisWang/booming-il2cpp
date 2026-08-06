"""Move iter trace inside the loop body block."""
import os

FILE = r'D:\agent\booming-il2cpp\testing\project-test\runtime-self-test\codegen\RuntimeSelfTest\generated\native-aot.generated.cpp'

with open(FILE, 'rb') as f:
    data = f.read()

# Move the iter trace from after // br to after the opening {
# Old: \t// br ...\r\n\tfprintf(stderr, "TRACE b0: iter...
# New: \t// br ...\r\n\t\tfprintf(stderr, "TRACE b0: iter... inside block

old = b'\t// br (handled via structured EH branches)\r\n\tfprintf(stderr, "TRACE b0: iter=%lld\\n", (long long)chaos_locals[0]);\r\n\r\n\t{'
new = b'\t// br (handled via structured EH branches)\r\n\r\n\t{\r\n\t\tfprintf(stderr, "TRACE b0: iter=%lld\\n", (long long)chaos_locals[0]);'

count = data.count(old)
print(f"Count: {count}")

if count == 1:
    data = data.replace(old, new)
    print("Moved trace inside loop body")
else:
    print(f"Pattern appears {count} times, can't safely replace")

with open(FILE, 'wb') as f:
    f.write(data)
print("Done")
