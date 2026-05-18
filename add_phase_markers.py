import re

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# Add fprintf markers before key phases
# NOTE: '\\n' in C = \\\\n in Python string (write literal backslash-n)
marker_text = '    fprintf(stderr, "COL: {}\\n"); fflush(stderr);\n'

anchors = [
    ('// Phase 1: Mark roots.',              'P1_MARK_ROOTS'),
    ('// Clear all page bitmaps',             'P1_CLEAR_BITMAPS'),
    ('// Mark pinned roots from snapshot',    'P1_MARK_PINNED'),
    ('// Phase 2: Mark transitive closure',   'P2_CLOSURE'),
    ('// When there are enough pages, use parallel mark', 'P2_PARALLEL_MARK'),
    ('// Phase 3: Run finalizers',            'P3_FINALIZERS'),
    ('// Phase 3b: Re-mark pass',             'P3B_REMARK'),
    ('// Phase 3c: Process weak handles',     'P3C_WEAK_HANDLES'),
    ('// Sequential sweep (bypass GcWorkerPool', 'P4_SWEEP'),
]

insertions = []
for search_text, name in anchors:
    idx = content.find(search_text)
    if idx >= 0:
        before = content.rfind('\n', 0, idx)
        insertions.append((before, marker_text.format(name)))
        print(f'  Found "{search_text[:50]}"')
    else:
        print(f'  NOT FOUND "{search_text[:50]}"')

insertions.sort(key=lambda x: -x[0])
for pos, text in insertions:
    content = content[:pos+1] + text + content[pos+1:]

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'w', encoding='utf-8') as f:
    f.write(content)

print(f'\nAdded {len(insertions)} phase markers')
