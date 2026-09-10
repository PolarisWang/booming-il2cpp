"""Fix the broken TRACE b0: loop exited line - replace real newline with \\n escape."""
import os

FILE = r'D:\agent\booming-il2cpp\testing\project-test\runtime-self-test\codegen\RuntimeSelfTest\generated\native-aot.generated.cpp'

with open(FILE, 'rb') as f:
    data = f.read()

# Find the broken trace: \tfprintf(stderr, "TRACE b0: loop exited i=%lld\n

# The string has "loop exited i=%lld" then a REAL newline (0x0a), then " on next line
# Replace: "TRACE b0: loop exited i=%lld\n", with "TRACE b0: loop exited i=%lld\n",
# where \n is the C escape sequence (0x5c 0x6e), not a real newline (0x0a)

# Find the broken pattern
old = b'\tfprintf(stderr, "TRACE b0: loop exited i=%lld\n"\n\t\t\t\t\t\t\t\t", (long long)chaos_locals[0]);'

# Hmm, the closing " is on the next line with lots of tabs. Let me find it differently.
# Let me search for the unique substring
idx = data.find(b'TRACE b0: loop exited')
assert idx >= 0, "Not found!"

# From this offset, find the full fprintf statement
start = data.rfind(b'\n', 0, idx) + 1  # start of line
end = data.find(b'\n', idx)  # end of the 1st line (containing the real newline)
end2 = data.find(b'\n', end + 1)  # end of the 2nd line (containing the closing ")

line1 = data[start:end]
line2 = data[end+1:end2]

print(f"Line1: {line1!r}")
print(f"Line2: {line2!r}")

# Combine into proper single line: \tfprintf(stderr, "...\\n", args);
proper_line = b'\tfprintf(stderr, "TRACE b0: loop exited i=%lld\\n", (long long)chaos_locals[0]);\r\n'

data = data[:start] + proper_line + data[end2+1:]

with open(FILE, 'wb') as f:
    f.write(data)

print("Fixed!")
