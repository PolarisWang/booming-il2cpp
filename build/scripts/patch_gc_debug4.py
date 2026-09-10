import re

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# More precise matches - use unique strings
pairs = [
    # 1. After GcScanAllThreadRoots - insert after the log_debug_m line
    ('collect_scanned_thread_stacks", mark_stack_.size(), before_roots);',
     'collect_scanned_thread_stacks", mark_stack_.size(), before_roots);\n        fprintf(stderr, "DBG: a1_mark_stack_size=%zu\\n", mark_stack_.size()); fflush(stderr);'),

    # 2. After POH - insert after the closing of the POH block
    # Find "DrainMarkStack()" which is called in the sequential path
    ('DrainMarkStackParallel(pages.data(), static_cast<int>(pages.size()));',
     'fprintf(stderr, "DBG: a2_before_parallel_mark\\n"); fflush(stderr);\n        DrainMarkStackParallel(pages.data(), static_cast<int>(pages.size()));\n        fprintf(stderr, "DBG: a3_after_parallel_mark\\n"); fflush(stderr);'),

    # 3. Before fire MARK_DONE
    ('GcFireEvent(GcEvent::MARK_DONE);',
     'fprintf(stderr, "DBG: a4_before_mark_done_event\\n"); fflush(stderr);\n        GcFireEvent(GcEvent::MARK_DONE);'),

    # 4. After finalizers
    ('HandleReMarkPass();',
     'fprintf(stderr, "DBG: a5_before_remark\\n"); fflush(stderr);\n        HandleReMarkPass();\n        fprintf(stderr, "DBG: a6_after_remark\\n"); fflush(stderr);'),
]

for old, new in pairs:
    count = content.count(old)
    print(f'  "{old[:50]}...": {count} occurrences')
    if count == 1:
        content = content.replace(old, new, 1)
        print(f'    -> replaced')
    elif count > 1:
        print(f'    -> SKIP (not unique)')
    else:
        print(f'    -> NOT FOUND')

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'w', encoding='utf-8', newline='') as f:
    f.write(content)
print('Done')
