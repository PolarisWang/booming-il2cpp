"""Add safe wrapper for Subject_0 and reconfigure CMake."""
import subprocess, os

REPO = r'D:\agent\booming-il2cpp'
WRAPPER_PATH = REPO + r'\testing\foundation-dll\System.Private.CoreLib\convert-char\native\subject0_safe_wrapper.cpp'

wrapper_source = '''
#include <cstdio>
#include <cstdlib>

extern "C" void ConvertCharSubjects_ConvertCharSubjects_Subject_0(void);

extern "C" void SafeWrapperSubject0() {
    try {
        ConvertCharSubjects_ConvertCharSubjects_Subject_0();
    } catch (const chaos_managed_exception& e) {
        std::fprintf(stderr, "SAFE_WRAPPER_CATCH obj=%lld\\n", (long long)e.object_value);
    } catch (...) {
        std::fprintf(stderr, "SAFE_WRAPPER_ELLIPSIS\\n");
        throw;
    }
}
'''

with open(WRAPPER_PATH, 'w', encoding='utf-8') as f:
    f.write(wrapper_source)
print(f"Wrote {WRAPPER_PATH}")

# Add to native CMakeLists.txt
cmake_path = REPO + r'\testing\foundation-dll\System.Private.CoreLib\convert-char\native\CMakeLists.txt'
with open(cmake_path, 'r', encoding='utf-8') as f:
    cmake = f.read()

if 'subject0_safe_wrapper.cpp' not in cmake:
    cmake = cmake.replace(
        'set(CHAOS_ENTRY_SOURCES\n    "runtime-entry.cpp"\n    "runtime-patchdata.cpp"',
        'set(CHAOS_ENTRY_SOURCES\n    "runtime-entry.cpp"\n    "runtime-patchdata.cpp"\n    "subject0_safe_wrapper.cpp"'
    )
    with open(cmake_path, 'w', encoding='utf-8') as f:
        f.write(cmake)
    print("Updated CMakeLists.txt")
else:
    print("Already in CMakeLists.txt")

# Now reconfigure and build
build_dir = REPO + r'\testing\foundation-dll\System.Private.CoreLib\convert-char\native\build'
r = subprocess.run(['cmake', '-S', os.path.dirname(cmake_path), '-B', build_dir,
                    '-G', 'Visual Studio 17 2022', '-A', 'x64'],
                   capture_output=True, text=True, timeout=120)
if r.returncode != 0:
    print("CMake configure FAILED:", r.stderr[-500:])
else:
    print("CMake configure OK")

# Build
r = subprocess.run(['cmake', '--build', build_dir, '--config', 'Release', '--target', 'entry'],
                   capture_output=True, text=True, timeout=300)
if r.returncode != 0:
    print("Build FAILED:", r.stderr[-500:])
else:
    print("Build OK")
    # Run with a small test program that calls SafeWrapperSubject0
