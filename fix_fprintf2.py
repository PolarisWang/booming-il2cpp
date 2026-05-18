import re

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'rb') as f:
    data = f.read()

# Fix: replace real newlines inside C string literals with \n escape
# Pattern: fprintf(stderr, "COL: SWEEP_RETURN reclaimed=%llu
# ", ...)  --> fix by inserting backslash before 'n'

bk = chr(92).encode('ascii')  # single backslash

# Fix SWEEP_ENTER line at line ~1012
# Pattern: fprintf(stderr, "COL: SWEEP_ENTER pg=%p bitmap_bytes=%llu payload_size=%llu\n"
# The \n at position of bitmap_bytes should be \\n
patterns = [
    # SWEEP_ENTER
    (b'SWEEP_ENTER pg=%p bitmap_bytes=%llu payload_size=%llu\n',
     b'SWEEP_ENTER pg=%p bitmap_bytes=%llu payload_size=%llu' + bk + b'n'),
    # SWEEP_RETURN
    (b'SWEEP_RETURN reclaimed=%llu\n',
     b'SWEEP_RETURN reclaimed=%llu' + bk + b'n'),
]

for old, new in patterns:
    if old in data:
        data = data.replace(old, new)
        print(f"Fixed pattern: {old[:40]}")
    else:
        # Try with \r\n
        old_cr = old.replace(b'\n', b'\r\n')
        if old_cr in data:
            data = data.replace(old_cr, new)
            print(f"Fixed pattern (with CR): {old[:40]}")
        else:
            print(f"NOT FOUND: {old[:40]}")
            # Search for it
            search = old.split(b'\n')[0]
            idx = data.find(search)
            if idx >= 0:
                print(f"  Found at {idx}, bytes: {repr(data[idx:idx+80])}")

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'wb') as f:
    f.write(data)

print("Done")
