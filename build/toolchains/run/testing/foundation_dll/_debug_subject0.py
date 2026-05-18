"""Full debug injection for convert-char Subject_0 — reapply after each codegen."""
import sys, subprocess

REPO = r'D:\agent\booming-il2cpp'
GEN_PATH = (
    REPO + r'\verification\foundation-dll\System.Private.CoreLib'
    r'\convert-char\codegen\ConvertCharSubjects\generated\native-aot.generated.cpp'
)

def run_codegen():
    """Regenerate codegen from scratch."""
    print("Step 0: Regenerating codegen...")
    r = subprocess.run([
        'dotnet', 'run', '--no-build',
        '--project', REPO + r'\src\managed\Chaos.IL2CPP.Driver',
        '--', 'convert-to-cpp',
        '--assembly', REPO + r'\verification\foundation-dll\System.Private.CoreLib'
                      r'\convert-char\managed\subjects\build-output\ConvertCharSubjects.dll',
        '--assembly-dir', REPO + r'\verification\foundation-dll\System.Private.CoreLib'
                        r'\convert-char\managed\subjects\build-output',
        '--output', REPO + r'\verification\foundation-dll\System.Private.CoreLib'
                    r'\convert-char\codegen',
    ], capture_output=True, text=True, timeout=300)
    if r.returncode != 0:
        print(f"CODEGEN FAILED rc={r.returncode}")
        print(r.stderr[-500:])
        sys.exit(1)
    print("Codegen OK")

def inject_debug():
    """Inject entry + catch fprintf into Subject_0."""
    with open(GEN_PATH, 'rb') as f:
        data = f.read()

    # ── Entry fprintf ──
    func_marker = b'ConvertCharSubjects_ConvertCharSubjects_Subject_0(void)\r\n{'
    func_idx = data.rfind(func_marker)
    assert func_idx >= 0, "Subject_0 body not found"

    # Insert after '{'
    after_brace = func_idx + len(func_marker)
    rest = data[after_brace:]
    eol = rest.find(b'\n')
    insert_pos = after_brace + eol + 1

    BS = bytes([0x5C])  # backslash
    N = b'n'
    entry_line = (
        b'\tstd::fprintf(stderr, "ENTER Subject_0'
        + BS + N + b'");\r\n'
        b'\tstd::fflush(stderr);\r\n'
    )
    data = data[:insert_pos] + entry_line + data[insert_pos:]

    # ── Catch fprintf ──
    # Find the catch handler after the function body
    sub_data = data[func_idx:]  # work within Subject_0 region
    catch_marker = b'catch (const chaos_managed_exception& chaos_exception)'
    catch_idx = sub_data.find(catch_marker)
    assert catch_idx >= 0, "catch not found in Subject_0"

    brace_start = sub_data.find(b'{', catch_idx)
    after_brace2 = brace_start + 1
    rest2 = sub_data[after_brace2:]
    eol2 = rest2.find(b'\n')
    insert_pos2 = after_brace2 + eol2 + 1

    catch_line = (
        b'\t\t\tstd::fprintf(stderr, "CATCH Subject_0 object_value=%lld'
        + BS + N + b'", (long long)chaos_exception.object_value);\r\n'
        b'\t\t\tstd::fflush(stderr);\r\n\t\t\t'
    )
    data = data[:func_idx] + sub_data[:insert_pos2] + catch_line + sub_data[insert_pos2:]

    # Add cstdio include if missing
    if b'<cstdio>' not in data:
        data = data.replace(b'#include <cstdint>', b'#include <cstdint>\n#include <cstdio>', 1)

    with open(GEN_PATH, 'wb') as f:
        f.write(data)

    # Verify
    assert b'ENTER Subject_0' + BS + N in data, "entry fprintf bad"
    assert b'CATCH Subject_0' in data, "catch fprintf not found"
    ver = data[data.find(b'ENTER Subject_0')-2:data.find(b'ENTER Subject_0')+30]
    print(f"Entry bytes: {ver.hex(' ')}")
    ver2 = data[data.find(b'CATCH Subject_0')-2:data.find(b'CATCH Subject_0')+30]
    print(f"Catch bytes: {ver2.hex(' ')}")
    print("Injection OK")

def build():
    """Build entry.exe."""
    print("Building entry.exe...")
    build_dir = REPO + r'\verification\foundation-dll\System.Private.CoreLib\convert-char\native\build'
    r = subprocess.run(
        ['cmake', '--build', build_dir, '--config', 'Release', '--target', 'entry'],
        capture_output=True, text=True, timeout=300,
    )
    if r.returncode != 0:
        print("BUILD FAILED")
        for line in r.stderr.splitlines()[-15:]:
            print(f"  {line}")
        sys.exit(1)
    print("Build OK")

def run():
    """Run entry.exe and capture output."""
    exe = REPO + r'\verification\foundation-dll\System.Private.CoreLib\convert-char\native\build\Release\entry.exe'
    r = subprocess.run([exe], capture_output=False, text=True, timeout=120)
    print(f"EXIT: {r.returncode}")

if __name__ == '__main__':
    run_codegen()
    inject_debug()
    build()
    run()
