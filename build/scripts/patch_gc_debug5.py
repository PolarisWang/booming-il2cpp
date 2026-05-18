import re

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

pairs = [
    # After a1, insert before POH
    ('fprintf(stderr, "DBG: a1_after_GcScanAllThreadRoots\\n"); fflush(stderr);\n    }\n    \n    // Scan POH regions',
     'fprintf(stderr, "DBG: a1_after_GcScanAllThreadRoots\\n"); fflush(stderr);\n        fprintf(stderr, "DBG: a1b_checking_poh\\n"); fflush(stderr);\n    }\n    \n    // Scan POH regions'),

    # After the POH block, before LOH
    ('        }\n        }\n    }\n    \n    // Phase 2: Mark transitive closure.',
     '        }\n        }\n    }\n    fprintf(stderr, "DBG: a1c_after_poh\\n"); fflush(stderr);\n    \n    // Phase 2: Mark transitive closure.'),

    # In the parallel mark page list walk, print each page
    ('        pages.reserve(static_cast<size_t>(page_count_));\n            auto* p = page_list_;\n            while (p != nullptr) {\n                pages.push_back(p);\n                p = p->next;\n            }',
     '        pages.reserve(static_cast<size_t>(page_count_));\n            fprintf(stderr, "DBG: page_list_walk_start\\n"); fflush(stderr);\n            auto* p = page_list_;\n            int page_idx = 0;\n            while (p != nullptr) {\n                fprintf(stderr, "DBG: page_list[%d]=%p next=%p\\n", page_idx, (void*)p, (void*)(p->next.load(std::memory_order_acquire))); fflush(stderr);\n                pages.push_back(p);\n                p = p->next;\n                page_idx++;\n            }\n            fprintf(stderr, "DBG: page_list_walk_done count=%d\\n", page_idx); fflush(stderr);'),
]

for old, new in pairs:
    count = content.count(old)
    print(f'  pattern: {count} occurences')
    if count > 0:
        content = content.replace(old, new, 1)
        print(f'    -> replaced')

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'w', encoding='utf-8', newline='') as f:
    f.write(content)
print('Done')
