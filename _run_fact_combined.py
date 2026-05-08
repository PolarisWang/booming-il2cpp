import subprocess

bat = r"D:\agent\booming-il2cpp\build\toolchains\run\testing\foundation_dll\_build_fact_combined.bat"
proc = subprocess.run(bat, capture_output=True, text=True, shell=True, timeout=120)
out = proc.stdout + proc.stderr
print(out)
print("RC:", proc.returncode)