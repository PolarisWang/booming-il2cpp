import subprocess

bat = r"D:\agent\booming-il2cpp\build\toolchains\run\testing\foundation_dll\_build_all_isolate.bat"
proc = subprocess.run([bat], capture_output=True, text=True, shell=True)
print(proc.stdout)
if proc.stderr:
    print("STDERR:", proc.stderr[-2000:])
print("RC:", proc.returncode)