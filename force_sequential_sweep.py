# Force sequential sweep by reducing total_pages check threshold
import sys

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'rb') as f:
    data = f.read()

# Find the parallel/sequential threshold: "if (total_pages >= 2)"
# Change to "if (total_pages >= 999)" to force sequential path
search = b'    if (total_pages >= 2) {'
idx = data.find(search)
if idx < 0:
    print("Pattern not found!")
    sys.exit(1)

replacement = b'    if (total_pages >= 999) {'  # effectively disable parallel
data = data.replace(search, replacement)

# Verify
if data.find(replacement) >= 0:
    print("Disabled parallel sweep")
else:
    print("Replacement failed!")

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'wb') as f:
    f.write(data)
print("Done")
