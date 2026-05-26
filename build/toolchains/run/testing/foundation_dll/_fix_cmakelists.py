import sys

with open(sys.argv[1], 'r', encoding='utf-8') as f:
    content = f.read()

old = (
    "        f'set(CHAOS_ENTRY_SOURCES\\n'\n"
    "        f'    \"runtime-entry.cpp\"\\n'\n"
    "        f'    \"runtime-patchdata.cpp\"\\n'\n"
    "        f'    ${{CHAOS_NATIVE_STUBS}}\\n'\n"
    "        f'    ${{CHAOS_CODEGEN_CPP}}\\n'\n"
    "        f')\\n'"
)

new = (
    "        f'set(CHAOS_ENTRY_SOURCES\\n'\n"
    "        f'    \"runtime-entry.cpp\"\\n'\n"
    "        f'    \"runtime-patchdata.cpp\"\\n'\n"
    "        f'    \"${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/runtime_stubs/enum_stubs.cpp\"\\n'\n"
    "        f'    ${{CHAOS_NATIVE_STUBS}}\\n'\n"
    "        f'    ${{CHAOS_CODEGEN_CPP}}\\n'\n"
    "        f')\\n'"
)

if old in content:
    content = content.replace(old, new, 1)
    with open(sys.argv[1], 'w', encoding='utf-8') as f:
        f.write(content)
    print("OK")
else:
    print("FAIL: old string not found")
    lines = content.split('\n')
    for i, line in enumerate(lines):
        if 'CHAOS_ENTRY_SOURCES' in line:
            for j in range(i-1, min(i+6, len(lines))):
                print(f'{j+1}: {repr(lines[j])}')
