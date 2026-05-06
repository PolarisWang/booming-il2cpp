import subprocess, os, sys

base = r'D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib'
slug = sys.argv[1] if len(sys.argv) > 1 else 'math-numerics'
hu_dir = os.path.join(base, slug, 'il2cpp_dist', 'hotupdate')
build_dir = os.path.join(hu_dir, 'build')
vcvars = r'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat'
ucrt = '%WindowsSdkDir%lib\\%UCRTVersion%\\ucrt\\x64\\ucrt.lib'
all_libs = ' '.join([
    '"' + r'D:\agent\booming-il2cpp\build\native-runtime\Release\chaos_runtime_core.lib"',
    '"' + r'D:\agent\booming-il2cpp\build\native\src\native\interpreter\Release\chaos_interpreter.lib"',
    '"' + r'D:\agent\booming-il2cpp\build\native\src\native\bootstrap\Release\chaos_bootstrap.lib"',
    '"' + r'D:\agent\booming-il2cpp\build\native\src\native\support\Release\chaos_support.lib"',
    '"' + r'D:\agent\booming-il2cpp\build\native\src\native\hot-update\Release\chaos_hot_update.lib"',
    '"' + r'D:\agent\booming-il2cpp\build\native\fmt_build\Release\chaos_fmt.lib"',
    '"' + r'D:\agent\booming-il2cpp\build\native\src\native\common\Release\chaos_common.lib"',
    '"' + r'D:\agent\booming-il2cpp\build\native\bdwgc_build\Release\chaos_bdwgc.lib"',
])
inc_flags = ' '.join([
    '-I"D:\\agent\\booming-il2cpp\\src\\native\\common"',
    '-I"D:\\agent\\booming-il2cpp\\src\\native\\common\\chaos"',
    '-I"D:\\agent\\booming-il2cpp\\contracts\\native\\v0"',
    '-I"D:\\agent\\booming-il2cpp\\src\\native\\runtime-core"',
    '-I"D:\\agent\\booming-il2cpp\\src\\native\\bootstrap"',
    '-I"D:\\agent\\booming-il2cpp\\src\\native\\interpreter"',
    '-I"D:\\agent\\booming-il2cpp\\third_party\\fmt\\include"',
    '-I"D:\\agent\\booming-il2cpp\\verification\\foundation-dll\\System.Private.CoreLib"',
])
defines = '-DCHAOS_IL2CPP_CHECK -DCHAOS_RUNTIME_ABI_STATIC'

ht_cpp = os.path.join(hu_dir, 'HotUpdateTest.cpp')
gen_cpp = os.path.join(hu_dir, 'genuine-fixed', 'native-aot.generated.cpp')

# Full build WITHOUT errorlevel checks to see all output
print('=== Full build (no exit-on-error) ===')
for f in ['real_ht.obj', 'real_gen.obj', 'real_test.exe']:
    p = os.path.join(build_dir, f)
    if os.path.exists(p):
        os.remove(p)

# Step 1: compile HT
r1 = subprocess.run(['cmd.exe', '/c', 'call "' + vcvars + '" x64 >nul 2>nul && cl /nologo /std:c++17 /c /EHsc /W3 /utf-8 /O2 /MD /GS- ' + defines + ' ' + inc_flags + ' /Fo"' + build_dir + '/real_ht.obj" "' + ht_cpp + '"'], capture_output=True, text=True, timeout=120)
print('cl HT rc:', r1.returncode)
print(r1.stdout)
print(r1.stderr)

if r1.returncode == 0:
    # Step 2: compile genuine
    r2 = subprocess.run(['cmd.exe', '/c', 'call "' + vcvars + '" x64 >nul 2>nul && cl /nologo /std:c++17 /c /EHsc /W3 /utf-8 /O2 /MD /GS- ' + defines + ' ' + inc_flags + ' /Fo"' + build_dir + '/real_gen.obj" "' + gen_cpp + '"'], capture_output=True, text=True, timeout=120)
    print('\ncl Genuine rc:', r2.returncode)
    # Only print last 30 lines of stdout (warnings)
    lines = r2.stdout.split('\n')
    print('\n'.join(lines[-30:]))
    print(r2.stderr[-300:])

    if r2.returncode == 0:
        # Step 3: link
        r3 = subprocess.run(['cmd.exe', '/c', 'call "' + vcvars + '" x64 >nul 2>nul && link /nologo /nodefaultlib:ucrt /out:"' + build_dir + '/real_test.exe" "' + build_dir + '/real_ht.obj" "' + build_dir + '/real_gen.obj" ' + all_libs + ' "' + ucrt + '" ole32.lib user32.lib'], capture_output=True, text=True, timeout=120)
        print('\nlink rc:', r3.returncode)
        print(r3.stdout[-500:])
        print(r3.stderr[-500:])

        if r3.returncode == 0:
            r4 = subprocess.run([os.path.join(build_dir, 'real_test.exe')], capture_output=True, text=True, timeout=30)
            print('\nRun rc:', r4.returncode, '(0x%08X)' % r4.returncode)
            print('stdout:', r4.stdout[:500])
            print('stderr:', r4.stderr[:500])
