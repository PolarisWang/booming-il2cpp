import re

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# After EnumerateThreads call: insert debug before mark_stack_.size() check
old = (
    "            });if (mark_stack_.size() > before_roots) {\n"
    "            has_roots = true;\n"
    "        }\n"
    "        CHAOS_IL2CPP_LOG_DEBUG_M(\"OldGen\", \"collect_scanned_nurseries\");"
)

new = (
    "            });\n"
    "        fprintf(stderr, \"DBG: after_enum size=%zu before=%zu\\n\", mark_stack_.size(), before_roots); fflush(stderr);\n"
    "        if (mark_stack_.size() > before_roots) {\n"
    "            has_roots = true;\n"
    "        }\n"
    "        fprintf(stderr, \"DBG: after_has_roots\\n\"); fflush(stderr);\n"
    "        CHAOS_IL2CPP_LOG_DEBUG_M(\"OldGen\", \"collect_scanned_nurseries\");"
)

count = content.count(old)
print(f'Found {count} occurrences of target string')
if count > 0:
    content = content.replace(old, new, 1)
    with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'w', encoding='utf-8', newline='') as f:
        f.write(content)
    print('Done')
else:
    # Find near the EnumerateThreads call
    idx = content.find('before_roots')
    if idx >= 0:
        print(repr(content[idx-50:idx+200]))
    else:
        idx2 = content.find('collect_scanned_nurseries')
        if idx2 >= 0:
            print(repr(content[idx2-100:idx2+50]))
