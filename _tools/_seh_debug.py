#!/usr/bin/env python3
"""Inject SEH handler into HotUpdateTest.cpp for crash diagnosis."""
import re, subprocess, sys, os

base = r'D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib'
slug = sys.argv[1] if len(sys.argv) > 1 else 'array-indexing-copy'

cpp = os.path.join(base, slug, 'il2cpp_dist', 'hotupdate', 'HotUpdateTest.cpp')
content = open(cpp, encoding='utf-8').read()

# Rename main -> __d3_real_main (only last occurrence)
idx = content.rfind('int main()')
content = content[:idx] + 'int __d3_real_main()' + content[idx + len('int main()'):]

# Find end of include section
last_include = 0
for m in re.finditer(r'#include.*', content):
    last_include = m.end()

# Inject SEH wrapper
seh_code = """
// SEH crash handler injected for diagnosis
#include <Windows.h>
#include <cstdio>

static LONG WINAPI _d3_seh_handler(EXCEPTION_POINTERS* ep) {
    std::fprintf(stderr, "D3_CRASH: code=0x%08lX addr=%p\\n",
        ep->ExceptionRecord->ExceptionCode,
        ep->ExceptionRecord->ExceptionAddress);
    std::fprintf(stderr, "D3_CRASH: flags=0x%lX numParams=%lu\\n",
        ep->ExceptionRecord->ExceptionFlags,
        ep->ExceptionRecord->NumberParameters);
    if (ep->ExceptionRecord->NumberParameters > 0) {
        std::fprintf(stderr, "D3_CRASH: param0=0x%p\\n",
            (void*)ep->ExceptionRecord->ExceptionInformation[0]);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

int main() {
    SetUnhandledExceptionFilter(_d3_seh_handler);
    return __d3_real_main();
}

"""

content = content[:last_include] + seh_code + content[last_include:]

out_cpp = os.path.join(base, slug, 'il2cpp_dist', 'hotupdate', 'HotUpdateTest.cpp')
with open(out_cpp, 'w') as f:
    f.write(content)

print(f"SEH-instrumented {out_cpp}")

# Rebuild
bat = os.path.join(base, slug, 'il2cpp_dist', 'hotupdate', 'build', f'_build_d3_{slug}.bat')
r = subprocess.run(['cmd.exe', '/c', bat], capture_output=True, text=True, timeout=180)
print(f"Build: rc={r.returncode}")
if r.returncode != 0:
    print(r.stderr[-500:])
print(r.stdout[-300:])

# Run
exe = os.path.join(base, slug, 'il2cpp_dist', 'hotupdate', 'build', f'd3_hotupdate_{slug}.exe')
r2 = subprocess.run([exe], capture_output=True, text=True, timeout=30)
print(f"Run: rc={r2.returncode} (0x{r2.returncode:08X})")
print(f"stdout: {r2.stdout[:500]}")
print(f"stderr: {r2.stderr[:500]}")
