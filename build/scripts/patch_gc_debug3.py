import re

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\thread_state.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

old = (
    '    EnumerateThreads([](ManagedThread* thread) -> bool {\n'
    '        if (thread == nullptr || !thread->is_running) return true;\n'
    '\n'
    '        // If the current thread is calling this, skip self — its registers\n'
    '        // and locals are on the current native stack and will be found via\n'
    '        // conventional means (or the thread is the GC thread itself).\n'
    '        if (thread == tls_this_thread) return true;\n'
    '\n'
    '        // Conservatively scan the full stack range.\n'
    '        // Stack grows downward: stack_limit < stack_base on Windows/linux.\n'
    '        char* scan_start = static_cast<char*>(thread->stack_limit);\n'
    '        char* scan_end   = static_cast<char*>(thread->stack_base);\n'
    '\n'
    '        if (scan_start == nullptr || scan_end == nullptr) return true;\n'
    '        if (scan_start >= scan_end) return true;\n'
    '\n'
    '        // Align to pointer boundary.\n'
    '        uintptr_t start_aligned = (reinterpret_cast<uintptr_t>(scan_start) + sizeof(void*) - 1)\n'
    '            & ~static_cast<uintptr_t>(sizeof(void*) - 1);\n'
    '        uintptr_t end_aligned = reinterpret_cast<uintptr_t>(scan_end)\n'
    '            & ~static_cast<uintptr_t>(sizeof(void*) - 1);\n'
    '\n'
    '        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {\n'
    '            // Pre-filter: skip slots whose VALUE doesn\'t point into the\n'
    '            // managed heap range.  This eliminates false-positive roots\n'
    '            // from integers, code pointers, and OS handles that happen to\n'
    '            // be pointer-aligned on the stack.\n'
    '            auto* val_ptr = reinterpret_cast<void**>(slot);\n'
    '            if (*val_ptr != nullptr &&\n'
    '                reinterpret_cast<uintptr_t>(*val_ptr) >= g_heap_base) {\n'
    '                s_callback(reinterpret_cast<void*>(slot), /*is_interior=*/false, s_user_data);\n'
    '            }\n'
    '        }\n'
    '\n'
    '        return true;  // continue enumeration\n'
    '    });\n'
    '}}  // namespace chaos::il2cpp::runtime_core::threading'
)

new = (
    '    EnumerateThreads([](ManagedThread* thread) -> bool {\n'
    '        fprintf(stderr, "DBG_GCS: thread=%p running=%d tls=%p\\n", (void*)thread, (int)(thread ? thread->is_running : -1), (void*)tls_this_thread); fflush(stderr);\n'
    '        if (thread == nullptr || !thread->is_running) { fprintf(stderr, "DBG_GCS: skip !running\\n"); fflush(stderr); return true; }\n'
    '\n'
    '        // If the current thread is calling this, skip self.\n'
    '        fprintf(stderr, "DBG_GCS: check tls_this_thread thread=%p tls=%p\\n", (void*)thread, (void*)tls_this_thread); fflush(stderr);\n'
    '        if (thread == tls_this_thread) { fprintf(stderr, "DBG_GCS: skip self\\n"); fflush(stderr); return true; }\n'
    '\n'
    '        fprintf(stderr, "DBG_GCS: scanning stack\\n"); fflush(stderr);\n'
    '\n'
    '        // Conservatively scan the full stack range.\n'
    '        char* scan_start = static_cast<char*>(thread->stack_limit);\n'
    '        char* scan_end   = static_cast<char*>(thread->stack_base);\n'
    '\n'
    '        if (scan_start == nullptr || scan_end == nullptr) { fprintf(stderr, "DBG_GCS: null stack bounds\\n"); fflush(stderr); return true; }\n'
    '        if (scan_start >= scan_end) { fprintf(stderr, "DBG_GCS: empty stack range\\n"); fflush(stderr); return true; }\n'
    '\n'
    '        uintptr_t start_aligned = (reinterpret_cast<uintptr_t>(scan_start) + sizeof(void*) - 1)\n'
    '            & ~static_cast<uintptr_t>(sizeof(void*) - 1);\n'
    '        uintptr_t end_aligned = reinterpret_cast<uintptr_t>(scan_end)\n'
    '            & ~static_cast<uintptr_t>(sizeof(void*) - 1);\n'
    '\n'
    '        fprintf(stderr, "DBG_GCS: stack [%p-%p) aligned [%p-%p)\\n",\\n'
    '            (void*)scan_start, (void*)scan_end, (void*)start_aligned, (void*)end_aligned); fflush(stderr);\n'
    '\n'
    '        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {\n'
    '            auto* val_ptr = reinterpret_cast<void**>(slot);\n'
    '            if (*val_ptr != nullptr &&\n'
    '                reinterpret_cast<uintptr_t>(*val_ptr) >= g_heap_base) {\n'
    '                s_callback(reinterpret_cast<void*>(slot), /*is_interior=*/false, s_user_data);\n'
    '            }\n'
    '        }\n'
    '\n'
    '        fprintf(stderr, "DBG_GCS: done scanning thread\\n"); fflush(stderr);\n'
    '        return true;\n'
    '    });\n'
    '    fprintf(stderr, "DBG_GCS: EnumerateThreads done\\n"); fflush(stderr);\n'
    '}}  // namespace chaos::il2cpp::runtime_core::threading\n'
)

count = content.count(old)
print(f'Found {count} occurrences')
if count > 0:
    content = content.replace(old, new, 1)
    with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\thread_state.cpp', 'w', encoding='utf-8', newline='') as f:
        f.write(content)
    print('Done')
else:
    # Find the area
    idx = content.find('EnumerateThreads')
    if idx >= 0:
        print(repr(content[idx:idx+500]))
